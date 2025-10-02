#include "../onesparse.h"
#include "executor/spi.h"

PG_FUNCTION_INFO_V1(matrix_query);
Datum
matrix_query(PG_FUNCTION_ARGS)
{
	text	   *sql_text = PG_GETARG_TEXT_PP(0);
	char	   *query = text_to_cstring(sql_text);
	int			ret;
	SPIPlanPtr	plan;
	Portal		portal;
	TupleDesc	tupdesc;
	Oid			t0,
				t1,
				t2;
	uint64		i;
	uint32		batch_size;
	GrB_Matrix	tmp;
	GrB_Type	type;
	GrB_BinaryOp accum;
	GrB_Index	nrows,
				ncols;
	os_Matrix  *A;
	struct timeval start,
				end;
	MemoryContext oldctx = CurrentMemoryContext;

	OS_START_BENCH();
	nrows = PG_GETARG_INT64(1);
	ncols = PG_GETARG_INT64(2);
	batch_size = PG_GETARG_INT32(3);
	if (nrows == -1)
	{
		nrows = GrB_INDEX_MAX + 1;
	}
	if (ncols == -1)
		ncols = GrB_INDEX_MAX + 1;

	if ((ret = SPI_connect()) != SPI_OK_CONNECT)
	{
		elog(ERROR, "SPI_connect failed: %s", SPI_result_code_string(ret));
	}

	plan = SPI_prepare(query, 0, NULL);
	if (plan == NULL)
	{
		elog(ERROR, "SPI_prepare failed: %s", SPI_result_code_string(SPI_result));
	}
	portal = SPI_cursor_open(NULL, plan, NULL, NULL, true);
	if (!portal)
	{
		elog(ERROR, "SPI_cursor_open failed");
	}

	SPI_cursor_fetch(portal, true, 0);

	tupdesc = SPI_tuptable->tupdesc;
	if (tupdesc->natts != 3)
	{
		elog(ERROR, "query must return exactly three columns, got %d", tupdesc->natts);
	}
	t0 = TupleDescAttr(tupdesc, 0)->atttypid;
	t1 = TupleDescAttr(tupdesc, 1)->atttypid;
	t2 = TupleDescAttr(tupdesc, 2)->atttypid;

	if (t0 != INT8OID || t1 != INT8OID)
	{
		elog(ERROR, "first two columns must be bigint");
	}
	if (t2 != BOOLOID && t2 != INT4OID && t2 != INT8OID
		&& t2 != FLOAT4OID && t2 != FLOAT8OID)
	{
		elog(ERROR, "third column must be bool, integer, bigint, float4 or float8");
	}
	switch (t2)
	{
		case BOOLOID:
			type = GrB_BOOL;
			accum = GrB_SECOND_BOOL;
			break;
		case INT4OID:
			type = GrB_INT32;
			accum = GrB_SECOND_INT32;
			break;
		case INT8OID:
			type = GrB_INT64;
			accum = GrB_SECOND_INT64;
			break;
		case FLOAT4OID:
			type = GrB_FP32;
			accum = GrB_SECOND_FP32;
			break;
		case FLOAT8OID:
			type = GrB_FP64;
			accum = GrB_SECOND_FP64;
			break;
		default:
			elog(ERROR, "third column must be bool, integer, bigint, float4 or float8");
			break;
	}

	A = new_matrix(type, nrows, ncols, oldctx, NULL);

	while (true)
	{
		SPI_cursor_fetch(portal, true, batch_size);
		if (SPI_processed == 0)
			break;

		OS_CHECK(GrB_Matrix_new(&tmp, type, nrows, ncols),
				 tmp,
				 "Cannot create new Matrix.");

		for (i = 0; i < SPI_processed; i++)
		{
			HeapTuple	tup = SPI_tuptable->vals[i];
			bool		null1,
						null2,
						null3;
			Datum		d0 = SPI_getbinval(tup, tupdesc, 1, &null1);
			Datum		d1 = SPI_getbinval(tup, tupdesc, 2, &null2);
			Datum		d2 = SPI_getbinval(tup, tupdesc, 3, &null3);
			int64		i = DatumGetInt64(d0);
			int64		j = DatumGetInt64(d1);

			if (null1 || null2 || null3)
				elog(ERROR, "Matrix element values cannot be NULL");
			else
			{
				switch (t2)
				{
					case BOOLOID:
						OS_CHECK(GrB_Matrix_setElement(tmp, DatumGetBool(d2), i, j),
								 tmp,
								 "Error setting matrix element.");
						break;
					case INT4OID:
						OS_CHECK(GrB_Matrix_setElement(tmp, DatumGetInt32(d2), i, j),
								 tmp,
								 "Error setting matrix element.");
						break;
					case INT8OID:
						OS_CHECK(GrB_Matrix_setElement(tmp, DatumGetInt64(d2), i, j),
								 tmp,
								 "Error setting matrix element.");
						break;
					case FLOAT4OID:
						OS_CHECK(GrB_Matrix_setElement(tmp, DatumGetFloat4(d2), i, j),
								 tmp,
								 "Error setting matrix element.");
						break;
					case FLOAT8OID:
						OS_CHECK(GrB_Matrix_setElement(tmp, DatumGetFloat8(d2), i, j),
								 tmp,
								 "Error setting matrix element.");
						break;
					default:	/* unreachable */
						break;
				}
			}
		}
		OS_CHECK(GrB_assign(A->matrix, NULL, accum, tmp, GrB_ALL, 0, GrB_ALL, 0, NULL),
				 A->matrix,
				 "cannot assign batch to matrix.");

		OS_CHECK(GrB_Matrix_free(&tmp),
				 tmp,
				 "Cannot GrB_Free batch Matrix");

	}
	SPI_cursor_close(portal);
	SPI_finish();
	OS_END_BENCH();
	OS_RETURN_MATRIX(A);
}
