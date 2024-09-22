#include "onesparse.h"

static void context_callback_vector_free(void*);
static Size vector_get_flat_size(ExpandedObjectHeader *eohptr);

static void flatten_vector(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size);

static const ExpandedObjectMethods vector_methods = {
	vector_get_flat_size,
	flatten_vector
};

PG_FUNCTION_INFO_V1(vector_in);
PG_FUNCTION_INFO_V1(vector_out);

PG_FUNCTION_INFO_V1(vector_nvals);
PG_FUNCTION_INFO_V1(vector_size);
PG_FUNCTION_INFO_V1(vector_wait);
PG_FUNCTION_INFO_V1(vector_dup);
PG_FUNCTION_INFO_V1(vector_clear);
PG_FUNCTION_INFO_V1(vector_ewise_add);
PG_FUNCTION_INFO_V1(vector_ewise_mult);
PG_FUNCTION_INFO_V1(vector_ewise_union);

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
//	void (*free_function)(void *p);

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

	/* free_function = NULL; */
	/* ERRORIF(GrB_get(GrB_Global, (void*)free_function, GxB_FREE_FUNCTION) != GrB_SUCCESS, */
	/* 		"Cannot get SuiteSparse free function."); */

	/* free_function(vector->serialized_data); */
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
	GrB_Type typ;
	char *short_name;
    char *input_copy;
	char *token;
    char *saveptr;
    char *endptr;
    char *number_token;
    char *number_saveptr;
    int index = 0;
	bool is_short = false;
	GrB_Index dimesion;

	input = PG_GETARG_CSTRING(0);
	input_copy = strdup(input);
    token = strtok_r(input_copy, "[", &saveptr);

    if (token != NULL)
	{
		short_name = palloc(strlen(token)+1);
        strcpy(short_name, token);
		typ = short_type(short_name);
    }
	else
		elog(ERROR, "Vector parse error, no short type code prefix.");

	vector = new_vector(typ, GrB_INDEX_MAX+1, CurrentMemoryContext, NULL);

    strcpy(input_copy, input);
    strtok_r(input_copy, "[", &saveptr);
    token = strtok_r(NULL, "]", &saveptr);

    for (number_token = strtok_r(token, " ", &number_saveptr); number_token != NULL;
         number_token = strtok_r(NULL, " ", &number_saveptr))
	{
        errno = 0;

        if (typ == GrB_INT64) {
            int64_t num = strtoll(number_token, &endptr, 10);
            if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid INT64 %s", number_token);
			ERRORIF(GrB_Vector_setElement(vector->vector, num, index) != GrB_SUCCESS,
				"Error setting Vector Element");
        }
        else if (typ == GrB_INT32) {
            long num = strtol(number_token, &endptr, 10);
            if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid INT32 %s", number_token);
			ERRORIF(GrB_Vector_setElement(vector->vector, num, index) != GrB_SUCCESS,
					"Error setting Vector Element");
        }
        else if (typ == GrB_INT16) {
            int num = (int)strtol(number_token, &endptr, 10);
            if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid INT16 %s", number_token);
			ERRORIF(GrB_Vector_setElement(vector->vector, num, index) != GrB_SUCCESS,
					"Error setting Vector Element");
        }
		else if (typ == GrB_FP64)
		{
            double num = strtod(number_token, &endptr);
            if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid FP64 %s", number_token);
			ERRORIF(GrB_Vector_setElement(vector->vector, num, index) != GrB_SUCCESS,
					"Error setting Vector Element");
        }
		else if (typ == GrB_FP32)
		{
            float num = strtof(number_token, &endptr);
            if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid FP32 %s", number_token);
			ERRORIF(GrB_Vector_setElement(vector->vector, num, index) != GrB_SUCCESS,
					"Error setting Vector Element");
        }
		else if (typ == GrB_BOOL)
		{
			bool num;
            if (strcmp(number_token, "t") == 0)
				num = true;
			else if  (strcmp(number_token, "f") == 0)
				num = false;
			else
				elog(ERROR, "Invalid BOOL %s", number_token);
			ERRORIF(GrB_Vector_setElement(vector->vector, num, index) != GrB_SUCCESS,
					"Error setting Vector Element");
        }
		index++;
    }
	ONESPARSE_RETURN_VECTOR(vector);
}

