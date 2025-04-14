#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_apply_second);
Datum vector_apply_second(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Vector *u, *w;
	os_Scalar *s;
	GrB_Vector mask;
	GrB_Descriptor descriptor;
	GrB_BinaryOp accum;
	GrB_BinaryOp binaryop;
	GrB_Index usize;
	int nargs;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	nargs = PG_NARGS();
	u = OS_GETARG_VECTOR(0);
	s = OS_GETARG_SCALAR(1);

	OS_VTYPE(type, u);
	binaryop = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 2);
	if (binaryop == NULL)
	{
		binaryop = default_binaryop(type);
	}

	if (PG_ARGISNULL(3))
	{
		OS_VSIZE(usize, u);
		w = new_vector(type, usize, CurrentMemoryContext, NULL);
	}
	else
		w = OS_GETARG_VECTOR_A(3, u);

	mask = OS_GETARG_VECTOR_HANDLE_OR_NULL_AB(nargs, 4, u, w);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_apply(w->vector,
					   mask,
					   accum,
					   binaryop,
					   u->vector,
					   s->scalar,
					   descriptor),
			 w->vector,
			 "Error in grb_vector_apply_binaryop1st");
	OS_RETURN_VECTOR(w);
}

SUPPORT_FN(vector_apply_second, lfourth);

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
