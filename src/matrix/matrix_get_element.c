#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_get_element);
Datum matrix_get_element(PG_FUNCTION_ARGS)
{
	os_Matrix *matrix;
	os_Scalar *scalar;
	GrB_Index i, j;
	GrB_Type type;
    struct timeval start, end;

	OS_START_BENCH();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	matrix = OS_GETARG_MATRIX(0);
	i = PG_GETARG_INT64(1);
	j = PG_GETARG_INT64(2);

	OS_MTYPE(type, matrix);
	scalar = new_scalar(type, CurrentMemoryContext, NULL);

	OS_CHECK(GrB_Matrix_extractElement(scalar->scalar, matrix->matrix, i, j),
		  matrix->matrix,
		  "Error extracting setting matrix element.");

	OS_END_BENCH();
	OS_RETURN_SCALAR(scalar);
}

