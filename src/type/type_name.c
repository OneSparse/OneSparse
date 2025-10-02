#include "../onesparse.h"

PG_FUNCTION_INFO_V1(type_name);

Datum
type_name(PG_FUNCTION_ARGS)
{
	os_Type    *type;

	type = OS_GETARG_TYPE(0);
	PG_RETURN_TEXT_P(cstring_to_text(type->name));
}
