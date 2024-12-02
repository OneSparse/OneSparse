#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_apply);
Datum vector_apply(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Vector *u, *w, *mask;
	os_Descriptor *descriptor;
	os_BinaryOp *accum;
	os_UnaryOp *op;
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
		w = OS_GETARG_VECTOR(2);

	mask = OS_GETARG_VECTOR_OR_NULL(nargs, 3);
	accum = OS_GETARG_BINARYOP_OR_NULL(nargs, 4);
	descriptor = OS_GETARG_DESCRIPTOR_OR_NULL(nargs, 5);

	OS_CHECK(GrB_apply(w->vector,
					mask ? mask->vector : NULL,
					accum ? accum->binaryop : NULL,
					op->unaryop,
					u->vector,
					descriptor ? descriptor->descriptor : NULL),
		  w->vector,
		  "Error in GrB_Vector_apply");

	OS_RETURN_VECTOR(w);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
