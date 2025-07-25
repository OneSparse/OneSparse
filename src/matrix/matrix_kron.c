#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_kron);
Datum matrix_kron(PG_FUNCTION_ARGS)
{
	GrB_Type atype, btype, ctype;
	os_Matrix *a, *b, *c;
	GrB_Matrix mask;
	GrB_Descriptor descriptor;
	GrB_BinaryOp accum;
	GrB_Semiring semiring;
	GrB_Index anrows, ancols, bnrows, bncols;
	int nargs;
    struct timeval start, end;

	OS_START_BENCH();
	ERRORNULL(0);
	ERRORNULL(1);

	nargs = PG_NARGS();
	a = OS_GETARG_MATRIX(0);
	b = OS_GETARG_MATRIX_A(1, a);

	OS_MTYPE(atype, a);
	OS_MTYPE(btype, b);
	ctype = type_promote(atype, btype);

	semiring = OS_GETARG_SEMIRING_HANDLE_OR_NULL(nargs, 2);
	if (semiring == NULL)
	{
		semiring = default_semiring(ctype);
	}

	if (PG_ARGISNULL(3))
	{
		OS_MNROWS(anrows, a);
		OS_MNROWS(bnrows, b);
		OS_MNCOLS(ancols, a);
		OS_MNCOLS(bncols, b);
		ctype = type_promote(atype, btype);
		c = new_matrix(ctype, anrows * bncols, ancols * bnrows, CurrentMemoryContext, NULL);
	}
	else
		c = OS_GETARG_MATRIX_AB(3, a, b);

	mask = OS_GETARG_MATRIX_HANDLE_OR_NULL_ABC(nargs, 4, a, b, c);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_kronecker(c->matrix,
						   mask,
						   accum,
						   semiring,
						   a->matrix,
						   b->matrix,
						   descriptor),
			 c->matrix,
			 "Error matrix kron.");

	OS_END_BENCH();
	OS_RETURN_MATRIX(c);
}

SUPPORT_FN(matrix_kron, lfourth);

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
