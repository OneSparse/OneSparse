static void
context_callback_matrix_free(void* m) {
  pgGrB_Matrix *mat = (pgGrB_Matrix *) m;
  GrB_Matrix_free(&mat->A);
}

Datum
matrix_agg_acc(PG_FUNCTION_ARGS)
{
  pgGrB_Matrix_AggState *mstate;
  MemoryContext aggcxt, oldcxt;
  Datum *row, *col, *val;

  if (!AggCheckCallContext(fcinfo, &aggcxt))
    elog(ERROR, "aggregate function called in non-aggregate context");

  if (PG_ARGISNULL(1) || PG_ARGISNULL(2) || PG_ARGISNULL(3))
    elog(ERROR, "matrices cannot contain null values");

  oldcxt = MemoryContextSwitchTo(aggcxt);

  /* lazy create a new state */
  if (PG_ARGISNULL(0)) {
    mstate = palloc0(sizeof(pgGrB_Matrix_AggState));
  }  else  {
    mstate = (pgGrB_Matrix_AggState *)PG_GETARG_POINTER(0);
  }

  row = palloc(sizeof(int64));
  col = palloc(sizeof(int64));
  val = palloc(sizeof(int64));

  *row = PG_GETARG_INT64(1);
  *col = PG_GETARG_INT64(2);
  *val = PG_GETARG_INT64(3);

  mstate->rows = lappend(mstate->rows, row);
  mstate->cols = lappend(mstate->cols, col);
  mstate->vals = lappend(mstate->vals, val);

  MemoryContextSwitchTo(oldcxt);

  PG_RETURN_POINTER(mstate);
}

Datum
matrix_final_int8(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *retval;
  MemoryContextCallback *ctxcb;

  pgGrB_Matrix_AggState *mstate = (pgGrB_Matrix_AggState*)PG_GETARG_POINTER(0);
  size_t n = 0, count = list_length(mstate->rows);
  GrB_Index *row_indices, *col_indices;
  int64 *matrix_vals;

  ListCell *li, *lj, *lv;

  if (PG_ARGISNULL(0))
    PG_RETURN_NULL();

  mstate = (pgGrB_Matrix_AggState *)PG_GETARG_POINTER(0);

  row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * count);
  col_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * count);
  matrix_vals = (int64*) palloc0(sizeof(int64) * count);

  forthree (li, (mstate)->rows, lj, (mstate)->cols, lv, (mstate)->vals) {
    row_indices[n] = DatumGetInt64(*(Datum*)lfirst(li));
    col_indices[n] = DatumGetInt64(*(Datum*)lfirst(lj));
    matrix_vals[n] = DatumGetInt64(*(Datum*)lfirst(lv));
    n++;
  }

  retval = (pgGrB_Matrix*)palloc(sizeof(pgGrB_Matrix));

  ctxcb = (MemoryContextCallback*) palloc(sizeof(MemoryContextCallback));
  ctxcb->func = context_callback_matrix_free;
  ctxcb->arg = retval;
  MemoryContextRegisterResetCallback(CurrentMemoryContext, ctxcb);

  CHECK(GrB_Matrix_new(&(retval->A),
                       GrB_INT64,
                       count,
                       count));

  CHECK(GrB_Matrix_build(retval->A,
                         row_indices,
                         col_indices,
                         matrix_vals,
                         count,
                         GrB_SECOND_INT64));
  PG_RETURN_POINTER(retval);
}


