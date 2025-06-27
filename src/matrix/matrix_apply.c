#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_apply);
Datum matrix_apply(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *A, *C;
	os_UnaryOp *op;
	GrB_Matrix mask;
	GrB_Descriptor descriptor;
	GrB_BinaryOp accum;
	GrB_Index nrows, ncols;
	int nargs;
    struct timeval start, end;

	OS_START_BENCH();
	ERRORNULL(0);
	ERRORNULL(1);

	nargs = PG_NARGS();
	A = OS_GETARG_MATRIX(0);
	op = OS_GETARG_UNARYOP(1);

	if (PG_ARGISNULL(2))
	{
		OS_MTYPE(type, A);
		OS_MNROWS(nrows, A);
		OS_MNCOLS(ncols, A);
		C = new_matrix(type, nrows, ncols, CurrentMemoryContext, NULL);
	}
	else
		C = OS_GETARG_MATRIX_A(2, A);

	mask = OS_GETARG_MATRIX_HANDLE_OR_NULL_AB(nargs, 3, A, C);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 4);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 5);

	OS_CHECK(GrB_apply(C->matrix,
					   mask,
					   accum,
					   op->unaryop,
					   A->matrix,
					   descriptor),
		  C->matrix,
		  "Error in GrB_Matrix_apply");

	OS_END_BENCH();
	OS_RETURN_MATRIX(C);
}

SUPPORT_FN(matrix_apply, lthird);

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
