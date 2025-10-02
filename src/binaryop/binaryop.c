#include "../onesparse.h"

static void context_callback_binaryop_free(void *);
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
PG_FUNCTION_INFO_V1(binaryop_name);

void		initialize_binaryops(void);

#include "binaryop_header.h"

static Size
binaryop_get_flat_size(ExpandedObjectHeader *eohptr)
{
	os_BinaryOp *binaryop;

	binaryop = (os_BinaryOp *) eohptr;
	Assert(binaryop->em_magic == binaryop_MAGIC);

	if (binaryop->flat_size)
		return binaryop->flat_size;

	binaryop->flat_size = OS_BINARYOP_FLATSIZE();
	return binaryop->flat_size;
}

/* Flatten binaryop into a pre-allocated result buffer that is
   allocated_size in bytes.  */
static void
flatten_binaryop(
				 ExpandedObjectHeader *eohptr,
				 void *result,
				 Size allocated_size)
{
	os_BinaryOp *binaryop;
	os_FlatBinaryOp *flat;

	LOGF();

	binaryop = (os_BinaryOp *) eohptr;
	flat = (os_FlatBinaryOp *) result;

	Assert(binaryop->em_magic == binaryop_MAGIC);
	Assert(allocated_size == binaryop->flat_size);
	memset(flat, 0, allocated_size);
	strncpy(flat->name, binaryop->name, strlen(binaryop->name) + 1);
	SET_VARSIZE(flat, allocated_size);
}

/* Construct an empty expanded binaryop. */
os_BinaryOp *
new_binaryop(
			 char *name,
			 MemoryContext parentcontext,
			 GrB_BinaryOp binop)
{
	os_BinaryOp *binaryop;

	MemoryContext objcxt,
				oldcxt;
	MemoryContextCallback *ctxcb;
	size_t		len;

	LOGF();
	if (binop == NULL)
	{
		binop = lookup_binaryop(name);
		if (binop == NULL)
			elog(ERROR, "Unknown binaryop %s", name);
	}
	else
	{
		OS_CHECK(GrB_get(binop,
						 &len,
						 GrB_NAME),
				 binop,
				 "Error extracting binop name len.");

		name = palloc(len + 1);
		OS_CHECK(GrB_get(binop,
						 name,
						 GrB_NAME),
				 binop,
				 "Error extracting binop name.");
	}

	objcxt = AllocSetContextCreate(parentcontext,
								   "expanded binaryop",
								   ALLOCSET_DEFAULT_SIZES);

	binaryop = MemoryContextAllocZero(objcxt, sizeof(os_BinaryOp));

	EOH_init_header(&binaryop->hdr, &binaryop_methods, objcxt);

	oldcxt = MemoryContextSwitchTo(objcxt);

	binaryop->em_magic = binaryop_MAGIC;
	binaryop->flat_size = 0;
	binaryop->name = palloc(strlen(name) + 1);
	strncpy(binaryop->name, name, strlen(name) + 1);
	binaryop->binaryop = binop;

	ctxcb = MemoryContextAlloc(objcxt, sizeof(MemoryContextCallback));

	ctxcb->func = context_callback_binaryop_free;
	ctxcb->arg = binaryop;

	MemoryContextRegisterResetCallback(objcxt, ctxcb);

	MemoryContextSwitchTo(oldcxt);
	return binaryop;
}

/* Expand a flat binaryop in to an Expanded one, return as Postgres Datum. */
Datum
expand_binaryop(os_FlatBinaryOp * flat, MemoryContext parentcontext)
{
	os_BinaryOp *binaryop;

	LOGF();

	binaryop = new_binaryop(flat->name, parentcontext, NULL);
	OS_RETURN_BINARYOP(binaryop);
}

static void
context_callback_binaryop_free(void *ptr)
{
	os_BinaryOp *binaryop = (os_BinaryOp *) ptr;

	OS_CHECK(GrB_BinaryOp_free(&binaryop->binaryop),
			 binaryop->binaryop,
			 "Cannot GrB_Free BinaryOp");
}

/* Helper function to always expand datum

   This is used by PG_GETARG_BINARYOP */
os_BinaryOp *
DatumGetBinaryOp(Datum datum)
{
	os_BinaryOp *binaryop;
	os_FlatBinaryOp *flat;

	if (VARATT_IS_EXTERNAL_EXPANDED(DatumGetPointer(datum)))
	{
		binaryop = BinaryOpGetEOHP(datum);
		Assert(binaryop->em_magic == binaryop_MAGIC);
		return binaryop;
	}
	flat = OS_DETOAST_BINARYOP(datum);
	datum = expand_binaryop(flat, CurrentMemoryContext);
	return BinaryOpGetEOHP(datum);
}

Datum
binaryop_in(PG_FUNCTION_ARGS)
{
	os_BinaryOp *binaryop;
	char	   *input;

	input = PG_GETARG_CSTRING(0);
	binaryop = new_binaryop(input, CurrentMemoryContext, NULL);
	OS_RETURN_BINARYOP(binaryop);
}

Datum
binaryop_out(PG_FUNCTION_ARGS)
{
	char	   *result;
	os_BinaryOp *binaryop;

	binaryop = OS_GETARG_BINARYOP(0);

	result = palloc(strlen(binaryop->name) + 1);
	snprintf(result, strlen(binaryop->name) + 1, "%s", binaryop->name);
	PG_RETURN_CSTRING(result);
}

Datum
binaryop_name(PG_FUNCTION_ARGS)
{
	os_BinaryOp *binaryop;

	binaryop = OS_GETARG_BINARYOP(0);
	PG_RETURN_TEXT_P(cstring_to_text(binaryop->name));
}
