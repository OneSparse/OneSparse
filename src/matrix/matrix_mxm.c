#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_mxm);
Datum matrix_mxm(PG_FUNCTION_ARGS)
{
	GrB_Type atype, btype, ctype;
	os_Matrix *a, *b, *c;
	GrB_Matrix mask;
	GrB_Descriptor descriptor;
	GrB_BinaryOp accum;
	GrB_Semiring semiring;
	GrB_Index nrows, ncols;
	int nargs;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	nargs = PG_NARGS();
	a = OS_GETARG_MATRIX(0);
	b = OS_GETARG_MATRIX(1);

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
		OS_MNROWS(nrows, a);
		OS_MNCOLS(ncols, b);
		c = new_matrix(ctype, nrows, ncols, CurrentMemoryContext, NULL);
	}
	else
		c = OS_GETARG_MATRIX(3);

	mask = OS_GETARG_MATRIX_HANDLE_OR_NULL(nargs, 4);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_mxm(c->matrix,
					 mask,
					 accum,
					 semiring,
					 a->matrix,
					 b->matrix,
					 descriptor),
		  c->matrix,
		  "Error matrix mxm.");

	OS_RETURN_MATRIX(c);
}

SUPPORT_FN(matrix_mxm, lfourth);

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
