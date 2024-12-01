#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_nrows);
Datum matrix_nrows(PG_FUNCTION_ARGS)
{
	GrB_Index result;
	os_Matrix *matrix;

	LOGF();
	ERRORNULL(0);
	matrix = OS_GETARG_MATRIX(0);
	OS_MNROWS(result, matrix);
	PG_RETURN_INT64(result);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
