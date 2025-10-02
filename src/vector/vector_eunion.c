#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_eunion);
Datum
vector_eunion(PG_FUNCTION_ARGS)
{
	GrB_Type	wtype,
				utype,
				vtype;
	os_Vector  *u,
			   *v,
			   *w;
	GrB_Vector	mask;
	os_Scalar  *a,
			   *b;
	GrB_Descriptor descriptor;
	GrB_BinaryOp op,
				accum;
	GrB_Index	usize;
	int			nargs;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);
	ERRORNULL(3);

	u = OS_GETARG_VECTOR(0);
	a = OS_GETARG_SCALAR(1);
	v = OS_GETARG_VECTOR_A(2, u);
	b = OS_GETARG_SCALAR(3);
	nargs = PG_NARGS();

	OS_VTYPE(utype, u);
	OS_VTYPE(vtype, v);
	wtype = type_promote(utype, vtype);

	op = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 4);
	if (op == NULL)
	{
		op = default_binaryop(wtype);
	}

	if (PG_ARGISNULL(5))
	{
		OS_VSIZE(usize, u);
		w = new_vector(wtype, usize, CurrentMemoryContext, NULL);
	}
	else
		w = OS_GETARG_VECTOR_AB(5, u, v);

	mask = OS_GETARG_VECTOR_HANDLE_OR_NULL_ABC(nargs, 6, u, v, w);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 7);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 8);

	OS_CHECK(GxB_eWiseUnion(w->vector,
							mask,
							accum,
							op,
							u->vector,
							a->scalar,
							v->vector,
							b->scalar,
							descriptor),
			 w->vector,
			 "Error vector eWiseUnion.");
	OS_RETURN_VECTOR(w);
}

#define lsixth(l) lfirst(list_nth_cell(l, 5))
SUPPORT_FN(vector_eunion, lsixth);
#undef lsixth
