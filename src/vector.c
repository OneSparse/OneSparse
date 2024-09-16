#include "vector.h"

static Size vector_get_flat_size(ExpandedObjectHeader *eohptr) {
	onesparse_Vector *vector;
	void *serialized_data;
	GrB_Index serialized_size;

	LOGF();

	vector = (onesparse_Vector*) eohptr;
	Assert(vector->em_magic == vector_MAGIC);

	if (vector->flat_size)
	{
		Assert(vector->serialized_data != NULL);
		Assert(vector->serialized_size > 0);
		return vector->flat_size;
	}

	ERRORIF(GxB_Vector_serialize(&serialized_data, &serialized_size, vector->vector, NULL) != GrB_SUCCESS,
			"Error serializing vector");

	vector->serialized_data = serialized_data;
	vector->serialized_size = serialized_size;
	vector->flat_size = ONESPARSE_VECTOR_FLATSIZE() + serialized_size;
	elog(INFO, "flat size %zu", vector->flat_size);
	return vector->flat_size;
}

/* Flatten vector into a pre-allocated result buffer that is
   allocated_size in bytes.  */
static void flatten_vector(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size)
{
	onesparse_Vector *vector;
	onesparse_FlatVector *flat;
	void* data;

	LOGF();

	vector = (onesparse_Vector *) eohptr;
	flat = (onesparse_FlatVector *) result;

	Assert(vector->em_magic == vector_MAGIC);
	Assert(allocated_size == vector->flat_size);
	Assert(vector->serialized_data != NULL);
	Assert(vector->serialized_size > 0);

	memset(flat, 0, allocated_size);

	ERRORIF(GrB_get(vector->vector, &flat->type_code, GrB_EL_TYPE_CODE) != GrB_SUCCESS,
			"Cannot get Vector Type code.");

	ERRORIF(GrB_Vector_size(&flat->size, vector->vector) != GrB_SUCCESS,
			"Error extracting vector size.");

	data = ONESPARSE_VECTOR_DATA(flat);

	memcpy(data, vector->serialized_data, vector->serialized_size);
	flat->serialized_size = vector->serialized_size;
	SET_VARSIZE(flat, allocated_size);
}

/* Construct an empty expanded vector. */
onesparse_Vector* new_vector(
	GrB_Type type,
	GrB_Index size,
	MemoryContext parentcontext,
	GrB_Vector _vector)
{
	onesparse_Vector *vector;

	MemoryContext objcxt, oldcxt;
	MemoryContextCallback *ctxcb;

	LOGF();
	objcxt = AllocSetContextCreate(parentcontext,
								   "expanded vector",
								   ALLOCSET_DEFAULT_SIZES);

	vector = MemoryContextAlloc(objcxt,	sizeof(onesparse_Vector));

	EOH_init_header(&vector->hdr, &vector_methods, objcxt);

	oldcxt = MemoryContextSwitchTo(objcxt);

	vector->em_magic = vector_MAGIC;
	vector->flat_size = 0;
	vector->serialized_data = NULL;
	vector->serialized_size = 0;

	if (_vector == NULL)
	{
		ERRORIF(GrB_Vector_new(&vector->vector, type, size) != GrB_SUCCESS,
				"Cannot create new Vector.");
	}
	else
	{
		vector->vector = _vector;
	}

	ctxcb = MemoryContextAlloc(objcxt, sizeof(MemoryContextCallback));

	ctxcb->func = context_callback_vector_free;
	ctxcb->arg = vector;

	MemoryContextRegisterResetCallback(objcxt, ctxcb);

	MemoryContextSwitchTo(oldcxt);
	return vector;
}

/* Expand a flat vector in to an Expanded one, return as Postgres Datum. */
Datum expand_vector(onesparse_FlatVector *flat, MemoryContext parentcontext)
{
	GrB_Type type;
	onesparse_Vector *vector;
	void* data;

	LOGF();

	if (flat->type_code == GrB_INT64_CODE)
	{
		type = GrB_INT64;
	}
	else if (flat->type_code == GrB_INT32_CODE)
	{
		type = GrB_INT32;
	}
	else if (flat->type_code == GrB_INT16_CODE)
	{
		type = GrB_INT16;
	}
	else if (flat->type_code == GrB_FP64_CODE)
	{
		type = GrB_FP64;
	}
	else if (flat->type_code == GrB_FP32_CODE)
	{
		type = GrB_FP32;
	}
	else if (flat->type_code == GrB_BOOL_CODE)
	{
		type = GrB_BOOL;
	}
	else
		elog(ERROR, "Unknown type code.");

	vector = new_vector(type, flat->size, parentcontext, NULL);
	data = ONESPARSE_VECTOR_DATA(flat);
	ERRORIF(GxB_Vector_deserialize(&vector->vector, type, data, flat->serialized_size, NULL) != GrB_SUCCESS,
			"Error deserializing vector");
	ONESPARSE_RETURN_VECTOR(vector);
}

