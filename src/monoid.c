#include "monoid.h"

void initialize_monoids(void);
GrB_Monoid lookup_monoid(char *name);

#include "monoid_header.h"

static Size monoid_get_flat_size(ExpandedObjectHeader *eohptr) {
	onesparse_Monoid *monoid;

	LOGF();

	monoid = (onesparse_Monoid*) eohptr;
	Assert(monoid->em_magic == monoid_MAGIC);

	if (monoid->flat_size)
		return monoid->flat_size;

	monoid->flat_size = ONESPARSE_MONOID_FLATSIZE();
	return monoid->flat_size;
}

/* Flatten monoid into a pre-allocated result buffer that is
   allocated_size in bytes.  */
static void flatten_monoid(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size)
{
	onesparse_Monoid *monoid;
	onesparse_FlatMonoid *flat;

	LOGF();

	monoid = (onesparse_Monoid *) eohptr;
	flat = (onesparse_FlatMonoid *) result;

	Assert(monoid->em_magic == monoid_MAGIC);
	Assert(allocated_size == monoid->flat_size);
	memset(flat, 0, allocated_size);
	strncpy(flat->name, monoid->name, strlen(monoid->name)+1);
	SET_VARSIZE(flat, allocated_size);
}

/* Construct an empty expanded monoid. */
onesparse_Monoid* new_monoid(
	char* name,
	MemoryContext parentcontext)
{
	GrB_Monoid binop;
	onesparse_Monoid *monoid;

	MemoryContext objcxt, oldcxt;
	MemoryContextCallback *ctxcb;

	LOGF();
	binop = lookup_monoid(name);

	if (binop == NULL)
		elog(ERROR, "Unknown monoid %s", name);

	objcxt = AllocSetContextCreate(parentcontext,
								   "expanded monoid",
								   ALLOCSET_DEFAULT_SIZES);

	monoid = MemoryContextAlloc(objcxt, sizeof(onesparse_Monoid));

	EOH_init_header(&monoid->hdr, &monoid_methods, objcxt);

	oldcxt = MemoryContextSwitchTo(objcxt);

	monoid->em_magic = monoid_MAGIC;
	monoid->flat_size = 0;
	monoid->name = palloc(strlen(name)+1);
	strncpy(monoid->name, name, strlen(name)+1);
	monoid->monoid = binop;

	ctxcb = MemoryContextAlloc(objcxt, sizeof(MemoryContextCallback));

	ctxcb->func = context_callback_monoid_free;
	ctxcb->arg = monoid;

	MemoryContextRegisterResetCallback(objcxt, ctxcb);

	MemoryContextSwitchTo(oldcxt);
	return monoid;
}

/* Expand a flat monoid in to an Expanded one, return as Postgres Datum. */
Datum expand_monoid(onesparse_FlatMonoid *flat, MemoryContext parentcontext)
{
	onesparse_Monoid *monoid;

	LOGF();

	monoid = new_monoid(flat->name, parentcontext);
	ONESPARSE_RETURN_MONOID(monoid);
}

static void
context_callback_monoid_free(void* ptr)
{
	onesparse_Monoid *monoid = (onesparse_Monoid *) ptr;
	LOGF();

	ERRORIF(GrB_Monoid_free(&monoid->monoid) != GrB_SUCCESS,
			"Cannot GrB_Free Monoid");
}

/* Helper function to always expand datum

   This is used by PG_GETARG_MONOID */
onesparse_Monoid* DatumGetMonoid(Datum datum)
{
	onesparse_Monoid *monoid;
	onesparse_FlatMonoid *flat;

	LOGF();
	if (VARATT_IS_EXTERNAL_EXPANDED_RW(DatumGetPointer(datum))) {
		monoid = MonoidGetEOHP(datum);
		Assert(monoid->em_magic == monoid_MAGIC);
		return monoid;
	}
	flat = ONESPARSE_DETOAST_MONOID(datum);
	datum = expand_monoid(flat, CurrentMemoryContext);
	return MonoidGetEOHP(datum);
}

Datum monoid_in(PG_FUNCTION_ARGS)
{
	onesparse_Monoid *monoid;
	char* input;

	input = PG_GETARG_CSTRING(0);
	monoid = new_monoid(input, CurrentMemoryContext);
	ONESPARSE_RETURN_MONOID(monoid);
}

Datum monoid_out(PG_FUNCTION_ARGS)
{
	char *result;
	onesparse_Monoid *monoid;

	LOGF();
	monoid = ONESPARSE_GETARG_MONOID(0);

	result = palloc(strlen(monoid->name)+1);
	snprintf(result, strlen(monoid->name)+1, "%s", monoid->name);
	PG_RETURN_CSTRING(result);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
