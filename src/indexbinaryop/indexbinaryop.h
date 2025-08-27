#ifndef OS_INDEXBINARYOP_H
#define OS_INDEXBINARYOP_H

#define indexbinaryop_MAGIC 689276816

typedef struct os_FlatIndexBinaryOp {
    int32 vl_len_;
	char name[GxB_MAX_NAME_LEN];
} os_FlatIndexBinaryOp;

typedef struct os_IndexBinaryOp  {
    ExpandedObjectHeader hdr;
    int em_magic;
    Size flat_size;
    GxB_IndexBinaryOp indexbinaryop;
	char* name;
} os_IndexBinaryOp;

Datum expand_indexbinaryop(os_FlatIndexBinaryOp *flat,	MemoryContext parentcontext);

os_IndexBinaryOp* new_indexbinaryop(
	char* name,
	MemoryContext parentcontext,
	GxB_IndexBinaryOp binop);

os_IndexBinaryOp* DatumGetIndexBinaryOp(Datum d);

#define OS_DETOAST_INDEXBINARYOP(_datum) (os_FlatIndexBinaryOp*)PG_DETOAST_DATUM(datum)
#define OS_GETARG_INDEXBINARYOP(_arg_num)  DatumGetIndexBinaryOp(PG_GETARG_DATUM(_arg_num))
#define OS_GETARG_INDEXBINARYOP_HANDLE_OR_NULL(_nargs, _arg_num) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_INDEXBINARYOP(_arg_num)->indexbinaryop : NULL;
#define OS_RETURN_INDEXBINARYOP(_indexbinaryop) return EOHPGetRWDatum(&(_indexbinaryop)->hdr)
#define OS_INDEXBINARYOP_FLATSIZE() MAXALIGN(sizeof(os_FlatIndexBinaryOp))

#define OS_INDEXBINARYOP_DATA(_flat) ((char*)(_flat) + OS_INDEXBINARYOP_FLATSIZE())
#define IndexBinaryOpGetEOHP(_datum) (os_IndexBinaryOp *) DatumGetEOHP(_datum)

#endif
