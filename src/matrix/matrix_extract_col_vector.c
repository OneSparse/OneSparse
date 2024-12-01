#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_extract_col_vector);
Datum
matrix_extract_col_vector(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *A;
	os_Vector *C;
	GrB_Vector mask;
	GrB_BinaryOp accum;
	GrB_Descriptor descriptor;
	GrB_Index size, col, nj, *indexes = NULL;
	int nargs;

	nargs = PG_NARGS();
	A = OS_GETARG_MATRIX(0);
	col = PG_GETARG_INT64(1);
	OS_MTYPE(type, A);
	OS_MNROWS(size, A);

	nj = size;
	if (!PG_ARGISNULL(2))
	{
		indexes = get_c_array_from_pg_array(fcinfo, 2, &nj);
	}

	if (PG_ARGISNULL(3))
	{
		C = new_vector(type, nj, CurrentMemoryContext, NULL);
	}
	else
	{
		C = OS_GETARG_VECTOR(3);
	}

	mask = OS_GETARG_VECTOR_HANDLE_OR_NULL(nargs, 4);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_Col_extract(C->vector,
							 mask,
							 accum,
							 A->matrix,
							 indexes ? indexes : GrB_ALL,
							 nj,
							 col,
							 descriptor),
			 C->vector,
			 "Error in column vector matrix.");

	OS_RETURN_VECTOR(C);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
