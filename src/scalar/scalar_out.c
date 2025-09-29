#include "../onesparse.h"

PG_FUNCTION_INFO_V1(scalar_out);
Datum scalar_out(PG_FUNCTION_ARGS)
{
	char *result;
	char *type_name;
	size_t type_name_len;
	int32_t type_code;
	os_Scalar *scalar;
	GrB_Index nvals;
	GrB_Type type;

	LOGF();
	scalar = OS_GETARG_SCALAR(0);

	nvals = 0;
	OS_CHECK(GrB_Scalar_nvals(&nvals, scalar->scalar),
		  scalar->scalar,
		  "Error extracting scalar nvals.");

	OS_CHECK(GrB_get(scalar->scalar, &type_code, GrB_EL_TYPE_CODE),
		  scalar->scalar,
		  "Cannot get Scalar Type code.");

	if (type_code == GrB_UDT_CODE)
	{
		OS_CHECK(GxB_Scalar_type(&type, scalar->scalar),
				 scalar->scalar,
				 "Cannot get scalar type");

		OS_CHECK(GrB_get(type, &type_name_len, GrB_NAME),
				 type,
				 "Cannot get type name len.");

		type_name = palloc(type_name_len);

		OS_CHECK(GrB_get(type, type_name, GrB_NAME),
				 type,
				 "Cannot get type name.");
	}
	else
	{
		type_name = short_code(type_code);
	}

	if (nvals)
	{
		if (type_code == GrB_INT64_CODE)
		{
			int64_t value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:" "%" PRIi64, type_name, value);
		}
		else if (type_code == GrB_UINT64_CODE)
		{
			uint64_t value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:" "%" PRIu64, type_name, value);
		}
		else if (type_code == GrB_INT32_CODE)
		{
			int32_t value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:" "%" PRIi32, type_name, value);
		}
		else if (type_code == GrB_UINT32_CODE)
		{
			uint32_t value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:" "%" PRIu32, type_name, value);
		}
		else if (type_code == GrB_INT16_CODE)
		{
			int16_t value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:" "%" PRIi16, type_name, value);
		}
		else if (type_code == GrB_INT8_CODE)
		{
			int8_t value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:" "%" PRIi8, type_name, value);
		}
		else if (type_code == GrB_UINT16_CODE)
		{
			uint16_t value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:" "%" PRIu16, type_name, value);
		}
		else if (type_code == GrB_UINT8_CODE)
		{
			uint8_t value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:" "%" PRIu8, type_name, value);
		}
		else if (type_code == GrB_FP64_CODE)
		{
			double value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:%f", type_name, value);
		}
		else if (type_code == GrB_FP32_CODE)
		{
			float value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:%f", type_name, value);
		}
		else if (type_code == GrB_BOOL_CODE)
		{
			bool value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				   scalar->scalar,
				   "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:%s", type_name, value ? "t" : "f");
		}
		else if (type_code == GrB_UDT_CODE)
		{
			size_t ssize;
			void *value;
			int      enc_len;
			int      out_len;
			size_t   tlen;
			size_t result_len;

			OS_CHECK(GxB_Type_size(&ssize, type),
					 type,
					 "Error extracting scalar element.");

			value = palloc(ssize);
			OS_CHECK(GxB_Scalar_extractElement_UDT(value, scalar->scalar),
				   scalar->scalar,
				   "Error extracting scalar element.");

			enc_len = pg_b64_enc_len((int) ssize);
			tlen = strlen(type_name);
			result_len = tlen + 1 + enc_len + 1;
			result = palloc(result_len);
			memcpy(result, type_name, tlen);
			result[tlen] = ':';
			out_len = pg_b64_encode((const unsigned char *) value, (int) ssize,
									result + tlen + 1, enc_len + 1);

			result[tlen + 1 + out_len] = '\0';
			pfree(value);
		}
		else
			elog(ERROR, "Unsupported type code %i.", type_code);
	}
	else
	{
		result = palloc(4);
		snprintf(result, 4, "%s:", type_name);
	}
	PG_RETURN_CSTRING(result);
}

