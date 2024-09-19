#ifndef ONESPARSE_SCALAR_H
#define ONESPARSE_SCALAR_H

#define scalar_MAGIC 689276813

typedef struct onesparse_FlatScalar {
    int32 vl_len_;
	int32_t type_code;
	int16_t nvals;
} onesparse_FlatScalar;

typedef struct onesparse_Scalar  {
    ExpandedObjectHeader hdr;
    int em_magic;
    GrB_Scalar scalar;
    Size flat_size;
} onesparse_Scalar;

Datum expand_scalar(onesparse_FlatScalar *flat,	MemoryContext parentcontext);

onesparse_Scalar* new_scalar(
	GrB_Type type,
	MemoryContext parentcontext,
	GrB_Scalar _scalar);

onesparse_Scalar* DatumGetScalar(Datum d);

#define ONESPARSE_DETOAST_SCALAR(_datum) (onesparse_FlatScalar*)PG_DETOAST_DATUM(datum)
#define ONESPARSE_GETARG_SCALAR(_arg_num)  DatumGetScalar(PG_GETARG_DATUM(_arg_num))
#define ONESPARSE_RETURN_SCALAR(_scalar) return EOHPGetRWDatum(&(_scalar)->hdr)
#define ONESPARSE_SCALAR_FLATSIZE() MAXALIGN(sizeof(onesparse_FlatScalar))

#define ONESPARSE_SCALAR_DATA(_flat) ((char*)(_flat) + ONESPARSE_SCALAR_FLATSIZE())
#define ScalarGetEOHP(_datum) (onesparse_Scalar *) DatumGetEOHP(_datum)

#endif /* ONESPARSE_SCALAR_H */

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
