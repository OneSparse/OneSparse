#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_eadd);
Datum vector_eadd(PG_FUNCTION_ARGS)
{
	GrB_Type wtype, utype, vtype;
	os_Vector *u, *v, *w;
	GrB_Vector mask;
	GrB_Descriptor descriptor;
	GrB_BinaryOp op, accum;
	GrB_Index usize;
	int nargs;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	u = OS_GETARG_VECTOR(0);
	v = OS_GETARG_VECTOR(1);
	nargs = PG_NARGS();

	OS_VTYPE(utype, u);
	OS_VTYPE(vtype, v);
	wtype = type_promote(utype, vtype);

	op = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 2);
	if (op == NULL)
	{
		op = default_binaryop(wtype);
	}

	if (PG_ARGISNULL(3))
	{
		OS_VSIZE(usize, u);
		w = new_vector(wtype, usize, CurrentMemoryContext, NULL);
	}
	else
		w = OS_GETARG_VECTOR(3);

	mask = OS_GETARG_VECTOR_HANDLE_OR_NULL(nargs, 4);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_eWiseAdd(w->vector,
						  mask,
						  accum,
						  op,
						  u->vector,
						  v->vector,
						  descriptor),
			 w->vector,
			 "Error vector eWiseAdd.");

	OS_RETURN_VECTOR(w);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
