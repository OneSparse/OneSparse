#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_deserialize);
Datum matrix_deserialize(PG_FUNCTION_ARGS)
{
	os_Matrix *matrix;
	GrB_Matrix m;
	bytea *input;

	LOGF();

	input = PG_GETARG_BYTEA_P(0);

	OS_CHECK(GxB_Matrix_deserialize(
			  &m,
			  NULL, // TODO, support udts
			  VARDATA_ANY(input),
			  VARSIZE_ANY_EXHDR(input),
			  NULL),
		  matrix->matrix,
		  "Error deserializing matrix");

	matrix = new_matrix(NULL, 0, 0, CurrentMemoryContext, m);
	OS_RETURN_MATRIX(matrix);
}


/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
