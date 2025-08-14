#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_random);
Datum vector_random(PG_FUNCTION_ARGS)
{
	os_Type *type;
	GrB_Index size;
	double density;
	uint64_t seed;
	os_Vector *v;
	GrB_Matrix result;
	GrB_Vector col;
	char msg [LAGRAPH_MSG_LEN];
    struct timeval start, end;

	type = OS_GETARG_TYPE(0);
	size = PG_GETARG_INT64(1);
	density = PG_GETARG_FLOAT8(2);

	if (PG_ARGISNULL(3))
	{
		if (!pg_strong_random(&seed, sizeof(seed)))
		{
			ereport(ERROR, (errmsg("could not generate random seed")));
		}
	}
	else
	{
		seed = PG_GETARG_INT64(3);
	}
	if (size == -1)
	{
		size = GrB_INDEX_MAX+1;
	}

	v = new_vector(type->type, size, CurrentMemoryContext, NULL);

	OS_START_BENCH();
	LA_CHECK(LAGraph_Random_Matrix(
				 &result,
				 type->type,
				 size,
				 1,
				 density,
				 seed,
				 msg
				 ));

	OS_CHECK(GrB_Col_extract(
				 v->vector,
				 NULL,
				 NULL,
				 result,
				 GrB_ALL,
				 0,
				 0,
				 NULL
				 ),
			 col,
			 "Error extracting random vector");

	OS_END_BENCH();

	OS_RETURN_VECTOR(v);
}

