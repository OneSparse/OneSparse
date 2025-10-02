#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_out);
Datum
vector_out(PG_FUNCTION_ARGS)
{
	GrB_Info	info;
	GrB_Index	i,
				usize,
				nvals;
	GxB_Iterator iterator;
	os_Vector  *vector;
	StringInfoData buf;
	int32_t		type_code;

	LOGF();
	vector = OS_GETARG_VECTOR(0);

	initStringInfo(&buf);
	OS_CHECK(GrB_get(vector->vector, &type_code, GrB_EL_TYPE_CODE),
			 vector->vector,
			 "Cannot get Vector Type code.");

	OS_VSIZE(usize, vector);

	appendStringInfo(&buf, "%s", short_code(type_code));
	if (usize < GrB_INDEX_MAX + 1)
	{
		appendStringInfo(&buf, "(%lu)", usize);
	}
	OS_VNVALS(nvals, vector);

	if (nvals == 0)
	{
		PG_RETURN_CSTRING(buf.data);
	}

	appendStringInfo(&buf, "[");

	GxB_Iterator_new(&iterator);
	OS_CHECK(GxB_Vector_Iterator_attach(iterator, vector->vector, NULL),
			 vector->vector,
			 "Cannot attach vector iterator.");

	info = GxB_Vector_Iterator_seek(iterator, 0);
	while (info != GxB_EXHAUSTED)
	{
		i = GxB_Vector_Iterator_getIndex(iterator);
		switch (type_code)
		{
			case GrB_INT64_CODE:
				{
					int64_t		vi64 = GxB_Iterator_get_INT64(iterator);

					appendStringInfo(&buf, "%lu:%ld", i, vi64);
					break;
				}
			case GrB_UINT64_CODE:
				{
					uint64_t	vu64 = GxB_Iterator_get_UINT64(iterator);

					appendStringInfo(&buf, "%lu:%ld", i, vu64);
					break;
				}
			case GrB_INT32_CODE:
				{
					int32_t		vi32 = GxB_Iterator_get_INT32(iterator);

					appendStringInfo(&buf, "%lu:%d", i, vi32);
					break;
				}
			case GrB_UINT32_CODE:
				{
					uint32_t	vu32 = GxB_Iterator_get_UINT32(iterator);

					appendStringInfo(&buf, "%lu:%d", i, vu32);
					break;
				}
			case GrB_INT16_CODE:
				{
					int16_t		vi16 = GxB_Iterator_get_INT16(iterator);

					appendStringInfo(&buf, "%lu:%d", i, vi16);
					break;
				}
			case GrB_UINT16_CODE:
				{
					uint16_t	vu16 = GxB_Iterator_get_UINT16(iterator);

					appendStringInfo(&buf, "%lu:%d", i, vu16);
					break;
				}
			case GrB_FP64_CODE:
				{
					double		vf64 = GxB_Iterator_get_FP64(iterator);

					appendStringInfo(&buf, "%lu:%f", i, vf64);
					break;
				}
			case GrB_FP32_CODE:
				{
					float		vf32 = GxB_Iterator_get_FP32(iterator);

					appendStringInfo(&buf, "%lu:%f", i, vf32);
					break;
				}
			case GrB_BOOL_CODE:
				{
					bool		b = GxB_Iterator_get_BOOL(iterator);

					appendStringInfo(&buf, "%lu:%s", i, b ? "t" : "f");
					break;
				}
		}
		info = GxB_Vector_Iterator_next(iterator);
		if (info != GxB_EXHAUSTED)
			appendStringInfo(&buf, " ");
	}
	GrB_free(&iterator);

	appendStringInfo(&buf, "]");
	PG_RETURN_CSTRING(buf.data);
}
