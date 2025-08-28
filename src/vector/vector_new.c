#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_new);
Datum vector_new(PG_FUNCTION_ARGS)
{
	os_Type *type;
	GrB_Index size;
	os_Vector *A;

	LOGF();
	type = OS_GETARG_TYPE(0);
	size = PG_GETARG_INT64(1);
	if (size == -1)
	{
		size = GrB_INDEX_MAX+1;
	}
	A = new_vector(type->type, size, CurrentMemoryContext, NULL);
	OS_RETURN_VECTOR(A);
}

