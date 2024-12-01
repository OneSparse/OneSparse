#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_contains);
Datum matrix_contains(PG_FUNCTION_ARGS)
{
	os_Matrix *matrix;
	GrB_Index i, j;
	GrB_Info info;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	matrix = OS_GETARG_MATRIX(0);
	i = PG_GETARG_INT64(1);
	j = PG_GETARG_INT64(2);

	info = GxB_Matrix_isStoredElement(matrix->matrix, i, j);
	if (info == GrB_SUCCESS)
		PG_RETURN_BOOL(true);
	else if (info == GrB_NO_VALUE)
		PG_RETURN_BOOL(false);
	else
		elog(ERROR, "Error checking stored element.");
	PG_RETURN_BOOL(false);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
