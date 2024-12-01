#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_ncols);
Datum matrix_ncols(PG_FUNCTION_ARGS)
{
	GrB_Index result;
	os_Matrix *matrix;

	LOGF();
	ERRORNULL(0);
	matrix = OS_GETARG_MATRIX(0);
	OS_MNCOLS(result, matrix);
	PG_RETURN_INT64(result);
}


/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
