#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_dup);
Datum matrix_dup(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *matrix;
	os_Matrix *result;
	GrB_Index nrows, ncols;
    struct timeval start, end;

	OS_START_BENCH();
	ERRORNULL(0);

	matrix = OS_GETARG_MATRIX(0);
	OS_MTYPE(type, matrix);
	OS_MNROWS(nrows, matrix);
	OS_MNCOLS(ncols, matrix);
	result = new_matrix(type, nrows, ncols, CurrentMemoryContext, NULL);

	OS_CHECK(GrB_Matrix_dup(&result->matrix, matrix->matrix),
		  result->matrix,
		  "Error duping matrix.");

	OS_END_BENCH();
	OS_RETURN_MATRIX(result);
}


