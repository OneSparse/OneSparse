/* This is a "header template" see matrix.c for specific instanciations */

/* Set Returning Function (matrix_elements) state for generating tuples
   from a matrix.
*/
typedef struct FN(OS_Matrix_ExtractState)
{
	OS_Matrix *mat;
	GrB_Index *rows;
	GrB_Index *cols;
	PG_TYPE *vals;
} FN(OS_Matrix_ExtractState);

/* Expanded Object Header "methods" for flattening matrices for storage */
static Size FN(matrix_get_flat_size)(ExpandedObjectHeader *eohptr);
static void FN(matrix_flatten_into)(ExpandedObjectHeader *eohptr,
									void *result, Size allocated_size);

static const ExpandedObjectMethods FN(matrix_methods) =
{
	FN(matrix_get_flat_size),
	FN(matrix_flatten_into)
};

/* Utility function that expands a flattened matrix datum. */
Datum
FN(expand_flat_matrix)(OS_FlatMatrix *flat,
					   MemoryContext parentcontext);

OS_Matrix *
FN(construct_empty_expanded_matrix)(GrB_Index nrows,
									GrB_Index ncols,
									MemoryContext parentcontext);


PG_FUNCTION_INFO_V1(FN(matrix));
PG_FUNCTION_INFO_V1(FN(matrix_empty));

/* Compute size of storage needed for matrix */
static Size
FN(matrix_get_flat_size)(ExpandedObjectHeader *eohptr)
{
	GrB_Info info;
	OS_Matrix *A = (OS_Matrix *) eohptr;

	Assert(A->em_magic == matrix_MAGIC);

	if (A->flat_size)
		return A->flat_size;

	A->flat_size = OS_MATRIX_OVERHEAD();
	return A->flat_size;
}

/* Flatten matrix into allocated result buffer  */
static void
FN(matrix_flatten_into)(ExpandedObjectHeader *eohptr,
						void *result, Size allocated_size)
{
	GrB_Info info;
	GrB_Index *rows, nrows, ncols, nvals;
	OS_Matrix *A = (OS_Matrix *) eohptr;
	OS_FlatMatrix *flat = (OS_FlatMatrix *) result;

	Assert(A->em_magic == matrix_MAGIC);
	Assert(allocated_size == A->flat_size);

	memset(flat, 0, allocated_size);

	flat->type = GB_TYPE;
	flat->nrows = nrows;
	flat->ncols = ncols;
	flat->nvals = nvals;
	SET_VARSIZE(flat, allocated_size);
}

/* Expand a flat matrix. */
Datum
FN(expand_flat_matrix)(OS_FlatMatrix *flat,
					   MemoryContext parentcontext)
{
	GrB_Info info;

	OS_Matrix *A;

	MemoryContext objcxt, oldcxt;
	MemoryContextCallback *ctxcb;

	GrB_Index ncols, nrows, nvals;
	GrB_Index *rows, *cols;
	void *vals;
	GrB_Type type;

	/* Create a new context that will hold the expanded object. */
	objcxt = AllocSetContextCreate(parentcontext,
								   "expanded matrix",
								   ALLOCSET_DEFAULT_SIZES);

	/* Allocate a new expanded matrix */
	A = (OS_Matrix*)MemoryContextAlloc(objcxt,
									   sizeof(OS_Matrix));

	/* Initialize the ExpandedObjectHeader member with flattening
	 * methods and new object context */
	EOH_init_header(&A->hdr, &FN(matrix_methods), objcxt);

	/* Used for debugging checks */
	A->em_magic = matrix_MAGIC;

	/* Switch to new object context */
	oldcxt = MemoryContextSwitchTo(objcxt);

	/* Get dimensional information from flat */
	nrows = flat->nrows;
	ncols = flat->ncols;
	nvals = flat->nvals;
	type = flat->type;
	A->type = type;
	A->flat_size = 0;

	/* Create a context callback to free matrix when context is cleared */
	ctxcb = MemoryContextAlloc(objcxt,
							   sizeof(MemoryContextCallback));

	ctxcb->func = context_callback_matrix_free;
	ctxcb->arg = A;
	MemoryContextRegisterResetCallback(objcxt, ctxcb);

	/* Switch back to old context */
	MemoryContextSwitchTo(oldcxt);
	OS_RETURN_MATRIX(A);
}

