#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_mxv);
Datum matrix_mxv(PG_FUNCTION_ARGS)
{
	GrB_Type atype, btype, ctype;
	os_Matrix *a;
	os_Vector *b, *c;
	GrB_Vector mask;
	GrB_Descriptor descriptor;
	GrB_BinaryOp accum;
	GrB_Semiring semiring;
	GrB_Index bsize;
	int nargs;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	nargs = PG_NARGS();
	a = OS_GETARG_MATRIX(0);
	b = OS_GETARG_VECTOR(1);

	OS_MTYPE(atype, a);
	OS_VTYPE(btype, b);
	ctype = type_promote(atype, btype);

	semiring = OS_GETARG_SEMIRING_HANDLE_OR_NULL(nargs, 2);
	if (semiring == NULL)
	{
		semiring = default_semiring(ctype);
	}

	if (PG_ARGISNULL(3))
	{
		OS_VSIZE(bsize, b);
		c = new_vector(ctype, bsize, CurrentMemoryContext, NULL);
	}
	else
		c = OS_GETARG_VECTOR(3);

	mask = OS_GETARG_VECTOR_HANDLE_OR_NULL(nargs, 4);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_mxv(c->vector,
					 mask,
					 accum,
					 semiring,
					 a->matrix,
					 b->vector,
					 descriptor),
		  c->vector,
		  "Error matrix mxv.");

	OS_RETURN_VECTOR(c);
}

SUPPORT_FN(matrix_mxv, lfourth);

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
