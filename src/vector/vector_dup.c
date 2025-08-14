#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_dup);
Datum vector_dup(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Vector *vector;
	os_Vector *result;
	GrB_Index size;

	LOGF();
	ERRORNULL(0);

	vector = OS_GETARG_VECTOR(0);
	OS_VTYPE(type, vector);
	OS_VSIZE(size, vector);
	result = new_vector(type, size, CurrentMemoryContext, NULL);

	OS_CHECK(GrB_Vector_dup(&result->vector, vector->vector),
		  vector->vector,
		  "Error duping vector.");
	OS_RETURN_VECTOR(result);
}

