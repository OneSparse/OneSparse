#ifndef OS_SCALAR_H
#define OS_SCALAR_H

#define scalar_MAGIC 689276815

typedef struct os_FlatScalar {
    int32 vl_len_;
	int32_t type_code;
	int16_t nvals;
} os_FlatScalar;

typedef struct os_Scalar  {
    ExpandedObjectHeader hdr;
    int em_magic;
    GrB_Scalar scalar;
    Size flat_size;
} os_Scalar;

Datum expand_scalar(os_FlatScalar *flat,	MemoryContext parentcontext);

os_Scalar* new_scalar(
	GrB_Type type,
	MemoryContext parentcontext,
	GrB_Scalar _scalar);

os_Scalar* DatumGetScalar(Datum d);

#define OS_DETOAST_SCALAR(_datum) (os_FlatScalar*)PG_DETOAST_DATUM(datum)
#define OS_GETARG_SCALAR(_arg_num)  DatumGetScalar(PG_GETARG_DATUM(_arg_num))
#define OS_GETARG_SCALAR_HANDLE_OR_NULL(_nargs, _arg_num) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_SCALAR(_arg_num)->scalar : NULL;
#define OS_RETURN_SCALAR(_scalar) return EOHPGetRWDatum(&(_scalar)->hdr)
#define OS_SCALAR_FLATSIZE() MAXALIGN(sizeof(os_FlatScalar))

#define OS_SCALAR_DATA(_flat) ((char*)(_flat) + OS_SCALAR_FLATSIZE())
#define ScalarGetEOHP(_datum) (os_Scalar *) DatumGetEOHP(_datum)

#endif /* OS_SCALAR_H */

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
