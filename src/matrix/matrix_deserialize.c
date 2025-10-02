#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_deserialize);
Datum
matrix_deserialize(PG_FUNCTION_ARGS)
{
	os_Matrix  *matrix;
	GrB_Matrix	m;
	bytea	   *input;
	struct timeval start,
				end;

	input = PG_GETARG_BYTEA_P(0);

	OS_START_BENCH();
	OS_CHECK(GxB_Matrix_deserialize(
									&m,
									NULL, //TODO, support udts
									VARDATA_ANY(input),
									VARSIZE_ANY_EXHDR(input),
									NULL),
			 m,
			 "Error deserializing matrix");
	OS_END_BENCH();

	matrix = new_matrix(NULL, 0, 0, CurrentMemoryContext, m);
	OS_RETURN_MATRIX(matrix);
}
