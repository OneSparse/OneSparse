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
	Pointer flat_datum_pointer;
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

os_Matrix* DatumGetMatrixMaybeA(Datum d, os_Matrix *A);
os_Matrix* DatumGetMatrixMaybeAB(Datum d, os_Matrix *A, os_Matrix *B);
os_Matrix* DatumGetMatrixMaybeABC(Datum d, os_Matrix *A, os_Matrix *B, os_Matrix *C);

#define OS_DETOAST_MATRIX(_datum) (os_FlatMatrix*)PG_DETOAST_DATUM(datum)

#define OS_GETARG_MATRIX(_arg_num)  DatumGetMatrix(PG_GETARG_DATUM(_arg_num))
#define OS_GETARG_MATRIX_A(_arg_num, _A)  DatumGetMatrixMaybeA(PG_GETARG_DATUM(_arg_num), _A)
#define OS_GETARG_MATRIX_AB(_arg_num, _A, _B)  DatumGetMatrixMaybeAB(PG_GETARG_DATUM(_arg_num), _A, _B)
#define OS_GETARG_MATRIX_ABC(_arg_num, _A, _B, _C)  DatumGetMatrixMaybeABC(PG_GETARG_DATUM(_arg_num), _A, _B, _C)

#define OS_GETARG_MATRIX_HANDLE_OR_NULL(_nargs, _arg_num) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_MATRIX(_arg_num)->matrix : NULL;
#define OS_GETARG_MATRIX_HANDLE_OR_NULL_A(_nargs, _arg_num, _A) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_MATRIX_A(_arg_num, _A)->matrix : NULL;
#define OS_GETARG_MATRIX_HANDLE_OR_NULL_AB(_nargs, _arg_num, _A, _B) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_MATRIX_AB(_arg_num, _A, _B)->matrix : NULL;
#define OS_GETARG_MATRIX_HANDLE_OR_NULL_ABC(_nargs, _arg_num, _A, _B, _C) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_MATRIX_ABC(_arg_num, _A, _B, _C)->matrix : NULL;

#define OS_RETURN_MATRIX(_matrix) return EOHPGetRWDatum(&(_matrix)->hdr)
#define OS_MATRIX_FLATSIZE() MAXALIGN(sizeof(os_FlatMatrix))

#define OS_MATRIX_DATA(_flat) ((char*)(_flat) + OS_MATRIX_FLATSIZE())
#define MatrixGetEOHP(_datum) (os_Matrix *) DatumGetEOHP(_datum)

#endif /* OS_MATRIX_H */

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
