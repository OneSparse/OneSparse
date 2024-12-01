#include "../onesparse.h"

static void context_callback_matrix_free(void*);
static Size matrix_get_flat_size(ExpandedObjectHeader *eohptr);

static void flatten_matrix(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size);

static const ExpandedObjectMethods matrix_methods = {
	matrix_get_flat_size,
	flatten_matrix
};

static Size matrix_get_flat_size(ExpandedObjectHeader *eohptr) {
	os_Matrix *matrix;
	void *serialized_data;
	GrB_Index serialized_size;

	LOGF();

	matrix = (os_Matrix*) eohptr;
	Assert(matrix->em_magic == matrix_MAGIC);

	if (matrix->flat_size)
	{
		Assert(matrix->serialized_data != NULL);
		Assert(OS_MATRIX_FLATSIZE() + matrix->serialized_size == matrix->flat_size);
		return matrix->flat_size;
	}

    OS_CHECK(GrB_wait(matrix->matrix, GrB_MATERIALIZE),
		  matrix->matrix,
		  "Error waiting to materialize matrix.");

	OS_CHECK(GxB_Matrix_serialize(
			  &serialized_data,
			  &serialized_size,
			  matrix->matrix, NULL),
		  matrix->matrix,
		  "Error serializing matrix");

	matrix->serialized_data = serialized_data;
	matrix->serialized_size = serialized_size;
	matrix->flat_size = OS_MATRIX_FLATSIZE() + serialized_size;
	return matrix->flat_size;
}

/* Flatten matrix into a pre-allocated result buffer that is
   allocated_size in bytes.	 */
static void flatten_matrix(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size)
{
	os_Matrix *matrix;
	os_FlatMatrix *flat;
	void* data;

	LOGF();

	matrix = (os_Matrix *) eohptr;
	flat = (os_FlatMatrix *) result;

	Assert(matrix->em_magic == matrix_MAGIC);
	Assert(allocated_size == matrix->flat_size);
	Assert(matrix->serialized_data != NULL);
	Assert(matrix->serialized_size > 0);

	memset(flat, 0, allocated_size);

	OS_CHECK(GrB_get(
			  matrix->matrix,
			  &flat->type_code,
			  GrB_EL_TYPE_CODE),
		  matrix->matrix,
		  "Cannot get Matrix Type code.");

	OS_MNROWS(flat->nrows, matrix);
	OS_MNCOLS(flat->ncols, matrix);
	data = OS_MATRIX_DATA(flat);

	memcpy(data, matrix->serialized_data, matrix->serialized_size);
	flat->serialized_size = matrix->serialized_size;

	/* free_function = NULL; */
	/* ERRORIF(GrB_get(GrB_Global, (void*)free_function, GxB_FREE_FUNCTION) != GrB_SUCCESS, */
	/*		"Cannot get SuiteSparse free function."); */

	/* free_function(matrix->serialized_data); */
	SET_VARSIZE(flat, allocated_size);
}

/* Construct an empty expanded matrix. */
os_Matrix* new_matrix(
	GrB_Type type,
	GrB_Index nrows,
	GrB_Index ncols,
	MemoryContext parentcontext,
	GrB_Matrix _matrix)
{
	os_Matrix *matrix;

	MemoryContext objcxt, oldcxt;
	MemoryContextCallback *ctxcb;

	LOGF();
	objcxt = AllocSetContextCreate(parentcontext,
								   "expanded matrix",
								   ALLOCSET_DEFAULT_SIZES);

	matrix = MemoryContextAlloc(objcxt,	sizeof(os_Matrix));

	EOH_init_header(&matrix->hdr, &matrix_methods, objcxt);

	oldcxt = MemoryContextSwitchTo(objcxt);

	matrix->em_magic = matrix_MAGIC;
	matrix->flat_size = 0;
	matrix->serialized_data = NULL;
	matrix->serialized_size = 0;

	if (_matrix == NULL)
	{
		OS_CHECK(GrB_Matrix_new(&matrix->matrix, type, nrows, ncols),
			  matrix->matrix,
			  "Cannot create new Matrix.");
	}
	else
	{
		matrix->matrix = _matrix;
	}

	ctxcb = MemoryContextAlloc(objcxt, sizeof(MemoryContextCallback));

	ctxcb->func = context_callback_matrix_free;
	ctxcb->arg = matrix;

	MemoryContextRegisterResetCallback(objcxt, ctxcb);

	MemoryContextSwitchTo(oldcxt);
	return matrix;
}

