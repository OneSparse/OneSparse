
#define VECTOR_BINOP_PREAMBLE()                           \
  do {                                                    \
    A = PGGRB_GETARG_VECTOR(0);                           \
    B = PGGRB_GETARG_VECTOR(1);                           \
    C = (pgGrB_Vector *) palloc0(sizeof(pgGrB_Vector));   \
  } while (0)


Datum
vector_ewise_mult(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *A, *B, *C;
  GrB_Index size;
  VECTOR_BINOP_PREAMBLE();

  CHECKD(GrB_Vector_size(&size, A->V));
  C = construct_empty_expanded_vector_int64(size, CurrentMemoryContext);
  CHECKD(GrB_eWiseMult(C->V, NULL, NULL, GrB_TIMES_INT64, A->V, B->V, NULL));
  PGGRB_RETURN_VECTOR(C);
}

Datum
vector_ewise_add(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *A, *B, *C;
  GrB_Index size;
  VECTOR_BINOP_PREAMBLE();

  CHECKD(GrB_Vector_size(&size, A->V));
  C = construct_empty_expanded_vector_int64(size, CurrentMemoryContext);
  CHECKD(GrB_eWiseAdd(C->V, NULL, NULL, GrB_PLUS_BOOL, A->V, B->V, NULL));
  PGGRB_RETURN_VECTOR(C);
}

Datum
vector_eq(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *A, *B, *C;
  GrB_Index asize, bsize, anvals, bnvals;
  GrB_Type atype, btype;
  bool result = 0;

  VECTOR_BINOP_PREAMBLE();
  
  CHECKD(GxB_Vector_type(&atype, A->V));
  CHECKD(GxB_Vector_type(&btype, B->V));
  
  if (atype != btype)
    PG_RETURN_BOOL(result);
  
  CHECKD(GrB_Vector_size(&asize, A->V));
  CHECKD(GrB_Vector_size(&bsize, B->V));
  
  if (asize != bsize)
    PG_RETURN_BOOL(result);
  
  CHECKD(GrB_Vector_nvals(&anvals, A->V));
  CHECKD(GrB_Vector_nvals(&bnvals, B->V));

  if (anvals != bnvals)
    PG_RETURN_BOOL(result);

  CHECKD(GrB_Vector_new (&C->V, GrB_BOOL, asize));

  CHECKD(GrB_eWiseMult(C->V, NULL, NULL, GxB_ISEQ_INT64, A->V, B->V, NULL));
  CHECKD(GrB_Vector_reduce_BOOL(&result, NULL, GxB_LAND_BOOL_MONOID, C->V, NULL));
  PG_RETURN_BOOL(result);
}

Datum
vector_neq(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *A, *B, *C;
  GrB_Type atype, btype;
  GrB_Index asize, bsize, anvals, bnvals;
  bool result = 1;

  VECTOR_BINOP_PREAMBLE();
  CHECKD(GxB_Vector_type(&atype, A->V));
  CHECKD(GxB_Vector_type(&btype, B->V));
  
  if (atype != btype)
    PG_RETURN_BOOL(result);
  
  CHECKD(GrB_Vector_size(&asize, A->V));
  CHECKD(GrB_Vector_size(&bsize, B->V));
  
  if (asize != bsize)
    PG_RETURN_BOOL(result);
  
  CHECKD(GrB_Vector_nvals(&anvals, A->V));
  CHECKD(GrB_Vector_nvals(&bnvals, B->V));

  if (anvals != bnvals)
    PG_RETURN_BOOL(result);

  CHECKD(GrB_Vector_new (&(C->V), GrB_BOOL, asize));

  CHECKD(GrB_eWiseMult(C->V, NULL, NULL, GxB_ISNE_INT64, A->V, B->V, NULL));
  CHECKD(GrB_Vector_reduce_BOOL(&result, NULL, GxB_LAND_BOOL_MONOID, C->V, NULL));
  PG_RETURN_BOOL(result);
}
