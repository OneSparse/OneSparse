#include "../onesparse.h"

/*
 * matrix_binary_sum: an aggregate that combines many matrices (one per row)
 * into a single result using LAGraph_Matrix_Binary_Sum.
 *
 * This is the sibling of matrix_sum (src/matrix/matrix_sum.c): same interface,
 * same result, different summing technique inside LAGraph. Where
 * LAGraph_Matrix_Sum concatenates the tuples of all inputs and does a single
 * GrB_Matrix_build with a dup operator, LAGraph_Matrix_Binary_Sum combines the
 * inputs with a different strategy (a binary reduction tree of GrB_eWiseAdds);
 * both yield identical results for a commutative+associative dup operator but
 * trade off parallelism, time, and peak memory differently. matrix_agg
 * (pairwise eWiseAdd into a running accumulator) is the serial baseline; this
 * brings the family of summing aggregates to three.
 *
 * The dup operator defaults to PLUS (so matrix_binary_sum(m) is the element-wise
 * sum); an optional binaryop argument overrides it. dup must be commutative and
 * associative for the result to be well defined.
 *
 * Builtin types only: LAGraph_Matrix_Binary_Sum returns GrB_NOT_IMPLEMENTED for
 * user-defined types.
 *
 * Gated behind ONESPARSE_HAVE_LAGRAPH_MATRIX_SUM (same flag as matrix_sum)
 * because LAGraph_Matrix_Binary_Sum only exists on the LAGraph "matrix-sum"
 * branch (https://github.com/GraphBLAS/LAGraph/pull/407). Build with
 * `make WITH_MATRIX_SUM=1` against that branch to enable it.
 */

#ifdef ONESPARSE_HAVE_LAGRAPH_MATRIX_SUM

typedef struct MatrixBinarySumState
{
	GrB_Matrix *mats;			/* dup'd input handles, grown geometrically */
	int			n;				/* number of handles collected */
	int			cap;			/* allocated slots in mats */
	GrB_Type	type;			/* element type, fixed by the first row */
	GrB_BinaryOp dup;			/* duplicate combiner for the final sum */
} MatrixBinarySumState;

/*
 * Free the collected dup'd input matrices when the aggregate's memory context
 * is reset/deleted. GraphBLAS objects are malloc'd (not in a PG context), so
 * they must be freed explicitly to avoid leaks (including on error/abort).
 */
static void
context_callback_matrix_binary_sum_free(void *ptr)
{
	MatrixBinarySumState *state = (MatrixBinarySumState *) ptr;
	int			i;

	for (i = 0; i < state->n; i++)
	{
		if (state->mats[i] != NULL)
			OS_CHECK(GrB_Matrix_free(&state->mats[i]),
					 state->mats[i],
					 "Cannot GrB_Free matrix_binary_sum input matrix");
	}
}

PG_FUNCTION_INFO_V1(matrix_binary_sum_matrix);
Datum
matrix_binary_sum_matrix(PG_FUNCTION_ARGS)
{
	GrB_Type	type;
	os_Matrix  *A;
	GrB_Matrix	dupmat;
	GrB_BinaryOp op;
	MatrixBinarySumState *state;
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

		state = MemoryContextAllocZero(aggcontext, sizeof(MatrixBinarySumState));
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
		ctxcb->func = context_callback_matrix_binary_sum_free;
		ctxcb->arg = state;
		MemoryContextRegisterResetCallback(aggcontext, ctxcb);
	}
	else
	{
		state = (MatrixBinarySumState *) PG_GETARG_POINTER(0);
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
			 "Cannot dup matrix in matrix_binary_sum_matrix");
	state->mats[state->n++] = dupmat;

	PG_RETURN_POINTER(state);
}

PG_FUNCTION_INFO_V1(matrix_binary_sum_final);
Datum
matrix_binary_sum_final(PG_FUNCTION_ARGS)
{
	MatrixBinarySumState *state;
	GrB_Matrix	C;
	char		msg[LAGRAPH_MSG_LEN];

	if (PG_ARGISNULL(0))
		PG_RETURN_NULL();

	state = (MatrixBinarySumState *) PG_GETARG_POINTER(0);

	if (state->n == 0)
		PG_RETURN_NULL();

	LA_CHECK(LAGraph_Matrix_Binary_Sum(&C,
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
