#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_random);
Datum
matrix_random(PG_FUNCTION_ARGS)
{
	os_Type    *type;
	GrB_Index	nrows,
				ncols;
	double		density;
	uint64_t	seed;
	os_Matrix  *A;
	GrB_Matrix	result;
	char		msg[LAGRAPH_MSG_LEN];
	struct timeval start,
				end;

	type = OS_GETARG_TYPE(0);
	nrows = PG_GETARG_INT64(1);
	ncols = PG_GETARG_INT64(2);
	density = PG_GETARG_FLOAT8(3);

	if (PG_ARGISNULL(4))
	{
		if (!pg_strong_random(&seed, sizeof(seed)))
		{
			ereport(ERROR, (errmsg("could not generate random seed")));
		}
	}
	else
	{
		seed = PG_GETARG_INT64(4);
	}
	if (nrows == -1)
	{
		nrows = GrB_INDEX_MAX + 1;
	}
	if (ncols == -1)
	{
		ncols = GrB_INDEX_MAX + 1;
	}

	OS_START_BENCH();
	LA_CHECK(LAGraph_Random_Matrix(
								   &result,
								   type->type,
								   nrows,
								   ncols,
								   density,
								   seed,
								   msg
								   ));
	OS_END_BENCH();

	A = new_matrix(NULL, nrows, ncols, CurrentMemoryContext, result);
	OS_RETURN_MATRIX(A);
}
