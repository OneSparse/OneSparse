#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_wait);
Datum vector_wait(PG_FUNCTION_ARGS)
{
	os_Vector *vector;
	int waitmode;

	LOGF();
	ERRORNULL(0);

	vector = OS_GETARG_VECTOR(0);
	waitmode = PG_GETARG_INT32(1);

	OS_CHECK(GrB_Vector_wait(vector->vector, waitmode),
		  vector->vector,
		  "Error waiting for vector.");
	OS_RETURN_VECTOR(vector);
}

SUPPORT_FN(vector_wait, linitial);

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
