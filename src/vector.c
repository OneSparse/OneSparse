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
PG_FUNCTION_INFO_V1(vector_reduce_scalar);
PG_FUNCTION_INFO_V1(vector_assign);
PG_FUNCTION_INFO_V1(vector_select);

static Size vector_get_flat_size(ExpandedObjectHeader *eohptr) {
	os_Vector *vector;
	void *serialized_data;
	GrB_Index serialized_size;

	LOGF();

	vector = (os_Vector*) eohptr;
	Assert(vector->em_magic == vector_MAGIC);

	if (vector->flat_size)
	{
		Assert(vector->serialized_data != NULL);
		Assert(vector->serialized_size > 0);
		return vector->flat_size;
	}

	CHECK(GxB_Vector_serialize(&serialized_data, &serialized_size, vector->vector, NULL),
		  vector->vector,
		  "Error serializing vector");

	vector->serialized_data = serialized_data;
	vector->serialized_size = serialized_size;
	vector->flat_size = OS_VECTOR_FLATSIZE() + serialized_size;
	return vector->flat_size;
}

/* Flatten vector into a pre-allocated result buffer that is
   allocated_size in bytes.  */
static void flatten_vector(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size)
{
	os_Vector *vector;
	os_FlatVector *flat;
	void* data;
//	void (*free_function)(void *p);

	LOGF();

	vector = (os_Vector *) eohptr;
	flat = (os_FlatVector *) result;

	Assert(vector->em_magic == vector_MAGIC);
	Assert(allocated_size == vector->flat_size);
	Assert(vector->serialized_data != NULL);
	Assert(vector->serialized_size > 0);

	memset(flat, 0, allocated_size);

	CHECK(GrB_get(vector->vector, &flat->type_code, GrB_EL_TYPE_CODE),
		  vector->vector,
		  "Cannot get Vector Type code.");

	CHECK(GrB_Vector_size(&flat->size, vector->vector),
		  vector->vector,
		  "Error extracting vector size.");

	data = OS_VECTOR_DATA(flat);

	memcpy(data, vector->serialized_data, vector->serialized_size);
	flat->serialized_size = vector->serialized_size;

	/* free_function = NULL; */
	/* ERRORIF(GrB_get(GrB_Global, (void*)free_function, GxB_FREE_FUNCTION) != GrB_SUCCESS, */
	/* 		"Cannot get SuiteSparse free function."); */

	/* free_function(vector->serialized_data); */
	SET_VARSIZE(flat, allocated_size);
}

