#include "../onesparse.h"

Datum		_scalar_in(char *input);

PG_FUNCTION_INFO_V1(scalar_in);

Datum
scalar_in(PG_FUNCTION_ARGS)
{
	char	   *input;

	input = PG_GETARG_CSTRING(0);
	return _scalar_in(input);
}

Datum
_scalar_in(char *input)
{
	os_Scalar  *scalar;
	GrB_Type	typ;
	char		str_val[GxB_MAX_NAME_LEN] = "";
	char		str_type[GxB_MAX_NAME_LEN];
	char	   *colon_pos;
	char	   *fmt = "";

	colon_pos = strchr(input, ':');
	if (colon_pos != NULL)
	{
		sscanf(input, "%[^:]:%s", str_type, str_val);
	}
	else
	{
		sscanf(input, "%s", str_type);
	}

	typ = lookup_type(str_type);
	scalar = new_scalar(typ, CurrentMemoryContext, NULL);

	if (typ == GrB_INT64)
	{
		int64_t		value;

		fmt = "%ld";
		if (colon_pos != NULL)
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
	else if (typ == GrB_UINT64)
	{
		uint64_t	value;

		fmt = "%llu";
		if (colon_pos != NULL)
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
	else if (typ == GrB_INT32)
	{
		int32_t		value;

		fmt = "%i";
		if (colon_pos != NULL)
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
	else if (typ == GrB_UINT32)
	{
		uint32_t	value;

		fmt = "%u";
		if (colon_pos != NULL)
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
	else if (typ == GrB_INT16)
	{
		int16_t		value;

		fmt = "%i";
		if (colon_pos != NULL)
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
	else if (typ == GrB_UINT16)
	{
		uint16_t	value;

		fmt = "%hu";
		if (colon_pos != NULL)
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
	else if (typ == GrB_FP64)
	{
		double		value;

		fmt = "%lf";
		if (colon_pos != NULL)
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
	else if (typ == GrB_FP32)
	{
		float		value;

		fmt = "%f";
		if (colon_pos != NULL)
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
	else if (typ == GrB_BOOL)
	{
		char		value;

		fmt = "%c";
		if (colon_pos != NULL)
		{
			if (sscanf(str_val, fmt, &value) == 1)
			{
				bool		_value;

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
	{
		/* UDT */
		if (colon_pos != NULL)
		{
			size_t		b64len;
			int			out_len;
			size_t		max_bytes;
			void	   *_value;

			b64len = strlen(str_val);
			max_bytes = pg_b64_dec_len((int) b64len);
			_value = palloc(max_bytes);

			out_len = pg_b64_decode((const char *) str_val, (int) b64len,
									(uint8 *) _value, (int) max_bytes);
			if (out_len < 0)
				elog(ERROR, "scalar_in: invalid base64 data");
			OS_CHECK(GrB_Scalar_setElement(scalar->scalar, _value),
					 scalar->scalar,
					 "Cannot set scalar element UDT.");
		}
	}
	OS_RETURN_SCALAR(scalar);
}
