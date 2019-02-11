
Datum
vector_out(PG_FUNCTION_ARGS)
{

  /* This is all wrong and provisional until I can get my head around
     the ::array API. */

  GrB_Info info;
  pgGrB_Vector *vec = PGGRB_GETARG_VECTOR(0);
  char *result;
  GrB_Index size, nvals;

  GrB_Index *row_indices;
  int64 *vector_vals;

  CHECKD(GrB_Vector_size(&size, vec->V));
  CHECKD(GrB_Vector_nvals(&nvals, vec->V));

  row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * size);
  vector_vals = (int64*) palloc0(sizeof(int64) * size);

  CHECKD(GrB_Vector_extractTuples(row_indices,
                                 vector_vals,
                                 &size,
                                 vec->V));

  result = psprintf("{{");

  for (int i = 0; i < size; i++) {
    result = strcat(result, psprintf("%lu", row_indices[i]));
    if (i != size - 1)
      result = strcat(result, ",");
  }
  result = strcat(result, "},{");

  for (int i = 0; i < nvals; i++) {
    result = strcat(result, psprintf("%lu", vector_vals[i]));
    if (i != nvals - 1)
      result = strcat(result, ",");
  }
  result = strcat(result, "}}");

  PG_RETURN_CSTRING(result);
}

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
  C = construct_empty_expanded_vector_int64(size, GrB_INT64, CurrentMemoryContext);
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
  C = construct_empty_expanded_vector_int64(size, GrB_INT64, CurrentMemoryContext);
  CHECKD(GrB_eWiseAdd(C->V, NULL, NULL, GrB_PLUS_INT64, A->V, B->V, NULL));
  PGGRB_RETURN_VECTOR(C);
}

Datum
vector_eq(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *A, *B, *C;
  GrB_Index asize, bsize, anvals, bnvals;
  bool result = 0;

  VECTOR_BINOP_PREAMBLE();
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
  GrB_Index asize, bsize, anvals, bnvals;
  bool result = 0;

  VECTOR_BINOP_PREAMBLE();
  CHECKD(GrB_Vector_size(&asize, A->V));
  CHECKD(GrB_Vector_size(&bsize, B->V));
  CHECKD(GrB_Vector_nvals(&anvals, A->V));
  CHECKD(GrB_Vector_nvals(&bnvals, B->V));

  if (asize != bsize || anvals != bnvals)
    PG_RETURN_BOOL(result);

  CHECKD(GrB_Vector_new (&(C->V), GrB_BOOL, asize));

  CHECKD(GrB_eWiseMult(C->V, NULL, NULL, GxB_ISNE_INT64, A->V, B->V, NULL));
  CHECKD(GrB_Vector_reduce_BOOL(&result, NULL, GxB_LAND_BOOL_MONOID, C->V, NULL));
  PG_RETURN_BOOL(result);
}

Datum
vector_nvals(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *vec;
  GrB_Index nvals;
  vec = PGGRB_GETARG_VECTOR(0);
  CHECKD(GrB_Vector_nvals(&nvals, vec->V));
  return Int64GetDatum(nvals);
}

Datum
vector_size(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *vec;
  GrB_Index size;
  vec = PGGRB_GETARG_VECTOR(0);
  CHECKD(GrB_Vector_size(&size, vec->V));
  return Int64GetDatum(size);
}
