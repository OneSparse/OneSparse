#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_assign);
Datum
vector_assign(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Vector *u, *v;
	GrB_Vector mask;
	GrB_BinaryOp accum;
	GrB_Descriptor descriptor;
	GrB_Index size, ni = 0, *indexes = NULL;
	int nargs;

	LOGF();
	nargs = PG_NARGS();
	u = OS_GETARG_VECTOR(0);
	v = OS_GETARG_VECTOR_A(1, u);

	OS_VTYPE(type, u);
	OS_VSIZE(size, u);

	ni = size;
	if (!PG_ARGISNULL(2))
	{
		indexes = get_c_array_from_pg_array(fcinfo, 2, &ni);
	}

	mask = OS_GETARG_VECTOR_HANDLE_OR_NULL_AB(nargs, 4, u, v);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_assign(u->vector,
						mask,
						accum,
						v->vector,
						indexes ? indexes : GrB_ALL,
						ni,
						descriptor),
			 u->vector,
			 "Error in assign vector.");

	OS_RETURN_VECTOR(u);
}

SUPPORT_FN(vector_assign, linitial);

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
