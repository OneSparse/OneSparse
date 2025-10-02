#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_remove_element);
Datum
vector_remove_element(PG_FUNCTION_ARGS)
{
	os_Vector  *vector;
	GrB_Index	i;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	vector = OS_GETARG_VECTOR(0);
	i = PG_GETARG_INT64(1);

	OS_CHECK(GrB_Vector_removeElement(vector->vector, i),
			 vector->vector,
			 "Error setting vector element.");

	OS_CHECK(GrB_wait(vector->vector, GrB_MATERIALIZE),
			 vector->vector,
			 "Error waiting to materialize vector.");

	OS_RETURN_VECTOR(vector);
}

SUPPORT_FN(vector_remove_element, linitial);
