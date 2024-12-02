#include "../onesparse.h"

PG_FUNCTION_INFO_V1(scalar_dup);
Datum scalar_dup(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Scalar *scalar;
	os_Scalar *result;

	LOGF();
	ERRORNULL(0);

	scalar = OS_GETARG_SCALAR(0);
	OS_CHECK(GxB_Scalar_type(&type, scalar->scalar),
		  scalar->scalar,
		  "Cannot get scalar type");

	result = new_scalar(type, CurrentMemoryContext, NULL);

	OS_CHECK(GrB_Scalar_dup(&result->scalar, scalar->scalar),
		  scalar->scalar,
		  "Error duping scalar.");
	OS_RETURN_SCALAR(result);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
