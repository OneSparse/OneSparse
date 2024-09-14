#ifndef ONESPARSE_SEMIRING_H
#define ONESPARSE_SEMIRING_H

#include "onesparse.h"

#define semiring_MAGIC 689276813

typedef struct onesparse_FlatSemiring {
    int32 vl_len_;
	char name[GxB_MAX_NAME_LEN];
} onesparse_FlatSemiring;

typedef struct onesparse_Semiring  {
    ExpandedObjectHeader hdr;
    int em_magic;
    Size flat_size;
    GrB_Semiring semiring;
	char* name;
} onesparse_Semiring;

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

Datum expand_semiring(onesparse_FlatSemiring *flat,	MemoryContext parentcontext);

onesparse_Semiring* new_semiring(
	char* name,
	MemoryContext parentcontext);

onesparse_Semiring* DatumGetSemiring(Datum d);

#define ONESPARSE_DETOAST_SEMIRING(_datum) (onesparse_FlatSemiring*)PG_DETOAST_DATUM(datum)
#define ONESPARSE_GETARG_SEMIRING(_arg_num)  DatumGetSemiring(PG_GETARG_DATUM(_arg_num))
#define ONESPARSE_RETURN_SEMIRING(_semiring) return EOHPGetRWDatum(&(_semiring)->hdr)
#define ONESPARSE_SEMIRING_FLATSIZE() MAXALIGN(sizeof(onesparse_FlatSemiring))

#define ONESPARSE_SEMIRING_DATA(_flat) ((char*)(_flat) + ONESPARSE_SEMIRING_FLATSIZE())
#define SemiringGetEOHP(_datum) (onesparse_Semiring *) DatumGetEOHP(_datum)

PG_FUNCTION_INFO_V1(semiring_in);
PG_FUNCTION_INFO_V1(semiring_out);

#endif /* ONESPARSE_SEMIRING_H */

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
