#ifndef OS_DESCRIPTOR_H
#define OS_DESCRIPTOR_H

#define descriptor_MAGIC 689276813

typedef struct os_FlatDescriptor {
    int32 vl_len_;
	char name[GxB_MAX_NAME_LEN];
} os_FlatDescriptor;

typedef struct os_Descriptor  {
    ExpandedObjectHeader hdr;
    int em_magic;
    Size flat_size;
    GrB_Descriptor descriptor;
	char* name;
} os_Descriptor;

Datum expand_descriptor(os_FlatDescriptor *flat,	MemoryContext parentcontext);

os_Descriptor* new_descriptor(
	char* name,
	MemoryContext parentcontext);

os_Descriptor* DatumGetDescriptor(Datum d);

#define OS_DETOAST_DESCRIPTOR(_datum) (os_FlatDescriptor*)PG_DETOAST_DATUM(datum)
#define OS_GETARG_DESCRIPTOR(_arg_num)  DatumGetDescriptor(PG_GETARG_DATUM(_arg_num))
#define OS_GETARG_DESCRIPTOR_HANDLE(_arg_num)  DatumGetDescriptor(PG_GETARG_DATUM(_arg_num))->descriptor;
#define OS_GETARG_DESCRIPTOR_OR_NULL(_nargs, _arg_num) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_DESCRIPTOR(_arg_num) : NULL;
#define OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(_nargs, _arg_num) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_DESCRIPTOR(_arg_num)->descriptor : NULL;
#define OS_RETURN_DESCRIPTOR(_descriptor) return EOHPGetRWDatum(&(_descriptor)->hdr)
#define OS_DESCRIPTOR_FLATSIZE() MAXALIGN(sizeof(os_FlatDescriptor))

#define OS_DESCRIPTOR_DATA(_flat) ((char*)(_flat) + OS_DESCRIPTOR_FLATSIZE())
#define DescriptorGetEOHP(_datum) (os_Descriptor *) DatumGetEOHP(_datum)

#endif /* OS_DESCRIPTOR_H */

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
