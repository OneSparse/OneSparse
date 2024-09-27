#include "onesparse.h"

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

void initialize_unaryops(void);
GrB_UnaryOp lookup_unaryop(char *name);

#include "unaryop_header.h"

static Size unaryop_get_flat_size(ExpandedObjectHeader *eohptr) {
	onesparse_UnaryOp *unaryop;

	LOGF();

	unaryop = (onesparse_UnaryOp*) eohptr;
	Assert(unaryop->em_magic == unaryop_MAGIC);

	if (unaryop->flat_size)
		return unaryop->flat_size;

	unaryop->flat_size = ONESPARSE_UNARYOP_FLATSIZE();
	return unaryop->flat_size;
}

/* Flatten unaryop into a pre-allocated result buffer that is
   allocated_size in bytes.  */
static void flatten_unaryop(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size)
{
	onesparse_UnaryOp *unaryop;
	onesparse_FlatUnaryOp *flat;

	LOGF();

	unaryop = (onesparse_UnaryOp *) eohptr;
	flat = (onesparse_FlatUnaryOp *) result;

	Assert(unaryop->em_magic == unaryop_MAGIC);
	Assert(allocated_size == unaryop->flat_size);
	memset(flat, 0, allocated_size);
	strncpy(flat->name, unaryop->name, strlen(unaryop->name)+1);
	SET_VARSIZE(flat, allocated_size);
}

/* Construct an empty expanded unaryop. */
onesparse_UnaryOp* new_unaryop(
	char* name,
	MemoryContext parentcontext)
{
	GrB_UnaryOp binop;
	onesparse_UnaryOp *unaryop;

	MemoryContext objcxt, oldcxt;
	MemoryContextCallback *ctxcb;

	LOGF();
	binop = lookup_unaryop(name);

	if (binop == NULL)
		elog(ERROR, "Unknown unaryop %s", name);

	objcxt = AllocSetContextCreate(parentcontext,
								   "expanded unaryop",
								   ALLOCSET_DEFAULT_SIZES);

	unaryop = MemoryContextAlloc(objcxt, sizeof(onesparse_UnaryOp));

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
Datum expand_unaryop(onesparse_FlatUnaryOp *flat, MemoryContext parentcontext)
{
	onesparse_UnaryOp *unaryop;

	LOGF();

	unaryop = new_unaryop(flat->name, parentcontext);
	ONESPARSE_RETURN_UNARYOP(unaryop);
}

static void
context_callback_unaryop_free(void* ptr)
{
	onesparse_UnaryOp *unaryop = (onesparse_UnaryOp *) ptr;
	LOGF();

	CHECK(GrB_UnaryOp_free(&unaryop->unaryop),
		  unaryop->unaryop,
		  "Cannot GrB_Free UnaryOp");
}

/* Helper function to always expand datum

   This is used by PG_GETARG_UNARYOP */
onesparse_UnaryOp* DatumGetUnaryOp(Datum datum)
{
	onesparse_UnaryOp *unaryop;
	onesparse_FlatUnaryOp *flat;

	LOGF();
	if (VARATT_IS_EXTERNAL_EXPANDED_RW(DatumGetPointer(datum))) {
		unaryop = UnaryOpGetEOHP(datum);
		Assert(unaryop->em_magic == unaryop_MAGIC);
		return unaryop;
	}
	flat = ONESPARSE_DETOAST_UNARYOP(datum);
	datum = expand_unaryop(flat, CurrentMemoryContext);
	return UnaryOpGetEOHP(datum);
}

Datum unaryop_in(PG_FUNCTION_ARGS)
{
	onesparse_UnaryOp *unaryop;
	char* input;

	input = PG_GETARG_CSTRING(0);
	unaryop = new_unaryop(input, CurrentMemoryContext);
	ONESPARSE_RETURN_UNARYOP(unaryop);
}

Datum unaryop_out(PG_FUNCTION_ARGS)
{
	char *result;
	onesparse_UnaryOp *unaryop;

	LOGF();
	unaryop = ONESPARSE_GETARG_UNARYOP(0);

	result = palloc(strlen(unaryop->name)+1);
	snprintf(result, strlen(unaryop->name)+1, "%s", unaryop->name);
	PG_RETURN_CSTRING(result);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
