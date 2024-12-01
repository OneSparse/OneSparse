#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_apply);
Datum matrix_apply(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *A, *C, *mask;
	os_Descriptor *descriptor;
	os_BinaryOp *accum;
	os_UnaryOp *op;
	GrB_Index nrows, ncols;
	int nargs;

	LOGF();
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
		C = OS_GETARG_MATRIX(2);

	mask = OS_GETARG_MATRIX_OR_NULL(nargs, 3);
	accum = OS_GETARG_BINARYOP_OR_NULL(nargs, 4);
	descriptor = OS_GETARG_DESCRIPTOR_OR_NULL(nargs, 5);

	OS_CHECK(GrB_apply(C->matrix,
					mask ? mask->matrix : NULL,
					accum ? accum->binaryop : NULL,
					op->unaryop,
					A->matrix,
					descriptor ? descriptor->descriptor : NULL),
		  C->matrix,
		  "Error in GrB_Matrix_apply");

	OS_RETURN_MATRIX(C);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