static void
context_callback_vector_free(void* ptr)
{
	onesparse_Vector *vector = (onesparse_Vector *) ptr;
	LOGF();

	ERRORIF(GrB_Vector_free(&vector->vector) != GrB_SUCCESS,
			"Cannot GrB_Free Vector");
}

/* Helper function to always expand datum

   This is used by PG_GETARG_VECTOR */
onesparse_Vector* DatumGetVector(Datum datum)
{
	onesparse_Vector *vector;
	onesparse_FlatVector *flat;

	LOGF();
	if (VARATT_IS_EXTERNAL_EXPANDED_RW(DatumGetPointer(datum))) {
		vector = VectorGetEOHP(datum);
		Assert(vector->em_magic == vector_MAGIC);
		return vector;
	}
	flat = ONESPARSE_DETOAST_VECTOR(datum);
	datum = expand_vector(flat, CurrentMemoryContext);
	return VectorGetEOHP(datum);
}

Datum vector_in(PG_FUNCTION_ARGS)
{
	onesparse_Vector *vector;
	char *input;
	size_t len;
	GrB_Type typ;
	char *str_val = "";
	char *str_type = "";
	char *fmt = "";

	input = PG_GETARG_CSTRING(0);
	len = strlen(input);

	vector = new_vector(GrB_INT64, GrB_INDEX_MAX+1, CurrentMemoryContext, NULL);
	/* sscanf(input, "%[^:]:%s", str_val, str_type); */
	/* if (strcmp(str_type, "i8") == 0) */
	/* { */
	/* 	int64_t value; */
	/* 	typ = GrB_INT64; */
	/* 	fmt = "%ld"; */
	/* 	vector = new_vector(typ, CurrentMemoryContext, NULL); */
	/* 	if (len) */
	/* 	{ */
	/* 		if (sscanf(input, fmt, &value) == 1) */
	/* 		{ */
	/* 			ERRORIF(GrB_Vector_setElement(vector->vector, value) != GrB_SUCCESS, */
	/* 					"Cannot set vector element in expand."); */
	/* 		} */
	/* 		else */
	/* 			elog(ERROR, "Invalid format for %s %s", fmt, input); */
	/* 	} */
	/* } */
	/* else if (strcmp(str_type, "i4") == 0) */
	/* { */
	/* 	int32_t value; */
	/* 	typ = GrB_INT32; */
	/* 	fmt = "%i"; */
	/* 	vector = new_vector(typ, CurrentMemoryContext, NULL); */
	/* 	if (len) */
	/* 	{ */
	/* 		if (sscanf(input, fmt, &value) == 1) */
	/* 		{ */
	/* 			ERRORIF(GrB_Vector_setElement(vector->vector, value) != GrB_SUCCESS, */
	/* 					"Cannot set vector element in expand."); */
	/* 		} */
	/* 		else */
	/* 			elog(ERROR, "Invalid format for %s %s", fmt, input); */
	/* 	} */
	/* } */
	/* else if (strcmp(str_type, "i2") == 0) */
	/* { */
	/* 	int16_t value; */
	/* 	typ = GrB_INT16; */
	/* 	fmt = "%i"; */
	/* 	vector = new_vector(typ, CurrentMemoryContext, NULL); */
	/* 	if (len) */
	/* 	{ */
	/* 		if (sscanf(input, fmt, &value) == 1) */
	/* 		{ */
	/* 			ERRORIF(GrB_Vector_setElement(vector->vector, value) != GrB_SUCCESS, */
	/* 					"Cannot set vector element in expand."); */
	/* 		} */
	/* 		else */
	/* 			elog(ERROR, "Invalid format for %s %s", fmt, input); */
	/* 	} */
	/* } */
	/* else if (strcmp(str_type, "f8") == 0) */
	/* { */
	/* 	double value; */
	/* 	typ = GrB_FP64; */
	/* 	fmt = "%lf"; */
	/* 	vector = new_vector(typ, CurrentMemoryContext, NULL); */
	/* 	if (len) */
	/* 	{ */
	/* 		if (sscanf(input, fmt, &value) == 1) */
	/* 		{ */
	/* 			ERRORIF(GrB_Vector_setElement(vector->vector, value) != GrB_SUCCESS, */
	/* 					"Cannot set vector element in expand."); */
	/* 		} */
	/* 		else */
	/* 			elog(ERROR, "Invalid format for %s %s", fmt, input); */
	/* 	} */
	/* } */
	/* else if (strcmp(str_type, "f4") == 0) */
	/* { */
	/* 	float value; */
	/* 	typ = GrB_FP32; */
	/* 	fmt = "%f"; */
	/* 	vector = new_vector(typ, CurrentMemoryContext, NULL); */
	/* 	if (len) */
	/* 	{ */
	/* 		if (sscanf(input, fmt, &value) == 1) */
	/* 		{ */
	/* 			ERRORIF(GrB_Vector_setElement(vector->vector, value) != GrB_SUCCESS, */
	/* 					"Cannot set vector element in expand."); */
	/* 		} */
	/* 		else */
	/* 			elog(ERROR, "Invalid format for %s %s", fmt, input); */
	/* 	} */
	/* } */
	/* else if (strcmp(str_type, "b") == 0) */
	/* { */
	/* 	char value; */
	/* 	typ = GrB_BOOL; */
	/* 	fmt = "%c"; */
	/* 	vector = new_vector(typ, CurrentMemoryContext, NULL); */
	/* 	if (len) */
	/* 	{ */
	/* 		if (sscanf(input, fmt, &value) == 1) */
	/* 		{ */
	/* 			bool _value; */
	/* 			if (value == 't') */
	/* 				_value = true; */
	/* 			else if (value == 'f') */
	/* 				_value = false; */
	/* 			else */
	/* 				elog(ERROR, "Invalid value for bool %c", value); */
	/* 			ERRORIF(GrB_Vector_setElement(vector->vector, _value) != GrB_SUCCESS, */
	/* 					"Cannot set vector element in expand."); */
	/* 		} */
	/* 		else */
	/* 			elog(ERROR, "Invalid format for %s %s", fmt, input); */
	/* 	} */
	/* } */
	/* else */
	/* 	elog(ERROR, "Unknown type code %s", str_type); */
	ONESPARSE_RETURN_VECTOR(vector);
}

