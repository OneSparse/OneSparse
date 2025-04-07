#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_apply);
Datum vector_apply(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Vector *u, *w;
	os_UnaryOp *op;
	GrB_Vector mask;
	GrB_Descriptor descriptor;
	GrB_BinaryOp accum;
	GrB_Index usize;
	int nargs;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	u = OS_GETARG_VECTOR(0);
	op = OS_GETARG_UNARYOP(1);

	mask = NULL;
	accum = NULL;
	descriptor = NULL;
	nargs = PG_NARGS();

	if (PG_ARGISNULL(2))
	{
		OS_VTYPE(type, u);
		OS_VSIZE(usize, u);
		w = new_vector(type, usize, CurrentMemoryContext, NULL);
	}
	else
		w = OS_GETARG_VECTOR_A(2, u);

	mask = OS_GETARG_VECTOR_HANDLE_OR_NULL_AB(nargs, 3, u, w);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 4);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 5);

	OS_CHECK(GrB_apply(w->vector,
					   mask,
					   accum,
					   op->unaryop,
					   u->vector,
					   descriptor),
			 w->vector,
			 "Error in GrB_Vector_apply");

	OS_RETURN_VECTOR(w);
}

SUPPORT_FN(vector_apply, lthird);

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
