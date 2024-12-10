#include "../onesparse.h"

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

static Size vector_get_flat_size(ExpandedObjectHeader *eohptr) {
	os_Vector *vector;
	void *serialized_data;
	GrB_Index serialized_size;

	vector = (os_Vector*) eohptr;
	Assert(vector->em_magic == vector_MAGIC);

	if (vector->flat_size)
	{
		Assert(vector->serialized_data != NULL);
		Assert(vector->serialized_size > 0);
		return vector->flat_size;
	}

    OS_CHECK(GrB_wait(vector->vector, GrB_MATERIALIZE),
		  vector->vector,
		  "Error waiting to materialize vector.");

	OS_CHECK(GxB_Vector_serialize(&serialized_data, &serialized_size, vector->vector, NULL),
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

	LOGF();

	vector = (os_Vector *) eohptr;
	flat = (os_FlatVector *) result;

	Assert(vector->em_magic == vector_MAGIC);
	Assert(allocated_size == vector->flat_size);
	Assert(vector->serialized_data != NULL);
	Assert(vector->serialized_size > 0);

	memset(flat, 0, allocated_size);

	OS_CHECK(GrB_get(vector->vector, &flat->type_code, GrB_EL_TYPE_CODE),
		  vector->vector,
		  "Cannot get Vector Type code.");

	OS_VSIZE(flat->size, vector);
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
		OS_CHECK(GrB_Vector_new(&vector->vector, type, size),
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

	type = code_type(flat->type_code);
	vector = new_vector(type, flat->size, parentcontext, NULL);
	data = OS_VECTOR_DATA(flat);
	OS_CHECK(GxB_Vector_deserialize(&vector->vector, type, data, flat->serialized_size, NULL),
		  vector->vector,
		  "Error deserializing vector");
	OS_RETURN_VECTOR(vector);
}

static void
context_callback_vector_free(void* ptr)
{
	os_Vector *vector = (os_Vector *) ptr;
	OS_CHECK(GrB_Vector_free(&vector->vector),
		  vector->vector,
		  "Cannot GrB_Free Vector");
}

/* Helper function to always expand datum

   This is used by PG_GETARG_VECTOR */
os_Vector* DatumGetVector(Datum datum)
{
	os_Vector *vector;
	os_FlatVector *flat;

	if (VARATT_IS_EXTERNAL_EXPANDED(DatumGetPointer(datum))) {
		vector = VectorGetEOHP(datum);
		Assert(vector->em_magic == vector_MAGIC);
		return vector;
	}
	flat = OS_DETOAST_VECTOR(datum);
	datum = expand_vector(flat, CurrentMemoryContext);
	return VectorGetEOHP(datum);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
