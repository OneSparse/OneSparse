#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_resize);
Datum matrix_resize(PG_FUNCTION_ARGS)
{
	os_Matrix *A;
	GrB_Index i, j;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	A = OS_GETARG_MATRIX(0);
	i = PG_GETARG_INT64(1);
	j = PG_GETARG_INT64(2);

	if (i == -1)
		i = GxB_INDEX_MAX;

	if (j == -1)
		j = GxB_INDEX_MAX;

	OS_CHECK(GrB_Matrix_resize(A->matrix, i, j),
		  A->matrix,
		  "Error resizing matrix.");
	OS_RETURN_MATRIX(A);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
