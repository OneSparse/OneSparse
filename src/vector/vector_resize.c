#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_resize);
Datum vector_resize(PG_FUNCTION_ARGS)
{
	os_Vector *a;
	GrB_Index i;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	a = OS_GETARG_VECTOR(0);
	i = PG_GETARG_INT64(1);

	if (i == -1)
		i = GxB_INDEX_MAX;

	OS_CHECK(GrB_Matrix_resize((GrB_Matrix)a->vector, i, 1),
		  a->vector,
		  "Error resizing vector.");
	OS_RETURN_VECTOR(a);
}

SUPPORT_FN(vector_resize, linitial);

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
