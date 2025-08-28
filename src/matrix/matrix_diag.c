#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_diag);
Datum matrix_diag(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Vector *a;
    GrB_Matrix c;
	os_Matrix *result;
    GrB_Index nrows, ncols;
    struct timeval start, end;

	OS_START_BENCH();
	ERRORNULL(0);
	a = OS_GETARG_VECTOR(0);

 	OS_CHECK(GrB_Matrix_diag(&c, a->vector, 0),
			 a->vector,
			 "Error matrix_diag.");

	OS_CHECK(GrB_Matrix_nrows(&nrows, c), c,
		  "Error extracting matrix nrows.");

	OS_CHECK(GrB_Matrix_ncols(&ncols, c), c,
		  "Error extracting matrix ncols.");

    OS_VTYPE(type, a);
    result = new_matrix(type, nrows, ncols, CurrentMemoryContext, c);
	OS_END_BENCH();
	OS_RETURN_MATRIX(result);
}

