#include "onesparse.h"

static void context_callback_binaryop_free(void*);
static Size binaryop_get_flat_size(ExpandedObjectHeader *eohptr);

static void flatten_binaryop(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size);

static const ExpandedObjectMethods binaryop_methods = {
	binaryop_get_flat_size,
	flatten_binaryop
};

PG_FUNCTION_INFO_V1(binaryop_in);
PG_FUNCTION_INFO_V1(binaryop_out);

void initialize_binaryops(void);
GrB_BinaryOp lookup_binaryop(char *name);

#include "binaryop_header.h"

static Size binaryop_get_flat_size(ExpandedObjectHeader *eohptr) {
	onesparse_BinaryOp *binaryop;

	LOGF();

	binaryop = (onesparse_BinaryOp*) eohptr;
	Assert(binaryop->em_magic == binaryop_MAGIC);

	if (binaryop->flat_size)
		return binaryop->flat_size;

	binaryop->flat_size = ONESPARSE_BINARYOP_FLATSIZE();
	return binaryop->flat_size;
}

/* Flatten binaryop into a pre-allocated result buffer that is
   allocated_size in bytes.  */
static void flatten_binaryop(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size)
{
	onesparse_BinaryOp *binaryop;
	onesparse_FlatBinaryOp *flat;

	LOGF();

	binaryop = (onesparse_BinaryOp *) eohptr;
	flat = (onesparse_FlatBinaryOp *) result;

	Assert(binaryop->em_magic == binaryop_MAGIC);
	Assert(allocated_size == binaryop->flat_size);
	memset(flat, 0, allocated_size);
	strncpy(flat->name, binaryop->name, strlen(binaryop->name)+1);
	SET_VARSIZE(flat, allocated_size);
}

/* Construct an empty expanded binaryop. */
onesparse_BinaryOp* new_binaryop(
	char* name,
	MemoryContext parentcontext)
{
	GrB_BinaryOp binop;
	onesparse_BinaryOp *binaryop;

	MemoryContext objcxt, oldcxt;
	MemoryContextCallback *ctxcb;

	LOGF();
	binop = lookup_binaryop(name);

	if (binop == NULL)
		elog(ERROR, "Unknown binaryop %s", name);

	objcxt = AllocSetContextCreate(parentcontext,
								   "expanded binaryop",
								   ALLOCSET_DEFAULT_SIZES);

	binaryop = MemoryContextAlloc(objcxt, sizeof(onesparse_BinaryOp));

	EOH_init_header(&binaryop->hdr, &binaryop_methods, objcxt);

	oldcxt = MemoryContextSwitchTo(objcxt);

	binaryop->em_magic = binaryop_MAGIC;
	binaryop->flat_size = 0;
	binaryop->name = palloc(strlen(name)+1);
	strncpy(binaryop->name, name, strlen(name)+1);
	binaryop->binaryop = binop;

	ctxcb = MemoryContextAlloc(objcxt, sizeof(MemoryContextCallback));

	ctxcb->func = context_callback_binaryop_free;
	ctxcb->arg = binaryop;

	MemoryContextRegisterResetCallback(objcxt, ctxcb);

	MemoryContextSwitchTo(oldcxt);
	return binaryop;
}

/* Expand a flat binaryop in to an Expanded one, return as Postgres Datum. */
Datum expand_binaryop(onesparse_FlatBinaryOp *flat, MemoryContext parentcontext)
{
	onesparse_BinaryOp *binaryop;

	LOGF();

	binaryop = new_binaryop(flat->name, parentcontext);
	ONESPARSE_RETURN_BINARYOP(binaryop);
}

static void
context_callback_binaryop_free(void* ptr)
{
	onesparse_BinaryOp *binaryop = (onesparse_BinaryOp *) ptr;
	LOGF();

	ERRORIF(GrB_BinaryOp_free(&binaryop->binaryop) != GrB_SUCCESS,
			"Cannot GrB_Free BinaryOp");
}

/* Helper function to always expand datum

   This is used by PG_GETARG_BINARYOP */
onesparse_BinaryOp* DatumGetBinaryOp(Datum datum)
{
	onesparse_BinaryOp *binaryop;
	onesparse_FlatBinaryOp *flat;

	LOGF();
	if (VARATT_IS_EXTERNAL_EXPANDED_RW(DatumGetPointer(datum))) {
		binaryop = BinaryOpGetEOHP(datum);
		Assert(binaryop->em_magic == binaryop_MAGIC);
		return binaryop;
	}
	flat = ONESPARSE_DETOAST_BINARYOP(datum);
	datum = expand_binaryop(flat, CurrentMemoryContext);
	return BinaryOpGetEOHP(datum);
}

Datum binaryop_in(PG_FUNCTION_ARGS)
{
	onesparse_BinaryOp *binaryop;
	char* input;

	input = PG_GETARG_CSTRING(0);
	binaryop = new_binaryop(input, CurrentMemoryContext);
	ONESPARSE_RETURN_BINARYOP(binaryop);
}

Datum binaryop_out(PG_FUNCTION_ARGS)
{
	char *result;
	onesparse_BinaryOp *binaryop;

	LOGF();
	binaryop = ONESPARSE_GETARG_BINARYOP(0);

	result = palloc(strlen(binaryop->name)+1);
	snprintf(result, strlen(binaryop->name)+1, "%s", binaryop->name);
	PG_RETURN_CSTRING(result);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
