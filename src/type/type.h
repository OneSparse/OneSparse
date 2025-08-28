#ifndef OS_TYPE_H
#define OS_TYPE_H

#define type_MAGIC 689276821

typedef struct os_FlatType {
    int32 vl_len_;
	char name[GxB_MAX_NAME_LEN];
} os_FlatType;

typedef struct os_Type  {
    ExpandedObjectHeader hdr;
    int em_magic;
    Size flat_size;
    GrB_Type type;
	char* name;
} os_Type;

Datum expand_type(os_FlatType *flat, MemoryContext parentcontext);

os_Type* new_type(
	char* name,
	MemoryContext parentcontext);

os_Type* DatumGetType(Datum d);

#define OS_DETOAST_TYPE(_datum) (os_FlatType*)PG_DETOAST_DATUM(datum)
#define OS_GETARG_TYPE(_arg_num)  DatumGetType(PG_GETARG_DATUM(_arg_num))
#define OS_GETARG_TYPE_HANDLE_OR_NULL(_nargs, _arg_num) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_TYPE(_arg_num)->type : NULL;
#define OS_RETURN_TYPE(_type) return EOHPGetRWDatum(&(_type)->hdr)
#define OS_TYPE_FLATSIZE() MAXALIGN(sizeof(os_FlatType))

#define OS_TYPE_DATA(_flat) ((char*)(_flat) + OS_TYPE_FLATSIZE())
#define TypeGetEOHP(_datum) (os_Type *) DatumGetEOHP(_datum)

#endif 