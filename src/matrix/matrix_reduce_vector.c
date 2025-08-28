#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_reduce_vector);
Datum matrix_reduce_vector(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *a;
	os_Vector *w;
	GrB_Vector mask;
	GrB_Descriptor descriptor;
	GrB_Monoid monoid;
	GrB_BinaryOp accum;
	GrB_Index vsize;
	int nargs;
    struct timeval start, end;

	OS_START_BENCH();
	ERRORNULL(0);

	nargs = PG_NARGS();
	a = OS_GETARG_MATRIX(0);

	OS_MTYPE(type, a);

	monoid = OS_GETARG_MONOID_HANDLE_OR_NULL(nargs, 1);
	if (monoid == NULL)
	{
		monoid = default_monoid(type);
	}

	if (PG_ARGISNULL(2))
	{
		OS_MNROWS(vsize, a);
		w = new_vector(type, vsize, CurrentMemoryContext, NULL);
	}
	else
		w = OS_GETARG_VECTOR(2);

	mask = OS_GETARG_VECTOR_HANDLE_OR_NULL(nargs, 3);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 4);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 5);

	OS_CHECK(GrB_Matrix_reduce_Monoid(
				 w->vector,
				 mask,
				 accum,
				 monoid,
				 a->matrix,
				 descriptor),
			 w->vector,
			 "Error matrix vector reduce.");

	OS_END_BENCH();
	OS_RETURN_VECTOR(w);
}

SUPPORT_FN(matrix_reduce_vector, lthird);

