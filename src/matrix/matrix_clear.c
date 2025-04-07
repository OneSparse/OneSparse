#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_clear);
Datum matrix_clear(PG_FUNCTION_ARGS)
{
	os_Matrix *A;

	LOGF();
	ERRORNULL(0);

	A = OS_GETARG_MATRIX(0);

	OS_CHECK(GrB_Matrix_clear(A->matrix),
			 A->matrix,
			 "Error clearing matrix.");
	OS_RETURN_MATRIX(A);
}

SUPPORT_FN(matrix_clear, linitial);

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
