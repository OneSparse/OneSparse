#ifndef OS_BINARYOP_H
#define OS_BINARYOP_H

#define binaryop_MAGIC 689276816

typedef struct os_FlatBinaryOp {
    int32 vl_len_;
	char name[GxB_MAX_NAME_LEN];
} os_FlatBinaryOp;

typedef struct os_BinaryOp  {
    ExpandedObjectHeader hdr;
    int em_magic;
    Size flat_size;
    GrB_BinaryOp binaryop;
	char* name;
} os_BinaryOp;

Datum expand_binaryop(os_FlatBinaryOp *flat,	MemoryContext parentcontext);

os_BinaryOp* new_binaryop(
	char* name,
	MemoryContext parentcontext,
	GrB_BinaryOp binop);

os_BinaryOp* DatumGetBinaryOp(Datum d);

#define OS_DETOAST_BINARYOP(_datum) (os_FlatBinaryOp*)PG_DETOAST_DATUM(datum)
#define OS_GETARG_BINARYOP(_arg_num)  DatumGetBinaryOp(PG_GETARG_DATUM(_arg_num))
#define OS_GETARG_BINARYOP_HANDLE_OR_NULL(_nargs, _arg_num) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_BINARYOP(_arg_num)->binaryop : NULL;
#define OS_RETURN_BINARYOP(_binaryop) return EOHPGetRWDatum(&(_binaryop)->hdr)
#define OS_BINARYOP_FLATSIZE() MAXALIGN(sizeof(os_FlatBinaryOp))

#define OS_BINARYOP_DATA(_flat) ((char*)(_flat) + OS_BINARYOP_FLATSIZE())
#define BinaryOpGetEOHP(_datum) (os_BinaryOp *) DatumGetEOHP(_datum)

#endif
