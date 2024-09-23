#include "onesparse.h"

static void context_callback_descriptor_free(void*);
static Size descriptor_get_flat_size(ExpandedObjectHeader *eohptr);

static void flatten_descriptor(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size);

static const ExpandedObjectMethods descriptor_methods = {
	descriptor_get_flat_size,
	flatten_descriptor
};

PG_FUNCTION_INFO_V1(descriptor_in);
PG_FUNCTION_INFO_V1(descriptor_out);

void initialize_descriptors(void);
GrB_Descriptor lookup_descriptor(char *name);

#include "descriptor_header.h"

static Size descriptor_get_flat_size(ExpandedObjectHeader *eohptr) {
	onesparse_Descriptor *descriptor;

	LOGF();

	descriptor = (onesparse_Descriptor*) eohptr;
	Assert(descriptor->em_magic == descriptor_MAGIC);

	if (descriptor->flat_size)
		return descriptor->flat_size;

	descriptor->flat_size = ONESPARSE_DESCRIPTOR_FLATSIZE();
	return descriptor->flat_size;
}

/* Flatten descriptor into a pre-allocated result buffer that is
   allocated_size in bytes.  */
static void flatten_descriptor(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size)
{
	onesparse_Descriptor *descriptor;
	onesparse_FlatDescriptor *flat;

	LOGF();

	descriptor = (onesparse_Descriptor *) eohptr;
	flat = (onesparse_FlatDescriptor *) result;

	Assert(descriptor->em_magic == descriptor_MAGIC);
	Assert(allocated_size == descriptor->flat_size);
	memset(flat, 0, allocated_size);
	strncpy(flat->name, descriptor->name, strlen(descriptor->name)+1);
	SET_VARSIZE(flat, allocated_size);
}

/* Construct an empty expanded descriptor. */
onesparse_Descriptor* new_descriptor(
	char* name,
	MemoryContext parentcontext)
{
	GrB_Descriptor binop;
	onesparse_Descriptor *descriptor;

	MemoryContext objcxt, oldcxt;
	MemoryContextCallback *ctxcb;

	LOGF();
	binop = lookup_descriptor(name);

	if (binop == NULL)
		elog(ERROR, "Unknown descriptor %s", name);

	objcxt = AllocSetContextCreate(parentcontext,
								   "expanded descriptor",
								   ALLOCSET_DEFAULT_SIZES);

	descriptor = MemoryContextAlloc(objcxt, sizeof(onesparse_Descriptor));

	EOH_init_header(&descriptor->hdr, &descriptor_methods, objcxt);

	oldcxt = MemoryContextSwitchTo(objcxt);

	descriptor->em_magic = descriptor_MAGIC;
	descriptor->flat_size = 0;
	descriptor->name = palloc(strlen(name)+1);
	strncpy(descriptor->name, name, strlen(name)+1);
	descriptor->descriptor = binop;

	ctxcb = MemoryContextAlloc(objcxt, sizeof(MemoryContextCallback));

	ctxcb->func = context_callback_descriptor_free;
	ctxcb->arg = descriptor;

	MemoryContextRegisterResetCallback(objcxt, ctxcb);

	MemoryContextSwitchTo(oldcxt);
	return descriptor;
}

/* Expand a flat descriptor in to an Expanded one, return as Postgres Datum. */
Datum expand_descriptor(onesparse_FlatDescriptor *flat, MemoryContext parentcontext)
{
	onesparse_Descriptor *descriptor;

	LOGF();

	descriptor = new_descriptor(flat->name, parentcontext);
	ONESPARSE_RETURN_DESCRIPTOR(descriptor);
}

static void
context_callback_descriptor_free(void* ptr)
{
	onesparse_Descriptor *descriptor = (onesparse_Descriptor *) ptr;
	LOGF();

	ERRORIF(GrB_Descriptor_free(&descriptor->descriptor) != GrB_SUCCESS,
			"Cannot GrB_Free Descriptor");
}

/* Helper function to always expand datum

   This is used by PG_GETARG_DESCRIPTOR */
onesparse_Descriptor* DatumGetDescriptor(Datum datum)
{
	onesparse_Descriptor *descriptor;
	onesparse_FlatDescriptor *flat;

	LOGF();
	if (VARATT_IS_EXTERNAL_EXPANDED_RW(DatumGetPointer(datum))) {
		descriptor = DescriptorGetEOHP(datum);
		Assert(descriptor->em_magic == descriptor_MAGIC);
		return descriptor;
	}
	flat = ONESPARSE_DETOAST_DESCRIPTOR(datum);
	datum = expand_descriptor(flat, CurrentMemoryContext);
	return DescriptorGetEOHP(datum);
}

Datum descriptor_in(PG_FUNCTION_ARGS)
{
	onesparse_Descriptor *descriptor;
	char* input;

	input = PG_GETARG_CSTRING(0);
	descriptor = new_descriptor(input, CurrentMemoryContext);
	ONESPARSE_RETURN_DESCRIPTOR(descriptor);
}

Datum descriptor_out(PG_FUNCTION_ARGS)
{
	char *result;
	onesparse_Descriptor *descriptor;

	LOGF();
	descriptor = ONESPARSE_GETARG_DESCRIPTOR(0);

	result = palloc(strlen(descriptor->name)+1);
	snprintf(result, strlen(descriptor->name)+1, "%s", descriptor->name);
	PG_RETURN_CSTRING(result);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
