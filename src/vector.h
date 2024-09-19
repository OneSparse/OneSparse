#ifndef ONESPARSE_VECTOR_H
#define ONESPARSE_VECTOR_H

#define vector_MAGIC 689276813

typedef struct onesparse_FlatVector {
    int32 vl_len_;
	int32_t type_code;
	GrB_Index size;
	GrB_Index serialized_size;
} onesparse_FlatVector;

typedef struct onesparse_Vector  {
    ExpandedObjectHeader hdr;
    int em_magic;
    GrB_Vector vector;
    Size flat_size;
    void* serialized_data;
    GrB_Index serialized_size;
} onesparse_Vector;

Datum expand_vector(onesparse_FlatVector *flat,	MemoryContext parentcontext);

onesparse_Vector* new_vector(
	GrB_Type type,
	GrB_Index size,
	MemoryContext parentcontext,
	GrB_Vector _vector);

onesparse_Vector* DatumGetVector(Datum d);

#define ONESPARSE_DETOAST_VECTOR(_datum) (onesparse_FlatVector*)PG_DETOAST_DATUM(datum)
#define ONESPARSE_GETARG_VECTOR(_arg_num)  DatumGetVector(PG_GETARG_DATUM(_arg_num))
#define ONESPARSE_RETURN_VECTOR(_vector) return EOHPGetRWDatum(&(_vector)->hdr)
#define ONESPARSE_VECTOR_FLATSIZE() MAXALIGN(sizeof(onesparse_FlatVector))

#define ONESPARSE_VECTOR_DATA(_flat) ((char*)(_flat) + ONESPARSE_VECTOR_FLATSIZE())
#define VectorGetEOHP(_datum) (onesparse_Vector *) DatumGetEOHP(_datum)

#endif /* ONESPARSE_VECTOR_H */

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