Datum vector_out(PG_FUNCTION_ARGS)
{
	char *result;
	int32_t type_code;
	onesparse_Vector *vector;
	GrB_Index nvals;

	LOGF();
	vector = ONESPARSE_GETARG_VECTOR(0);

	nvals = 0;
	ERRORIF(GrB_Vector_nvals(&nvals, vector->vector) != GrB_SUCCESS,
			"Error extracting vector nvals.");

	/* if (nvals) */
	/* { */
	/* 	ERRORIF(GrB_get(vector->vector, &type_code, GrB_EL_TYPE_CODE) != GrB_SUCCESS, */
	/* 			"Cannot get Vector Type code."); */

	/* 	if (type_code == GrB_INT64_CODE) */
	/* 	{ */
	/* 		int64_t value; */
	/* 		ERRORIF(GrB_Vector_extractElement(&value, vector->vector) != GrB_SUCCESS, */
	/* 				"Error extracting vector element."); */
	/* 		result = palloc(23); */
	/* 		snprintf(result, 23, "%s:" "%" PRIi64, "i8", value); */
	/* 	} */
	/* 	else if (type_code == GrB_INT32_CODE) */
	/* 	{ */
	/* 		int32_t value; */
	/* 		ERRORIF(GrB_Vector_extractElement(&value, vector->vector) != GrB_SUCCESS, */
	/* 				"Error extracting vector element."); */
	/* 		result = palloc(13); */
	/* 		snprintf(result, 13, "%s:" "%" PRIi32, "i4", value); */
	/* 	} */
	/* 	else if (type_code == GrB_INT16_CODE) */
	/* 	{ */
	/* 		int16_t value; */
	/* 		ERRORIF(GrB_Vector_extractElement(&value, vector->vector) != GrB_SUCCESS, */
	/* 				"Error extracting vector element."); */
	/* 		result = palloc(8); */
	/* 		snprintf(result, 8, "%s:" "%" PRIi16, "i2", value); */
	/* 	} */
	/* 	else if (type_code == GrB_FP64_CODE) */
	/* 	{ */
	/* 		double value; */
	/* 		ERRORIF(GrB_Vector_extractElement(&value, vector->vector) != GrB_SUCCESS, */
	/* 				"Error extracting vector element."); */
	/* 		result = palloc(21); */
	/* 		snprintf(result, 21, "%s:%f", "fp64", value); */
	/* 	} */
	/* 	else if (type_code == GrB_FP32_CODE) */
	/* 	{ */
	/* 		float value; */
	/* 		ERRORIF(GrB_Vector_extractElement(&value, vector->vector) != GrB_SUCCESS, */
	/* 				"Error extracting vector element."); */
	/* 		result = palloc(12); */
	/* 		snprintf(result, 12, "%s:%f", "fp32", value); */
	/* 	} */
	/* 	else if (type_code == GrB_BOOL_CODE) */
	/* 	{ */
	/* 		bool value; */
	/* 		ERRORIF(GrB_Vector_extractElement(&value, vector->vector) != GrB_SUCCESS, */
	/* 				"Error extracting vector element."); */
	/* 		result = palloc(7); */
	/* 		snprintf(result, 7, "%s:%s", "bool", value ? "t" : "f"); */
	/* 	} */
	/* 	else */
	/* 		elog(ERROR, "Unsupported type code %i.", type_code); */
	/* } */
	/* else */
	/* { */
		result = palloc(1);
		/* snprintf(result, 1, ""); */
//	}
	PG_RETURN_CSTRING(result);
}

