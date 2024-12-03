#include "../onesparse.h"

PG_FUNCTION_INFO_V1(scalar_type);
Datum scalar_type(PG_FUNCTION_ARGS) {
	os_Scalar *A;
	GrB_Type type;
	char *type_name;
	size_t type_name_len;
	os_Type *result;

	ERRORNULL(0);

	A = OS_GETARG_SCALAR(0);

	OS_CHECK(GxB_Scalar_type(&type, A->scalar),
		  A->scalar,
		  "Cannot get scalar type");

	OS_CHECK(GrB_get(type, &type_name_len, GrB_NAME),
		  type,
		  "Cannot get type name len.");

	type_name = palloc(type_name_len);

	OS_CHECK(GrB_get(type, type_name, GrB_NAME),
		  type,
		  "Cannot get type name.");

	for (int i = 4; i < type_name_len; i++)
	{
        type_name[i - 4] = tolower(type_name[i]);
    }
    type_name[type_name_len - 4] = '\0';
	result = new_type(type_name, CurrentMemoryContext);
	OS_RETURN_TYPE(result);
}


/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */