#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_rows);
Datum
matrix_rows(PG_FUNCTION_ARGS)
{
	FuncCallContext *funcctx;
	GrB_Index	nvals,
				row,
				col;
	os_Matrix  *matrix;
	os_Matrix_ExtractState *state;

	if (SRF_IS_FIRSTCALL())
	{
		MemoryContext oldcontext;

		funcctx = SRF_FIRSTCALL_INIT();
		oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
		matrix = OS_GETARG_MATRIX(0);

		state = (os_Matrix_ExtractState *) palloc0(sizeof(os_Matrix_ExtractState));
		OS_MNVALS(nvals, matrix);

		state->matrix = matrix;
		GxB_Iterator_new(&(state->iterator));
		OS_CHECK(GxB_Matrix_Iterator_attach(state->iterator, matrix->matrix, NULL),
				 matrix->matrix,
				 "Cannot attach matrix row iterator.");
		OS_MTYPE(state->type, matrix);
		state->info = GxB_Matrix_Iterator_seek(state->iterator, 0);
		funcctx->max_calls = nvals;
		funcctx->user_fctx = (void *) state;
		MemoryContextSwitchTo(oldcontext);
	}

	funcctx = SRF_PERCALL_SETUP();
	state = (os_Matrix_ExtractState *) funcctx->user_fctx;
	if (state->info == GxB_EXHAUSTED || funcctx->call_cntr > funcctx->max_calls)
	{
		if (state->temporary_matrix != NULL)
			GrB_Matrix_free(&state->temporary_matrix);
		SRF_RETURN_DONE(funcctx);
	}
	else
	{
		GxB_Matrix_Iterator_getIndex(state->iterator, &row, &col);
		while ((state->info = GxB_Matrix_Iterator_next(state->iterator)) != GxB_EXHAUSTED)
		{
			GrB_Index	next_row;

			GxB_Matrix_Iterator_getIndex(state->iterator, &next_row, &col);
			if (next_row != row)
				break;
		}
		SRF_RETURN_NEXT(funcctx, Int64GetDatum(row));
	}
}
