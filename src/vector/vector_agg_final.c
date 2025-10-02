#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_agg_final);
Datum
vector_agg_final(PG_FUNCTION_ARGS)
{
	os_Vector  *state = OS_GETARG_VECTOR(0);

	if (state == NULL)
	{
		PG_RETURN_NULL();
	}

	OS_RETURN_VECTOR(state);
}
