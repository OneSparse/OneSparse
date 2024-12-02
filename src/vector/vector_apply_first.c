#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_apply_first);
Datum vector_apply_first(PG_FUNCTION_ARGS)
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
	s = OS_GETARG_SCALAR(0);
	u = OS_GETARG_VECTOR(1);

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
		w = OS_GETARG_VECTOR(3);

	mask = OS_GETARG_VECTOR_HANDLE_OR_NULL(nargs, 4);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_apply(w->vector,
					   mask,
					   accum,
					   binaryop,
					   s->scalar,
					   u->vector,
					   descriptor),
			 w->vector,
			 "Error in grb_vector_apply_binaryop1st");
	OS_RETURN_VECTOR(w);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
