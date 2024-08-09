/* This is a "header template" see vector.c for specific instanciations */

typedef struct FN(OS_Vector_ExtractState)
{
	OS_Vector *vec;
	GrB_Index *indices;
	PG_TYPE *vals;
} FN(OS_Vector_ExtractState);

/* Expanded Object Header "methods" for flattening vectors for storage */
static Size FN(vector_get_flat_size)(ExpandedObjectHeader *eohptr);
static void FN(vector_flatten_into)(ExpandedObjectHeader *eohptr,
									void *result, Size allocated_size);

static const ExpandedObjectMethods FN(vector_methods) =
{
	FN(vector_get_flat_size),
	FN(vector_flatten_into)
};

/* Utility function that expands a flattened vector datum. */
Datum
FN(expand_flat_vector)(OS_FlatVector *flat,
					   MemoryContext parentcontext);

/* Helper function that creates an expanded empty vector. */
OS_Vector *
FN(construct_empty_expanded_vector)(GrB_Index size,
									MemoryContext parentcontext);


/* Compute size of storage needed for vector */
static Size
FN(vector_get_flat_size)(ExpandedObjectHeader *eohptr)
{
	GrB_Info info;
	OS_Vector *A = (OS_Vector *) eohptr;
	GrB_Index size, nvals;

	Assert(A->ev_magic == vector_MAGIC);

	if (A->flat_size)
		return A->flat_size;

	CHECKD(GrB_Vector_size(&size, A->V), A->V);
	CHECKD(GrB_Vector_nvals(&nvals, A->V), A->V);

	A->flat_size = OS_VECTOR_OVERHEAD();
	return A->flat_size;
}

/* Flatten vector into allocated result buffer  */
static void
FN(vector_flatten_into)(ExpandedObjectHeader *eohptr,
						void *result, Size allocated_size)
{
	GrB_Info info;
	GrB_Index size, nvals;
	OS_Vector *A = (OS_Vector *) eohptr;
	OS_FlatVector *flat = (OS_FlatVector *) result;

	Assert(A->ev_magic == vector_MAGIC);
	Assert(allocated_size == A->flat_size);

	memset(flat, 0, allocated_size);

	CHECKV(GrB_Vector_nvals(&nvals, A->V), A->V);
	CHECKV(GrB_Vector_size(&size, A->V), A->V);

	flat->size = size;
	flat->nvals = nvals;
	flat->type = GB_TYPE;

	SET_VARSIZE(flat, allocated_size);
}

/* Expand a flat vector. */
Datum
FN(expand_flat_vector)(OS_FlatVector *flat,
					   MemoryContext parentcontext)
{
	GrB_Info info;

	OS_Vector *A;

	MemoryContext objcxt, oldcxt;
	MemoryContextCallback *ctxcb;

	GrB_Index size, nvals;
	GrB_Type type;

	/* Create a new context that will hold the expanded object. */
	objcxt = AllocSetContextCreate(
		parentcontext,
		"expanded vector",
		ALLOCSET_DEFAULT_SIZES);

	/* Allocate a new expanded vector */
	A = (OS_Vector*)MemoryContextAlloc(
		objcxt,
		sizeof(OS_Vector));

	/* Initialize the ExpandedObjectHeader member with flattening
	 * methods and new object context */
	EOH_init_header(&A->hdr, &FN(vector_methods), objcxt);

	/* Used for debugging checks */
	A->ev_magic = vector_MAGIC;

	/* Switch to new object context */
	oldcxt = MemoryContextSwitchTo(objcxt);

	/* Get dimensional information from flat */
	size = flat->size;
	nvals = flat->nvals;
	type = flat->type;

	/* Create a context callback to free vector when context is cleared */
	ctxcb = (MemoryContextCallback*)MemoryContextAlloc(
		objcxt,
		sizeof(MemoryContextCallback));

	ctxcb->func = context_callback_vector_free;
	ctxcb->arg = A;
	MemoryContextRegisterResetCallback(objcxt, ctxcb);

	A->type = type;
	A->flat_size = 0;

	/* Switch back to old context */
	MemoryContextSwitchTo(oldcxt);
	OS_RETURN_VECTOR(A);
}

