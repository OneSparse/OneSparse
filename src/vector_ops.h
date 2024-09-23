
PG_FUNCTION_INFO_V1(FN(vector));
PG_FUNCTION_INFO_V1(FN(cast_vector));

Datum FN(vector)(PG_FUNCTION_ARGS)
{
	onesparse_Vector* vector;
	PG_TYPE val;
	LOGF();
	vector = new_vector(GB_TYPE, CurrentMemoryContext, NULL);
	/* if (!PG_ARGISNULL(0)) */
	/* { */
	/* 	val = PG_GETARG(0); */
	/* 	ERRORIF(GrB_Vector_setElement( */
	/* 				vector->vector, */
	/* 				val), */
	/* 			"Error setting Vector element"); */
	/* } */
	ONESPARSE_RETURN_VECTOR(vector);
}

Datum FN(cast_vector)(PG_FUNCTION_ARGS)
{
	onesparse_Vector* vector;
	PG_TYPE value;

	LOGF();
	ERRORNULL(0);

	vector = ONESPARSE_GETARG_VECTOR(0);
	ERRORIF(GrB_Vector_extractElement(
				&value,
				vector->vector),
			"Error extracting Vector element");

	PG_RETURN(value);
}

#undef SUFFIX
#undef PG_TYPE
#undef GB_TYPE
#undef PG_GETARG
#undef PG_RETURN
#undef NO_VECTOR_MATH