Datum vector_out(PG_FUNCTION_ARGS)
{
	GrB_Info info;
	GrB_Index i;
    GxB_Iterator iterator;
	onesparse_Vector *vector;
    StringInfoData buf;
	int32_t type_code;

	LOGF();
	vector = ONESPARSE_GETARG_VECTOR(0);

    GxB_Iterator_new(&iterator);
	ERRORIF(GxB_Vector_Iterator_attach(iterator, vector->vector, NULL) != GrB_SUCCESS,
			"Cannot attach vector iterator.");

    initStringInfo(&buf);
	ERRORIF(GrB_get(vector->vector, &type_code, GrB_EL_TYPE_CODE) != GrB_SUCCESS,
			"Cannot get Vector Type code.");

	appendStringInfo(&buf, "%s[", short_code(type_code));

    info = GxB_Vector_Iterator_seek(iterator, 0);
    while (info != GxB_EXHAUSTED)
    {
        i = GxB_Vector_Iterator_getIndex(iterator);
		switch(type_code)
		{
			case GrB_INT64_CODE:
				int64_t vi64 = GxB_Iterator_get_INT64(iterator);
				appendStringInfo(&buf, "%lu:%ld", i, vi64);
				break;
			case GrB_INT32_CODE:
				int32_t vi32 = GxB_Iterator_get_INT32(iterator);
				appendStringInfo(&buf, "%lu:%d", i, vi32);
				break;
			case GrB_INT16_CODE:
				int16_t vi16 = GxB_Iterator_get_INT16(iterator);
				appendStringInfo(&buf, "%lu:%d", i, vi16);
				break;
			case GrB_FP64_CODE:
				double vf64 = GxB_Iterator_get_FP64(iterator);
				appendStringInfo(&buf, "%lu:%f", i, vf64);
				break;
			case GrB_FP32_CODE:
				float vf32 = GxB_Iterator_get_FP32(iterator);
				appendStringInfo(&buf, "%lu:%f", i, vf32);
				break;
		}
        info = GxB_Vector_Iterator_next(iterator);
		if (info != GxB_EXHAUSTED)
			appendStringInfo(&buf, " ");
    }
    GrB_free(&iterator);

	appendStringInfo(&buf, "]");
	PG_RETURN_CSTRING(buf.data);
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

Datum vector_size(PG_FUNCTION_ARGS)
{
	GrB_Index result;
	onesparse_Vector *vector;

	LOGF();
	ERRORNULL(0);

	vector = ONESPARSE_GETARG_VECTOR(0);

	ERRORIF(GrB_Vector_size(&result, vector->vector) != GrB_SUCCESS,
			"Error extracting vector size.");
	PG_RETURN_INT64(result);
}

Datum vector_ewise_add(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	onesparse_Vector *u, *v, *w, *mask;
	onesparse_Descriptor *descriptor;
	onesparse_BinaryOp *op, *accum;
	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	u = ONESPARSE_GETARG_VECTOR(0);
	v = ONESPARSE_GETARG_VECTOR(1);
	op = ONESPARSE_GETARG_BINARYOP(2);

	mask = NULL;
	accum = NULL;
	descriptor = NULL;

	if (PG_NARGS() > 3)
	{
		if (PG_ARGISNULL(3))
		{
			ERRORIF(GxB_Vector_type(&type, u->vector) != GrB_SUCCESS,
					"Cannot get vector type");
			w = new_vector(type, GrB_INDEX_MAX+1, CurrentMemoryContext, NULL);
		}
		else
			w = ONESPARSE_GETARG_VECTOR(3);
	}
	if (PG_NARGS() > 4)
		mask = PG_ARGISNULL(4) ? NULL : ONESPARSE_GETARG_VECTOR(4);
	if (PG_NARGS() > 5)
		 accum = PG_ARGISNULL(5) ? NULL : ONESPARSE_GETARG_BINARYOP(5);
	if (PG_NARGS() > 6)
		 descriptor = PG_ARGISNULL(6) ? NULL : ONESPARSE_GETARG_DESCRIPTOR(6);

	ERRORIF(GrB_eWiseAdd(w->vector,
						 mask ? mask->vector : NULL,
						 accum ? accum->binaryop : NULL,
						 op->binaryop,
						 u->vector,
						 v->vector,
						 descriptor ? descriptor->descriptor : NULL) != GrB_SUCCESS,
			"Error vector eWiseAdd.");

	ONESPARSE_RETURN_VECTOR(w);
}

Datum vector_ewise_mult(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	onesparse_Vector *u, *v, *w, *mask;
	onesparse_Descriptor *descriptor;
	onesparse_BinaryOp *op, *accum;
	LOGF();

	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	u = ONESPARSE_GETARG_VECTOR(0);
	v = ONESPARSE_GETARG_VECTOR(1);
	op = ONESPARSE_GETARG_BINARYOP(2);

	mask = NULL;
	accum = NULL;
	descriptor = NULL;

	if (PG_NARGS() > 3)
	{
		if (PG_ARGISNULL(3))
		{
			ERRORIF(GxB_Vector_type(&type, u->vector) != GrB_SUCCESS,
					"Cannot get vector type");
			w = new_vector(type, GrB_INDEX_MAX+1, CurrentMemoryContext, NULL);
		}
		else
			w = ONESPARSE_GETARG_VECTOR(3);
	}
	if (PG_NARGS() > 4)
		mask = PG_ARGISNULL(4) ? NULL : ONESPARSE_GETARG_VECTOR(4);
	if (PG_NARGS() > 5)
		 accum = PG_ARGISNULL(5) ? NULL : ONESPARSE_GETARG_BINARYOP(5);
	if (PG_NARGS() > 6)
		 descriptor = PG_ARGISNULL(6) ? NULL : ONESPARSE_GETARG_DESCRIPTOR(6);

	ERRORIF(GrB_eWiseMult(w->vector,
						  mask ? mask->vector : NULL,
						  accum ? accum->binaryop : NULL,
						  op->binaryop,
						  u->vector,
						  v->vector,
						  descriptor ? descriptor->descriptor : NULL) != GrB_SUCCESS,
			"Error vector eWiseMult.");

	ONESPARSE_RETURN_VECTOR(w);
}

Datum vector_ewise_union(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	onesparse_Vector *u, *v, *w, *mask;
	onesparse_Scalar *a, *b;
	onesparse_Descriptor *descriptor;
	onesparse_BinaryOp *op, *accum;
	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);
	ERRORNULL(3);
	ERRORNULL(4);

	u = ONESPARSE_GETARG_VECTOR(0);
	a = ONESPARSE_GETARG_SCALAR(1);
	v = ONESPARSE_GETARG_VECTOR(2);
	b = ONESPARSE_GETARG_SCALAR(3);
	op = ONESPARSE_GETARG_BINARYOP(4);

	mask = NULL;
	accum = NULL;
	descriptor = NULL;

	if (PG_NARGS() > 5)
	{
		if (PG_ARGISNULL(5))
		{
			ERRORIF(GxB_Vector_type(&type, u->vector) != GrB_SUCCESS,
					"Cannot get vector type");
			w = new_vector(type, GrB_INDEX_MAX+1, CurrentMemoryContext, NULL);
		}
		else
			w = ONESPARSE_GETARG_VECTOR(5);
	}
	if (PG_NARGS() > 6)
		mask = PG_ARGISNULL(6) ? NULL : ONESPARSE_GETARG_VECTOR(6);
	if (PG_NARGS() > 7)
		 accum = PG_ARGISNULL(7) ? NULL : ONESPARSE_GETARG_BINARYOP(7);
	if (PG_NARGS() > 8)
		 descriptor = PG_ARGISNULL(8) ? NULL : ONESPARSE_GETARG_DESCRIPTOR(8);

	ERRORIF(GxB_eWiseUnion(w->vector,
						   mask ? mask->vector : NULL,
						   accum ? accum->binaryop : NULL,
						   op->binaryop,
						   u->vector,
						   a->scalar,
						   v->vector,
						   b->scalar,
						   descriptor ? descriptor->descriptor : NULL) != GrB_SUCCESS,
			"Error vector eWiseUnion.");

	ONESPARSE_RETURN_VECTOR(w);
}