/* Construct an empty expanded vector. */
OS_Vector *
FN(construct_empty_expanded_vector)(GrB_Index size,
									MemoryContext parentcontext)
{
	OS_FlatVector  *flat;
	Datum	d;
	flat = construct_empty_flat_vector(size, GB_TYPE);
	d = FN(expand_flat_vector)(flat, parentcontext);
	pfree(flat);
	return (OS_Vector *) DatumGetEOHP(d);
}

Datum
FN(vector_empty)(PG_FUNCTION_ARGS)
{
	OS_Vector *retval;
	GrB_Index max_index;
	max_index = PG_GETARG_INT64(0);
	retval = FN(construct_empty_expanded_vector)(max_index, CurrentMemoryContext);
	OS_RETURN_VECTOR(retval);
}

/* cast implementation function. */
Datum
FN(vector)(PG_FUNCTION_ARGS)
{
	GrB_Info info;
	OS_Vector *retval;
	ArrayType *indexes, *vals;
	int *idims, *vdims;
	int i;
	GrB_Index vcount, size;

	GrB_Index *row_indices;
	PG_TYPE *values;
	ArrayIterator array_iterator;
	Datum	value;
	bool isnull;

	if (PG_NARGS() == 1)
	{
		indexes = NULL;
		vals = PG_GETARG_ARRAYTYPE_P(0);
		if (ARR_HASNULL(vals))
			ereport(ERROR, (errmsg("Value array may not contain NULLs")));
		vdims = ARR_DIMS(vals);
	}
	else
	{
		indexes = PG_GETARG_ARRAYTYPE_P(0);
		vals = PG_GETARG_ARRAYTYPE_P(1);
		if (ARR_HASNULL(vals) || ARR_HASNULL(indexes))
			ereport(ERROR, (errmsg("Neither value or indexc arrays may not contain NULLs")));

		idims = ARR_DIMS(indexes);
		vdims = ARR_DIMS(vals);

		if (idims[0] != vdims[0])
			ereport(ERROR, (errmsg("Index and Value arrays must be same size.")));
	}

	size = vcount = vdims[0];

	if (PG_NARGS() > 2 && !PG_ARGISNULL(2))
		size = PG_GETARG_INT64(2);

	row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * vcount);
	values = (PG_TYPE*) palloc0(sizeof(PG_TYPE) * vcount);

	i = 0;
	array_iterator = array_create_iterator(vals, 0, NULL);
	while (array_iterate(array_iterator, &value, &isnull))
	{
		if (indexes == NULL)
			row_indices[i] = i;
		values[i] = PG_DGT(value);
		i++;
	}

	i = 0;
	if (indexes != NULL)
	{
		array_iterator = array_create_iterator(indexes, 0, NULL);
		while (array_iterate(array_iterator, &value, &isnull))
		{
			row_indices[i] = DatumGetInt64(value);
			if (row_indices[i] > size)
				size = row_indices[i] + 1;
			i++;
		}
	}

	retval = FN(construct_empty_expanded_vector)(size, CurrentMemoryContext);

	CHECKD(GrB_Vector_build(retval->V,
							row_indices,
							values,
							vcount,
							GB_DUP), retval->V);

	OS_RETURN_VECTOR(retval);
}

#include "vector_ops.h"

#undef SUFFIX
#undef PG_TYPE
#undef GB_TYPE
#undef GB_DUP
#undef GB_MUL
#undef GB_ADD
#undef GB_EQ
#undef GB_NE
#undef GB_RNG
#undef PG_GET
#undef PG_RET
#undef PG_DGT
#undef PG_TGD
#undef PRINT_FMT

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
