#include "../onesparse.h"

PG_FUNCTION_INFO_V1(scalar_wait);
Datum scalar_wait(PG_FUNCTION_ARGS)
{
	os_Scalar *scalar;
	int waitmode;

	LOGF();
	ERRORNULL(0);

	scalar = OS_GETARG_SCALAR(0);
	waitmode = PG_GETARG_INT32(1);

	OS_CHECK(GrB_Scalar_wait(scalar->scalar, waitmode),
		  scalar->scalar,
		  "Error waiting for scalar.");
	OS_RETURN_SCALAR(scalar);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
