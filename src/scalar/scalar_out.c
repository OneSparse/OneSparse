#include "../onesparse.h"

PG_FUNCTION_INFO_V1(scalar_out);
Datum scalar_out(PG_FUNCTION_ARGS)
{
	char *result, *sname;
	int32_t type_code;
	os_Scalar *scalar;
	GrB_Index nvals;

	LOGF();
	scalar = OS_GETARG_SCALAR(0);

	nvals = 0;
	OS_CHECK(GrB_Scalar_nvals(&nvals, scalar->scalar),
		  scalar->scalar,
		  "Error extracting scalar nvals.");

	OS_CHECK(GrB_get(scalar->scalar, &type_code, GrB_EL_TYPE_CODE),
		  scalar->scalar,
		  "Cannot get Scalar Type code.");

	sname = short_code(type_code);

	if (nvals)
	{
		if (type_code == GrB_INT64_CODE)
		{
			int64_t value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:" "%" PRIi64, sname, value);
		}
		else if (type_code == GrB_UINT64_CODE)
		{
			uint64_t value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:" "%" PRIu64, sname, value);
		}
		else if (type_code == GrB_INT32_CODE)
		{
			int32_t value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:" "%" PRIi32, sname, value);
		}
		else if (type_code == GrB_UINT32_CODE)
		{
			uint32_t value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:" "%" PRIu32, sname, value);
		}
		else if (type_code == GrB_INT16_CODE)
		{
			int16_t value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:" "%" PRIi16, sname, value);
		}
		else if (type_code == GrB_INT8_CODE)
		{
			int8_t value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:" "%" PRIi8, sname, value);
		}
		else if (type_code == GrB_UINT16_CODE)
		{
			uint16_t value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:" "%" PRIu16, sname, value);
		}
		else if (type_code == GrB_UINT8_CODE)
		{
			uint8_t value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:" "%" PRIu8, sname, value);
		}
		else if (type_code == GrB_FP64_CODE)
		{
			double value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:%f", sname, value);
		}
		else if (type_code == GrB_FP32_CODE)
		{
			float value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:%f", sname, value);
		}
		else if (type_code == GrB_BOOL_CODE)
		{
			bool value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				   scalar->scalar,
				   "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:%s", sname, value ? "t" : "f");
		}
		else
			elog(ERROR, "Unsupported type code %i.", type_code);
	}
	else
	{
		result = palloc(4);
		snprintf(result, 4, "%s:", sname);
	}
	PG_RETURN_CSTRING(result);
}

