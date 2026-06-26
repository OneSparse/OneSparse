#include "../onesparse.h"

/*
 * matrix_agg(a matrix): an aggregate that combines many matrices (one per row)
 * into a single result using a binary-counter merge.
 *
 * This is the portable sibling of matrix_sum / matrix_binary_sum: same result,
 * but implemented purely with GrB_eWiseAdd (no LAGraph dependency, so it ships
 * in the stock extension and is not gated). It is single-threaded but streams:
 * each input is freed the moment it is absorbed, so it never holds all N inputs
 * at once.
 *
 * The merge is a balanced binary reduction maintained as a binary counter:
 * levels[r], when non-NULL, holds the sum of exactly 2^r already-seen matrices
 * (like binary addition with carry). Each new matrix enters at rank 0 and is
 * carried up, summing-and-freeing occupied levels, until it lands in an empty
 * slot. At finalize the occupied levels are folded into a single result. This is
 * O(N log N) total eWiseAdd work, versus the old serial accumulator which was
 * ~O(N^2) on low-overlap data (it re-touched the whole growing accumulator on
 * every row).
 *
 * The dup operator defaults to PLUS (so matrix_agg(m) is the element-wise sum,
 * matching matrix_sum and matrix_binary_sum); an optional binaryop argument
 * overrides it. dup must be commutative and associative for the result to be
 * well defined (the binary tree then yields the same result as a serial fold).
 * All inputs must share dimensions (a GrB_eWiseAdd requirement).
 *
 * NB: this is the matrix-combining matrix_agg(a matrix). The per-type element
 * builder matrix_agg(i, j, v) is a separate aggregate (src/matrix/matrix_ops.h,
 * finalfunc matrix_agg_final) and is unaffected.
 */

#define MATRIX_AGG_MAX_LEVELS 64

typedef struct MatrixAggState
{
	GrB_Matrix	levels[MATRIX_AGG_MAX_LEVELS];	/* carry slots: levels[r] = sum of 2^r matrices, or NULL */
	GrB_Type	type;			/* element type, fixed by the first row */
	GrB_BinaryOp dup;			/* duplicate combiner for the sum */
} MatrixAggState;

/*
 * Free any carry matrices still held when the aggregate's memory context is
 * reset/deleted. GraphBLAS objects are malloc'd (not in a PG context), so they
 * must be freed explicitly to avoid leaks (including on error/abort). Slots the
 * finalfunc has already taken ownership of are set to NULL and skipped here.
 */
static void
context_callback_matrix_agg_free(void *ptr)
{
	MatrixAggState *state = (MatrixAggState *) ptr;
	int			r;

	for (r = 0; r < MATRIX_AGG_MAX_LEVELS; r++)
	{
		if (state->levels[r] != NULL)
			OS_CHECK(GrB_Matrix_free(&state->levels[r]),
					 state->levels[r],
					 "Cannot GrB_Free matrix_agg carry matrix");
	}
}

PG_FUNCTION_INFO_V1(matrix_agg_matrix);
Datum
matrix_agg_matrix(PG_FUNCTION_ARGS)
{
	GrB_Type	type;
	os_Matrix  *A;
	GrB_Matrix	carry;
	GrB_BinaryOp op;
	MatrixAggState *state;
	MemoryContext aggcontext;
	int			nargs;
	int			r;

	if (PG_ARGISNULL(1))
	{
		elog(ERROR, "Cannot aggregate a null value to a matrix");
	}

	if (!AggCheckCallContext(fcinfo, &aggcontext))
	{
		elog(ERROR, "aggregate function called in non-aggregate context");
	}

	A = OS_GETARG_MATRIX(1);
	OS_MTYPE(type, A);

	if (PG_ARGISNULL(0))
	{
		MemoryContextCallback *ctxcb;

		state = MemoryContextAllocZero(aggcontext, sizeof(MatrixAggState));
		state->type = type;

		nargs = PG_NARGS();
		op = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 2);
		if (op == NULL)
			op = plus_binaryop(type);
		state->dup = op;

		ctxcb = MemoryContextAlloc(aggcontext, sizeof(MemoryContextCallback));
		ctxcb->func = context_callback_matrix_agg_free;
		ctxcb->arg = state;
		MemoryContextRegisterResetCallback(aggcontext, ctxcb);
	}
	else
	{
		state = (MatrixAggState *) PG_GETARG_POINTER(0);
	}

	/*
	 * The input matrix lives in a per-row memory context that is reset before
	 * finalize, so we must dup it into a handle we own for the whole aggregate.
	 */
	OS_CHECK(GrB_Matrix_dup(&carry, A->matrix),
			 A->matrix,
			 "Cannot dup matrix in matrix_agg_matrix");

	/*
	 * Carry the new matrix up the counter: while a level is occupied, sum it
	 * into carry (C may alias A in GrB_eWiseAdd) and free the absorbed level,
	 * until carry lands in an empty slot.
	 */
	for (r = 0; r < MATRIX_AGG_MAX_LEVELS && state->levels[r] != NULL; r++)
	{
		OS_CHECK(GrB_eWiseAdd(carry, NULL, NULL, state->dup,
							  carry, state->levels[r], NULL),
				 carry,
				 "Error matrix eadd in matrix_agg_matrix.");
		OS_CHECK(GrB_Matrix_free(&state->levels[r]),
				 state->levels[r],
				 "Cannot GrB_Free matrix_agg carry matrix");
	}

	if (r == MATRIX_AGG_MAX_LEVELS)
		elog(ERROR, "matrix_agg carry overflow (too many input matrices)");

	state->levels[r] = carry;

	PG_RETURN_POINTER(state);
}
