#ifndef ONESPARSE_BINARYOP_H
#define ONESPARSE_BINARYOP_H

#define binaryop_MAGIC 689276813

typedef struct onesparse_FlatBinaryOp {
    int32 vl_len_;
	char name[GxB_MAX_NAME_LEN];
} onesparse_FlatBinaryOp;

typedef struct onesparse_BinaryOp  {
    ExpandedObjectHeader hdr;
    int em_magic;
    Size flat_size;
    GrB_BinaryOp binaryop;
	char* name;
} onesparse_BinaryOp;

Datum expand_binaryop(onesparse_FlatBinaryOp *flat,	MemoryContext parentcontext);

onesparse_BinaryOp* new_binaryop(
	char* name,
	MemoryContext parentcontext);

onesparse_BinaryOp* DatumGetBinaryOp(Datum d);

#define ONESPARSE_DETOAST_BINARYOP(_datum) (onesparse_FlatBinaryOp*)PG_DETOAST_DATUM(datum)
#define ONESPARSE_GETARG_BINARYOP(_arg_num)  DatumGetBinaryOp(PG_GETARG_DATUM(_arg_num))
#define ONESPARSE_RETURN_BINARYOP(_binaryop) return EOHPGetRWDatum(&(_binaryop)->hdr)
#define ONESPARSE_BINARYOP_FLATSIZE() MAXALIGN(sizeof(onesparse_FlatBinaryOp))

#define ONESPARSE_BINARYOP_DATA(_flat) ((char*)(_flat) + ONESPARSE_BINARYOP_FLATSIZE())
#define BinaryOpGetEOHP(_datum) (onesparse_BinaryOp *) DatumGetEOHP(_datum)

#endif /* ONESPARSE_BINARYOP_H */

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
