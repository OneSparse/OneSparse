#ifndef ONESPARSE_MATRIX_H
#define ONESPARSE_MATRIX_H

#define matrix_MAGIC 689276813

typedef struct onesparse_FlatMatrix {
    int32 vl_len_;
	int32_t type_code;
	GrB_Index nrows;
	GrB_Index ncols;
	GrB_Index serialized_size;
} onesparse_FlatMatrix;

typedef struct onesparse_Matrix  {
    ExpandedObjectHeader hdr;
    int em_magic;
    GrB_Matrix matrix;
    Size flat_size;
    void* serialized_data;
    GrB_Index serialized_size;
} onesparse_Matrix;

Datum expand_matrix(onesparse_FlatMatrix *flat,	MemoryContext parentcontext);

onesparse_Matrix* new_matrix(
	GrB_Type type,
	GrB_Index nrows,
	GrB_Index ncols,
	MemoryContext parentcontext,
	GrB_Matrix _matrix);

onesparse_Matrix* DatumGetMatrix(Datum d);

#define ONESPARSE_DETOAST_MATRIX(_datum) (onesparse_FlatMatrix*)PG_DETOAST_DATUM(datum)
#define ONESPARSE_GETARG_MATRIX(_arg_num)  DatumGetMatrix(PG_GETARG_DATUM(_arg_num))
#define ONESPARSE_RETURN_MATRIX(_matrix) return EOHPGetRWDatum(&(_matrix)->hdr)
#define ONESPARSE_MATRIX_FLATSIZE() MAXALIGN(sizeof(onesparse_FlatMatrix))

#define ONESPARSE_MATRIX_DATA(_flat) ((char*)(_flat) + ONESPARSE_MATRIX_FLATSIZE())
#define MatrixGetEOHP(_datum) (onesparse_Matrix *) DatumGetEOHP(_datum)

#endif /* ONESPARSE_MATRIX_H */

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
