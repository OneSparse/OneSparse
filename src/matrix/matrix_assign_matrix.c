#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_assign_matrix);
Datum matrix_assign_matrix(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *A, *C;
	GrB_Matrix mask;
	GrB_BinaryOp accum;
	GrB_Descriptor descriptor;
	GrB_Index nrows, ncols, ni = 0, nj = 0, *rows = NULL, *cols = NULL;
	int nargs;

	nargs = PG_NARGS();
	C = OS_GETARG_MATRIX(0);
	A = OS_GETARG_MATRIX_A(1, C);

	OS_MTYPE(type, A);
	OS_MNROWS(nrows, A);
	OS_MNCOLS(ncols, A);

	ni = nrows;
	nj = ncols;
	if (!PG_ARGISNULL(2))
	{
		rows = get_c_array_from_pg_array(fcinfo, 2, &ni);
	}

	if (!PG_ARGISNULL(3))
	{
		cols = get_c_array_from_pg_array(fcinfo, 3, &nj);
	}

	mask = OS_GETARG_MATRIX_HANDLE_OR_NULL_AB(nargs, 4, C, A);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_assign(C->matrix,
						mask,
						accum,
						A->matrix,
						rows ? rows : GrB_ALL,
						ni,
						cols ? cols : GrB_ALL,
						nj,
						descriptor),
			 C->matrix,
			 "Error in assign matrix.");

	OS_RETURN_MATRIX(C);
}

SUPPORT_FN(matrix_assign_matrix, linitial);

