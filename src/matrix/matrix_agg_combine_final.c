#include "../onesparse.h"

/*
 * Finalfunc for the matrix-combining matrix_agg(a matrix) binary-counter merge
 * (sfunc: matrix_agg_matrix). Folds the occupied carry levels into a single
 * result. Each taken level is stolen out of the state (slot set to NULL) so the
 * aggregate context reset callback won't double-free it; the returned C is owned
 * by the new os_Matrix.
 *
 * The per-type element builder matrix_agg(i, j, v) uses a different finalfunc
 * (matrix_agg_final) and is unaffected by this.
 */

/* mirror of the MatrixAggState in matrix_agg_matrix.c */
#define MATRIX_AGG_MAX_LEVELS 64

typedef struct MatrixAggState
{
	GrB_Matrix	levels[MATRIX_AGG_MAX_LEVELS];
	GrB_Type	type;
	GrB_BinaryOp dup;
} MatrixAggState;

PG_FUNCTION_INFO_V1(matrix_agg_combine_final);
Datum
matrix_agg_combine_final(PG_FUNCTION_ARGS)
{
	MatrixAggState *state;
	GrB_Matrix	C = NULL;
	int			r;

	if (PG_ARGISNULL(0))
		PG_RETURN_NULL();

	state = (MatrixAggState *) PG_GETARG_POINTER(0);

	for (r = 0; r < MATRIX_AGG_MAX_LEVELS; r++)
	{
		if (state->levels[r] == NULL)
			continue;

		if (C == NULL)
		{
			C = state->levels[r];
			state->levels[r] = NULL;
		}
		else
		{
			OS_CHECK(GrB_eWiseAdd(C, NULL, NULL, state->dup,
								  C, state->levels[r], NULL),
					 C,
					 "Error matrix eadd in matrix_agg_combine_final.");
			OS_CHECK(GrB_Matrix_free(&state->levels[r]),
					 state->levels[r],
					 "Cannot GrB_Free matrix_agg carry matrix");
		}
	}

	if (C == NULL)
		PG_RETURN_NULL();

	OS_RETURN_MATRIX(new_matrix(NULL, 0, 0, CurrentMemoryContext, C));
}
