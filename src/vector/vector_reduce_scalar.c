#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_reduce_scalar);
Datum
vector_reduce_scalar(PG_FUNCTION_ARGS)
{
	os_Vector *A;
	GrB_Monoid monoid;
	GrB_BinaryOp accum;
	GrB_Descriptor descriptor;
	os_Scalar *result;
	GrB_Type type;
	int nargs;

	nargs = PG_NARGS();
	A = OS_GETARG_VECTOR(0);
	OS_VTYPE(type, A);

	monoid = OS_GETARG_MONOID_HANDLE_OR_NULL(nargs, 1);
	if (monoid == NULL)
	{
		monoid = default_monoid(type);
	}

	if (PG_ARGISNULL(2))
	{
		result = new_scalar(type, CurrentMemoryContext, NULL);

	}
	else
		result = OS_GETARG_SCALAR(2);

	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 3);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 4);

	OS_CHECK(GrB_Vector_reduce_Monoid_Scalar(
			  result->scalar,
			  accum,
			  monoid,
			  A->vector,
			  descriptor),
		  result->scalar,
		  "Cannot reduce vector to scalar");
	OS_RETURN_SCALAR(result);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
