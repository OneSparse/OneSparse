#ifndef ONESPARSE_DESCRIPTOR_H
#define ONESPARSE_DESCRIPTOR_H

#define descriptor_MAGIC 689276813

typedef struct onesparse_FlatDescriptor {
    int32 vl_len_;
	char name[GxB_MAX_NAME_LEN];
} onesparse_FlatDescriptor;

typedef struct onesparse_Descriptor  {
    ExpandedObjectHeader hdr;
    int em_magic;
    Size flat_size;
    GrB_Descriptor descriptor;
	char* name;
} onesparse_Descriptor;

Datum expand_descriptor(onesparse_FlatDescriptor *flat,	MemoryContext parentcontext);

onesparse_Descriptor* new_descriptor(
	char* name,
	MemoryContext parentcontext);

onesparse_Descriptor* DatumGetDescriptor(Datum d);

#define ONESPARSE_DETOAST_DESCRIPTOR(_datum) (onesparse_FlatDescriptor*)PG_DETOAST_DATUM(datum)
#define ONESPARSE_GETARG_DESCRIPTOR(_arg_num)  DatumGetDescriptor(PG_GETARG_DATUM(_arg_num))
#define ONESPARSE_RETURN_DESCRIPTOR(_descriptor) return EOHPGetRWDatum(&(_descriptor)->hdr)
#define ONESPARSE_DESCRIPTOR_FLATSIZE() MAXALIGN(sizeof(onesparse_FlatDescriptor))

#define ONESPARSE_DESCRIPTOR_DATA(_flat) ((char*)(_flat) + ONESPARSE_DESCRIPTOR_FLATSIZE())
#define DescriptorGetEOHP(_datum) (onesparse_Descriptor *) DatumGetEOHP(_datum)

#endif /* ONESPARSE_DESCRIPTOR_H */

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
