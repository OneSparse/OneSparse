#include "../onesparse.h"

PG_FUNCTION_INFO_V1(monoid_terminal);

Datum monoid_terminal(PG_FUNCTION_ARGS)
{
	os_Monoid *monoid;
	os_Scalar *scalar;
	GrB_Type_Code code;
	GrB_Type type;
	GrB_Scalar tmp;

	monoid = OS_GETARG_MONOID(0);

	OS_CHECK(GrB_get(monoid->monoid,
					 (int32_t*)&code,
					 GrB_OUTP_TYPE_CODE),
			 monoid->monoid,
			 "Error getting monoid type code");

	type = code_type(code);

	OS_CHECK(GrB_Scalar_new(&tmp, type),
			 tmp,
			 "Error creating new monoid terminal scalar.");

	OS_CHECK(GrB_get(monoid->monoid,
                     tmp,
                     GxB_MONOID_TERMINAL),
             monoid->monoid,
             "Error extracting monoid terminal.");

	scalar = new_scalar(type, CurrentMemoryContext, tmp);

	OS_RETURN_SCALAR(scalar);
}