Datum
matrix_tuples(PG_FUNCTION_ARGS) {
  GrB_Info info;
  FuncCallContext  *funcctx;
  TupleDesc tupdesc;
  Datum result;

  Datum values[3];
  bool nulls[3] = {false, false, false};
  HeapTuple tuple;
  GrB_Index nvals = 0;
  pgGrB_Matrix *mat;
  pgGrB_Matrix_ExtractState *state;

  if (SRF_IS_FIRSTCALL()) {
    MemoryContext oldcontext;

    funcctx = SRF_FIRSTCALL_INIT();
    oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
    mat = (pgGrB_Matrix *) PG_GETARG_POINTER(0);

    state = (pgGrB_Matrix_ExtractState*)palloc(sizeof(pgGrB_Matrix_ExtractState));
    CHECK(GrB_Matrix_nvals(&nvals, mat->A));

    state->rows = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);
    state->cols = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);
    state->vals = (int64*) palloc0(sizeof(int64) * nvals);

    CHECK(GrB_Matrix_extractTuples(state->rows,
                                   state->cols,
                                   state->vals,
                                   &nvals,
                                   mat->A));
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
  state = (pgGrB_Matrix_ExtractState*)funcctx->user_fctx;
  mat = state->mat;

  if (funcctx->call_cntr < funcctx->max_calls) {
    values[0] = Int64GetDatum(state->rows[funcctx->call_cntr]);
    values[1] = Int64GetDatum(state->cols[funcctx->call_cntr]);
    values[2] = Int64GetDatum(state->vals[funcctx->call_cntr]);

    tuple = heap_form_tuple(funcctx->tuple_desc, values, nulls);
    result = HeapTupleGetDatum(tuple);
    SRF_RETURN_NEXT(funcctx, result);
  } else {
    SRF_RETURN_DONE(funcctx);
  }
}

Datum
matrix_in(PG_FUNCTION_ARGS)
{
  GrB_Info info;
  pgGrB_Matrix *retval;

  MemoryContextCallback *ctxcb;

  Datum arr;
  ArrayType *vals;
  FunctionCallInfoData locfcinfo;
  int ndim, *dims;
  int64 count, maxrows, maxcols;

  GrB_Index *row_indices, *col_indices;
  int64 *matrix_vals, *data;

  InitFunctionCallInfoData(locfcinfo,
                           fcinfo->flinfo,
                           3,
                           InvalidOid,
                           NULL,
                           NULL);

  locfcinfo.arg[0] = PG_GETARG_DATUM(0);
  locfcinfo.arg[1] = ObjectIdGetDatum(INT8OID);
  locfcinfo.arg[2] = Int32GetDatum(-1);
  locfcinfo.argnull[0] = false;
  locfcinfo.argnull[1] = false;
  locfcinfo.argnull[2] = false;

  arr = array_in(&locfcinfo);

  Assert(!locfcinfo.isnull);

  vals = DatumGetArrayTypeP(arr);

  if (ARR_HASNULL(vals)) {
    ereport(ERROR, (errmsg("Array may not contain NULLs")));
  }

  ndim = ARR_NDIM(vals);
  if (ndim != 2) {
    ereport(ERROR, (errmsg("Two-dimesional arrays are required")));
  }

  dims = ARR_DIMS(vals);

  if (dims[0] != 3) {
    ereport(ERROR, (errmsg("First dimension must contain 3 arrays")));
  }

  count = dims[1];
  maxrows = count;
  maxcols = count;

  row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * count);
  col_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * count);
  matrix_vals = (int64*) palloc0(sizeof(int64) * count);

  data = (int64*)ARR_DATA_PTR(vals);

  for (int i = 0; i < count; i++) {
    row_indices[i] = data[i];
    if (data[i] > maxrows)
      maxrows = data[i];
    col_indices[i] = data[i+count];
    if (data[i+count] > maxcols)
      maxcols = data[i+count];
    matrix_vals[i] = data[i+count+count];
  }

  retval = (pgGrB_Matrix*) palloc(sizeof(pgGrB_Matrix));

  ctxcb = (MemoryContextCallback*) palloc(sizeof(MemoryContextCallback));
  ctxcb->func = context_callback_matrix_free;
  ctxcb->arg = retval;
  MemoryContextRegisterResetCallback(CurrentMemoryContext, ctxcb);

  CHECK(GrB_Matrix_new(&(retval->A), GrB_INT64, maxrows+1, maxcols+1));

  CHECK(GrB_Matrix_build(retval->A,
                         row_indices,
                         col_indices,
                         matrix_vals,
                         count,
                         GrB_PLUS_INT64));

  PG_RETURN_POINTER(retval);
}

