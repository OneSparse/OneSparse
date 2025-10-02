#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_reduce_scalar);
Datum
matrix_reduce_scalar(PG_FUNCTION_ARGS)
{
	GrB_Type	type;
	os_Matrix  *A;
	GrB_Monoid	monoid;
	GrB_BinaryOp accum;
	GrB_Descriptor descriptor;
	os_Scalar  *result;
	int			nargs;
	struct timeval start,
				end;

	OS_START_BENCH();

	nargs = PG_NARGS();
	A = OS_GETARG_MATRIX(0);
	OS_MTYPE(type, A);

	monoid = OS_GETARG_MONOID_HANDLE_OR_NULL(nargs, 1);
	if (monoid == NULL)
	{
		monoid = default_monoid(type);
	}

	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 2);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 3);

	result = new_scalar(type, CurrentMemoryContext, NULL);

	OS_CHECK(GrB_Matrix_reduce_Monoid_Scalar(
											 result->scalar,
											 accum,
											 monoid,
											 A->matrix,
											 descriptor),
			 result->scalar,
			 "Cannot reduce matrix to scalar");

	OS_END_BENCH();
	OS_RETURN_SCALAR(result);
}
