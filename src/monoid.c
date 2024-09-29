#include "onesparse.h"

static void context_callback_monoid_free(void*);
static Size monoid_get_flat_size(ExpandedObjectHeader *eohptr);

static void flatten_monoid(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size);

static const ExpandedObjectMethods monoid_methods = {
	monoid_get_flat_size,
	flatten_monoid
};

PG_FUNCTION_INFO_V1(monoid_in);
PG_FUNCTION_INFO_V1(monoid_out);

void initialize_monoids(void);
GrB_Monoid lookup_monoid(char *name);

#include "monoid_header.h"

static Size monoid_get_flat_size(ExpandedObjectHeader *eohptr) {
	os_Monoid *monoid;

	LOGF();

	monoid = (os_Monoid*) eohptr;
	Assert(monoid->em_magic == monoid_MAGIC);

	if (monoid->flat_size)
		return monoid->flat_size;

	monoid->flat_size = OS_MONOID_FLATSIZE();
	return monoid->flat_size;
}

/* Flatten monoid into a pre-allocated result buffer that is
   allocated_size in bytes.  */
static void flatten_monoid(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size)
{
	os_Monoid *monoid;
	os_FlatMonoid *flat;

	LOGF();

	monoid = (os_Monoid *) eohptr;
	flat = (os_FlatMonoid *) result;

	Assert(monoid->em_magic == monoid_MAGIC);
	Assert(allocated_size == monoid->flat_size);
	memset(flat, 0, allocated_size);
	strncpy(flat->name, monoid->name, strlen(monoid->name)+1);
	SET_VARSIZE(flat, allocated_size);
}

/* Construct an empty expanded monoid. */
os_Monoid* new_monoid(
	char* name,
	MemoryContext parentcontext)
{
	GrB_Monoid binop;
	os_Monoid *monoid;

	MemoryContext objcxt, oldcxt;
	MemoryContextCallback *ctxcb;

	LOGF();
	binop = lookup_monoid(name);

	if (binop == NULL)
		elog(ERROR, "Unknown monoid %s", name);

	objcxt = AllocSetContextCreate(parentcontext,
								   "expanded monoid",
								   ALLOCSET_DEFAULT_SIZES);

	monoid = MemoryContextAlloc(objcxt, sizeof(os_Monoid));

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
Datum expand_monoid(os_FlatMonoid *flat, MemoryContext parentcontext)
{
	os_Monoid *monoid;

	LOGF();

	monoid = new_monoid(flat->name, parentcontext);
	OS_RETURN_MONOID(monoid);
}

static void
context_callback_monoid_free(void* ptr)
{
	os_Monoid *monoid = (os_Monoid *) ptr;
	LOGF();

	CHECK(GrB_Monoid_free(&monoid->monoid),
		  monoid->monoid,
		  "Cannot GrB_Free Monoid");
}

/* Helper function to always expand datum

   This is used by PG_GETARG_MONOID */
os_Monoid* DatumGetMonoid(Datum datum)
{
	os_Monoid *monoid;
	os_FlatMonoid *flat;

	LOGF();
	if (VARATT_IS_EXTERNAL_EXPANDED_RW(DatumGetPointer(datum))) {
		monoid = MonoidGetEOHP(datum);
		Assert(monoid->em_magic == monoid_MAGIC);
		return monoid;
	}
	flat = OS_DETOAST_MONOID(datum);
	datum = expand_monoid(flat, CurrentMemoryContext);
	return MonoidGetEOHP(datum);
}

Datum monoid_in(PG_FUNCTION_ARGS)
{
	os_Monoid *monoid;
	char* input;

	input = PG_GETARG_CSTRING(0);
	monoid = new_monoid(input, CurrentMemoryContext);
	OS_RETURN_MONOID(monoid);
}

Datum monoid_out(PG_FUNCTION_ARGS)
{
	char *result;
	os_Monoid *monoid;

	LOGF();
	monoid = OS_GETARG_MONOID(0);

	result = palloc(strlen(monoid->name)+1);
	snprintf(result, strlen(monoid->name)+1, "%s", monoid->name);
	PG_RETURN_CSTRING(result);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
