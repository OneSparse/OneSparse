#ifndef OS_INDEXUNARYOP_H
#define OS_INDEXUNARYOP_H

#define indexunaryop_MAGIC 689276818

typedef struct os_FlatIndexUnaryOp {
    int32 vl_len_;
	char name[GxB_MAX_NAME_LEN];
} os_FlatIndexUnaryOp;

typedef struct os_IndexUnaryOp  {
    ExpandedObjectHeader hdr;
    int em_magic;
    Size flat_size;
    GrB_IndexUnaryOp indexunaryop;
	char* name;
} os_IndexUnaryOp;

Datum expand_indexunaryop(os_FlatIndexUnaryOp *flat,	MemoryContext parentcontext);

os_IndexUnaryOp* new_indexunaryop(
	char* name,
	MemoryContext parentcontext);

os_IndexUnaryOp* DatumGetIndexUnaryOp(Datum d);

#define OS_DETOAST_INDEXUNARYOP(_datum) (os_FlatIndexUnaryOp*)PG_DETOAST_DATUM(datum)
#define OS_GETARG_INDEXUNARYOP(_arg_num)  DatumGetIndexUnaryOp(PG_GETARG_DATUM(_arg_num))
#define OS_GETARG_INDEXUNARYOP_HANDLE(_arg_num)  DatumGetIndexUnaryOp(PG_GETARG_DATUM(_arg_num))->indexunaryop;
#define OS_GETARG_INDEXUNARYOP_OR_NULL(_nargs, _arg_num) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_INDEXUNARYOP(_arg_num) : NULL;
#define OS_GETARG_INDEXUNARYOP_HANDLE_OR_NULL(_nargs, _arg_num) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_INDEXUNARYOP(_arg_num)->indexunaryop : NULL;
#define OS_RETURN_INDEXUNARYOP(_indexunaryop) return EOHPGetRWDatum(&(_indexunaryop)->hdr)
#define OS_INDEXUNARYOP_FLATSIZE() MAXALIGN(sizeof(os_FlatIndexUnaryOp))

#define OS_INDEXUNARYOP_DATA(_flat) ((char*)(_flat) + OS_INDEXUNARYOP_FLATSIZE())
#define IndexUnaryOpGetEOHP(_datum) (os_IndexUnaryOp *) DatumGetEOHP(_datum)

#endif /* OS_INDEXUNARYOP_H */

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
