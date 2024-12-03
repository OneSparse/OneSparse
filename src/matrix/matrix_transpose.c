#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_transpose);
Datum matrix_transpose(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *a, *c;
	GrB_Matrix mask;
	GrB_Descriptor descriptor;
	GrB_BinaryOp accum;
	GrB_Index nrows, ncols;
	int nargs;

	LOGF();
	ERRORNULL(0);

	nargs = PG_NARGS();
	a = OS_GETARG_MATRIX(0);

	if (PG_ARGISNULL(1))
	{
		OS_MTYPE(type, a);
		OS_MNROWS(nrows, a);
		OS_MNCOLS(ncols, a);
		c = new_matrix(type, nrows, ncols, CurrentMemoryContext, NULL);
	}
	else
		c = OS_GETARG_MATRIX(1);

	mask = OS_GETARG_MATRIX_HANDLE_OR_NULL(nargs, 2);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 3);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 4);

	OS_CHECK(GrB_transpose(c->matrix,
						   mask,
						   accum,
						   a->matrix,
						   descriptor),
		  c->matrix,
		  "Error in transpose");
	OS_RETURN_MATRIX(c);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */