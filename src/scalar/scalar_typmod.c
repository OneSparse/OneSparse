#include "../onesparse.h"

PG_FUNCTION_INFO_V1(scalar_typmod_in);
PG_FUNCTION_INFO_V1(scalar_typmod_out);

static int32 scalar_typmodin(ArrayType *arr);
static char *scalar_typmodout(int32 typmod);

static int32 scalar_typmodin(ArrayType *arr)
{
	Datum *elem_values;
    int n;
    int32 result;

	if (ARR_ELEMTYPE(arr) != CSTRINGOID)
		ereport(ERROR,
				(errcode(ERRCODE_ARRAY_ELEMENT_ERROR),
				 errmsg("typmod array must be type cstring[]")));

	if (ARR_NDIM(arr) != 1)
		ereport(ERROR,
				(errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
				 errmsg("typmod array must be one-dimensional")));

	if (array_contains_nulls(arr))
		ereport(ERROR,
				(errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
				 errmsg("typmod array must not contain nulls")));

	deconstruct_array_builtin(arr, CSTRINGOID, &elem_values, NULL, &n);

	if (n > 1)
		ereport(ERROR,
				(errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
				 errmsg("typmod array must contain only one element")));

    result = pg_strtoint32(DatumGetCString(elem_values[0]));

	pfree(elem_values);
    elog(INFO, "typmod in %i", result);

	return result;
}

static char *scalar_typmodout(int32 typmod)
{
    return psprintf("(%d)", (int) typmod);
}

Datum scalar_typmod_in(PG_FUNCTION_ARGS)
{
	ArrayType *ta = PG_GETARG_ARRAYTYPE_P(0);

	PG_RETURN_INT32(scalar_typmodin(ta));
}

Datum scalar_typmod_out(PG_FUNCTION_ARGS)
{
	int32 typmod = PG_GETARG_INT32(0);

	PG_RETURN_CSTRING(scalar_typmodout(typmod));
}
