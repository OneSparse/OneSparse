#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_out);
Datum matrix_out(PG_FUNCTION_ARGS)
{
	GrB_Info info;
	GrB_Index row, col, nrows, ncols, nvals;
	GxB_Iterator iterator;
	os_Matrix *matrix;
	StringInfoData buf;
	int32_t type_code;

	LOGF();
	matrix = OS_GETARG_MATRIX(0);

	GxB_Iterator_new(&iterator);
	OS_CHECK(GxB_Matrix_Iterator_attach(iterator, matrix->matrix, NULL),
		  matrix->matrix,
		  "Cannot attach matrix iterator.");

	initStringInfo(&buf);
	OS_CHECK(GrB_get(matrix->matrix, &type_code, GrB_EL_TYPE_CODE),
		  matrix->matrix,
		  "Cannot get Matrix Type code.");

	appendStringInfo(&buf, "%s", short_code(type_code));

	OS_MNROWS(nrows, matrix);
	OS_MNCOLS(ncols, matrix);
	if (nrows < GrB_INDEX_MAX+1 || ncols < GrB_INDEX_MAX+1)
	{
		appendStringInfo(&buf, "(");
		if (nrows < GrB_INDEX_MAX+1)
		{
			appendStringInfo(&buf, "%lu", nrows);
		}
		appendStringInfo(&buf, ":");
		if (ncols < GrB_INDEX_MAX+1)
		{
			appendStringInfo(&buf, "%lu", ncols);
		}
		appendStringInfo(&buf, ")");
	}
	OS_MNVALS(nvals, matrix);

	if (nvals == 0)
	{
		PG_RETURN_CSTRING(buf.data);
	}

	appendStringInfo(&buf, "[");

	info = GxB_Matrix_Iterator_seek(iterator, 0);
	while (info != GxB_EXHAUSTED)
	{
		GxB_Matrix_Iterator_getIndex(iterator, &row, &col);
		switch(type_code)
		{
			case GrB_INT64_CODE:
				{
				int64_t vi64 = GxB_Iterator_get_INT64(iterator);
				appendStringInfo(&buf, "%lu:%lu:%ld", row, col, vi64);
				break;
				}
			case GrB_UINT64_CODE:
				{
				uint64_t vu64 = GxB_Iterator_get_UINT64(iterator);
				appendStringInfo(&buf, "%lu:%lu:%lu", row, col, vu64);
				break;
				}
			case GrB_INT32_CODE:
				{
				int32_t vi32 = GxB_Iterator_get_INT32(iterator);
				appendStringInfo(&buf, "%lu:%lu:%d", row, col, vi32);
				break;
				}
			case GrB_UINT32_CODE:
				{
				uint32_t vu32 = GxB_Iterator_get_UINT32(iterator);
				appendStringInfo(&buf, "%lu:%lu:%u", row, col, vu32);
				break;
				}
			case GrB_INT16_CODE:
				{
				int16_t vi16 = GxB_Iterator_get_INT16(iterator);
				appendStringInfo(&buf, "%lu:%lu:%d", row, col, vi16);
				break;
				}
			case GrB_UINT16_CODE:
				{
				uint16_t vu16 = GxB_Iterator_get_UINT16(iterator);
				appendStringInfo(&buf, "%lu:%lu:%u", row, col, vu16);
				break;
				}
			case GrB_FP64_CODE:
				{
				double vf64 = GxB_Iterator_get_FP64(iterator);
				appendStringInfo(&buf, "%lu:%lu:%f", row, col, vf64);
				break;
				}
			case GrB_FP32_CODE:
				{
				float vf32 = GxB_Iterator_get_FP32(iterator);
				appendStringInfo(&buf, "%lu:%lu:%f", row, col, vf32);
				break;
				}
			case GrB_BOOL_CODE:
				{
				bool b = GxB_Iterator_get_BOOL(iterator);
				appendStringInfo(&buf, "%lu:%lu:%s", row, col, b ? "t" : "f");
				break;
				}
		}
		info = GxB_Matrix_Iterator_next(iterator);
		if (info != GxB_EXHAUSTED)
			appendStringInfo(&buf, " ");
	}
	GrB_free(&iterator);

	appendStringInfo(&buf, "]");
	PG_RETURN_CSTRING(buf.data);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
