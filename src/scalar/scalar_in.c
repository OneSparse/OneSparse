#include "../onesparse.h"

Datum _scalar_in(char *input);
PG_FUNCTION_INFO_V1(scalar_in);

Datum scalar_in(PG_FUNCTION_ARGS)
{
	char *input;
	input = PG_GETARG_CSTRING(0);
	return _scalar_in(input);
}

Datum _scalar_in(char *input)
{
	os_Scalar *scalar;
	size_t len;
	GrB_Type typ;
	char str_val[GxB_MAX_NAME_LEN];
	char str_type[GxB_MAX_NAME_LEN];
	char *fmt = "";

	len = strlen(input);

	sscanf(input, "%[^:]:%s", str_type, str_val);

	if (strcmp(str_type, "int64") == 0)
	{
		int64_t value;
		typ = GrB_INT64;
		fmt = "%ld";
		scalar = new_scalar(typ, CurrentMemoryContext, NULL);
		if (len)
		{
			if (sscanf(str_val, fmt, &value) == 1)
			{
				OS_CHECK(GrB_Scalar_setElement(scalar->scalar, value),
					  scalar->scalar,
					  "Cannot set scalar element in expand.");
			}
			else
				elog(ERROR, "Invalid format for %s %s", fmt, str_val);
		}
	}
	else if (strcmp(str_type, "uint64") == 0)
	{
		uint64_t value;
		typ = GrB_UINT64;
		fmt = "%llu";
		scalar = new_scalar(typ, CurrentMemoryContext, NULL);
		if (len)
		{
			if (sscanf(str_val, fmt, &value) == 1)
			{
				OS_CHECK(GrB_Scalar_setElement(scalar->scalar, value),
					  scalar->scalar,
					  "Cannot set scalar element in expand.");
			}
			else
				elog(ERROR, "Invalid format for %s %s", fmt, str_val);
		}
	}
	else if (strcmp(str_type, "int32") == 0)
	{
		int32_t value;
		typ = GrB_INT32;
		fmt = "%i";
		scalar = new_scalar(typ, CurrentMemoryContext, NULL);
		if (len)
		{
			if (sscanf(str_val, fmt, &value) == 1)
			{
				OS_CHECK(GrB_Scalar_setElement(scalar->scalar, value),
					  scalar->scalar,
					  "Cannot set scalar element in expand.");
			}
			else
				elog(ERROR, "Invalid format for %s %s", fmt, str_val);
		}
	}
	else if (strcmp(str_type, "uint32") == 0)
	{
		uint32_t value;
		typ = GrB_UINT32;
		fmt = "%u";
		scalar = new_scalar(typ, CurrentMemoryContext, NULL);
		if (len)
		{
			if (sscanf(str_val, fmt, &value) == 1)
			{
				OS_CHECK(GrB_Scalar_setElement(scalar->scalar, value),
					  scalar->scalar,
					  "Cannot set scalar element in expand.");
			}
			else
				elog(ERROR, "Invalid format for %s %s", fmt, str_val);
		}
	}
	else if (strcmp(str_type, "int16") == 0)
	{
		int16_t value;
		typ = GrB_INT16;
		fmt = "%i";
		scalar = new_scalar(typ, CurrentMemoryContext, NULL);
		if (len)
		{
			if (sscanf(str_val, fmt, &value) == 1)
			{
				OS_CHECK(GrB_Scalar_setElement(scalar->scalar, value),
					  scalar->scalar,
					  "Cannot set scalar element in expand.");
			}
			else
				elog(ERROR, "Invalid format for %s %s", fmt, str_val);
		}
	}
	else if (strcmp(str_type, "uint16") == 0)
	{
		uint16_t value;
		typ = GrB_UINT16;
		fmt = "%hu";
		scalar = new_scalar(typ, CurrentMemoryContext, NULL);
		if (len)
		{
			if (sscanf(str_val, fmt, &value) == 1)
			{
				OS_CHECK(GrB_Scalar_setElement(scalar->scalar, value),
					  scalar->scalar,
					  "Cannot set scalar element in expand.");
			}
			else
				elog(ERROR, "Invalid format for %s %s", fmt, str_val);
		}
	}
	else if (strcmp(str_type, "fp64") == 0)
	{
		double value;
		typ = GrB_FP64;
		fmt = "%lf";
		scalar = new_scalar(typ, CurrentMemoryContext, NULL);
		if (len)
		{
			if (sscanf(str_val, fmt, &value) == 1)
			{
				OS_CHECK(GrB_Scalar_setElement(scalar->scalar, value),
					  scalar->scalar,
					  "Cannot set scalar element in expand.");
			}
			else
				elog(ERROR, "Invalid format for %s %s", fmt, str_val);
		}
	}
	else if (strcmp(str_type, "fp32") == 0)
	{
		float value;
		typ = GrB_FP32;
		fmt = "%f";
		scalar = new_scalar(typ, CurrentMemoryContext, NULL);
		if (len)
		{
			if (sscanf(str_val, fmt, &value) == 1)
			{
				OS_CHECK(GrB_Scalar_setElement(scalar->scalar, value),
					  scalar->scalar,
					  "Cannot set scalar element in expand.");
			}
			else
				elog(ERROR, "Invalid format for %s %s", fmt, str_val);
		}
	}
	else if (strcmp(str_type, "bool") == 0)
	{
		char value;
		typ = GrB_BOOL;
		fmt = "%c";
		scalar = new_scalar(typ, CurrentMemoryContext, NULL);
		if (len)
		{
			if (sscanf(str_val, fmt, &value) == 1)
			{
				bool _value;
				if (value == 't')
					_value = true;
				else if (value == 'f')
					_value = false;
				else
					elog(ERROR, "Invalid value for bool %c", value);
				OS_CHECK(GrB_Scalar_setElement(scalar->scalar, _value),
					  scalar->scalar,
					  "Cannot set scalar element in expand.");
			}
			else
				elog(ERROR, "Invalid format for %s %s", fmt, str_val);
		}
	}
	else
		elog(ERROR, "Unknown type code %s", str_type);
	OS_RETURN_SCALAR(scalar);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
