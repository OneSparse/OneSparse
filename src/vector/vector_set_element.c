#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_set_element);
Datum vector_set_element(PG_FUNCTION_ARGS)
{
	os_Vector *vector;
	os_Scalar *scalar;
	GrB_Index i;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	vector = OS_GETARG_VECTOR(0);
	i = PG_GETARG_INT64(1);
	scalar = OS_GETARG_SCALAR(2);

	OS_CHECK(GrB_Vector_setElement(vector->vector, scalar->scalar, i),
		  vector->vector,
		  "Error setting vector element.");

	OS_RETURN_VECTOR(vector);
}

SUPPORT_FN(vector_set_element, linitial);

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
