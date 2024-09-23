#ifndef ONESPARSE_MONOID_H
#define ONESPARSE_MONOID_H

#define monoid_MAGIC 689276813

typedef struct onesparse_FlatMonoid {
    int32 vl_len_;
	char name[GxB_MAX_NAME_LEN];
} onesparse_FlatMonoid;

typedef struct onesparse_Monoid  {
    ExpandedObjectHeader hdr;
    int em_magic;
    Size flat_size;
    GrB_Monoid monoid;
	char* name;
} onesparse_Monoid;

Datum expand_monoid(onesparse_FlatMonoid *flat,	MemoryContext parentcontext);

onesparse_Monoid* new_monoid(
	char* name,
	MemoryContext parentcontext);

onesparse_Monoid* DatumGetMonoid(Datum d);

#define ONESPARSE_DETOAST_MONOID(_datum) (onesparse_FlatMonoid*)PG_DETOAST_DATUM(datum)
#define ONESPARSE_GETARG_MONOID(_arg_num)  DatumGetMonoid(PG_GETARG_DATUM(_arg_num))
#define ONESPARSE_RETURN_MONOID(_monoid) return EOHPGetRWDatum(&(_monoid)->hdr)
#define ONESPARSE_MONOID_FLATSIZE() MAXALIGN(sizeof(onesparse_FlatMonoid))

#define ONESPARSE_MONOID_DATA(_flat) ((char*)(_flat) + ONESPARSE_MONOID_FLATSIZE())
#define MonoidGetEOHP(_datum) (onesparse_Monoid *) DatumGetEOHP(_datum)

#endif /* ONESPARSE_MONOID_H */

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
