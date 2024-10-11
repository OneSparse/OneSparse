#ifndef OS_UNARYOP_H
#define OS_UNARYOP_H

#define unaryop_MAGIC 689276822

typedef struct os_FlatUnaryOp {
    int32 vl_len_;
	char name[GxB_MAX_NAME_LEN];
} os_FlatUnaryOp;

typedef struct os_UnaryOp  {
    ExpandedObjectHeader hdr;
    int em_magic;
    Size flat_size;
    GrB_UnaryOp unaryop;
	char* name;
} os_UnaryOp;

Datum expand_unaryop(os_FlatUnaryOp *flat,	MemoryContext parentcontext);

os_UnaryOp* new_unaryop(
	char* name,
	MemoryContext parentcontext);

os_UnaryOp* DatumGetUnaryOp(Datum d);

#define OS_DETOAST_UNARYOP(_datum) (os_FlatUnaryOp*)PG_DETOAST_DATUM(datum)
#define OS_GETARG_UNARYOP(_arg_num)  DatumGetUnaryOp(PG_GETARG_DATUM(_arg_num))
#define OS_GETARG_UNARYOP_HANDLE(_arg_num)  DatumGetUnaryOp(PG_GETARG_DATUM(_arg_num))->unaryop;
#define OS_GETARG_UNARYOP_OR_NULL(_nargs, _arg_num) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_UNARYOP(_arg_num) : NULL;
#define OS_GETARG_UNARYOP_HANDLE_OR_NULL(_nargs, _arg_num) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_UNARYOP(_arg_num)->unaryop : NULL;
#define OS_RETURN_UNARYOP(_unaryop) return EOHPGetRWDatum(&(_unaryop)->hdr)
#define OS_UNARYOP_FLATSIZE() MAXALIGN(sizeof(os_FlatUnaryOp))

#define OS_UNARYOP_DATA(_flat) ((char*)(_flat) + OS_UNARYOP_FLATSIZE())
#define UnaryOpGetEOHP(_datum) (os_UnaryOp *) DatumGetEOHP(_datum)

#endif /* OS_UNARYOP_H */

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