/* Construct an empty expanded vector. */
os_Vector* new_vector(
	GrB_Type type,
	GrB_Index size,
	MemoryContext parentcontext,
	GrB_Vector _vector)
{
	os_Vector *vector;

	MemoryContext objcxt, oldcxt;
	MemoryContextCallback *ctxcb;

	LOGF();
	objcxt = AllocSetContextCreate(parentcontext,
								   "expanded vector",
								   ALLOCSET_DEFAULT_SIZES);

	vector = MemoryContextAlloc(objcxt,	sizeof(os_Vector));

	EOH_init_header(&vector->hdr, &vector_methods, objcxt);

	oldcxt = MemoryContextSwitchTo(objcxt);

	vector->em_magic = vector_MAGIC;
	vector->flat_size = 0;
	vector->serialized_data = NULL;
	vector->serialized_size = 0;

	if (_vector == NULL)
	{
		CHECK(GrB_Vector_new(&vector->vector, type, size),
			  vector->vector,
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
Datum expand_vector(os_FlatVector *flat, MemoryContext parentcontext)
{
	GrB_Type type;
	os_Vector *vector;
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
	data = OS_VECTOR_DATA(flat);
	CHECK(GxB_Vector_deserialize(&vector->vector, type, data, flat->serialized_size, NULL),
		  vector->vector,
		  "Error deserializing vector");
	OS_RETURN_VECTOR(vector);
}

static void
context_callback_vector_free(void* ptr)
{
	os_Vector *vector = (os_Vector *) ptr;
	LOGF();

	CHECK(GrB_Vector_free(&vector->vector),
		  vector->vector,
		  "Cannot GrB_Free Vector");
}

/* Helper function to always expand datum

   This is used by PG_GETARG_VECTOR */
os_Vector* DatumGetVector(Datum datum)
{
	os_Vector *vector;
	os_FlatVector *flat;

	LOGF();
	if (VARATT_IS_EXTERNAL_EXPANDED_RW(DatumGetPointer(datum))) {
		vector = VectorGetEOHP(datum);
		Assert(vector->em_magic == vector_MAGIC);
		return vector;
	}
	flat = OS_DETOAST_VECTOR(datum);
	datum = expand_vector(flat, CurrentMemoryContext);
	return VectorGetEOHP(datum);
}

Datum vector_in(PG_FUNCTION_ARGS)
{
	os_Vector *vector;
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
	GrB_Index wsize;
	int matched;

	input = PG_GETARG_CSTRING(0);
	input_copy = strdup(input);
    token = strtok_r(input_copy, "[", &saveptr);

	wsize = GrB_INDEX_MAX+1;
    if (token != NULL)
	{
		short_name = palloc(strlen(token)+1);
		matched = sscanf(token, "%99[^()](%lu)", short_name, &wsize);
		if (matched == 1)
		{
			wsize = GrB_INDEX_MAX+1;
		}
		else if (matched != 2)
		{
			elog(ERROR, "Invalid prefix %s", token);
		}
		typ = short_type(short_name);
    }
	else
		elog(ERROR, "Vector parse error, no short type code prefix.");

	vector = new_vector(typ, wsize, CurrentMemoryContext, NULL);

    strcpy(input_copy, input);
    strtok_r(input_copy, "[", &saveptr);
    token = strtok_r(NULL, "]", &saveptr);
	if (token == NULL)
	{
		OS_RETURN_VECTOR(vector);
	}

    for (number_token = strtok_r(token, " ", &number_saveptr); number_token != NULL;
         number_token = strtok_r(NULL, " ", &number_saveptr))
	{
        errno = 0;

		if (sscanf(number_token, "%d:%s", &index, number_token) != 2)
			elog(ERROR, "Invalid vector entry %s", number_token);

        if (typ == GrB_INT64) {
            int64_t num;
			matched = strtoll(number_token, &endptr, 10);
            if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid INT64 %s", number_token);
			CHECK(GrB_Vector_setElement(vector->vector, num, index),
				  vector->vector,
				  "Error setting Vector Element");
        }
        else if (typ == GrB_INT32) {
            long num = strtol(number_token, &endptr, 10);
            if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid INT32 %s", number_token);
			CHECK(GrB_Vector_setElement(vector->vector, num, index),
				  vector->vector,
				  "Error setting Vector Element");
        }
        else if (typ == GrB_INT16) {
            int num = (int)strtol(number_token, &endptr, 10);
            if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid INT16 %s", number_token);
			CHECK(GrB_Vector_setElement(vector->vector, num, index),
				  vector->vector,
				  "Error setting Vector Element");
        }
		else if (typ == GrB_FP64)
		{
            double num = strtod(number_token, &endptr);
            if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid FP64 %s", number_token);
			CHECK(GrB_Vector_setElement(vector->vector, num, index),
				  vector->vector,
				  "Error setting Vector Element");
        }
		else if (typ == GrB_FP32)
		{
            float num = strtof(number_token, &endptr);
            if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid FP32 %s", number_token);
			CHECK(GrB_Vector_setElement(vector->vector, num, index),
				  vector->vector,
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
			CHECK(GrB_Vector_setElement(vector->vector, num, index),
				  vector->vector,
				  "Error setting Vector Element");
        }
    }
	OS_RETURN_VECTOR(vector);
}

Datum vector_out(PG_FUNCTION_ARGS)
{
	GrB_Info info;
	GrB_Index i;
    GxB_Iterator iterator;
	os_Vector *vector;
    StringInfoData buf;
	int32_t type_code;

	LOGF();
	vector = OS_GETARG_VECTOR(0);

    GxB_Iterator_new(&iterator);
	CHECK(GxB_Vector_Iterator_attach(iterator, vector->vector, NULL),
		  vector->vector,
		  "Cannot attach vector iterator.");

    initStringInfo(&buf);
	CHECK(GrB_get(vector->vector, &type_code, GrB_EL_TYPE_CODE),
		  vector->vector,
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
	os_Vector *vector;

	LOGF();
	ERRORNULL(0);

	vector = OS_GETARG_VECTOR(0);

	CHECK(GrB_Vector_nvals(&result, vector->vector),
		  vector->vector,
		  "Error extracting vector nvals.");
	PG_RETURN_INT64(result);
}

Datum vector_size(PG_FUNCTION_ARGS)
{
	GrB_Index result;
	os_Vector *vector;

	LOGF();
	ERRORNULL(0);

	vector = OS_GETARG_VECTOR(0);

	CHECK(GrB_Vector_size(&result, vector->vector),
		  vector->vector,
		  "Error extracting vector size.");
	PG_RETURN_INT64(result);
}

Datum vector_ewise_add(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Vector *u, *v, *w, *mask;
	os_Descriptor *descriptor;
	os_BinaryOp *op, *accum;
	GrB_Index wsize;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	u = OS_GETARG_VECTOR(0);
	v = OS_GETARG_VECTOR(1);
	op = OS_GETARG_BINARYOP(2);

	mask = NULL;
	accum = NULL;
	descriptor = NULL;

	if (PG_NARGS() > 3)
	{
		if (PG_ARGISNULL(3))
		{
			CHECK(GxB_Vector_type(&type, u->vector),
				  u->vector,
				  "Cannot get vector type");

			CHECK(GrB_Vector_size(&wsize, u->vector),
				  u->vector,
				  "Cannot get vector size");

			w = new_vector(type, wsize, CurrentMemoryContext, NULL);
		}
		else
			w = OS_GETARG_VECTOR(3);
	}
	if (PG_NARGS() > 4)
		mask = PG_ARGISNULL(4) ? NULL : OS_GETARG_VECTOR(4);
	if (PG_NARGS() > 5)
		 accum = PG_ARGISNULL(5) ? NULL : OS_GETARG_BINARYOP(5);
	if (PG_NARGS() > 6)
		 descriptor = PG_ARGISNULL(6) ? NULL : OS_GETARG_DESCRIPTOR(6);

	CHECK(GrB_eWiseAdd(w->vector,
					   mask ? mask->vector : NULL,
					   accum ? accum->binaryop : NULL,
					   op->binaryop,
						 u->vector,
					   v->vector,
					   descriptor ? descriptor->descriptor : NULL),
		  w->vector,
		  "Error vector eWiseAdd.");

	OS_RETURN_VECTOR(w);
}

Datum vector_ewise_mult(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Vector *u, *v, *w, *mask;
	os_Descriptor *descriptor;
	os_BinaryOp *op, *accum;
	GrB_Index wsize;

	LOGF();

	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	u = OS_GETARG_VECTOR(0);
	v = OS_GETARG_VECTOR(1);
	op = OS_GETARG_BINARYOP(2);

	mask = NULL;
	accum = NULL;
	descriptor = NULL;

	if (PG_NARGS() > 3)
	{
		if (PG_ARGISNULL(3))
		{
			CHECK(GxB_Vector_type(&type, u->vector),
				  u->vector,
				  "Cannot get vector type");

			CHECK(GrB_Vector_size(&wsize, u->vector),
				  u->vector,
				  "Cannot get vector size");

			w = new_vector(type, wsize, CurrentMemoryContext, NULL);
		}
		else
			w = OS_GETARG_VECTOR(3);
	}
	if (PG_NARGS() > 4)
		mask = PG_ARGISNULL(4) ? NULL : OS_GETARG_VECTOR(4);
	if (PG_NARGS() > 5)
		 accum = PG_ARGISNULL(5) ? NULL : OS_GETARG_BINARYOP(5);
	if (PG_NARGS() > 6)
		 descriptor = PG_ARGISNULL(6) ? NULL : OS_GETARG_DESCRIPTOR(6);

	CHECK(GrB_eWiseMult(w->vector,
						mask ? mask->vector : NULL,
						accum ? accum->binaryop : NULL,
						op->binaryop,
						u->vector,
						v->vector,
						descriptor ? descriptor->descriptor : NULL),
		  w->vector,
		  "Error vector eWiseMult.");

	OS_RETURN_VECTOR(w);
}

Datum vector_ewise_union(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Vector *u, *v, *w, *mask;
	os_Scalar *a, *b;
	os_Descriptor *descriptor;
	os_BinaryOp *op, *accum;
	GrB_Index wsize;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);
	ERRORNULL(3);
	ERRORNULL(4);

	u = OS_GETARG_VECTOR(0);
	a = OS_GETARG_SCALAR(1);
	v = OS_GETARG_VECTOR(2);
	b = OS_GETARG_SCALAR(3);
	op = OS_GETARG_BINARYOP(4);

	mask = NULL;
	accum = NULL;
	descriptor = NULL;

	if (PG_NARGS() > 5)
	{
		if (PG_ARGISNULL(5))
		{
			CHECK(GxB_Vector_type(&type, u->vector),
				  u->vector,
				  "Cannot get vector type");

			CHECK(GrB_Vector_size(&wsize, u->vector),
				  u->vector,
				  "Cannot get vector size");

			w = new_vector(type, wsize, CurrentMemoryContext, NULL);
		}
		else
			w = OS_GETARG_VECTOR(5);
	}
	if (PG_NARGS() > 6)
		mask = PG_ARGISNULL(6) ? NULL : OS_GETARG_VECTOR(6);
	if (PG_NARGS() > 7)
		 accum = PG_ARGISNULL(7) ? NULL : OS_GETARG_BINARYOP(7);
	if (PG_NARGS() > 8)
		 descriptor = PG_ARGISNULL(8) ? NULL : OS_GETARG_DESCRIPTOR(8);

	CHECK(GxB_eWiseUnion(w->vector,
						 mask ? mask->vector : NULL,
						 accum ? accum->binaryop : NULL,
						 op->binaryop,
						 u->vector,
						 a->scalar,
						 v->vector,
						   b->scalar,
						 descriptor ? descriptor->descriptor : NULL),
		  w->vector,
		  "Error vector eWiseUnion.");

	OS_RETURN_VECTOR(w);
}

Datum
vector_reduce_scalar(PG_FUNCTION_ARGS)
{
	os_Vector *A;
	os_Monoid *monoid;
	os_BinaryOp *accum;
	os_Descriptor *descriptor;
	os_Scalar *result;
	GrB_Type type;

	A = OS_GETARG_VECTOR(0);
	monoid = OS_GETARG_MONOID(1);

	if (PG_NARGS() > 2)
		 accum = PG_ARGISNULL(2) ? NULL : OS_GETARG_BINARYOP(2);
	if (PG_NARGS() > 3)
		 descriptor = PG_ARGISNULL(3) ? NULL : OS_GETARG_DESCRIPTOR(3);

	CHECK(GxB_Vector_type(&type, A->vector),
		  A->vector,
		  "Cannot get Vector type");

	result = new_scalar(type, CurrentMemoryContext, NULL);

	CHECK(GrB_Vector_reduce_Monoid_Scalar(
			  result->scalar,
			  accum ? accum->binaryop : NULL,
			  monoid->monoid,
			  A->vector,
			  descriptor ? descriptor->descriptor : NULL),
		  result->scalar,
		  "Cannot reduce vector to scalar");

	OS_RETURN_SCALAR(result);
}

Datum
vector_assign(PG_FUNCTION_ARGS)
{
	os_Vector *A, *B, *mask;
	os_BinaryOp *accum;
	os_Descriptor *descriptor;
	GrB_Index nvals, *Ilist = NULL;

	A = OS_GETARG_VECTOR(0);
	B = OS_GETARG_VECTOR(1);

	if (PG_NARGS() > 2)
		mask = PG_ARGISNULL(2) ? NULL : OS_GETARG_VECTOR(2);
	if (PG_NARGS() > 3)
		accum = PG_ARGISNULL(3) ? NULL : OS_GETARG_BINARYOP(3);
	if (PG_NARGS() > 4)
		descriptor = PG_ARGISNULL(4) ? NULL : OS_GETARG_DESCRIPTOR(4);

	if (B != NULL)
	{
		CHECK(GrB_Vector_nvals(&nvals, B->vector),
			  B->vector,
			  "Error getting nvals.");

		Ilist = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);

		CHECK(GrB_Vector_extractTuples(Ilist,
									   NULL,
									   &nvals,
									   A->vector),
			  A->vector,
			  "Error extracting tuples.");
	}

	CHECK(GrB_assign(A->vector,
					 mask ? mask->vector : NULL,
					 accum ? accum->binaryop : NULL,
					 B->vector,
					 Ilist ? Ilist : GrB_ALL,
					 nvals,
					 descriptor ? descriptor->descriptor : NULL),
		  A->vector,
		  "Error in assign vector.");

	OS_RETURN_VECTOR(A);
}

Datum vector_select(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Vector *u, *w, *mask;
	os_Scalar *y;
	os_Descriptor *descriptor;
	os_BinaryOp *accum;
	os_IndexUnaryOp *op;
	GrB_Index wsize;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	u = OS_GETARG_VECTOR(0);
	op = OS_GETARG_INDEXUNARYOP(1);
	y = OS_GETARG_SCALAR(2);

	mask = NULL;
	accum = NULL;
	descriptor = NULL;

	if (PG_NARGS() > 3)
	{
		if (PG_ARGISNULL(3))
		{
			CHECK(GxB_Vector_type(&type, u->vector),
				  u->vector,
				  "Cannot get vector type");

			CHECK(GrB_Vector_size(&wsize, u->vector),
				  u->vector,
				  "Cannot get vector size");

			w = new_vector(type, wsize, CurrentMemoryContext, NULL);
		}
		else
			w = OS_GETARG_VECTOR(3);
	}
	if (PG_NARGS() > 4)
		mask = PG_ARGISNULL(4) ? NULL : OS_GETARG_VECTOR(4);
	if (PG_NARGS() > 5)
		accum = PG_ARGISNULL(5) ? NULL : OS_GETARG_BINARYOP(5);
	if (PG_NARGS() > 6)
		descriptor = PG_ARGISNULL(6) ? NULL : OS_GETARG_DESCRIPTOR(6);

	CHECK(GrB_select(w->vector,
					 mask ? mask->vector : NULL,
					 accum ? accum->binaryop : NULL,
					 op->indexunaryop,
					 u->vector,
					 y ? y->scalar : NULL,
					 descriptor ? descriptor->descriptor : NULL),
		  w->vector,
		  "Error in GrB_select");

	OS_RETURN_VECTOR(w);
}

Datum vector_wait(PG_FUNCTION_ARGS)
{
	os_Vector *vector;
	int waitmode;

	LOGF();
	ERRORNULL(0);

	vector = OS_GETARG_VECTOR(0);
	waitmode = PG_GETARG_INT32(1);

	CHECK(GrB_Vector_wait(vector->vector, waitmode),
		  vector->vector,
		  "Error waiting for vector.");
	PG_RETURN_VOID();
}

Datum vector_dup(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Vector *vector;
	os_Vector *result;
	GrB_Index size;

	LOGF();
	ERRORNULL(0);

	vector = OS_GETARG_VECTOR(0);
	CHECK(GxB_Vector_type(&type, vector->vector),
		  vector->vector,
		  "Cannot get vector type");

	CHECK(GrB_Vector_size(&size, vector->vector),
		  vector->vector,
		  "Error extracting vector size.");

	result = new_vector(type, size, CurrentMemoryContext, NULL);

	CHECK(GrB_Vector_dup(&result->vector, vector->vector),
		  vector->vector,
		  "Error duping vector.");
	OS_RETURN_VECTOR(result);
}

Datum vector_clear(PG_FUNCTION_ARGS)
{
	os_Vector *vector;

	LOGF();
	ERRORNULL(0);

	vector = OS_GETARG_VECTOR(0);

	CHECK(GrB_Vector_clear(vector->vector),
		  vector->vector,
		  "Error clearing vector.");
	PG_RETURN_VOID();
}
/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
