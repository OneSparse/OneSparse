#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_vxv);
Datum vector_vxv(PG_FUNCTION_ARGS)
{
	GrB_Type atype, btype, ctype;
	os_Vector *a, *b, *c;
	GrB_Matrix tmp;
	GrB_Vector mask;
	// GrB_Descriptor descriptor;
	GrB_BinaryOp accum;
	GrB_Semiring semiring;
	GrB_Index asize;
	int nargs;
    struct timeval start, end;

	ERRORNULL(0);
	ERRORNULL(1);

	nargs = PG_NARGS();
	a = OS_GETARG_VECTOR(0);
	b = OS_GETARG_VECTOR_A(1, a);

	OS_VTYPE(atype, a);
	OS_VTYPE(btype, b);
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
		c = OS_GETARG_VECTOR_AB(3, a, b);

	mask = OS_GETARG_VECTOR_HANDLE_OR_NULL(nargs, 4);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	//descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_Matrix_new(&tmp, ctype, asize, asize),
			 tmp,
			 "Error constructing tmp vxv matrix.");

	OS_START_BENCH();
	OS_CHECK(GrB_mxm(
				 tmp,
				 (GrB_Matrix)mask,
				 accum,
				 semiring,
				 (GrB_Matrix)a->vector,
				 (GrB_Matrix)b->vector,
				 GrB_DESC_T0
				 ),
			 c->vector,
			 "Error vector_vxv.");

	OS_CHECK(GrB_Col_extract(
				 c->vector,
				 NULL,
				 NULL,
				 tmp,
				 GrB_ALL,
				 0,
				 0,
				 NULL
				 ),
			 c->vector,
			 "Error extracting vxv vector");

	OS_END_BENCH();
	OS_RETURN_VECTOR(c);
}

SUPPORT_FN(vector_vxv, lfourth);

