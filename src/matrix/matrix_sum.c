#include "../onesparse.h"

/*
 * matrix_sum: an aggregate that combines many matrices (one per row) into a
 * single result using LAGraph_Matrix_Sum.
 *
 * Unlike matrix_agg, which combines the rows with a serial binary-counter merge
 * of GrB_eWiseAdds (no LAGraph dependency), matrix_sum collects the input matrix
 * handles during the scan and performs a
 * single LAGraph_Matrix_Sum at finalize. That primitive concatenates the tuples
 * of all inputs into one buffer and calls GrB_Matrix_build once with a "dup"
 * binary operator to combine duplicate (i,j) entries -- a single internally
 * parallel build pass instead of N-1 sparse merges.
 *
 * The dup operator defaults to PLUS (so matrix_sum(m) is the element-wise sum);
 * an optional binaryop argument overrides it. dup must be commutative and
 * associative for the result to be well defined, since the tuple order seen by
 * GrB_Matrix_build is unspecified.
 *
 * Builtin types only: LAGraph_Matrix_Sum returns GrB_NOT_IMPLEMENTED for
 * user-defined types.
 *
 * This whole file is gated behind ONESPARSE_HAVE_LAGRAPH_MATRIX_SUM because
 * LAGraph_Matrix_Sum only exists on the LAGraph "matrix-sum" branch
 * (https://github.com/GraphBLAS/LAGraph/pull/407). Build with
 * `make WITH_MATRIX_SUM=1` against that branch to enable it.
 */

#ifdef ONESPARSE_HAVE_LAGRAPH_MATRIX_SUM

typedef struct MatrixSumState
{
	GrB_Matrix *mats;			/* dup'd input handles, grown geometrically */
	int			n;				/* number of handles collected */
	int			cap;			/* allocated slots in mats */
	GrB_Type	type;			/* element type, fixed by the first row */
	GrB_BinaryOp dup;			/* duplicate combiner for the final build */
} MatrixSumState;

/*
 * Free the collected dup'd input matrices when the aggregate's memory context
 * is reset/deleted. GraphBLAS objects are malloc'd (not in a PG context), so
 * they must be freed explicitly to avoid leaks (including on error/abort).
 */
static void
context_callback_matrix_sum_free(void *ptr)
{
	MatrixSumState *state = (MatrixSumState *) ptr;
	int			i;

	for (i = 0; i < state->n; i++)
	{
		if (state->mats[i] != NULL)
			OS_CHECK(GrB_Matrix_free(&state->mats[i]),
					 state->mats[i],
					 "Cannot GrB_Free matrix_sum input matrix");
	}
}

PG_FUNCTION_INFO_V1(matrix_sum_matrix);
Datum
matrix_sum_matrix(PG_FUNCTION_ARGS)
{
	GrB_Type	type;
	os_Matrix  *A;
	GrB_Matrix	dupmat;
	GrB_BinaryOp op;
	MatrixSumState *state;
	MemoryContext aggcontext;
	int			nargs;

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

		state = MemoryContextAllocZero(aggcontext, sizeof(MatrixSumState));
		state->cap = 16;
		state->mats = MemoryContextAlloc(aggcontext,
										 state->cap * sizeof(GrB_Matrix));
		state->n = 0;
		state->type = type;

		nargs = PG_NARGS();
		op = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 2);
		if (op == NULL)
			op = plus_binaryop(type);
		state->dup = op;

		ctxcb = MemoryContextAlloc(aggcontext, sizeof(MemoryContextCallback));
		ctxcb->func = context_callback_matrix_sum_free;
		ctxcb->arg = state;
		MemoryContextRegisterResetCallback(aggcontext, ctxcb);
	}
	else
	{
		state = (MatrixSumState *) PG_GETARG_POINTER(0);
	}

	if (state->n == state->cap)
	{
		state->cap *= 2;
		state->mats = repalloc(state->mats,
							   state->cap * sizeof(GrB_Matrix));
	}

	/*
	 * The input matrix lives in a per-row memory context that is reset before
	 * finalize, so we must dup it into a handle we own for the whole aggregate.
	 */
	OS_CHECK(GrB_Matrix_dup(&dupmat, A->matrix),
			 A->matrix,
			 "Cannot dup matrix in matrix_sum_matrix");
	state->mats[state->n++] = dupmat;

	PG_RETURN_POINTER(state);
}

PG_FUNCTION_INFO_V1(matrix_sum_final);
Datum
matrix_sum_final(PG_FUNCTION_ARGS)
{
	MatrixSumState *state;
	GrB_Matrix	C;
	char		msg[LAGRAPH_MSG_LEN];

	if (PG_ARGISNULL(0))
		PG_RETURN_NULL();

	state = (MatrixSumState *) PG_GETARG_POINTER(0);

	if (state->n == 0)
		PG_RETURN_NULL();

	LA_CHECK(LAGraph_Matrix_Sum(&C,
								state->mats,
								(GrB_Index) state->n,
								state->dup,
								msg));

	/*
	 * The dup'd inputs in state->mats are freed by the aggregate context reset
	 * callback; C is owned by the returned os_Matrix.
	 */
	OS_RETURN_MATRIX(new_matrix(NULL, 0, 0, CurrentMemoryContext, C));
}

#endif							/* ONESPARSE_HAVE_LAGRAPH_MATRIX_SUM */
