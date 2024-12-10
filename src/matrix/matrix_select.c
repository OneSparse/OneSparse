#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_select);
Datum matrix_select(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *A, *C, *mask;
	os_Scalar *y;
	os_Descriptor *descriptor;
	os_BinaryOp *accum;
	os_IndexUnaryOp *op;
	GrB_Index nrows, ncols;
	int nargs;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	nargs = PG_NARGS();
	A = OS_GETARG_MATRIX(0);
	op = OS_GETARG_INDEXUNARYOP(1);
	y = OS_GETARG_SCALAR(2);

	if (PG_ARGISNULL(3))
	{
		OS_MTYPE(type, A);
		OS_MNROWS(nrows, A);
		OS_MNCOLS(ncols, A);
		C = new_matrix(type, nrows, ncols, CurrentMemoryContext, NULL);
	}
	else
		C = OS_GETARG_MATRIX(3);

	mask = OS_GETARG_MATRIX_OR_NULL(nargs, 4);
	accum = OS_GETARG_BINARYOP_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_OR_NULL(nargs, 6);

	OS_CHECK(GrB_select(C->matrix,
					 mask ? mask->matrix : NULL,
					 accum ? accum->binaryop : NULL,
					 op->indexunaryop,
					 A->matrix,
					 y ? y->scalar : NULL,
					 descriptor ? descriptor->descriptor : NULL),
		  C->matrix,
		  "Error in GrB_select");

	OS_RETURN_MATRIX(C);
}

SUPPORT_FN(matrix_select, lfourth);

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
