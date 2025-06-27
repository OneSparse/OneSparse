#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_set_element);
Datum matrix_set_element(PG_FUNCTION_ARGS)
{
	os_Matrix *matrix;
	os_Scalar *scalar;
	GrB_Index i, j;
    struct timeval start, end;

	OS_START_BENCH();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);
	ERRORNULL(3);

	matrix = OS_GETARG_MATRIX(0);
	i = PG_GETARG_INT64(1);
	j = PG_GETARG_INT64(2);
	scalar = OS_GETARG_SCALAR(3);

	OS_CHECK(GrB_Matrix_setElement(matrix->matrix, scalar->scalar, i, j),
		  matrix->matrix,
		  "Error setting matrix element.");

	OS_END_BENCH();
	OS_RETURN_MATRIX(matrix);
}

SUPPORT_FN(matrix_set_element, linitial);

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
