GrB_Index FN(extract_rowscols)(OS_Matrix *A,
                               GrB_Index **rows,
                               GrB_Index **cols,
                               GrB_Index nvals);

Datum FN(matrix_ewise_mult)(OS_Matrix *A,
                            OS_Matrix *B,
                            OS_Matrix *C,
                            OS_Matrix *mask,
                            GrB_BinaryOp binop,
                            GrB_BinaryOp accum,
                            GrB_Descriptor desc);

Datum FN(matrix_ewise_add)(OS_Matrix *A,
                           OS_Matrix *B,
                           OS_Matrix *C,
                           OS_Matrix *mask,
                           GrB_BinaryOp binop,
                           GrB_BinaryOp accum,
                           GrB_Descriptor desc);

Datum
FN(mxm)(OS_Matrix *A,
        OS_Matrix *B,
        OS_Matrix *C,
        OS_Matrix *mask,
        GrB_Semiring semiring,
        GrB_BinaryOp binop,
        GrB_Descriptor desc);

Datum
FN(matrix_kron)(OS_Matrix *A,
                OS_Matrix *B,
                OS_Matrix *C,
                OS_Matrix *mask,
                GrB_BinaryOp accum,
                GrB_BinaryOp mulop,
                GrB_Descriptor desc);

Datum
FN(mxv)(OS_Matrix *A,
        OS_Vector *B,
        OS_Vector *C,
        OS_Vector *mask,
        GrB_Semiring semiring,
        GrB_BinaryOp binop,
        GrB_Descriptor desc);

Datum
FN(vxm)(OS_Vector *A,
        OS_Matrix *B,
        OS_Vector *C,
        OS_Vector *mask,
        GrB_Semiring semiring,
        GrB_BinaryOp binop,
        GrB_Descriptor desc);


PG_FUNCTION_INFO_V1(FN(matrix_elements));
PG_FUNCTION_INFO_V1(FN(matrix_reduce));
PG_FUNCTION_INFO_V1(FN(matrix_assign));
PG_FUNCTION_INFO_V1(FN(matrix_random));
PG_FUNCTION_INFO_V1(FN(matrix_set_element));

Datum
FN(matrix_elements)(PG_FUNCTION_ARGS) {
  GrB_Info info;
  FuncCallContext  *funcctx;
  TupleDesc tupdesc;
  Datum result;

  Datum values[3];
  bool nulls[3] = {false, false, false};
  HeapTuple tuple;
  GrB_Index nvals = 0;
  OS_Matrix *mat;
  FN(OS_Matrix_ExtractState) *state;

  if (SRF_IS_FIRSTCALL()) {
    MemoryContext oldcontext;

    funcctx = SRF_FIRSTCALL_INIT();
    oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
    mat = OS_GETARG_MATRIX(0);

    state = (FN(OS_Matrix_ExtractState)*)palloc(sizeof(FN(OS_Matrix_ExtractState)));
    CHECKD(GrB_Matrix_nvals(&nvals, mat->M), mat->M);

    state->rows = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);
    state->cols = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);
    state->vals = (PG_TYPE*) palloc0(sizeof(PG_TYPE) * nvals);

    CHECKD(GrB_Matrix_extractTuples(state->rows,
                                   state->cols,
                                   state->vals,
                                   &nvals,
                                   mat->M), mat->M);
    state->mat = mat;
    funcctx->max_calls = nvals;
    funcctx->user_fctx = (void*)state;

    /* One-time setup code appears here: */
    if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
      ereport(ERROR,
              (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
               errmsg("function returning record called in context "
                      "that cannot accept type record")));
    BlessTupleDesc(tupdesc);
    funcctx->tuple_desc = tupdesc;

    MemoryContextSwitchTo(oldcontext);
  }

  funcctx = SRF_PERCALL_SETUP();
  state = (FN(OS_Matrix_ExtractState)*)funcctx->user_fctx;
  mat = state->mat;

  if (funcctx->call_cntr < funcctx->max_calls) {
    values[0] = Int64GetDatum(state->rows[funcctx->call_cntr]);
    values[1] = Int64GetDatum(state->cols[funcctx->call_cntr]);
    values[2] = PG_TGD(state->vals[funcctx->call_cntr]);

    tuple = heap_form_tuple(funcctx->tuple_desc, values, nulls);
    result = HeapTupleGetDatum(tuple);
    SRF_RETURN_NEXT(funcctx, result);
  } else {
    SRF_RETURN_DONE(funcctx);
  }
}

