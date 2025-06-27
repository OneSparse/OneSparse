#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_deserialize_file);
Datum matrix_deserialize_file(PG_FUNCTION_ARGS) {
	GrB_Matrix m;
	void *data;
	size_t size;
	text *path;
	char *filepath;
	FILE *file;
    struct timeval start, end;

	path = PG_GETARG_TEXT_P(0);
    filepath = text_to_cstring(path);

	OS_START_BENCH();
	file = fopen(filepath, "rb");

	if (!file) ereport(ERROR, (errmsg("Failed to open file")));

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);

    data = palloc(1);
	data = repalloc_huge(data, size);
    if (fread(data, 1, size, file) != size) {
        fclose(file);
        ereport(ERROR, (errmsg("Failed to read file: %s", filepath)));
    }

	fclose(file);

	OS_CHECK(GxB_Matrix_deserialize(
			  &m,
			  NULL, // TODO, support udts
			  data,
			  size,
			  NULL),
			 m,
			 "Error deserializing matrix");

	OS_END_BENCH();
	OS_RETURN_MATRIX(new_matrix(NULL, 0, 0, CurrentMemoryContext, m));
}


/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
