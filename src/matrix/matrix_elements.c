#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_elements);
Datum
matrix_elements(PG_FUNCTION_ARGS)
{
	FuncCallContext *funcctx;
	TupleDesc	tupdesc;
	Datum		result;

	Datum		values[3];
	bool		nulls[3] = {false, false, false};
	HeapTuple	tuple;
	GrB_Index	nvals,
				row,
				col;
	os_Matrix  *matrix;
	os_Scalar  *scalar;
	os_Matrix_ExtractState *state;

	if (SRF_IS_FIRSTCALL())
	{
		MemoryContext oldcontext;

		funcctx = SRF_FIRSTCALL_INIT();
		oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
		matrix = OS_GETARG_MATRIX(0);

		state = (os_Matrix_ExtractState *) palloc(sizeof(os_Matrix_ExtractState));
		OS_MNVALS(nvals, matrix);

		state->matrix = matrix;
		GxB_Iterator_new(&(state->iterator));
		OS_CHECK(GxB_Matrix_Iterator_attach(state->iterator, matrix->matrix, NULL),
				 matrix->matrix,
				 "Cannot attach matrix iterator.");
		OS_MTYPE(state->type, matrix);
		state->info = GxB_Matrix_Iterator_seek(state->iterator, 0);
		funcctx->max_calls = nvals;
		funcctx->user_fctx = (void *) state;

		if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
			ereport(ERROR,
					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
					 errmsg("function returning record called in context "
							"that cannot accept type record")));
		BlessTupleDesc(tupdesc);
		funcctx->tuple_desc = tupdesc;
		MemoryContextSwitchTo(oldcontext);
	}

	funcctx = SRF_PERCALL_SETUP();
	state = (os_Matrix_ExtractState *) funcctx->user_fctx;
	if (state->info == GxB_EXHAUSTED || funcctx->call_cntr > funcctx->max_calls)
	{
		SRF_RETURN_DONE(funcctx);
	}
	else
	{
		GxB_Matrix_Iterator_getIndex(state->iterator, &row, &col);
		values[0] = Int64GetDatum(row);
		values[1] = Int64GetDatum(col);
		scalar = new_scalar(state->type, CurrentMemoryContext, NULL);
		matrix = state->matrix;
		OS_CHECK(GrB_Matrix_extractElement(scalar->scalar, matrix->matrix, row, col),
				 matrix->matrix,
				 "Error extracting setting matrix element.");
		values[2] = EOHPGetRWDatum(&(scalar)->hdr);
		tuple = heap_form_tuple(funcctx->tuple_desc, values, nulls);
		result = HeapTupleGetDatum(tuple);
		state->info = GxB_Matrix_Iterator_next(state->iterator);
		SRF_RETURN_NEXT(funcctx, result);
	}
}