Datum
FN(matrix_ewise_mult)(OS_Matrix *A,
                      OS_Matrix *B,
                      OS_Matrix *C,
                      OS_Matrix *mask,
                      GrB_BinaryOp binop,
                      GrB_BinaryOp accum,
                      GrB_Descriptor desc
                      ) {
  GrB_Info info;
  GrB_Index m, n;

  if (C == NULL) {
    CHECKD(GrB_Matrix_nrows(&m, A->M), A->M);
    CHECKD(GrB_Matrix_ncols(&n, A->M), A->M);
    C = FN(construct_empty_expanded_matrix)(m, n, CurrentMemoryContext);
  }
  CHECKD(GrB_eWiseMult(C->M, mask ? mask->M : NULL, accum, binop, A->M, B->M, desc), A->M);
  OS_RETURN_MATRIX(C);
}

Datum
FN(matrix_ewise_add)(OS_Matrix *A,
                     OS_Matrix *B,
                     OS_Matrix *C,
                     OS_Matrix *mask,
                     GrB_BinaryOp binop,
                     GrB_BinaryOp accum,
                     GrB_Descriptor desc
                     ) {
  GrB_Info info;
  GrB_Index m, n;

  if (C == NULL) {
    CHECKD(GrB_Matrix_nrows(&m, A->M), A->M);
    CHECKD(GrB_Matrix_ncols(&n, A->M), A->M);
    C = FN(construct_empty_expanded_matrix)(m, n, CurrentMemoryContext);
  }

  CHECKD(GrB_eWiseAdd(C->M, mask ? mask->M : NULL, accum, binop, A->M, B->M, desc), A->M);
  OS_RETURN_MATRIX(C);
}

Datum
FN(mxm)(OS_Matrix *A,
        OS_Matrix *B,
        OS_Matrix *C,
        OS_Matrix *mask,
        GrB_Semiring semiring,
        GrB_BinaryOp binop,
        GrB_Descriptor desc) {
  GrB_Info info;

  if (C == NULL) {
    GrB_Index m, n;
    CHECKD(GrB_Matrix_nrows(&m, A->M), A->M);
    CHECKD(GrB_Matrix_ncols(&n, B->M), A->M);
    C = FN(construct_empty_expanded_matrix)(m, n, CurrentMemoryContext);
  }
  CHECKD(GrB_mxm(C->M, mask ? mask->M : NULL, binop, semiring, A->M, B->M, desc), A->M);
  OS_RETURN_MATRIX(C);
}

Datum
FN(mxv)(OS_Matrix *A,
        OS_Vector *B,
        OS_Vector *C,
        OS_Vector *mask,
        GrB_Semiring semiring,
        GrB_BinaryOp binop,
        GrB_Descriptor desc) {
  GrB_Info info;
  GrB_Index size;

  if (C == NULL) {
    CHECKD(GrB_Matrix_ncols(&size, A->M), A->M);
    C = FN(construct_empty_expanded_vector)(size, CurrentMemoryContext);
  }
  CHECKD(GrB_mxv(C->V, mask ? mask->V : NULL, binop, semiring, A->M, B->V, desc), A->M);
  OS_RETURN_VECTOR(C);
}

Datum
FN(vxm)(OS_Vector *A,
        OS_Matrix *B,
        OS_Vector *C,
        OS_Vector *mask,
        GrB_Semiring semiring,
        GrB_BinaryOp binop,
        GrB_Descriptor desc) {
  GrB_Info info;
  GrB_Index size;

  if (C == NULL) {
    CHECKD(GrB_Vector_size(&size, A->V), A->V);
    C = FN(construct_empty_expanded_vector)(size, CurrentMemoryContext);
  }
  CHECKD(GrB_vxm(C->V, mask ? mask->V : NULL, binop, semiring, A->V, B->M, desc), A->V);
  OS_RETURN_VECTOR(C);
}

