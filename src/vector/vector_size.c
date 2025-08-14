#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_size);
Datum vector_size(PG_FUNCTION_ARGS)
{
	GrB_Index result;
	os_Vector *vector;

	LOGF();
	ERRORNULL(0);
	vector = OS_GETARG_VECTOR(0);
	OS_VSIZE(result, vector);
	PG_RETURN_INT64(result);
}

