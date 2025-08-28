#include "../onesparse.h"

PG_FUNCTION_INFO_V1(scalar_clear);
Datum scalar_clear(PG_FUNCTION_ARGS)
{
	os_Scalar *scalar;

	LOGF();
	ERRORNULL(0);

	scalar = OS_GETARG_SCALAR(0);

	OS_CHECK(GrB_Scalar_clear(scalar->scalar),
		  scalar->scalar,
		  "Error clearing scalar.");
	OS_RETURN_SCALAR(scalar);
}

SUPPORT_FN(scalar_clear, linitial);

