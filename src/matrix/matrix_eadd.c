#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_eadd);
Datum matrix_eadd(PG_FUNCTION_ARGS)
{
	GrB_Type atype, btype, ctype;
	os_Matrix *a, *b, *c;
	GrB_Matrix mask;
	GrB_Descriptor descriptor;
	GrB_BinaryOp op, accum;
	GrB_Index nrows, ncols;
	int nargs;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	nargs = PG_NARGS();
	a = OS_GETARG_MATRIX(0);
	b = OS_GETARG_MATRIX_A(1, a);

	OS_MTYPE(atype, a);
	OS_MTYPE(btype, b);
	ctype = type_promote(atype, btype);

	op = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 2);
	if (op == NULL)
	{
		op = default_binaryop(ctype);
	}

	if (PG_ARGISNULL(3))
	{
		OS_MNROWS(nrows, a);
		OS_MNCOLS(ncols, a);
		c = new_matrix(ctype, nrows, ncols, CurrentMemoryContext, NULL);
	}
	else
		c = OS_GETARG_MATRIX_AB(3, a, b);

	mask = OS_GETARG_MATRIX_HANDLE_OR_NULL_ABC(nargs, 4, a, b, c);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_eWiseAdd(c->matrix,
						  mask,
						  accum,
						  op,
						  a->matrix,
						  b->matrix,
						  descriptor),
			 c->matrix,
			 "Error matrix eWiseAdd.");

	OS_RETURN_MATRIX(c);
}

SUPPORT_FN(matrix_eadd, lfourth);

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