/* Expand a flat matrix in to an Expanded one, return as Postgres Datum. */
Datum expand_matrix(os_FlatMatrix *flat, MemoryContext parentcontext)
{
	GrB_Type type;
	os_Matrix *matrix;
	void* data;

	LOGF();

	type = code_type(flat->type_code);
	matrix = new_matrix(type, flat->nrows, flat->ncols, parentcontext, NULL);
	data = OS_MATRIX_DATA(flat);

	OS_CHECK(GxB_Matrix_deserialize(
			  &matrix->matrix,
			  type,
			  data,
			  flat->serialized_size, NULL),
		  matrix->matrix,
		  "Error deserializing matrix");

	OS_RETURN_MATRIX(matrix);
}

static void
context_callback_matrix_free(void* ptr)
{
	os_Matrix *matrix = (os_Matrix *) ptr;
	LOGF();

	OS_CHECK(GrB_Matrix_free(&matrix->matrix),
		  matrix->matrix,
		  "Cannot GrB_Free Matrix");
}

/* Helper function to always expand datum

   This is used by PG_GETARG_MATRIX */
os_Matrix* DatumGetMatrix(Datum datum)
{
	os_Matrix *matrix;
	os_FlatMatrix *flat;
	LOGF();
	if (VARATT_IS_EXTERNAL_EXPANDED_RW(DatumGetPointer(datum))) {
		matrix = MatrixGetEOHP(datum);
		Assert(matrix->em_magic == matrix_MAGIC);
		return matrix;
	}
	flat = OS_DETOAST_MATRIX(datum);
	datum = expand_matrix(flat, CurrentMemoryContext);
	return MatrixGetEOHP(datum);
}


#define SUFFIX _int64                // suffix for names
#define PG_TYPE int64                // postgres type
#define GB_TYPE GrB_INT64            // graphblas matrix type
#define PG_GETARG PG_GETARG_INT64       // how to get value args
#define PG_RETURN PG_RETURN_INT64
#include "matrix_ops.h"

#define SUFFIX _int32                // suffix for names
#define PG_TYPE int32                // postgres type
#define GB_TYPE GrB_INT32            // graphblas matrix type
#define PG_GETARG PG_GETARG_INT32       // how to get value args
#define PG_RETURN PG_RETURN_INT32
#include "matrix_ops.h"

#define SUFFIX _int16                // suffix for names
#define PG_TYPE int16                // postgres type
#define GB_TYPE GrB_INT16            // graphblas matrix type
#define PG_GETARG PG_GETARG_INT16       // how to get value args
#define PG_RETURN PG_RETURN_INT16
#include "matrix_ops.h"

#define SUFFIX _fp64                // suffix for names
#define PG_TYPE float8                // postgres type
#define GB_TYPE GrB_FP64            // graphblas matrix type
#define PG_GETARG PG_GETARG_FLOAT8       // how to get value args
#define PG_RETURN PG_RETURN_FLOAT8
#include "matrix_ops.h"

#define SUFFIX _fp32                // suffix for names
#define PG_TYPE float4                // postgres type
#define GB_TYPE GrB_FP32            // graphblas matrix type
#define PG_GETARG PG_GETARG_FLOAT4       // how to get value args
#define PG_RETURN PG_RETURN_FLOAT4
#include "matrix_ops.h"

#define SUFFIX _bool                // suffix for names
#define PG_TYPE bool                // postgres type
#define GB_TYPE GrB_BOOL            // graphblas matrix type
#define PG_GETARG PG_GETARG_BOOL       // how to get value args
#define PG_RETURN PG_RETURN_BOOL
#include "matrix_ops.h"

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