Datum vector_wait(PG_FUNCTION_ARGS)
{
	onesparse_Vector *vector;
	int waitmode;

	LOGF();
	ERRORNULL(0);

	vector = ONESPARSE_GETARG_VECTOR(0);
	waitmode = PG_GETARG_INT32(1);

	ERRORIF(GrB_Vector_wait(vector->vector, waitmode) != GrB_SUCCESS,
			"Error waiting for vector.");
	PG_RETURN_VOID();
}

Datum vector_dup(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	onesparse_Vector *vector;
	onesparse_Vector *result;
	GrB_Index size;

	LOGF();
	ERRORNULL(0);

	vector = ONESPARSE_GETARG_VECTOR(0);
	ERRORIF(GxB_Vector_type(&type, vector->vector) != GrB_SUCCESS,
			"Cannot get vector type");

	ERRORIF(GrB_Vector_size(&size, vector->vector) != GrB_SUCCESS,
			"Error extracting vector size.");

	result = new_vector(type, size, CurrentMemoryContext, NULL);

	ERRORIF(GrB_Vector_dup(&result->vector, vector->vector) != GrB_SUCCESS,
			"Error duping vector.");
	ONESPARSE_RETURN_VECTOR(result);
}

Datum vector_clear(PG_FUNCTION_ARGS)
{
	onesparse_Vector *vector;

	LOGF();
	ERRORNULL(0);

	vector = ONESPARSE_GETARG_VECTOR(0);

	ERRORIF(GrB_Vector_clear(vector->vector) != GrB_SUCCESS,
			"Error clearing vector.");
	PG_RETURN_VOID();
}
/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
