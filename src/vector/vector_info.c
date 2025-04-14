#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_info);
Datum vector_info(PG_FUNCTION_ARGS) {
	os_Vector *A;
	char *result, *buf;
	size_t size;
	FILE *fp;
	int level;
	LOGF();
	A = OS_GETARG_VECTOR(0);
	level = PG_GETARG_INT32(1);
	if (level > 5)
	{
		elog(ERROR, "Print level is from 0 to 5");
	}

	fp = open_memstream(&buf, &size);
	if (fp == NULL)
		elog(ERROR, "unable to open memstream for vector_print");
	GxB_fprint(A->vector, level, fp);
	fflush(fp);
	result = palloc(size + 1);
	memcpy(result, buf, size+1);
	free(buf);
	PG_RETURN_TEXT_P(cstring_to_text_with_len(result, size+1));
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