Datum
FN(matrix_reduce)(PG_FUNCTION_ARGS) {
  GrB_Info info;
  GrB_Type type;
  OS_Matrix *A;
  GrB_Semiring semiring;
  GrB_Monoid monoid;
  char *semiring_name;
  PG_TYPE val;

  A = OS_GETARG_MATRIX(0);
  semiring_name = PG_ARGISNULL(1)?
    NULL:
    text_to_cstring(PG_GETARG_TEXT_PP(1));

  if (semiring_name == NULL) {
    CHECKD(GxB_Matrix_type(&type, A->M), A->M);
    semiring_name = DEFAULT_SEMIRING(type);
  }
  semiring = lookup_semiring(semiring_name);
  CHECKD(GxB_Semiring_add(&monoid, semiring), A->M);
  CHECKD(GrB_reduce(&val, NULL, monoid, A->M, NULL), A->M);
  PG_RET(val);
}

GrB_Index FN(extract_rowscols)(OS_Matrix *A,
                         GrB_Index **rows,
                         GrB_Index **cols,
                         GrB_Index nvals) {
  PG_TYPE *values;
  GrB_Info info;

  *rows = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);
  *cols = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);
  values = (PG_TYPE*) palloc0(sizeof(PG_TYPE) * nvals);

  CHECKD(GrB_Matrix_extractTuples(*rows,
                                  *cols,
                                  values,
                                  &nvals,
                                  A->M), A->M);
  return nvals;
}

Datum
FN(matrix_assign)(PG_FUNCTION_ARGS) {
  GrB_Info info;
  OS_Matrix *A, *B=NULL, *mask=NULL;
  GrB_Index nrows, ncols, nvals;
  GrB_Index *rows = NULL, *cols = NULL;
  PG_TYPE val;

  A = OS_GETARG_MATRIX(0);
  val = PG_GET(1);
  B = PG_ARGISNULL(2) ? NULL : OS_GETARG_MATRIX(2);
  mask = PG_ARGISNULL(3)? NULL : OS_GETARG_MATRIX(3);

  if (B != NULL) {
    CHECKD(GrB_Matrix_nvals(&nvals, B->M), A->M);
    nrows = ncols = nvals = FN(extract_rowscols)(B, &rows, &cols, nvals);
  } else {
    CHECKD(GrB_Matrix_nrows(&nrows, A->M), A->M);
    CHECKD(GrB_Matrix_ncols(&ncols, A->M), A->M);
  }

  CHECKD(GrB_assign(A->M,
                    mask? mask->M: NULL,
                    NULL,
                    val,
                    rows?rows:GrB_ALL,
                    nrows,
                    cols?cols:GrB_ALL,
                    ncols,
                    NULL), A->M);

  OS_RETURN_MATRIX(A);
}

Datum
FN(matrix_kron)(OS_Matrix *A,
        OS_Matrix *B,
        OS_Matrix *C,
        OS_Matrix *mask,
        GrB_BinaryOp accum,
        GrB_BinaryOp mulop,
        GrB_Descriptor desc) {
  GrB_Info info;

  if (C == NULL) {
    GrB_Index m, n, p, q;
    CHECKD(GrB_Matrix_nrows(&m, A->M), A->M);
    CHECKD(GrB_Matrix_ncols(&n, A->M), A->M);
    CHECKD(GrB_Matrix_nrows(&p, B->M), B->M);
    CHECKD(GrB_Matrix_ncols(&q, B->M), B->M);
    C = FN(construct_empty_expanded_matrix)(m*p, n*q, CurrentMemoryContext);
  }
  CHECKD(GxB_kron(C->M, mask ? mask->M : NULL, accum, mulop, A->M, B->M, desc), A->M);
  OS_RETURN_MATRIX(C);
}

Datum
FN(matrix_set_element)(PG_FUNCTION_ARGS) {
  GrB_Info info;
  OS_Matrix *A;
  GrB_Index row, col;
  PG_TYPE val;

  A = OS_GETARG_MATRIX(0);
  row = PG_GETARG_INT64(1);
  col = PG_GETARG_INT64(2);
  val = PG_GET(3);

  CHECKD(GrB_Matrix_setElement(A->M, val, row, col), A->M);
  OS_RETURN_MATRIX(A);
}
