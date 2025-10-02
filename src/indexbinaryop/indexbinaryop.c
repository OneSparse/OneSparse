#include "../onesparse.h"

static void context_callback_indexbinaryop_free(void *);
static Size indexbinaryop_get_flat_size(ExpandedObjectHeader *eohptr);

static void flatten_indexbinaryop(
								  ExpandedObjectHeader *eohptr,
								  void *result,
								  Size allocated_size);

static const ExpandedObjectMethods indexbinaryop_methods = {
	indexbinaryop_get_flat_size,
	flatten_indexbinaryop
};

PG_FUNCTION_INFO_V1(indexbinaryop_in);
PG_FUNCTION_INFO_V1(indexbinaryop_out);
PG_FUNCTION_INFO_V1(indexbinaryop_name);

void		initialize_indexbinaryops(void);

#include "indexbinaryop_header.h"

static Size
indexbinaryop_get_flat_size(ExpandedObjectHeader *eohptr)
{
	os_IndexBinaryOp *indexbinaryop;

	indexbinaryop = (os_IndexBinaryOp *) eohptr;
	Assert(indexbinaryop->em_magic == indexbinaryop_MAGIC);

	if (indexbinaryop->flat_size)
		return indexbinaryop->flat_size;

	indexbinaryop->flat_size = OS_INDEXBINARYOP_FLATSIZE();
	return indexbinaryop->flat_size;
}

/* Flatten indexbinaryop into a pre-allocated result buffer that is
   allocated_size in bytes.  */
static void
flatten_indexbinaryop(
					  ExpandedObjectHeader *eohptr,
					  void *result,
					  Size allocated_size)
{
	os_IndexBinaryOp *indexbinaryop;
	os_FlatIndexBinaryOp *flat;

	LOGF();

	indexbinaryop = (os_IndexBinaryOp *) eohptr;
	flat = (os_FlatIndexBinaryOp *) result;

	Assert(indexbinaryop->em_magic == indexbinaryop_MAGIC);
	Assert(allocated_size == indexbinaryop->flat_size);
	memset(flat, 0, allocated_size);
	strncpy(flat->name, indexbinaryop->name, strlen(indexbinaryop->name) + 1);
	SET_VARSIZE(flat, allocated_size);
}

/* Construct an empty expanded indexbinaryop. */
os_IndexBinaryOp *
new_indexbinaryop(
				  char *name,
				  MemoryContext parentcontext,
				  GxB_IndexBinaryOp binop)
{
	os_IndexBinaryOp *indexbinaryop;

	MemoryContext objcxt,
				oldcxt;
	MemoryContextCallback *ctxcb;
	size_t		len;

	LOGF();
	if (binop == NULL)
	{
		binop = lookup_indexbinaryop(name);
		if (binop == NULL)
			elog(ERROR, "Unknown indexbinaryop %s", name);
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
								   "expanded indexbinaryop",
								   ALLOCSET_DEFAULT_SIZES);

	indexbinaryop = MemoryContextAllocZero(objcxt, sizeof(os_IndexBinaryOp));

	EOH_init_header(&indexbinaryop->hdr, &indexbinaryop_methods, objcxt);

	oldcxt = MemoryContextSwitchTo(objcxt);

	indexbinaryop->em_magic = indexbinaryop_MAGIC;
	indexbinaryop->flat_size = 0;
	indexbinaryop->name = palloc(strlen(name) + 1);
	strncpy(indexbinaryop->name, name, strlen(name) + 1);
	indexbinaryop->indexbinaryop = binop;

	ctxcb = MemoryContextAlloc(objcxt, sizeof(MemoryContextCallback));

	ctxcb->func = context_callback_indexbinaryop_free;
	ctxcb->arg = indexbinaryop;

	MemoryContextRegisterResetCallback(objcxt, ctxcb);

	MemoryContextSwitchTo(oldcxt);
	return indexbinaryop;
}

/* Expand a flat indexbinaryop in to an Expanded one, return as Postgres Datum. */
Datum
expand_indexbinaryop(os_FlatIndexBinaryOp * flat, MemoryContext parentcontext)
{
	os_IndexBinaryOp *indexbinaryop;

	LOGF();

	indexbinaryop = new_indexbinaryop(flat->name, parentcontext, NULL);
	OS_RETURN_INDEXBINARYOP(indexbinaryop);
}

static void
context_callback_indexbinaryop_free(void *ptr)
{
	os_IndexBinaryOp *indexbinaryop = (os_IndexBinaryOp *) ptr;

	OS_CHECK(GxB_IndexBinaryOp_free(&indexbinaryop->indexbinaryop),
			 indexbinaryop->indexbinaryop,
			 "Cannot GrB_Free IndexBinaryOp");
}

/* Helper function to always expand datum

   This is used by PG_GETARG_INDEXBINARYOP */
os_IndexBinaryOp *
DatumGetIndexBinaryOp(Datum datum)
{
	os_IndexBinaryOp *indexbinaryop;
	os_FlatIndexBinaryOp *flat;

	if (VARATT_IS_EXTERNAL_EXPANDED(DatumGetPointer(datum)))
	{
		indexbinaryop = IndexBinaryOpGetEOHP(datum);
		Assert(indexbinaryop->em_magic == indexbinaryop_MAGIC);
		return indexbinaryop;
	}
	flat = OS_DETOAST_INDEXBINARYOP(datum);
	datum = expand_indexbinaryop(flat, CurrentMemoryContext);
	return IndexBinaryOpGetEOHP(datum);
}

Datum
indexbinaryop_in(PG_FUNCTION_ARGS)
{
	os_IndexBinaryOp *indexbinaryop;
	char	   *input;

	input = PG_GETARG_CSTRING(0);
	indexbinaryop = new_indexbinaryop(input, CurrentMemoryContext, NULL);
	OS_RETURN_INDEXBINARYOP(indexbinaryop);
}

Datum
indexbinaryop_out(PG_FUNCTION_ARGS)
{
	char	   *result;
	os_IndexBinaryOp *indexbinaryop;

	indexbinaryop = OS_GETARG_INDEXBINARYOP(0);

	result = palloc(strlen(indexbinaryop->name) + 1);
	snprintf(result, strlen(indexbinaryop->name) + 1, "%s", indexbinaryop->name);
	PG_RETURN_CSTRING(result);
}

Datum
indexbinaryop_name(PG_FUNCTION_ARGS)
{
	os_IndexBinaryOp *indexbinaryop;

	indexbinaryop = OS_GETARG_INDEXBINARYOP(0);
	PG_RETURN_TEXT_P(cstring_to_text(indexbinaryop->name));
}
