#ifndef OS_MATRIX_H
#define OS_MATRIX_H

#define matrix_MAGIC 689276813

typedef struct os_FlatMatrix {
    int32 vl_len_;
	int32_t type_code;
	GrB_Index nrows;
	GrB_Index ncols;
	GrB_Index serialized_size;
	Oid loid;
} os_FlatMatrix;

typedef struct os_Matrix  {
    ExpandedObjectHeader hdr;
    int em_magic;
    GrB_Matrix matrix;
    Size flat_size;
    void* serialized_data;
    GrB_Index serialized_size;
	Oid loid;
} os_Matrix;

typedef struct os_Matrix_ExtractState {
	GrB_Type type;
	GrB_Info info;
	os_Matrix *matrix;
	GxB_Iterator iterator;
} os_Matrix_ExtractState;

typedef struct os_Matrix_BuildState {
	MemoryContext context;
	os_Matrix *matrix;
} os_Matrix_BuildState;

Datum expand_matrix(os_FlatMatrix *flat,	MemoryContext parentcontext);

os_Matrix* new_matrix(
	GrB_Type type,
	GrB_Index nrows,
	GrB_Index ncols,
	MemoryContext parentcontext,
	GrB_Matrix _matrix);

os_Matrix* DatumGetMatrix(Datum d);

#define OS_DETOAST_MATRIX(_datum) (os_FlatMatrix*)PG_DETOAST_DATUM(datum)
#define OS_GETARG_MATRIX(_arg_num)  DatumGetMatrix(PG_GETARG_DATUM(_arg_num))
#define OS_GETARG_MATRIX_HANDLE(_arg_num)  DatumGetMatrix(PG_GETARG_DATUM(_arg_num))->matrix;
#define OS_GETARG_MATRIX_OR_NULL(_nargs, _arg_num) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_MATRIX(_arg_num) : NULL;
#define OS_GETARG_MATRIX_HANDLE_OR_NULL(_nargs, _arg_num) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_MATRIX(_arg_num)->matrix : NULL;
#define OS_RETURN_MATRIX(_matrix) return EOHPGetRWDatum(&(_matrix)->hdr)
#define OS_MATRIX_FLATSIZE() MAXALIGN(sizeof(os_FlatMatrix))

#define OS_MATRIX_DATA(_flat) ((char*)(_flat) + OS_MATRIX_FLATSIZE())
#define MatrixGetEOHP(_datum) (os_Matrix *) DatumGetEOHP(_datum)

#endif /* OS_MATRIX_H */

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
