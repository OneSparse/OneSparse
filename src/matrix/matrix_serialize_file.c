#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_serialize_file);
Datum matrix_serialize_file(PG_FUNCTION_ARGS) {
	os_Matrix *matrix;
	void *data;
	size_t size;
	text *path;
	char *filepath;
	FILE *file;

	LOGF();

	matrix = OS_GETARG_MATRIX(0);
	path = PG_GETARG_TEXT_P(1);
    filepath = text_to_cstring(path);

	strncpy(filepath, VARDATA(path), VARSIZE(path) - VARHDRSZ);
	file = fopen(filepath, "wb");

	if (!file) ereport(ERROR, (errmsg("Failed to open file")));

	OS_CHECK(GrB_wait(matrix->matrix, GrB_MATERIALIZE),
			 matrix->matrix,
			 "Error waiting to materialize matrix.");

	OS_CHECK(GxB_Matrix_serialize(
				 &data,
				 &size,
				 matrix->matrix,
				 NULL),
			 matrix->matrix,
			 "Error serializing matrix");

	if (fwrite(data, 1, size, file) != size) {
		fclose(file);
		ereport(ERROR, (errmsg("Failed to write data to file")));
	}

	fclose(file);
	PG_RETURN_VOID();
}


/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
