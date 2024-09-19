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

void initialize_indexunaryops(void);
GrB_IndexUnaryOp lookup_indexunaryop(char *name);

#include "indexunaryop_header.h"

static Size indexunaryop_get_flat_size(ExpandedObjectHeader *eohptr) {
	onesparse_IndexUnaryOp *indexunaryop;

	LOGF();

	indexunaryop = (onesparse_IndexUnaryOp*) eohptr;
	Assert(indexunaryop->em_magic == indexunaryop_MAGIC);

	if (indexunaryop->flat_size)
		return indexunaryop->flat_size;

	indexunaryop->flat_size = ONESPARSE_INDEXUNARYOP_FLATSIZE();
	return indexunaryop->flat_size;
}

/* Flatten indexunaryop into a pre-allocated result buffer that is
   allocated_size in bytes.  */
static void flatten_indexunaryop(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size)
{
	onesparse_IndexUnaryOp *indexunaryop;
	onesparse_FlatIndexUnaryOp *flat;

	LOGF();

	indexunaryop = (onesparse_IndexUnaryOp *) eohptr;
	flat = (onesparse_FlatIndexUnaryOp *) result;

	Assert(indexunaryop->em_magic == indexunaryop_MAGIC);
	Assert(allocated_size == indexunaryop->flat_size);
	memset(flat, 0, allocated_size);
	strncpy(flat->name, indexunaryop->name, strlen(indexunaryop->name)+1);
	SET_VARSIZE(flat, allocated_size);
}

/* Construct an empty expanded indexunaryop. */
onesparse_IndexUnaryOp* new_indexunaryop(
	char* name,
	MemoryContext parentcontext)
{
	GrB_IndexUnaryOp binop;
	onesparse_IndexUnaryOp *indexunaryop;

	MemoryContext objcxt, oldcxt;
	MemoryContextCallback *ctxcb;

	LOGF();
	binop = lookup_indexunaryop(name);

	if (binop == NULL)
		elog(ERROR, "Unknown indexunaryop %s", name);

	objcxt = AllocSetContextCreate(parentcontext,
								   "expanded indexunaryop",
								   ALLOCSET_DEFAULT_SIZES);

	indexunaryop = MemoryContextAlloc(objcxt, sizeof(onesparse_IndexUnaryOp));

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
Datum expand_indexunaryop(onesparse_FlatIndexUnaryOp *flat, MemoryContext parentcontext)
{
	onesparse_IndexUnaryOp *indexunaryop;

	LOGF();

	indexunaryop = new_indexunaryop(flat->name, parentcontext);
	ONESPARSE_RETURN_INDEXUNARYOP(indexunaryop);
}

static void
context_callback_indexunaryop_free(void* ptr)
{
	onesparse_IndexUnaryOp *indexunaryop = (onesparse_IndexUnaryOp *) ptr;
	LOGF();

	ERRORIF(GrB_IndexUnaryOp_free(&indexunaryop->indexunaryop) != GrB_SUCCESS,
			"Cannot GrB_Free IndexUnaryOp");
}

/* Helper function to always expand datum

   This is used by PG_GETARG_INDEXUNARYOP */
onesparse_IndexUnaryOp* DatumGetIndexUnaryOp(Datum datum)
{
	onesparse_IndexUnaryOp *indexunaryop;
	onesparse_FlatIndexUnaryOp *flat;

	LOGF();
	if (VARATT_IS_EXTERNAL_EXPANDED_RW(DatumGetPointer(datum))) {
		indexunaryop = IndexUnaryOpGetEOHP(datum);
		Assert(indexunaryop->em_magic == indexunaryop_MAGIC);
		return indexunaryop;
	}
	flat = ONESPARSE_DETOAST_INDEXUNARYOP(datum);
	datum = expand_indexunaryop(flat, CurrentMemoryContext);
	return IndexUnaryOpGetEOHP(datum);
}

Datum indexunaryop_in(PG_FUNCTION_ARGS)
{
	onesparse_IndexUnaryOp *indexunaryop;
	char* input;

	input = PG_GETARG_CSTRING(0);
	indexunaryop = new_indexunaryop(input, CurrentMemoryContext);
	ONESPARSE_RETURN_INDEXUNARYOP(indexunaryop);
}

Datum indexunaryop_out(PG_FUNCTION_ARGS)
{
	char *result;
	onesparse_IndexUnaryOp *indexunaryop;

	LOGF();
	indexunaryop = ONESPARSE_GETARG_INDEXUNARYOP(0);

	result = palloc(strlen(indexunaryop->name)+1);
	snprintf(result, strlen(indexunaryop->name)+1, "%s", indexunaryop->name);
	PG_RETURN_CSTRING(result);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
