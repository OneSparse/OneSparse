#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_remove_element);
Datum matrix_remove_element(PG_FUNCTION_ARGS)
{
	os_Matrix *matrix;
	GrB_Index i, j;
    struct timeval start, end;

	OS_START_BENCH();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	matrix = OS_GETARG_MATRIX(0);
	i = PG_GETARG_INT64(1);
	j = PG_GETARG_INT64(2);

	OS_CHECK(GrB_Matrix_removeElement(matrix->matrix, i, j),
		  matrix->matrix,
		  "Error setting matrix element.");

    OS_CHECK(GrB_wait(matrix->matrix, GrB_MATERIALIZE),
		  matrix->matrix,
		  "Error waiting to materialize matrix.");

	OS_END_BENCH();
	OS_RETURN_MATRIX(matrix);
}

SUPPORT_FN(matrix_remove_element, linitial);

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
