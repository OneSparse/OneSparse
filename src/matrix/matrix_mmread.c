#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_mmread);
Datum
matrix_mmread(PG_FUNCTION_ARGS)
{
	GrB_Matrix	m;
	text	   *path;
	char	   *filepath;
	FILE	   *file;
	char		msg[LAGRAPH_MSG_LEN];
	struct timeval start,
				end;

	path = PG_GETARG_TEXT_P(0);
	filepath = text_to_cstring(path);
	file = fopen(filepath, "rb");
	if (!file)
		ereport(ERROR, (errmsg("Failed to open file")));

	OS_START_BENCH();
	LA_CHECK(LAGraph_MMRead(
							&m,
							file,
							msg));
	OS_END_BENCH();

	OS_RETURN_MATRIX(new_matrix(NULL, 0, 0, CurrentMemoryContext, m));
}
