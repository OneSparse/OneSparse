#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_new);
Datum
matrix_new(PG_FUNCTION_ARGS)
{
	os_Type    *type;
	GrB_Index	nrows,
				ncols;
	os_Matrix  *A;

	type = OS_GETARG_TYPE(0);
	nrows = PG_GETARG_INT64(1);
	ncols = PG_GETARG_INT64(2);
	if (nrows == -1)
	{
		nrows = GrB_INDEX_MAX + 1;
	}
	if (ncols == -1)
		ncols = GrB_INDEX_MAX + 1;
	A = new_matrix(type->type, nrows, ncols, CurrentMemoryContext, NULL);
	OS_RETURN_MATRIX(A);
}
