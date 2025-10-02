#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_eunion);
Datum
matrix_eunion(PG_FUNCTION_ARGS)
{
	GrB_Type	atype,
				btype,
				ctype;
	os_Matrix  *a,
			   *b,
			   *c;
	GrB_Matrix	mask;
	os_Scalar  *alpha,
			   *beta;
	GrB_Descriptor descriptor;
	GrB_BinaryOp op;
	GrB_BinaryOp accum;
	GrB_Index	nrows,
				ncols;
	int			nargs;
	struct timeval start,
				end;

	OS_START_BENCH();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);
	ERRORNULL(3);

	nargs = PG_NARGS();
	a = OS_GETARG_MATRIX(0);
	alpha = OS_GETARG_SCALAR(1);
	b = OS_GETARG_MATRIX_A(2, a);
	beta = OS_GETARG_SCALAR(3);

	OS_MTYPE(atype, a);
	OS_MTYPE(btype, b);
	ctype = type_promote(atype, btype);

	op = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 4);
	if (op == NULL)
	{
		op = default_binaryop(ctype);
	}
	if (PG_ARGISNULL(5))
	{
		OS_MNROWS(nrows, a);
		OS_MNCOLS(ncols, a);
		c = new_matrix(ctype, nrows, ncols, CurrentMemoryContext, NULL);
	}
	else
		c = OS_GETARG_MATRIX(5);

	mask = OS_GETARG_MATRIX_HANDLE_OR_NULL_ABC(nargs, 6, a, b, c);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 7);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 8);

	OS_CHECK(GxB_eWiseUnion(c->matrix,
							mask,
							accum,
							op,
							a->matrix,
							alpha->scalar,
							b->matrix,
							beta->scalar,
							descriptor),
			 c->matrix,
			 "Error matrix eWiseUnion.");
	OS_END_BENCH();
	OS_RETURN_MATRIX(c);
}

#define lsixth(l) lfirst(list_nth_cell(l, 5))
SUPPORT_FN(matrix_eunion, lsixth);
#undef lsixth
