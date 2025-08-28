#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_agg_matrix);
Datum matrix_agg_matrix(PG_FUNCTION_ARGS)
{
	GrB_Type type;
    os_Matrix *state, *A;
	GrB_BinaryOp op;
	MemoryContext aggcontext;
	int nargs;

    if (PG_ARGISNULL(1))
    {
		elog(ERROR, "Cannot aggregate a null value to a matrix");
	}

	A = OS_GETARG_MATRIX(1);
	OS_MTYPE(type, A);

    if (PG_ARGISNULL(0))
    {
        if (!AggCheckCallContext(fcinfo, &aggcontext)) {
            elog(ERROR, "aggregate function called in non-aggregate context");
        }
		state = new_matrix(type, GxB_INDEX_MAX, GxB_INDEX_MAX, aggcontext, NULL);
    }
    else
    {
        state = OS_GETARG_MATRIX(0);
    }

	nargs = PG_NARGS();
	op = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 2);
	if (op == NULL)
	{
		op = default_binaryop(type);
	}

	OS_CHECK(GrB_eWiseAdd(state->matrix,
						  NULL,
						  NULL,
						  op,
						  state->matrix,
						  A->matrix,
						  NULL),
		  state->matrix,
		  "Error matrix eadd in matrix_agg_matrix.");

    OS_RETURN_MATRIX(state);
}

