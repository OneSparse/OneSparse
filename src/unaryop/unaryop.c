#include "../onesparse.h"

static void context_callback_unaryop_free(void*);
static Size unaryop_get_flat_size(ExpandedObjectHeader *eohptr);

static void flatten_unaryop(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size);

static const ExpandedObjectMethods unaryop_methods = {
	unaryop_get_flat_size,
	flatten_unaryop
};

PG_FUNCTION_INFO_V1(unaryop_in);
PG_FUNCTION_INFO_V1(unaryop_out);
PG_FUNCTION_INFO_V1(unaryop_name);

#include "unaryop_header.h"

static Size unaryop_get_flat_size(ExpandedObjectHeader *eohptr) {
	os_UnaryOp *unaryop;

	unaryop = (os_UnaryOp*) eohptr;
	Assert(unaryop->em_magic == unaryop_MAGIC);

	if (unaryop->flat_size)
		return unaryop->flat_size;

	unaryop->flat_size = OS_UNARYOP_FLATSIZE();
	return unaryop->flat_size;
}

/* Flatten unaryop into a pre-allocated result buffer that is
   allocated_size in bytes.  */
static void flatten_unaryop(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size)
{
	os_UnaryOp *unaryop;
	os_FlatUnaryOp *flat;

	LOGF();

	unaryop = (os_UnaryOp *) eohptr;
	flat = (os_FlatUnaryOp *) result;

	Assert(unaryop->em_magic == unaryop_MAGIC);
	Assert(allocated_size == unaryop->flat_size);
	memset(flat, 0, allocated_size);
	strncpy(flat->name, unaryop->name, strlen(unaryop->name)+1);
	SET_VARSIZE(flat, allocated_size);
}

/* Construct an empty expanded unaryop. */
os_UnaryOp* new_unaryop(
	char* name,
	MemoryContext parentcontext)
{
	GrB_UnaryOp binop;
	os_UnaryOp *unaryop;

	MemoryContext objcxt, oldcxt;
	MemoryContextCallback *ctxcb;

	LOGF();
	binop = lookup_unaryop(name);

	if (binop == NULL)
		elog(ERROR, "Unknown unaryop %s", name);

	objcxt = AllocSetContextCreate(parentcontext,
								   "expanded unaryop",
								   ALLOCSET_DEFAULT_SIZES);

	unaryop = MemoryContextAllocZero(objcxt, sizeof(os_UnaryOp));

	EOH_init_header(&unaryop->hdr, &unaryop_methods, objcxt);

	oldcxt = MemoryContextSwitchTo(objcxt);

	unaryop->em_magic = unaryop_MAGIC;
	unaryop->flat_size = 0;
	unaryop->name = palloc(strlen(name)+1);
	strncpy(unaryop->name, name, strlen(name)+1);
	unaryop->unaryop = binop;

	ctxcb = MemoryContextAlloc(objcxt, sizeof(MemoryContextCallback));

	ctxcb->func = context_callback_unaryop_free;
	ctxcb->arg = unaryop;

	MemoryContextRegisterResetCallback(objcxt, ctxcb);

	MemoryContextSwitchTo(oldcxt);
	return unaryop;
}

/* Expand a flat unaryop in to an Expanded one, return as Postgres Datum. */
Datum expand_unaryop(os_FlatUnaryOp *flat, MemoryContext parentcontext)
{
	os_UnaryOp *unaryop;

	LOGF();

	unaryop = new_unaryop(flat->name, parentcontext);
	OS_RETURN_UNARYOP(unaryop);
}

static void
context_callback_unaryop_free(void* ptr)
{
	os_UnaryOp *unaryop = (os_UnaryOp *) ptr;
	OS_CHECK(GrB_UnaryOp_free(&unaryop->unaryop),
		  unaryop->unaryop,
		  "Cannot GrB_Free UnaryOp");
}

/* Helper function to always expand datum

   This is used by PG_GETARG_UNARYOP */
os_UnaryOp* DatumGetUnaryOp(Datum datum)
{
	os_UnaryOp *unaryop;
	os_FlatUnaryOp *flat;
	if (VARATT_IS_EXTERNAL_EXPANDED(DatumGetPointer(datum))) {
		unaryop = UnaryOpGetEOHP(datum);
		Assert(unaryop->em_magic == unaryop_MAGIC);
		return unaryop;
	}
	flat = OS_DETOAST_UNARYOP(datum);
	datum = expand_unaryop(flat, CurrentMemoryContext);
	return UnaryOpGetEOHP(datum);
}

Datum unaryop_in(PG_FUNCTION_ARGS)
{
	os_UnaryOp *unaryop;
	char* input;

	input = PG_GETARG_CSTRING(0);
	unaryop = new_unaryop(input, CurrentMemoryContext);
	OS_RETURN_UNARYOP(unaryop);
}

Datum unaryop_out(PG_FUNCTION_ARGS)
{
	char *result;
	os_UnaryOp *unaryop;
	unaryop = OS_GETARG_UNARYOP(0);

	result = palloc(strlen(unaryop->name)+1);
	snprintf(result, strlen(unaryop->name)+1, "%s", unaryop->name);
	PG_RETURN_CSTRING(result);
}

Datum unaryop_name(PG_FUNCTION_ARGS)
{
	os_UnaryOp *unaryop;
	unaryop = OS_GETARG_UNARYOP(0);
	PG_RETURN_TEXT_P(cstring_to_text(unaryop->name));
}
