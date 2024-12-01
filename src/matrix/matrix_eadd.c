#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_eadd);
Datum matrix_eadd(PG_FUNCTION_ARGS)
{
	GrB_Type utype, vtype, wtype;
	os_Matrix *u, *v, *w;
	GrB_Matrix mask;
	GrB_Descriptor descriptor;
	GrB_BinaryOp op, accum;
	GrB_Index nrows, ncols;
	int nargs;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	nargs = PG_NARGS();
	u = OS_GETARG_MATRIX(0);
	v = OS_GETARG_MATRIX(1);

	OS_MTYPE(utype, u);
	OS_MTYPE(vtype, v);
	wtype = type_promote(utype, vtype);

	op = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 2);
	if (op == NULL)
	{
		op = default_binaryop(wtype);
	}

	if (PG_ARGISNULL(3))
	{
		OS_MNROWS(nrows, u);
		OS_MNCOLS(ncols, u);
		w = new_matrix(wtype, nrows, ncols, CurrentMemoryContext, NULL);
	}
	else
		w = OS_GETARG_MATRIX(3);

	mask = OS_GETARG_MATRIX_HANDLE_OR_NULL(nargs, 4);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_eWiseAdd(w->matrix,
						  mask,
						  accum,
						  op,
						  u->matrix,
						  v->matrix,
						  descriptor),
		  w->matrix,
		  "Error matrix eWiseAdd.");

	OS_RETURN_MATRIX(w);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
