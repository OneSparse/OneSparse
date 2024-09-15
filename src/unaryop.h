#ifndef ONESPARSE_UNARYOP_H
#define ONESPARSE_UNARYOP_H

#include "onesparse.h"

#define unaryop_MAGIC 689276813

typedef struct onesparse_FlatUnaryOp {
    int32 vl_len_;
	char name[GxB_MAX_NAME_LEN];
} onesparse_FlatUnaryOp;

typedef struct onesparse_UnaryOp  {
    ExpandedObjectHeader hdr;
    int em_magic;
    Size flat_size;
    GrB_UnaryOp unaryop;
	char* name;
} onesparse_UnaryOp;

static void context_callback_unaryop_free(void*);
static Size unaryop_get_flat_size(ExpandedObjectHeader *eohptr);

static void flatten_unaryop(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size);

static const ExpandedObjectMethods unaryop_methods = {
	unaryop_get_flat_size,
	flatten_unaryop
};

Datum expand_unaryop(onesparse_FlatUnaryOp *flat,	MemoryContext parentcontext);

onesparse_UnaryOp* new_unaryop(
	char* name,
	MemoryContext parentcontext);

onesparse_UnaryOp* DatumGetUnaryOp(Datum d);

#define ONESPARSE_DETOAST_UNARYOP(_datum) (onesparse_FlatUnaryOp*)PG_DETOAST_DATUM(datum)
#define ONESPARSE_GETARG_UNARYOP(_arg_num)  DatumGetUnaryOp(PG_GETARG_DATUM(_arg_num))
#define ONESPARSE_RETURN_UNARYOP(_unaryop) return EOHPGetRWDatum(&(_unaryop)->hdr)
#define ONESPARSE_UNARYOP_FLATSIZE() MAXALIGN(sizeof(onesparse_FlatUnaryOp))

#define ONESPARSE_UNARYOP_DATA(_flat) ((char*)(_flat) + ONESPARSE_UNARYOP_FLATSIZE())
#define UnaryOpGetEOHP(_datum) (onesparse_UnaryOp *) DatumGetEOHP(_datum)

PG_FUNCTION_INFO_V1(unaryop_in);
PG_FUNCTION_INFO_V1(unaryop_out);

#endif /* ONESPARSE_UNARYOP_H */

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
