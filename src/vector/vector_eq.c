#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_eq);
Datum vector_eq(PG_FUNCTION_ARGS)
{
	GrB_Type utype, vtype;
	os_Vector *u, *v, *w;
	GrB_Index usize, vsize, unvals, vnvals;
	bool result;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	u = OS_GETARG_VECTOR(0);
	v = OS_GETARG_VECTOR_A(1, u);
	if (u == v)
		PG_RETURN_BOOL(true);

	OS_VTYPE(utype, u);
	OS_VTYPE(vtype, v);
	if (utype != vtype)
		PG_RETURN_BOOL(false);

	OS_VSIZE(usize, u);
	OS_VSIZE(vsize, v);
	if (usize != vsize)
		PG_RETURN_BOOL(false);

	OS_VNVALS(unvals, u);
	OS_VNVALS(vnvals, v);
	if (unvals != vnvals)
		PG_RETURN_BOOL(false);

	w = new_vector(GrB_BOOL, usize, CurrentMemoryContext, NULL);
	OS_CHECK(GrB_eWiseMult(w->vector,
						  NULL,
						  NULL,
						  GrB_EQ_BOOL,
						  u->vector,
						  v->vector,
						  NULL),
			 w->vector,
			 "Error vector eWiseAdd.");

	OS_VNVALS(unvals, u);
	OS_VNVALS(vnvals, w);
	if (unvals != vnvals)
		PG_RETURN_BOOL(false);

	OS_CHECK(GrB_Vector_reduce_BOOL(
			  &result,
			  NULL,
			  GrB_LAND_MONOID_BOOL,
			  w->vector,
			  NULL),
			 w->vector,
			 "Cannot reduce vector to scalar in eq");

	PG_RETURN_BOOL(result);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
