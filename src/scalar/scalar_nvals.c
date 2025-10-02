#include "../onesparse.h"

PG_FUNCTION_INFO_V1(scalar_nvals);
Datum
scalar_nvals(PG_FUNCTION_ARGS)
{
	GrB_Index	result;
	os_Scalar  *scalar;

	LOGF();
	ERRORNULL(0);

	scalar = OS_GETARG_SCALAR(0);

	OS_CHECK(GrB_Scalar_nvals(&result, scalar->scalar),
			 scalar->scalar,
			 "Error extracting scalar nvals.");

	PG_RETURN_INT16(result ? 1 : 0);
}
