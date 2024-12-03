#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_apply_second);
Datum matrix_apply_second(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *a, *c;
	os_Scalar *s;
	GrB_Matrix mask;
	GrB_Descriptor descriptor;
	GrB_BinaryOp accum;
	GrB_BinaryOp binaryop;
	GrB_Index nrows, ncols;
	int nargs;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	nargs = PG_NARGS();
	a = OS_GETARG_MATRIX(0);
	s = OS_GETARG_SCALAR(1);

	OS_MTYPE(type, a);
	binaryop = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 2);
	if (binaryop == NULL)
	{
		binaryop = default_binaryop(type);
	}

	if (PG_ARGISNULL(3))
	{
		OS_MNROWS(nrows, a);
		OS_MNCOLS(ncols, a);
		c = new_matrix(type, nrows, ncols, CurrentMemoryContext, NULL);
	}
	else
		c = OS_GETARG_MATRIX(3);

	mask = OS_GETARG_MATRIX_HANDLE_OR_NULL(nargs, 4);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_apply(c->matrix,
					   mask,
					   accum,
					   binaryop,
					   a->matrix,
					   s->scalar,
					   descriptor),
			 c->matrix,
			 "Error in grb_matrix_apply_binaryop1st");
	OS_RETURN_MATRIX(c);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */