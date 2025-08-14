#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_norm);
Datum vector_norm(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Vector *u, *w, *w2;
	GrB_Index usize;
    GrB_Vector scale;
	float sumsq, norm, inv;
    struct timeval start, end;

	ERRORNULL(0);

	u = OS_GETARG_VECTOR(0);
	OS_VTYPE(type, u);
	OS_VSIZE(usize, u);

	if (PG_ARGISNULL(1))
	{
		w = new_vector(type, usize, CurrentMemoryContext, NULL);
	}
	else
	{
		w = OS_GETARG_VECTOR_A(1, u);
	}

	OS_START_BENCH();

	w2 = new_vector(type, usize, CurrentMemoryContext, NULL);

	OS_CHECK(GrB_eWiseMult(
				 w2->vector,
				 NULL,
				 NULL,
				 GrB_TIMES_FP32,
				 u->vector,
				 u->vector,
				 NULL),
			 w->vector,
			 "Error squaring vector norm");

    OS_CHECK(GrB_reduce(
				 &sumsq,
				 NULL,
				 GrB_PLUS_MONOID_FP32,
				 w2->vector,
				 NULL),
			 w2->vector,
			 "Cannot reduce sum of squares in norm");

	norm = sqrtf(sumsq);

    OS_CHECK(GrB_apply(
				 w->vector,
				 NULL,
				 NULL,
				 GrB_DIV_FP32,
				 u->vector,
				 norm,
				 NULL),
			 w2->vector,
			 "Cannot scale norm vector");

	OS_END_BENCH();

	OS_RETURN_VECTOR(w);
}