Datum vector_nvals(PG_FUNCTION_ARGS)
{
	GrB_Index result;
	onesparse_Vector *vector;

	LOGF();
	ERRORNULL(0);

	vector = ONESPARSE_GETARG_VECTOR(0);

	ERRORIF(GrB_Vector_nvals(&result, vector->vector) != GrB_SUCCESS,
			"Error extracting vector nvals.");
	PG_RETURN_INT64(result);
}

/* #define SUFFIX _int64                // suffix for names */
/* #define PG_TYPE int64                // postgres type */
/* #define GB_TYPE GrB_INT64            // graphblas vector type */
/* #define PG_GETARG PG_GETARG_INT64       // how to get value args */
/* #define PG_RETURN PG_RETURN_INT64 */
/* #include "vector_ops.h" */

/* #define SUFFIX _int32                // suffix for names */
/* #define PG_TYPE int32                // postgres type */
/* #define GB_TYPE GrB_INT32            // graphblas vector type */
/* #define PG_GETARG PG_GETARG_INT32       // how to get value args */
/* #define PG_RETURN PG_RETURN_INT32 */
/* #include "vector_ops.h" */

/* #define SUFFIX _int16                // suffix for names */
/* #define PG_TYPE int16                // postgres type */
/* #define GB_TYPE GrB_INT16            // graphblas vector type */
/* #define PG_GETARG PG_GETARG_INT16       // how to get value args */
/* #define PG_RETURN PG_RETURN_INT16 */
/* #include "vector_ops.h" */

/* #define SUFFIX _fp64                // suffix for names */
/* #define PG_TYPE float8                // postgres type */
/* #define GB_TYPE GrB_FP64            // graphblas vector type */
/* #define PG_GETARG PG_GETARG_FLOAT8       // how to get value args */
/* #define PG_RETURN PG_RETURN_FLOAT8 */
/* #include "vector_ops.h" */

/* #define SUFFIX _fp32                // suffix for names */
/* #define PG_TYPE float4                // postgres type */
/* #define GB_TYPE GrB_FP32            // graphblas vector type */
/* #define PG_GETARG PG_GETARG_FLOAT4       // how to get value args */
/* #define PG_RETURN PG_RETURN_FLOAT4 */
/* #include "vector_ops.h" */

/* #define SUFFIX _bool                // suffix for names */
/* #define PG_TYPE bool                // postgres type */
/* #define GB_TYPE GrB_BOOL            // graphblas vector type */
/* #define PG_GETARG PG_GETARG_BOOL       // how to get value args */
/* #define PG_RETURN PG_RETURN_BOOL */
/* #define NO_VECTOR_MATH */
/* #include "vector_ops.h" */

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
