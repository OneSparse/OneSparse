#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_get_element);
Datum
vector_get_element(PG_FUNCTION_ARGS)
{
	os_Vector  *vector;
	os_Scalar  *scalar;
	GrB_Index	i;
	GrB_Type	type;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	vector = OS_GETARG_VECTOR(0);
	i = PG_GETARG_INT64(1);

	OS_CHECK(GxB_Vector_type(&type, vector->vector),
			 vector->vector,
			 "Cannot get vector type");

	scalar = new_scalar(type, CurrentMemoryContext, NULL);

	OS_CHECK(GrB_Vector_extractElement(scalar->scalar, vector->vector, i),
			 vector->vector,
			 "Error extracting setting vector element.");
	OS_RETURN_SCALAR(scalar);
}
