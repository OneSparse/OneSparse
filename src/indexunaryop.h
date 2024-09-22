#ifndef ONESPARSE_INDEXUNARYOP_H
#define ONESPARSE_INDEXUNARYOP_H

#define indexunaryop_MAGIC 689276813

typedef struct onesparse_FlatIndexUnaryOp {
    int32 vl_len_;
	char name[GxB_MAX_NAME_LEN];
} onesparse_FlatIndexUnaryOp;

typedef struct onesparse_IndexUnaryOp  {
    ExpandedObjectHeader hdr;
    int em_magic;
    Size flat_size;
    GrB_IndexUnaryOp indexunaryop;
	char* name;
} onesparse_IndexUnaryOp;

Datum expand_indexunaryop(onesparse_FlatIndexUnaryOp *flat,	MemoryContext parentcontext);

onesparse_IndexUnaryOp* new_indexunaryop(
	char* name,
	MemoryContext parentcontext);

onesparse_IndexUnaryOp* DatumGetIndexUnaryOp(Datum d);

#define ONESPARSE_DETOAST_INDEXUNARYOP(_datum) (onesparse_FlatIndexUnaryOp*)PG_DETOAST_DATUM(datum)
#define ONESPARSE_GETARG_INDEXUNARYOP(_arg_num)  DatumGetIndexUnaryOp(PG_GETARG_DATUM(_arg_num))
#define ONESPARSE_RETURN_INDEXUNARYOP(_indexunaryop) return EOHPGetRWDatum(&(_indexunaryop)->hdr)
#define ONESPARSE_INDEXUNARYOP_FLATSIZE() MAXALIGN(sizeof(onesparse_FlatIndexUnaryOp))

#define ONESPARSE_INDEXUNARYOP_DATA(_flat) ((char*)(_flat) + ONESPARSE_INDEXUNARYOP_FLATSIZE())
#define IndexUnaryOpGetEOHP(_datum) (onesparse_IndexUnaryOp *) DatumGetEOHP(_datum)

#endif /* ONESPARSE_INDEXUNARYOP_H */

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */