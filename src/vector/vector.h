#ifndef OS_VECTOR_H
#define OS_VECTOR_H

#define vector_MAGIC 689276814

typedef struct os_FlatVector {
    int32 vl_len_;
	int32_t type_code;
	GrB_Index size;
	GrB_Index serialized_size;
} os_FlatVector;

typedef struct os_Vector  {
    ExpandedObjectHeader hdr;
    int em_magic;
    GrB_Vector vector;
	Pointer flat_datum_pointer;
    Size flat_size;
    void* serialized_data;
    GrB_Index serialized_size;
} os_Vector;

typedef struct os_Vector_ExtractState {
	GrB_Type type;
	GrB_Info info;
	os_Vector *vector;
	GxB_Iterator iterator;
} os_Vector_ExtractState;

Datum expand_vector(os_FlatVector *flat,	MemoryContext parentcontext);

os_Vector* new_vector(
	GrB_Type type,
	GrB_Index size,
	MemoryContext parentcontext,
	GrB_Vector _vector);

os_Vector* DatumGetVector(Datum d);

os_Vector* DatumGetVectorMaybeA(Datum d, os_Vector *A);
os_Vector* DatumGetVectorMaybeAB(Datum d, os_Vector *A, os_Vector *B);
os_Vector* DatumGetVectorMaybeABC(Datum d, os_Vector *A, os_Vector *B, os_Vector *C);

#define OS_DETOAST_VECTOR(_datum) (os_FlatVector*)PG_DETOAST_DATUM(datum)

#define OS_GETARG_VECTOR(_arg_num)  DatumGetVector(PG_GETARG_DATUM(_arg_num))
#define OS_GETARG_VECTOR_A(_arg_num, _A)  DatumGetVectorMaybeA(PG_GETARG_DATUM(_arg_num), _A)
#define OS_GETARG_VECTOR_AB(_arg_num, _A, _B)  DatumGetVectorMaybeAB(PG_GETARG_DATUM(_arg_num), _A, _B)
#define OS_GETARG_VECTOR_ABC(_arg_num, _A, _B, _C)  DatumGetVectorMaybeABC(PG_GETARG_DATUM(_arg_num), _A, _B, _C)

#define OS_GETARG_VECTOR_HANDLE_OR_NULL(_nargs, _arg_num) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_VECTOR(_arg_num)->vector : NULL;
#define OS_GETARG_VECTOR_HANDLE_OR_NULL_A(_nargs, _arg_num, _A) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_VECTOR_A(_arg_num, _A)->vector : NULL;
#define OS_GETARG_VECTOR_HANDLE_OR_NULL_AB(_nargs, _arg_num, _A, _B) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_VECTOR_AB(_arg_num, _A, _B)->vector : NULL;
#define OS_GETARG_VECTOR_HANDLE_OR_NULL_ABC(_nargs, _arg_num, _A, _B, _C) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_VECTOR_ABC(_arg_num, _A, _B, _C)->vector : NULL;

#define OS_RETURN_VECTOR(_vector) return EOHPGetRWDatum(&(_vector)->hdr)
#define OS_VECTOR_FLATSIZE() MAXALIGN(sizeof(os_FlatVector))

#define OS_VECTOR_DATA(_flat) ((char*)(_flat) + OS_VECTOR_FLATSIZE())
#define VectorGetEOHP(_datum) (os_Vector *) DatumGetEOHP(_datum)

#endif /* OS_VECTOR_H */

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
