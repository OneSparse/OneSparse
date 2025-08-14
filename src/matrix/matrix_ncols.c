#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_ncols);
Datum matrix_ncols(PG_FUNCTION_ARGS)
{
	GrB_Index result;
	os_Matrix *matrix;
    struct timeval start, end;

	OS_START_BENCH();
	ERRORNULL(0);
	matrix = OS_GETARG_MATRIX(0);
	OS_MNCOLS(result, matrix);

	OS_END_BENCH();
	PG_RETURN_INT64(result);
}


