#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_contains);
Datum vector_contains(PG_FUNCTION_ARGS)
{
	os_Vector *vector;
	GrB_Index i;
	GrB_Info info;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	vector = OS_GETARG_VECTOR(0);
	i = PG_GETARG_INT64(1);

	info = GxB_Vector_isStoredElement(vector->vector, i);
	if (info == GrB_SUCCESS)
		PG_RETURN_BOOL(true);
	else if (info == GrB_NO_VALUE)
		PG_RETURN_BOOL(false);
	else
		elog(ERROR, "Error checking stored element.");
	PG_RETURN_BOOL(false);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
