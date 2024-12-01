#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_assign_col_vector);
Datum matrix_assign_col_vector(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *C;
	os_Vector *v;
	GrB_Vector mask;
	GrB_BinaryOp accum;
	GrB_Descriptor descriptor;
	GrB_Index nrows, ncols, ni = 0, j, *rows = NULL;
	int nargs;

	nargs = PG_NARGS();
	C = OS_GETARG_MATRIX(0);
	v = OS_GETARG_VECTOR(1);
	j = PG_GETARG_INT64(2);

	OS_MTYPE(type, C);
	OS_MNROWS(nrows, C);
	OS_MNCOLS(ncols, C);

	ni = ncols;
	if (!PG_ARGISNULL(3))
	{
		rows = get_c_array_from_pg_array(fcinfo, 3, &ni);
	}

	mask = OS_GETARG_VECTOR_HANDLE_OR_NULL(nargs, 4);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_Col_assign(C->matrix,
							mask,
							accum,
							v->vector,
							rows ? rows : GrB_ALL,
							ni,
							j,
							descriptor),
			 C->matrix,
			 "Error in assign matrix col vector.");

	OS_RETURN_MATRIX(C);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
