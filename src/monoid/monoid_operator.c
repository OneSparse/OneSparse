#include "../onesparse.h"

PG_FUNCTION_INFO_V1(monoid_operator);

Datum monoid_operator(PG_FUNCTION_ARGS)
{
	os_Monoid *monoid;
	os_BinaryOp *binop;
	GrB_BinaryOp tmp;

	monoid = OS_GETARG_MONOID(0);

	OS_CHECK(GrB_get(monoid->monoid,
                     (void*)&tmp,
                     GxB_MONOID_OPERATOR),
             monoid->monoid,
             "Error extracting monoid operator.");

	binop = new_binaryop(NULL, CurrentMemoryContext, tmp);

	OS_RETURN_BINARYOP(binop);
}
