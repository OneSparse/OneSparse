#include "onesparse.h"

static void context_callback_semiring_free(void*);
static Size semiring_get_flat_size(ExpandedObjectHeader *eohptr);

static void flatten_semiring(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size);

static const ExpandedObjectMethods semiring_methods = {
	semiring_get_flat_size,
	flatten_semiring
};

PG_FUNCTION_INFO_V1(semiring_in);
PG_FUNCTION_INFO_V1(semiring_out);

void initialize_semirings(void);
GrB_Semiring lookup_semiring(char *name);

#include "semiring_header.h"

static Size semiring_get_flat_size(ExpandedObjectHeader *eohptr) {
	os_Semiring *semiring;

	LOGF();

	semiring = (os_Semiring*) eohptr;
	Assert(semiring->em_magic == semiring_MAGIC);

	if (semiring->flat_size)
		return semiring->flat_size;

	semiring->flat_size = OS_SEMIRING_FLATSIZE();
	return semiring->flat_size;
}

/* Flatten semiring into a pre-allocated result buffer that is
   allocated_size in bytes.  */
static void flatten_semiring(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size)
{
	os_Semiring *semiring;
	os_FlatSemiring *flat;

	LOGF();

	semiring = (os_Semiring *) eohptr;
	flat = (os_FlatSemiring *) result;

	Assert(semiring->em_magic == semiring_MAGIC);
	Assert(allocated_size == semiring->flat_size);
	memset(flat, 0, allocated_size);
	strncpy(flat->name, semiring->name, strlen(semiring->name)+1);
	SET_VARSIZE(flat, allocated_size);
}

/* Construct an empty expanded semiring. */
os_Semiring* new_semiring(
	char* name,
	MemoryContext parentcontext)
{
	GrB_Semiring sring;
	os_Semiring *semiring;

	MemoryContext objcxt, oldcxt;
	MemoryContextCallback *ctxcb;

	LOGF();
	sring = lookup_semiring(name);

	if (sring == NULL)
		elog(ERROR, "Unknown semiring %s", name);

	objcxt = AllocSetContextCreate(parentcontext,
								   "expanded semiring",
								   ALLOCSET_DEFAULT_SIZES);

	semiring = MemoryContextAlloc(objcxt, sizeof(os_Semiring));

	EOH_init_header(&semiring->hdr, &semiring_methods, objcxt);

	oldcxt = MemoryContextSwitchTo(objcxt);

	semiring->em_magic = semiring_MAGIC;
	semiring->flat_size = 0;
	semiring->name = palloc(strlen(name)+1);
	strncpy(semiring->name, name, strlen(name)+1);
	semiring->semiring = sring;

	ctxcb = MemoryContextAlloc(objcxt, sizeof(MemoryContextCallback));

	ctxcb->func = context_callback_semiring_free;
	ctxcb->arg = semiring;

	MemoryContextRegisterResetCallback(objcxt, ctxcb);

	MemoryContextSwitchTo(oldcxt);
	return semiring;
}

/* Expand a flat semiring in to an Expanded one, return as Postgres Datum. */
Datum expand_semiring(os_FlatSemiring *flat, MemoryContext parentcontext)
{
	os_Semiring *semiring;

	LOGF();

	semiring = new_semiring(flat->name, parentcontext);
	OS_RETURN_SEMIRING(semiring);
}

static void
context_callback_semiring_free(void* ptr)
{
	os_Semiring *semiring = (os_Semiring *) ptr;
	LOGF();

	CHECK(GrB_Semiring_free(&semiring->semiring),
		  semiring->semiring,
		  "Cannot GrB_Free Semiring");
}

/* Helper function to always expand datum

   This is used by PG_GETARG_SEMIRING */
os_Semiring* DatumGetSemiring(Datum datum)
{
	os_Semiring *semiring;
	os_FlatSemiring *flat;

	LOGF();
	if (VARATT_IS_EXTERNAL_EXPANDED_RW(DatumGetPointer(datum))) {
		semiring = SemiringGetEOHP(datum);
		Assert(semiring->em_magic == semiring_MAGIC);
		return semiring;
	}
	flat = OS_DETOAST_SEMIRING(datum);
	datum = expand_semiring(flat, CurrentMemoryContext);
	return SemiringGetEOHP(datum);
}

Datum semiring_in(PG_FUNCTION_ARGS)
{
	os_Semiring *semiring;
	char* input;

	input = PG_GETARG_CSTRING(0);
	semiring = new_semiring(input, CurrentMemoryContext);
	OS_RETURN_SEMIRING(semiring);
}

Datum semiring_out(PG_FUNCTION_ARGS)
{
	char *result;
	os_Semiring *semiring;

	LOGF();
	semiring = OS_GETARG_SEMIRING(0);

	result = palloc(strlen(semiring->name)+1);
	snprintf(result, strlen(semiring->name)+1, "%s", semiring->name);
	PG_RETURN_CSTRING(result);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
