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
PG_FUNCTION_INFO_V1(descriptor_name);

void initialize_descriptors(void);
GrB_Descriptor lookup_descriptor(char *name);

#include "descriptor_header.h"

static Size descriptor_get_flat_size(ExpandedObjectHeader *eohptr) {
	os_Descriptor *descriptor;

	LOGF();

	descriptor = (os_Descriptor*) eohptr;
	Assert(descriptor->em_magic == descriptor_MAGIC);

	if (descriptor->flat_size)
		return descriptor->flat_size;

	descriptor->flat_size = OS_DESCRIPTOR_FLATSIZE();
	return descriptor->flat_size;
}

/* Flatten descriptor into a pre-allocated result buffer that is
   allocated_size in bytes.  */
static void flatten_descriptor(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size)
{
	os_Descriptor *descriptor;
	os_FlatDescriptor *flat;

	LOGF();

	descriptor = (os_Descriptor *) eohptr;
	flat = (os_FlatDescriptor *) result;

	Assert(descriptor->em_magic == descriptor_MAGIC);
	Assert(allocated_size == descriptor->flat_size);
	memset(flat, 0, allocated_size);
	strncpy(flat->name, descriptor->name, strlen(descriptor->name)+1);
	SET_VARSIZE(flat, allocated_size);
}

/* Construct an empty expanded descriptor. */
os_Descriptor* new_descriptor(
	char* name,
	MemoryContext parentcontext)
{
	GrB_Descriptor binop;
	os_Descriptor *descriptor;

	MemoryContext objcxt, oldcxt;
	MemoryContextCallback *ctxcb;

	LOGF();
	binop = lookup_descriptor(name);

	if (binop == NULL)
		elog(ERROR, "Unknown descriptor %s", name);

	objcxt = AllocSetContextCreate(parentcontext,
								   "expanded descriptor",
								   ALLOCSET_DEFAULT_SIZES);

	descriptor = MemoryContextAlloc(objcxt, sizeof(os_Descriptor));

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
Datum expand_descriptor(os_FlatDescriptor *flat, MemoryContext parentcontext)
{
	os_Descriptor *descriptor;

	LOGF();

	descriptor = new_descriptor(flat->name, parentcontext);
	OS_RETURN_DESCRIPTOR(descriptor);
}

static void
context_callback_descriptor_free(void* ptr)
{
	os_Descriptor *descriptor = (os_Descriptor *) ptr;
	LOGF();

	OS_CHECK(GrB_Descriptor_free(&descriptor->descriptor),
		  descriptor->descriptor,
		  "Cannot GrB_Free Descriptor");
}

/* Helper function to always expand datum

   This is used by PG_GETARG_DESCRIPTOR */
os_Descriptor* DatumGetDescriptor(Datum datum)
{
	os_Descriptor *descriptor;
	os_FlatDescriptor *flat;

	LOGF();
	if (VARATT_IS_EXTERNAL_EXPANDED_RW(DatumGetPointer(datum))) {
		descriptor = DescriptorGetEOHP(datum);
		Assert(descriptor->em_magic == descriptor_MAGIC);
		return descriptor;
	}
	flat = OS_DETOAST_DESCRIPTOR(datum);
	datum = expand_descriptor(flat, CurrentMemoryContext);
	return DescriptorGetEOHP(datum);
}

Datum descriptor_in(PG_FUNCTION_ARGS)
{
	os_Descriptor *descriptor;
	char* input;

	input = PG_GETARG_CSTRING(0);
	descriptor = new_descriptor(input, CurrentMemoryContext);
	OS_RETURN_DESCRIPTOR(descriptor);
}

Datum descriptor_out(PG_FUNCTION_ARGS)
{
	char *result;
	os_Descriptor *descriptor;

	LOGF();
	descriptor = OS_GETARG_DESCRIPTOR(0);

	result = palloc(strlen(descriptor->name)+1);
	snprintf(result, strlen(descriptor->name)+1, "%s", descriptor->name);
	PG_RETURN_CSTRING(result);
}

Datum descriptor_name(PG_FUNCTION_ARGS)
{
	os_Descriptor *descriptor;
	descriptor = OS_GETARG_DESCRIPTOR(0);
	PG_RETURN_TEXT_P(cstring_to_text(descriptor->name));
}
/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
