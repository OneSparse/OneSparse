#ifndef OS_SEMIRING_H
#define OS_SEMIRING_H

#define semiring_MAGIC 689276813

typedef struct os_FlatSemiring {
    int32 vl_len_;
	char name[GxB_MAX_NAME_LEN];
} os_FlatSemiring;

typedef struct os_Semiring  {
    ExpandedObjectHeader hdr;
    int em_magic;
    Size flat_size;
    GrB_Semiring semiring;
	char* name;
} os_Semiring;

Datum expand_semiring(os_FlatSemiring *flat,	MemoryContext parentcontext);

os_Semiring* new_semiring(
	char* name,
	MemoryContext parentcontext);

os_Semiring* DatumGetSemiring(Datum d);

#define OS_DETOAST_SEMIRING(_datum) (os_FlatSemiring*)PG_DETOAST_DATUM(datum)
#define OS_GETARG_SEMIRING(_arg_num)  DatumGetSemiring(PG_GETARG_DATUM(_arg_num))
#define OS_RETURN_SEMIRING(_semiring) return EOHPGetRWDatum(&(_semiring)->hdr)
#define OS_SEMIRING_FLATSIZE() MAXALIGN(sizeof(os_FlatSemiring))

#define OS_SEMIRING_DATA(_flat) ((char*)(_flat) + OS_SEMIRING_FLATSIZE())
#define SemiringGetEOHP(_datum) (os_Semiring *) DatumGetEOHP(_datum)

#endif /* OS_SEMIRING_H */

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
