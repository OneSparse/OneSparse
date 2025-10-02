#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_vxm);
Datum
matrix_vxm(PG_FUNCTION_ARGS)
{
	GrB_Type	atype,
				btype,
				ctype;
	os_Vector  *a,
			   *c;
	os_Matrix  *b;
	GrB_Vector	mask;
	GrB_Descriptor descriptor;
	GrB_BinaryOp accum;
	GrB_Semiring semiring;
	GrB_Index	asize;
	int			nargs;
	struct timeval start,
				end;

	ERRORNULL(0);
	ERRORNULL(1);

	nargs = PG_NARGS();
	a = OS_GETARG_VECTOR(0);
	b = OS_GETARG_MATRIX(1);

	OS_VTYPE(atype, a);
	OS_MTYPE(btype, b);
	ctype = type_promote(atype, btype);

	semiring = OS_GETARG_SEMIRING_HANDLE_OR_NULL(nargs, 2);
	if (semiring == NULL)
	{
		semiring = default_semiring(ctype);
	}

	if (PG_ARGISNULL(3))
	{
		OS_VSIZE(asize, a);
		c = new_vector(ctype, asize, CurrentMemoryContext, NULL);
	}
	else
		c = OS_GETARG_VECTOR(3);

	mask = OS_GETARG_VECTOR_HANDLE_OR_NULL(nargs, 4);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_START_BENCH();
	OS_CHECK(GrB_vxm(c->vector,
					 mask,
					 accum,
					 semiring,
					 a->vector,
					 b->matrix,
					 descriptor),
			 c->vector,
			 "Error matrix mxv.");

	OS_END_BENCH();
	OS_RETURN_VECTOR(c);
}

SUPPORT_FN(matrix_vxm, lfourth);