Datum
matrix_out(PG_FUNCTION_ARGS)
{
  GrB_Info info;
  pgGrB_Matrix *mat = (pgGrB_Matrix *) PG_GETARG_POINTER(0);
  char *result;
  GrB_Index nrows, ncols, nvals;
  GrB_Index *row_indices, *col_indices;
  int64 *matrix_vals;

  CHECK(GrB_Matrix_nrows(&nrows, mat->A));
  CHECK(GrB_Matrix_ncols(&ncols, mat->A));
  CHECK(GrB_Matrix_nvals(&nvals, mat->A));

  row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);
  col_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * ncols);
  matrix_vals = (int64*) palloc0(sizeof(int64) * nvals);

  CHECK(GrB_Matrix_extractTuples(row_indices,
                                 col_indices,
                                 matrix_vals,
                                 &nvals,
                                 mat->A));

  result = psprintf("{{");

  for (int i = 0; i < nrows; i++) {
    result = strcat(result, psprintf("%lu", row_indices[i]));
    if (i != nrows - 1)
      result = strcat(result, ",");
  }
  result = strcat(result, "},{");

  for (int i = 0; i < ncols; i++) {
    result = strcat(result, psprintf("%lu", col_indices[i]));
    if (i != ncols - 1)
      result = strcat(result, ",");
  }
  result = strcat(result, "},{");

  for (int i = 0; i < nvals; i++) {
    result = strcat(result, psprintf("%lu", matrix_vals[i]));
    if (i != nvals - 1)
      result = strcat(result, ",");
  }
  result = strcat(result, "}}");

  PG_RETURN_CSTRING(result);
}

Datum
matrix_nrows(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *mat;
  GrB_Index count;
  mat = (pgGrB_Matrix *) PG_GETARG_POINTER(0);
  CHECK(GrB_Matrix_nrows(&count, mat->A));
  return Int64GetDatum(count);
}

Datum
matrix_ncols(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *mat;
  GrB_Index count;
  mat = (pgGrB_Matrix *) PG_GETARG_POINTER(0);
  CHECK(GrB_Matrix_ncols(&count, mat->A));
  return Int64GetDatum(count);
}

Datum
matrix_nvals(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *mat;
  GrB_Index count;
  mat = (pgGrB_Matrix *) PG_GETARG_POINTER(0);
  CHECK(GrB_Matrix_nvals(&count, mat->A));
  return Int64GetDatum(count);
}


#define MATRIX_BINOP_PREAMBLE()                         \
  do {                                                  \
    A = (pgGrB_Matrix *) PG_GETARG_POINTER(0);          \
    B = (pgGrB_Matrix *) PG_GETARG_POINTER(1);          \
    C = (pgGrB_Matrix *) palloc0(sizeof(pgGrB_Matrix)); \
  } while (0)

Datum
matrix_eq(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A, *B, *C;
  GrB_Index arows, brows, acols, bcols, anvals, bnvals;
  bool result = 0;

  MATRIX_BINOP_PREAMBLE();
  CHECK(GrB_Matrix_nrows(&arows, A->A));
  CHECK(GrB_Matrix_nrows(&brows, B->A));
  CHECK(GrB_Matrix_ncols(&acols, A->A));
  CHECK(GrB_Matrix_ncols(&bcols, B->A));
  CHECK(GrB_Matrix_nvals(&anvals, A->A));
  CHECK(GrB_Matrix_nvals(&bnvals, B->A));

  if (arows != brows || acols != bcols || anvals != bnvals)
    PG_RETURN_BOOL(0);

  CHECK(GrB_Matrix_new (&(C->A), GrB_BOOL, arows, bcols));

  CHECK(GrB_eWiseMult(C->A, NULL, NULL, GxB_ISEQ_INT64, A->A, B->A, NULL));
  CHECK(GrB_Matrix_reduce_BOOL(&result, NULL, GxB_LAND_BOOL_MONOID, C->A, NULL));
  PG_RETURN_BOOL(result);
}

