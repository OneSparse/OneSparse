#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_extract_vector);
Datum
vector_extract_vector(PG_FUNCTION_ARGS)
{
	GrB_Type	type;
	os_Vector  *u,
			   *w;
	GrB_Vector	mask;
	GrB_BinaryOp accum;
	GrB_Descriptor descriptor;
	GrB_Index	size,
				ni = 0,
			   *indexes = NULL;
	int			nargs;

	LOGF();
	nargs = PG_NARGS();
	u = OS_GETARG_VECTOR(0);
	OS_VTYPE(type, u);
	OS_VSIZE(size, u);
	ni = size;

	if (!PG_ARGISNULL(1))
	{
		indexes = get_c_array_from_pg_array(fcinfo, 1, &ni);
	}

	if (PG_ARGISNULL(2))
	{
		w = new_vector(type, ni, CurrentMemoryContext, NULL);
	}
	else
	{
		w = OS_GETARG_VECTOR_A(2, u);
	}

	mask = OS_GETARG_VECTOR_HANDLE_OR_NULL_AB(nargs, 3, u, w);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 4);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 5);

	OS_CHECK(GrB_extract(w->vector,
						 mask,
						 accum,
						 u->vector,
						 indexes ? indexes : GrB_ALL,
						 ni,
						 descriptor),
			 w->vector,
			 "Error in extract vector.");
	OS_RETURN_VECTOR(w);
}

SUPPORT_FN(vector_extract_vector, lthird);
