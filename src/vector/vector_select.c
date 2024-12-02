#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_select);
Datum vector_select(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Vector *u, *w, *mask;
	os_Scalar *y;
	os_Descriptor *descriptor;
	os_BinaryOp *accum;
	os_IndexUnaryOp *op;
	GrB_Index usize;
	int nargs;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	u = OS_GETARG_VECTOR(0);
	op = OS_GETARG_INDEXUNARYOP(1);
	y = OS_GETARG_SCALAR(2);

	mask = NULL;
	accum = NULL;
	descriptor = NULL;
	nargs = PG_NARGS();

	if (PG_ARGISNULL(3))
	{
		OS_VTYPE(type, u);
		OS_VSIZE(usize, u);
		w = new_vector(type, usize, CurrentMemoryContext, NULL);
	}
	else
		w = OS_GETARG_VECTOR(3);

	mask = OS_GETARG_VECTOR_OR_NULL(nargs, 4);
	accum = OS_GETARG_BINARYOP_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_OR_NULL(nargs, 6);

	OS_CHECK(GrB_select(w->vector,
					 mask ? mask->vector : NULL,
					 accum ? accum->binaryop : NULL,
					 op->indexunaryop,
					 u->vector,
					 y ? y->scalar : NULL,
					 descriptor ? descriptor->descriptor : NULL),
		  w->vector,
		  "Error in GrB_select");

	OS_RETURN_VECTOR(w);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
