#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_extract_matrix);
Datum matrix_extract_matrix(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *A, *C;
	GrB_Matrix mask;
	GrB_BinaryOp accum;
	GrB_Descriptor descriptor;
	GrB_Index nrows, ncols, ni = 0, nj = 0, *rows = NULL, *cols = NULL;
	int nargs;

	nargs = PG_NARGS();
	A = OS_GETARG_MATRIX(0);
	OS_MTYPE(type, A);
	OS_MNROWS(nrows, A);
	OS_MNCOLS(ncols, A);

	ni = nrows;
	nj = ncols;
	if (!PG_ARGISNULL(1))
	{
		rows = get_c_array_from_pg_array(fcinfo, 1, &ni);
	}

	if (!PG_ARGISNULL(2))
	{
		cols = get_c_array_from_pg_array(fcinfo, 2, &nj);
	}

	if (PG_ARGISNULL(3))
	{
		C = new_matrix(type, ni, nj, CurrentMemoryContext, NULL);
	}
	else
	{
		C = OS_GETARG_MATRIX_A(3, A);
	}

	mask = OS_GETARG_MATRIX_HANDLE_OR_NULL_AB(nargs, 4, A, C);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_extract(C->matrix,
						 mask,
						 accum,
						 A->matrix,
						 rows ? rows : GrB_ALL,
						 ni,
						 cols ? cols : GrB_ALL,
						 nj,
						 descriptor),
			 C->matrix,
			 "Error in extract matrix.");

	OS_RETURN_MATRIX(C);
}

SUPPORT_FN(matrix_extract_matrix, lfourth);

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
