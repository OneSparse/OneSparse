#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_assign_row_vector);
Datum
matrix_assign_row_vector(PG_FUNCTION_ARGS)
{
	GrB_Type	type;
	os_Matrix  *C;
	os_Vector  *v;
	GrB_Vector	mask;
	GrB_BinaryOp accum;
	GrB_Descriptor descriptor;
	GrB_Index	nrows,
				ncols,
				nj = 0,
				i,
			   *cols = NULL;
	int			nargs;

	nargs = PG_NARGS();
	C = OS_GETARG_MATRIX(0);
	v = OS_GETARG_VECTOR(1);
	i = PG_GETARG_INT64(2);

	OS_MTYPE(type, C);
	OS_MNROWS(nrows, C);
	OS_MNCOLS(ncols, C);

	nj = ncols;
	if (!PG_ARGISNULL(3))
	{
		cols = get_c_array_from_pg_array(fcinfo, 3, &nj);
	}

	mask = OS_GETARG_VECTOR_HANDLE_OR_NULL(nargs, 4);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_Row_assign(C->matrix,
							mask,
							accum,
							v->vector,
							i,
							cols ? cols : GrB_ALL,
							nj,
							descriptor),
			 C->matrix,
			 "Error in assign row vector.");

	OS_RETURN_MATRIX(C);
}

SUPPORT_FN(matrix_assign_row_vector, linitial);
