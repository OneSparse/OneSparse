#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_norm);
Datum matrix_norm(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *u, *w;
	GrB_Index nrows, ncols;
    os_Matrix *A2, *scale;
	os_Vector *row_sums;
    struct timeval start, end;

	ERRORNULL(0);

	u = OS_GETARG_MATRIX(0);

	OS_MTYPE(type, u);
	OS_MNROWS(nrows, u);
	OS_MNCOLS(ncols, u);

	if (PG_ARGISNULL(1))
	{
		w = new_matrix(type, nrows, ncols, CurrentMemoryContext, NULL);
	}
	else
	{
		w = OS_GETARG_MATRIX_A(1, u);
	}

	OS_START_BENCH();

	A2 = new_matrix(type, nrows, ncols, CurrentMemoryContext, NULL);

	OS_CHECK(GrB_eWiseMult(
				 A2->matrix,
				 NULL,
				 NULL,
				 GrB_TIMES_FP32,
				 u->matrix,
				 u->matrix,
				 NULL),
			 A2->matrix,
			 "Error squaring matrix norm");

	row_sums = new_vector(type, nrows, CurrentMemoryContext, NULL);

	OS_CHECK(GrB_reduce(
				 row_sums->vector,
				 NULL,
				 NULL,
				 GrB_PLUS_MONOID_FP32,
				 A2->matrix,
				 NULL),
			 row_sums->vector,
			 "Cannot reduce row sum vector for matrix norm");

	OS_CHECK(GrB_apply(
				 row_sums->vector,
				 NULL,
				 NULL,
				 GxB_SQRT_FP32,
				 row_sums->vector,
				 NULL),
			 row_sums->vector,
			 "Cannot take sqrt of sum vector for matrix norm");

	OS_CHECK(GrB_apply(
				 row_sums->vector,
				 NULL,
				 NULL,
				 GrB_MINV_FP32,
				 row_sums->vector,
				 NULL),
			 row_sums->vector,
			 "Cannot take multiplicative inverse for matrix norm");

	scale = new_matrix(type, nrows, nrows, CurrentMemoryContext, NULL);

	OS_CHECK(GxB_Matrix_diag(
				 scale->matrix,
				 row_sums->vector,
				 0,
				 NULL),
			 scale->matrix,
			 "Cannot create diagonal scale matrix for matrix norm");

	OS_CHECK(GrB_mxm(
				 w->matrix,
				 NULL,
				 NULL,
				 GrB_PLUS_TIMES_SEMIRING_FP32,
				 scale->matrix,
				 u->matrix,
				 NULL),
			 w->matrix,
			 "Cannot scale matrix for matrix norm");

	OS_END_BENCH();

	OS_RETURN_MATRIX(w);
}