/* Construct an empty expanded matrix. */
OS_Matrix *
FN(construct_empty_expanded_matrix)(GrB_Index nrows,
									GrB_Index ncols,
									MemoryContext parentcontext)
{
	OS_FlatMatrix  *flat;
	Datum	d;
	flat = construct_empty_flat_matrix(nrows, ncols, GB_TYPE);
	d = FN(expand_flat_matrix)(flat, parentcontext);
	pfree(flat);
	return (OS_Matrix *) DatumGetEOHP(d);
}

Datum
FN(matrix_empty)(PG_FUNCTION_ARGS)
{
	OS_Matrix *retval;
	GrB_Index nrows, ncols;

	nrows = PG_GETARG_INT64(0);
	ncols = PG_GETARG_INT64(1);

	retval = FN(construct_empty_expanded_matrix)(nrows,
												 ncols,
												 CurrentMemoryContext);
	OS_RETURN_MATRIX(retval);
}


Datum
FN(matrix)(PG_FUNCTION_ARGS)
{
	GrB_Info info;
	OS_Matrix *retval;
	ArrayType *rows, *cols, *vals;
	int *idims, *jdims, *vdims;
	int i, count;

	GrB_Index *row_indices, *col_indices;
	GrB_Index max_row_index, max_col_index;
	PG_TYPE *values;
	ArrayIterator array_iterator;
	Datum	value;
	bool isnull;

	if (PG_NARGS() == 2) {
		max_row_index = PG_GETARG_INT64(0);
		max_col_index = PG_GETARG_INT64(1);
		retval = FN(construct_empty_expanded_matrix)(max_row_index,
													 max_col_index,
													 CurrentMemoryContext);
		OS_RETURN_MATRIX(retval);
	}

	rows = PG_GETARG_ARRAYTYPE_P(0);
	cols = PG_GETARG_ARRAYTYPE_P(1);
	vals = PG_GETARG_ARRAYTYPE_P(2);

	if (ARR_HASNULL(rows) || ARR_HASNULL(cols) || ARR_HASNULL(vals))
		ereport(ERROR, (errmsg("Matrix values may not be null")));

	idims = ARR_DIMS(rows);
	jdims = ARR_DIMS(cols);
	vdims =  ARR_DIMS(vals);

	if (!PG_ARGISNULL(3))
		max_row_index = PG_GETARG_INT64(3);
	else
		max_row_index = idims[0];

	if (!PG_ARGISNULL(4))
		max_col_index = PG_GETARG_INT64(4);
	else
		max_col_index = jdims[0];

	count = vdims[0];

	if ((idims[0] != jdims[0]) || (jdims[0] != vdims[0]))
		ereport(ERROR, (errmsg("Row, column, and value arrays must be same size.")));

	row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * max_row_index);
	col_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * max_col_index);
	values = (PG_TYPE*) palloc0(sizeof(PG_TYPE) * count);

	i = 0;
	array_iterator = array_create_iterator(rows, 0, NULL);
	while (array_iterate(array_iterator, &value, &isnull))
	{
		row_indices[i] = DatumGetInt64(value);
		if (row_indices[i] > max_row_index)
			max_row_index = row_indices[i] + 1;
		i++;
	}
	array_free_iterator(array_iterator);

	i = 0;
	array_iterator = array_create_iterator(cols, 0, NULL);
	while (array_iterate(array_iterator, &value, &isnull))
	{
		col_indices[i] = DatumGetInt64(value);
		if (col_indices[i] > max_col_index)
			max_col_index = col_indices[i] + 1;
		i++;
	}
	array_free_iterator(array_iterator);

	i = 0;
	array_iterator = array_create_iterator(vals, 0, NULL);
	while (array_iterate(array_iterator, &value, &isnull))
	{
		values[i] = PG_DGT(value);
		i++;
	}
	array_free_iterator(array_iterator);

	retval = FN(construct_empty_expanded_matrix)(max_row_index,
												 max_col_index,
												 CurrentMemoryContext);

	CHECKD(GrB_Matrix_build(retval->M,
							row_indices,
							col_indices,
							values,
							count,
							GB_DUP), retval->M);

	OS_RETURN_MATRIX(retval);
}

#include "matrix_ops.h"

#undef SUFFIX
#undef PG_TYPE
#undef GB_TYPE
#undef GB_DUP
#undef GB_MUL
#undef GB_ADD
#undef PG_GET
#undef PG_RET
#undef PG_DGT
#undef PG_TGD
#undef PRINT_FMT

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
