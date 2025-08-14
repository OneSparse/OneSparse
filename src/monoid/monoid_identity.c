#include "../onesparse.h"

PG_FUNCTION_INFO_V1(monoid_identity);

Datum monoid_identity(PG_FUNCTION_ARGS)
{
	os_Monoid *monoid;
	os_Scalar *scalar;
	monoid = OS_GETARG_MONOID(0);

	OS_CHECK(GrB_get(monoid->monoid,
                     scalar->scalar,
                     GxB_MONOID_IDENTITY),
             monoid->monoid,
             "Error extracting scalar nvals.");
	OS_RETURN_SCALAR(scalar);
}
