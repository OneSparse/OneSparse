
Datum
matrix_out(PG_FUNCTION_ARGS)
{
  GrB_Info info;
  pgGrB_Matrix *mat = PGGRB_GETARG_MATRIX(0);
  char *result;
  GrB_Index nrows, ncols, nvals;
  GrB_Type type;

  CHECKD(GrB_Matrix_nrows(&nrows, mat->M));
  CHECKD(GrB_Matrix_ncols(&ncols, mat->M));
  CHECKD(GrB_Matrix_nvals(&nvals, mat->M));
  CHECKD(GxB_Matrix_type(&type, mat->M));

  result = psprintf("matrix(%lu, %lu, %lu)", nrows, ncols, nvals);

  PG_RETURN_CSTRING(result);
}

Datum
matrix_nrows(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *mat;
  GrB_Index count;
  mat = (pgGrB_Matrix *) PGGRB_GETARG_MATRIX(0);
  CHECKD(GrB_Matrix_nrows(&count, mat->M));
  return Int64GetDatum(count);
}

Datum
matrix_ncols(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *mat;
  GrB_Index count;
  mat = (pgGrB_Matrix *) PGGRB_GETARG_MATRIX(0);
  CHECKD(GrB_Matrix_ncols(&count, mat->M));
  return Int64GetDatum(count);
}

Datum
matrix_nvals(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *mat;
  GrB_Index count;
  mat = (pgGrB_Matrix *) PGGRB_GETARG_MATRIX(0);
  CHECKD(GrB_Matrix_nvals(&count, mat->M));
  return Int64GetDatum(count);
}

Datum
matrix_eq(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A, *B;
  bool result;

  A = PGGRB_GETARG_MATRIX(0);
  B = PGGRB_GETARG_MATRIX(1);

  CHECKD(isequal(&result, A->M, B->M, NULL));
  PG_RETURN_BOOL(result);
}

Datum
matrix_neq(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A, *B;
  bool result;

  A = PGGRB_GETARG_MATRIX(0);
  B = PGGRB_GETARG_MATRIX(1);

  CHECKD(isequal(&result, A->M, B->M, NULL));
  PG_RETURN_BOOL(!result);
}

GrB_Semiring mxm_semiring(pgGrB_Matrix *left, pgGrB_Matrix *right) {
  return GxB_PLUS_TIMES_INT64;
}

GrB_Semiring mxv_semiring(pgGrB_Matrix *left, pgGrB_Vector *right) {
  return GxB_PLUS_TIMES_INT64;
}

GrB_Semiring vxm_semiring(pgGrB_Vector *left, pgGrB_Matrix *right) {
  return GxB_PLUS_TIMES_INT64;
}

Datum
matrix_mxm(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A, *B, *C;
  GrB_Index m, n;
  GrB_Semiring semiring;
  
  A = PGGRB_GETARG_MATRIX(0);
  B = PGGRB_GETARG_MATRIX(1);

  CHECKD(GrB_Matrix_nrows(&m, A->M));
  CHECKD(GrB_Matrix_ncols(&n, B->M));

  C = construct_empty_expanded_matrix(m, n, GrB_INT64, CurrentMemoryContext);
  semiring = mxm_semiring(A, B);
  
  CHECKD(GrB_mxm(C->M, NULL, NULL, semiring, A->M, B->M, NULL));
  PGGRB_RETURN_MATRIX(C);
}

Datum
matrix_mxv(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A;
  pgGrB_Vector *B, *C;
  GrB_Index size;
  GrB_Semiring semiring;

  A = (pgGrB_Matrix *) PGGRB_GETARG_MATRIX(0);
  B = (pgGrB_Vector *) PGGRB_GETARG_MATRIX(1);

  CHECKD(GrB_Vector_size(&size, B->V));
  
  C = construct_empty_expanded_vector_int64(size, GrB_INT64, CurrentMemoryContext);
  semiring = mxv_semiring(A, B);

  CHECKD(GrB_mxv(C->V, NULL, NULL, semiring, A->M, B->V, NULL));
  PGGRB_RETURN_VECTOR(C);
}

Datum
matrix_vxm(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *B;
  pgGrB_Vector *A, *C;
  GrB_Index size;
  GrB_Semiring semiring;

  A = (pgGrB_Vector *) PGGRB_GETARG_MATRIX(0);
  B = (pgGrB_Matrix *) PGGRB_GETARG_MATRIX(1);

  CHECKD(GrB_Vector_size(&size, A->V));
  C = construct_empty_expanded_vector_int64(size, GrB_INT64, CurrentMemoryContext);
  semiring = vxm_semiring(A, B);

  CHECKD(GrB_vxm(C->V, NULL, NULL, semiring, A->V, B->M, NULL));
  PGGRB_RETURN_VECTOR(C);
}

Datum
matrix_ewise_mult(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A, *B, *C;
  GrB_Index m, n;
  A = PGGRB_GETARG_MATRIX(0);
  B = PGGRB_GETARG_MATRIX(1);

  CHECKD(GrB_Matrix_nrows(&m, A->M));
  CHECKD(GrB_Matrix_ncols(&n, A->M));

  C = construct_empty_expanded_matrix(m, n, GrB_INT64, CurrentMemoryContext);
  CHECKD(GrB_eWiseMult(C->M, NULL, NULL, GrB_TIMES_INT64, A->M, B->M, NULL));
  PGGRB_RETURN_MATRIX(C);
}

Datum
matrix_ewise_add(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A, *B, *C;
  GrB_Index m, n;
  A = PGGRB_GETARG_MATRIX(0);
  B = PGGRB_GETARG_MATRIX(1);

  CHECKD(GrB_Matrix_nrows(&m, A->M));
  CHECKD(GrB_Matrix_ncols(&n, A->M));

  C = construct_empty_expanded_matrix(m, n, GrB_INT64, CurrentMemoryContext);
  CHECKD(GrB_eWiseAdd(C->M, NULL, NULL, GrB_PLUS_INT64, A->M, B->M, NULL));
  PGGRB_RETURN_MATRIX(C);
}
