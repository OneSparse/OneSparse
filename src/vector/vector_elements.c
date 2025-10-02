#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_elements);
Datum
vector_elements(PG_FUNCTION_ARGS)
{
	FuncCallContext *funcctx;
	TupleDesc	tupdesc;
	Datum		result;

	Datum		values[2];
	bool		nulls[2] = {false, false};
	HeapTuple	tuple;
	GrB_Index	nvals,
				i;
	os_Vector  *vector;
	os_Scalar  *scalar;
	os_Vector_ExtractState *state;

	if (SRF_IS_FIRSTCALL())
	{
		MemoryContext oldcontext;

		LOGF();
		funcctx = SRF_FIRSTCALL_INIT();
		oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
		vector = OS_GETARG_VECTOR(0);

		state = (os_Vector_ExtractState *) palloc(sizeof(os_Vector_ExtractState));
		OS_VNVALS(nvals, vector);

		state->vector = vector;
		GxB_Iterator_new(&(state->iterator));
		OS_CHECK(GxB_Vector_Iterator_attach(state->iterator, vector->vector, NULL),
				 vector->vector,
				 "Cannot attach vector iterator.");
		OS_VTYPE(state->type, vector);
		state->info = GxB_Vector_Iterator_seek(state->iterator, 0);
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
	state = (os_Vector_ExtractState *) funcctx->user_fctx;
	vector = state->vector;
	if (state->info == GxB_EXHAUSTED || funcctx->call_cntr > funcctx->max_calls)
	{
		SRF_RETURN_DONE(funcctx);
	}
	else
	{
		i = GxB_Vector_Iterator_getIndex(state->iterator);
		values[0] = Int64GetDatum(i);
		scalar = new_scalar(state->type, CurrentMemoryContext, NULL);
		OS_CHECK(GrB_Vector_extractElement(scalar->scalar, vector->vector, i),
				 vector->vector,
				 "Error extracting setting vector element.");
		values[1] = EOHPGetRWDatum(&(scalar)->hdr);
		tuple = heap_form_tuple(funcctx->tuple_desc, values, nulls);
		result = HeapTupleGetDatum(tuple);
		state->info = GxB_Vector_Iterator_next(state->iterator);
		SRF_RETURN_NEXT(funcctx, result);
	}
}
