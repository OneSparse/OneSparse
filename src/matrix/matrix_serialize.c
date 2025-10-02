#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_serialize);
Datum
matrix_serialize(PG_FUNCTION_ARGS)
{
	os_Matrix  *matrix;
	void	   *data;
	size_t		size;
	bytea	   *result;
	struct timeval start,
				end;

	OS_START_BENCH();
	matrix = OS_GETARG_MATRIX(0);

	OS_CHECK(GxB_Matrix_serialize(
								  &data,
								  &size,
								  matrix->matrix,
								  NULL),
			 matrix->matrix,
			 "Error serializing matrix");

	result = (bytea *) palloc(VARHDRSZ + size);
	SET_VARSIZE(result, VARHDRSZ + size);
	memcpy(VARDATA(result), data, size);

	OS_END_BENCH();
	PG_RETURN_BYTEA_P(result);
}
