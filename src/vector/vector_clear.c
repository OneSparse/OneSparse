#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_clear);
Datum vector_clear(PG_FUNCTION_ARGS)
{
	os_Vector *vector;

	LOGF();
	ERRORNULL(0);

	vector = OS_GETARG_VECTOR(0);

	OS_CHECK(GrB_Vector_clear(vector->vector),
		  vector->vector,
		  "Error clearing vector.");
	OS_RETURN_VECTOR(vector);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