Datum
matrix_neq(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A, *B, *C;
  GrB_Index arows, brows, acols, bcols, anvals, bnvals;
  bool result = 1;

  MATRIX_BINOP_PREAMBLE();
  CHECK(GrB_Matrix_nrows(&arows, A->A));
  CHECK(GrB_Matrix_nrows(&brows, B->A));
  CHECK(GrB_Matrix_ncols(&acols, A->A));
  CHECK(GrB_Matrix_ncols(&bcols, B->A));
  CHECK(GrB_Matrix_nvals(&anvals, A->A));
  CHECK(GrB_Matrix_nvals(&bnvals, B->A));

  if (arows != brows || acols != bcols || anvals != bnvals)
    PG_RETURN_BOOL(1);

  CHECK(GrB_Matrix_new (&(C->A), GrB_BOOL, arows, bcols));

  CHECK(GrB_eWiseMult(C->A, NULL, NULL, GxB_ISNE_INT64, A->A, B->A, NULL));
  CHECK(GrB_Matrix_reduce_BOOL(&result, NULL, GxB_LAND_BOOL_MONOID, C->A, NULL));
  PG_RETURN_BOOL(result);
}

Datum
matrix_x_matrix(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A, *B, *C;
  GrB_Index m, n;
  MATRIX_BINOP_PREAMBLE();

  CHECK(GrB_Matrix_nrows(&m, A->A));
  CHECK(GrB_Matrix_ncols(&n, B->A));
  CHECK(GrB_Matrix_new (&(C->A), GrB_INT64, m, n));

  CHECK(GrB_mxm(C->A, NULL, NULL, GxB_PLUS_TIMES_INT64, A->A, B->A, NULL));
  PG_RETURN_POINTER(C);
}

Datum
matrix_x_vector(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A;
  pgGrB_Vector *B, *C;
  GrB_Index size;

  A = (pgGrB_Matrix *) PG_GETARG_POINTER(0);            \
  B = (pgGrB_Vector *) PG_GETARG_POINTER(1);            \
  C = (pgGrB_Vector *) palloc0(sizeof(pgGrB_Vector));   \

  CHECK(GrB_Vector_size(&size, B->V));
  CHECK(GrB_Vector_new (&(C->V), GrB_INT64, size));

  CHECK(GrB_mxv(C->V, NULL, NULL, GxB_PLUS_TIMES_INT64, A->A, B->V, NULL));
  PG_RETURN_POINTER(C);
}

Datum
vector_x_matrix(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *B;
  pgGrB_Vector *A, *C;
  GrB_Index size;

  A = (pgGrB_Vector *) PG_GETARG_POINTER(0);
  B = (pgGrB_Matrix *) PG_GETARG_POINTER(1);
  C = (pgGrB_Vector *) palloc0(sizeof(pgGrB_Vector));

  CHECK(GrB_Vector_size(&size, A->V));
  CHECK(GrB_Vector_new (&(C->V), GrB_INT64, size));

  CHECK(GrB_vxm(C->V, NULL, NULL, GxB_PLUS_TIMES_INT64, A->V, B->A, NULL));
  PG_RETURN_POINTER(C);
}

Datum
matrix_ewise_mult(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A, *B, *C;
  GrB_Index m, n;
  MATRIX_BINOP_PREAMBLE();

  CHECK(GrB_Matrix_nrows(&m, A->A));
  CHECK(GrB_Matrix_ncols(&n, A->A));
  CHECK(GrB_Matrix_new (&(C->A), GrB_INT64, m, n));

  CHECK(GrB_eWiseMult(C->A, NULL, NULL, GrB_TIMES_INT64, A->A, B->A, NULL));
  PG_RETURN_POINTER(C);
}

Datum
matrix_ewise_add(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A, *B, *C;
  GrB_Index m, n;
  MATRIX_BINOP_PREAMBLE();

  CHECK(GrB_Matrix_nrows(&m, A->A));
  CHECK(GrB_Matrix_ncols(&n, A->A));
  CHECK(GrB_Matrix_new (&(C->A), GrB_INT64, m, n));

  CHECK(GrB_eWiseAdd(C->A, NULL, NULL, GrB_PLUS_INT64, A->A, B->A, NULL));
  PG_RETURN_POINTER(C);
}
