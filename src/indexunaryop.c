#include "onesparse.h"

static void context_callback_indexunaryop_free(void*);
static Size indexunaryop_get_flat_size(ExpandedObjectHeader *eohptr);

static void flatten_indexunaryop(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size);

static const ExpandedObjectMethods indexunaryop_methods = {
	indexunaryop_get_flat_size,
	flatten_indexunaryop
};

PG_FUNCTION_INFO_V1(indexunaryop_in);
PG_FUNCTION_INFO_V1(indexunaryop_out);
PG_FUNCTION_INFO_V1(indexunaryop_name);

void initialize_indexunaryops(void);
GrB_IndexUnaryOp lookup_indexunaryop(char *name);

#include "indexunaryop_header.h"

static Size indexunaryop_get_flat_size(ExpandedObjectHeader *eohptr) {
	os_IndexUnaryOp *indexunaryop;

	LOGF();

	indexunaryop = (os_IndexUnaryOp*) eohptr;
	Assert(indexunaryop->em_magic == indexunaryop_MAGIC);

	if (indexunaryop->flat_size)
		return indexunaryop->flat_size;

	indexunaryop->flat_size = OS_INDEXUNARYOP_FLATSIZE();
	return indexunaryop->flat_size;
}

/* Flatten indexunaryop into a pre-allocated result buffer that is
   allocated_size in bytes.  */
static void flatten_indexunaryop(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size)
{
	os_IndexUnaryOp *indexunaryop;
	os_FlatIndexUnaryOp *flat;

	LOGF();

	indexunaryop = (os_IndexUnaryOp *) eohptr;
	flat = (os_FlatIndexUnaryOp *) result;

	Assert(indexunaryop->em_magic == indexunaryop_MAGIC);
	Assert(allocated_size == indexunaryop->flat_size);
	memset(flat, 0, allocated_size);
	strncpy(flat->name, indexunaryop->name, strlen(indexunaryop->name)+1);
	SET_VARSIZE(flat, allocated_size);
}

/* Construct an empty expanded indexunaryop. */
os_IndexUnaryOp* new_indexunaryop(
	char* name,
	MemoryContext parentcontext)
{
	GrB_IndexUnaryOp binop;
	os_IndexUnaryOp *indexunaryop;

	MemoryContext objcxt, oldcxt;
	MemoryContextCallback *ctxcb;

	LOGF();
	binop = lookup_indexunaryop(name);

	if (binop == NULL)
		elog(ERROR, "Unknown indexunaryop %s", name);

	objcxt = AllocSetContextCreate(parentcontext,
								   "expanded indexunaryop",
								   ALLOCSET_DEFAULT_SIZES);

	indexunaryop = MemoryContextAlloc(objcxt, sizeof(os_IndexUnaryOp));

	EOH_init_header(&indexunaryop->hdr, &indexunaryop_methods, objcxt);

	oldcxt = MemoryContextSwitchTo(objcxt);

	indexunaryop->em_magic = indexunaryop_MAGIC;
	indexunaryop->flat_size = 0;
	indexunaryop->name = palloc(strlen(name)+1);
	strncpy(indexunaryop->name, name, strlen(name)+1);
	indexunaryop->indexunaryop = binop;

	ctxcb = MemoryContextAlloc(objcxt, sizeof(MemoryContextCallback));

	ctxcb->func = context_callback_indexunaryop_free;
	ctxcb->arg = indexunaryop;

	MemoryContextRegisterResetCallback(objcxt, ctxcb);

	MemoryContextSwitchTo(oldcxt);
	return indexunaryop;
}

/* Expand a flat indexunaryop in to an Expanded one, return as Postgres Datum. */
Datum expand_indexunaryop(os_FlatIndexUnaryOp *flat, MemoryContext parentcontext)
{
	os_IndexUnaryOp *indexunaryop;

	LOGF();

	indexunaryop = new_indexunaryop(flat->name, parentcontext);
	OS_RETURN_INDEXUNARYOP(indexunaryop);
}

static void
context_callback_indexunaryop_free(void* ptr)
{
	os_IndexUnaryOp *indexunaryop = (os_IndexUnaryOp *) ptr;
	LOGF();

	OS_CHECK(GrB_IndexUnaryOp_free(&indexunaryop->indexunaryop),
		  indexunaryop->indexunaryop,
		  "Cannot GrB_Free IndexUnaryOp");
}

/* Helper function to always expand datum

   This is used by PG_GETARG_INDEXUNARYOP */
os_IndexUnaryOp* DatumGetIndexUnaryOp(Datum datum)
{
	os_IndexUnaryOp *indexunaryop;
	os_FlatIndexUnaryOp *flat;

	LOGF();
	if (VARATT_IS_EXTERNAL_EXPANDED_RW(DatumGetPointer(datum))) {
		indexunaryop = IndexUnaryOpGetEOHP(datum);
		Assert(indexunaryop->em_magic == indexunaryop_MAGIC);
		return indexunaryop;
	}
	flat = OS_DETOAST_INDEXUNARYOP(datum);
	datum = expand_indexunaryop(flat, CurrentMemoryContext);
	return IndexUnaryOpGetEOHP(datum);
}

Datum indexunaryop_in(PG_FUNCTION_ARGS)
{
	os_IndexUnaryOp *indexunaryop;
	char* input;

	input = PG_GETARG_CSTRING(0);
	indexunaryop = new_indexunaryop(input, CurrentMemoryContext);
	OS_RETURN_INDEXUNARYOP(indexunaryop);
}

Datum indexunaryop_out(PG_FUNCTION_ARGS)
{
	char *result;
	os_IndexUnaryOp *indexunaryop;

	LOGF();
	indexunaryop = OS_GETARG_INDEXUNARYOP(0);

	result = palloc(strlen(indexunaryop->name)+1);
	snprintf(result, strlen(indexunaryop->name)+1, "%s", indexunaryop->name);
	PG_RETURN_CSTRING(result);
}

Datum indexunaryop_name(PG_FUNCTION_ARGS)
{
	os_IndexUnaryOp *indexunaryop;
	indexunaryop = OS_GETARG_INDEXUNARYOP(0);
	PG_RETURN_TEXT_P(cstring_to_text(indexunaryop->name));
}
/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
