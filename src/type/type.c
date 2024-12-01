#include "../onesparse.h"

static void context_callback_type_free(void*);
static Size type_get_flat_size(ExpandedObjectHeader *eohptr);

static void flatten_type(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size);

static const ExpandedObjectMethods type_methods = {
	type_get_flat_size,
	flatten_type
};

PG_FUNCTION_INFO_V1(type_in);
PG_FUNCTION_INFO_V1(type_out);
PG_FUNCTION_INFO_V1(type_name);

#include "type_header.h"

static Size type_get_flat_size(ExpandedObjectHeader *eohptr) {
	os_Type *type;

	LOGF();

	type = (os_Type*) eohptr;
	Assert(type->em_magic == type_MAGIC);

	if (type->flat_size)
		return type->flat_size;

	type->flat_size = OS_TYPE_FLATSIZE();
	return type->flat_size;
}

/* Flatten type into a pre-allocated result buffer that is
   allocated_size in bytes.  */
static void flatten_type(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size)
{
	os_Type *type;
	os_FlatType *flat;

	LOGF();

	type = (os_Type *) eohptr;
	flat = (os_FlatType *) result;

	Assert(type->em_magic == type_MAGIC);
	Assert(allocated_size == type->flat_size);
	memset(flat, 0, allocated_size);
	strncpy(flat->name, type->name, strlen(type->name)+1);
	SET_VARSIZE(flat, allocated_size);
}

/* Construct an empty expanded type. */
os_Type* new_type(
	char* name,
	MemoryContext parentcontext)
{
	GrB_Type binop;
	os_Type *type;

	MemoryContext objcxt, oldcxt;
	MemoryContextCallback *ctxcb;

	LOGF();
	binop = lookup_type(name);

	if (binop == NULL)
		elog(ERROR, "Unknown type %s", name);

	objcxt = AllocSetContextCreate(parentcontext,
								   "expanded type",
								   ALLOCSET_DEFAULT_SIZES);

	type = MemoryContextAlloc(objcxt, sizeof(os_Type));

	EOH_init_header(&type->hdr, &type_methods, objcxt);

	oldcxt = MemoryContextSwitchTo(objcxt);

	type->em_magic = type_MAGIC;
	type->flat_size = 0;
	type->name = palloc(strlen(name)+1);
	strncpy(type->name, name, strlen(name)+1);
	type->type = binop;

	ctxcb = MemoryContextAlloc(objcxt, sizeof(MemoryContextCallback));

	ctxcb->func = context_callback_type_free;
	ctxcb->arg = type;

	MemoryContextRegisterResetCallback(objcxt, ctxcb);

	MemoryContextSwitchTo(oldcxt);
	return type;
}

/* Expand a flat type in to an Expanded one, return as Postgres Datum. */
Datum expand_type(os_FlatType *flat, MemoryContext parentcontext)
{
	os_Type *type;

	LOGF();

	type = new_type(flat->name, parentcontext);
	OS_RETURN_TYPE(type);
}

static void
context_callback_type_free(void* ptr)
{
	os_Type *type = (os_Type *) ptr;
	LOGF();

	OS_CHECK(GrB_Type_free(&type->type),
		  type->type,
		  "Cannot GrB_Free Type");
}

/* Helper function to always expand datum

   This is used by PG_GETARG_TYPE */
os_Type* DatumGetType(Datum datum)
{
	os_Type *type;
	os_FlatType *flat;

	LOGF();
	if (VARATT_IS_EXTERNAL_EXPANDED_RW(DatumGetPointer(datum))) {
		type = TypeGetEOHP(datum);
		Assert(type->em_magic == type_MAGIC);
		return type;
	}
	flat = OS_DETOAST_TYPE(datum);
	datum = expand_type(flat, CurrentMemoryContext);
	return TypeGetEOHP(datum);
}

Datum type_in(PG_FUNCTION_ARGS)
{
	os_Type *type;
	char* input;

	input = PG_GETARG_CSTRING(0);
	type = new_type(input, CurrentMemoryContext);
	OS_RETURN_TYPE(type);
}

Datum type_out(PG_FUNCTION_ARGS)
{
	char *result;
	os_Type *type;

	LOGF();
	type = OS_GETARG_TYPE(0);

	result = palloc(strlen(type->name)+1);
	snprintf(result, strlen(type->name)+1, "%s", type->name);
	PG_RETURN_CSTRING(result);
}

Datum type_name(PG_FUNCTION_ARGS)
{
	os_Type *type;
	type = OS_GETARG_TYPE(0);
	PG_RETURN_TEXT_P(cstring_to_text(type->name));
}
/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
