#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_wait);
Datum matrix_wait(PG_FUNCTION_ARGS)
{
	os_Matrix *A;
	int waitmode;

	LOGF();
	ERRORNULL(0);

	A = OS_GETARG_MATRIX(0);
	waitmode = PG_GETARG_INT32(1);

	OS_CHECK(GrB_Matrix_wait(A->matrix, waitmode),
		  A->matrix,
		  "Error waiting for matrix.");
	OS_RETURN_MATRIX(A);
}


/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
