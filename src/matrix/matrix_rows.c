#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_rows);
Datum matrix_rows(PG_FUNCTION_ARGS)
{
	FuncCallContext  *funcctx;
	GrB_Index nvals, row;
	os_Matrix *matrix;
	os_Matrix_ExtractState *state;

	if (SRF_IS_FIRSTCALL()) {
		MemoryContext oldcontext;

		funcctx = SRF_FIRSTCALL_INIT();
		oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
		matrix = OS_GETARG_MATRIX(0);

		state = (os_Matrix_ExtractState*)palloc(sizeof(os_Matrix_ExtractState));
		OS_MNVALS(nvals, matrix);

		state->matrix = matrix;
		GxB_Iterator_new(&(state->iterator));
		OS_CHECK(GxB_rowIterator_attach(state->iterator, matrix->matrix, NULL),
				 matrix->matrix,
				 "Cannot attach matrix row iterator.");
		OS_MTYPE(state->type, matrix);
		state->info = GxB_rowIterator_seekRow(state->iterator, 0);
		funcctx->max_calls = nvals;
		funcctx->user_fctx = (void*)state;
		MemoryContextSwitchTo(oldcontext);
	}

	funcctx = SRF_PERCALL_SETUP();
	state = (os_Matrix_ExtractState*)funcctx->user_fctx;
	if (state->info == GxB_EXHAUSTED || funcctx->call_cntr > funcctx->max_calls)
	{
		SRF_RETURN_DONE(funcctx);
	}
	else
	{
		row = GxB_rowIterator_getRowIndex(state->iterator);
		state->info = GxB_rowIterator_nextRow(state->iterator);
		SRF_RETURN_NEXT(funcctx, Int64GetDatum(row));
	}
}

