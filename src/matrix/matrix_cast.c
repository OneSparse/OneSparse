#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_cast);
Datum
matrix_cast(PG_FUNCTION_ARGS)
{
	os_Type    *type;

	GrB_Type _type;
	os_Matrix  *A,
			   *C;
	GrB_UnaryOp op;
	GrB_Index	nrows,
				ncols;
	struct timeval start,
				end;

	OS_START_BENCH();
	ERRORNULL(0);
	ERRORNULL(1);

	A = OS_GETARG_MATRIX(0);
	type = OS_GETARG_TYPE(1);

	OS_MNROWS(nrows, A);
	OS_MNCOLS(ncols, A);
	C = new_matrix(type->type, nrows, ncols, CurrentMemoryContext, NULL);

	op = NULL;
	_type = type->type;
	if (_type == GrB_BOOL)
		op = GrB_IDENTITY_BOOL;
	else if (_type == GrB_INT8)
		op = GrB_IDENTITY_INT8;
	else if (_type == GrB_INT16)
		op = GrB_IDENTITY_INT16;
	else if (_type == GrB_INT32)
		op = GrB_IDENTITY_INT32;
	else if (_type == GrB_INT64)
		op = GrB_IDENTITY_INT64;
	else if (_type == GrB_UINT8)
		op = GrB_IDENTITY_UINT8;
	else if (_type == GrB_UINT16)
		op = GrB_IDENTITY_UINT16;
	else if (_type == GrB_UINT32)
		op = GrB_IDENTITY_UINT32;
	else if (_type == GrB_UINT64)
		op = GrB_IDENTITY_UINT64;
	else if (_type == GrB_FP32)
		op = GrB_IDENTITY_FP32;
	else if (_type == GrB_FP64)
		op = GrB_IDENTITY_FP64;
	else
		ereport(ERROR, (errmsg("Unsupported type cast")));

	OS_CHECK(GrB_apply(C->matrix,
					   NULL,
					   NULL,
					   op,
					   A->matrix,
					   NULL),
			 C->matrix,
			 "Error in GrB_matrix_apply");

	OS_END_BENCH();
	OS_RETURN_MATRIX(C);
}
