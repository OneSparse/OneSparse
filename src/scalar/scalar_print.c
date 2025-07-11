#include "../onesparse.h"

PG_FUNCTION_INFO_V1(scalar_print);
Datum scalar_print(PG_FUNCTION_ARGS) {
	int32 type_code;
	os_Scalar *s;
	GrB_Type type;
    StringInfoData buf;
	s = OS_GETARG_SCALAR(0);
	OS_STYPE(type, s);
    initStringInfo(&buf);

	OS_CHECK(GrB_get(s->scalar, &type_code, GrB_EL_TYPE_CODE),
		  s->scalar,
		  "Cannot get Scalar Type code.");

	switch(type_code)
	{
		case GrB_INT64_CODE:
			{
				int64_t vi64;
				OS_CHECK(GrB_Scalar_extractElement(&vi64, s->scalar),
					  s->scalar,
					  "Error extracting scalar value.");
				appendStringInfo(&buf, "%ld", vi64);
				break;
			}
		case GrB_UINT64_CODE:
			{
				uint64_t vu64;
				OS_CHECK(GrB_Scalar_extractElement(&vu64, s->scalar),
					  s->scalar,
					  "Error extracting scalar value.");
				appendStringInfo(&buf, "%lu", vu64);
				break;
			}
		case GrB_INT32_CODE:
			{
				int32_t vi32;
				OS_CHECK(GrB_Scalar_extractElement(&vi32, s->scalar),
					  s->scalar,
					  "Error extracting scalar value.");
				appendStringInfo(&buf, "%d", vi32);
				break;
			}
		case GrB_UINT32_CODE:
			{
				uint32_t vu32;
				OS_CHECK(GrB_Scalar_extractElement(&vu32, s->scalar),
					  s->scalar,
					  "Error extracting scalar value.");
				appendStringInfo(&buf, "%u", vu32);
				break;
			}
		case GrB_INT16_CODE:
			{
				int32_t vi16;
				OS_CHECK(GrB_Scalar_extractElement(&vi16, s->scalar),
					  s->scalar,
					  "Error extracting scalar value.");
				appendStringInfo(&buf, "%d", vi16);
				break;
			}
		case GrB_UINT16_CODE:
			{
				uint32_t vu16;
				OS_CHECK(GrB_Scalar_extractElement(&vu16, s->scalar),
					  s->scalar,
					  "Error extracting scalar value.");
				appendStringInfo(&buf, "%hu", vu16);
				break;
			}
		case GrB_FP64_CODE:
			{
				double fp64;
				OS_CHECK(GrB_Scalar_extractElement(&fp64, s->scalar),
					  s->scalar,
					  "Error extracting scalar value.");
				appendStringInfo(&buf, "%f", fp64);
				break;
			}
		case GrB_FP32_CODE:
			{
				float fp64;
				OS_CHECK(GrB_Scalar_extractElement(&fp64, s->scalar),
					  s->scalar,
					  "Error extracting scalar value.");
				appendStringInfo(&buf, "%f", fp64);
				break;
			}
		case GrB_BOOL_CODE:
			{
				bool b8;
				OS_CHECK(GrB_Scalar_extractElement(&b8, s->scalar),
					  s->scalar,
					  "Error extracting scalar value.");
				appendStringInfo(&buf, b8? "t" : "f");
				break;
			}
	}

	PG_RETURN_TEXT_P(cstring_to_text(buf.data));
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
