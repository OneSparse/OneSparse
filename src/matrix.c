
/* MemoryContextCallback function to free matrices */
static void
context_callback_matrix_free(void* m) {
  pgGrB_Matrix *mat = (pgGrB_Matrix *) m;
  GrB_Matrix_free(&mat->A);
}

/* Expanded Object Header "methods" for flattening matrices for storage */
static Size EM_get_flat_size(ExpandedObjectHeader *eohptr);
static void EM_flatten_into(ExpandedObjectHeader *eohptr,
                            void *result, Size allocated_size);

static const ExpandedObjectMethods EM_methods = {
  EM_get_flat_size,
  EM_flatten_into
};

/* Compute size of storage needed for matrix */
static Size
EM_get_flat_size(ExpandedObjectHeader *eohptr) {
  GrB_Info info;
  pgGrB_Matrix *A = (pgGrB_Matrix *) eohptr;
  Size nbytes;
  GrB_Index nrows, ncols, nvals;

  Assert(A->em_magic == EM_MAGIC);

  if (A->flat_value)
   return VARSIZE(A->flat_value);

  if (A->flat_size)
    return A->flat_size;

  CHECKD(GrB_Matrix_nrows(&nrows, A->A));
  CHECKD(GrB_Matrix_ncols(&ncols, A->A));
  CHECKD(GrB_Matrix_nvals(&nvals, A->A));

  nbytes = PGGRB_MATRIX_OVERHEAD();
  nbytes += nrows * sizeof(GrB_Index);
  nbytes += ncols * sizeof(GrB_Index);
  nbytes += nvals * sizeof(int64);

  A->flat_size = nbytes;
  return nbytes;
}

/* Flatten matrix into allocated result buffer  */
static void
EM_flatten_into(ExpandedObjectHeader *eohptr,
                void *result, Size allocated_size)  {
  GrB_Info info;
  GrB_Index *start;
  pgGrB_Matrix *A = (pgGrB_Matrix *) eohptr;
  pgGrB_FlatMatrix *flat = (pgGrB_FlatMatrix *) result;

  if (A->flat_value) {
    Assert(allocated_size == VARSIZE(A->flat_value));
    memcpy(flat, A->flat_value, allocated_size);
    return;
  }

  Assert(A->em_magic == EM_MAGIC);
  Assert(allocated_size == A->flat_size);

  memset(flat, 0, allocated_size);
  
  CHECKV(GrB_Matrix_nrows(&flat->nrows, A->A));
  CHECKV(GrB_Matrix_ncols(&flat->ncols, A->A));
  CHECKV(GrB_Matrix_nvals(&flat->nvals, A->A));
  
  start = PGGRB_MATRIX_DATA(flat);
  CHECKV(GrB_Matrix_extractTuples(start,
                                  start + flat->nrows,
                                  start + flat->nrows + flat->ncols,
                                  &flat->nvals,
                                  A->A));

  flat->type = GrB_INT64;

  SET_VARSIZE(flat, allocated_size);
}

/* Expand a flat matrix. */
Datum
expand_flat_matrix(Datum flatdatum,
                   MemoryContext parentcontext) {
  GrB_Info info;
  
  pgGrB_Matrix *A;
  pgGrB_FlatMatrix *flat;
  
  MemoryContext objcxt, oldcxt;
  MemoryContextCallback *ctxcb;
  
  GrB_Index ncols, nrows, nvals;
  GrB_Index *rows, *cols;
  int64 *vals;
  GrB_Type type;

  /* Create a new context that will hold the expanded object. */
  objcxt = AllocSetContextCreate(
          parentcontext,
          "expanded matrix",
          ALLOCSET_DEFAULT_SIZES);

  /* Allocate a new expanded matrix */
  A = (pgGrB_Matrix*)MemoryContextAlloc(
          objcxt,
          sizeof(pgGrB_Matrix));

  /* Initialize the ExpandedObjectHeader member with flattening
   * methods and new object context */
  EOH_init_header(&A->hdr, &EM_methods, objcxt);

  /* Used for debugging checks */
  A->em_magic = EM_MAGIC;

  /* Switch to new object context */
  oldcxt = MemoryContextSwitchTo(objcxt);

  /* Copy the flat datum into our context */
  flat = (pgGrB_FlatMatrix*)flatdatum;

  /* Get dimensional information from flat */
  nrows = flat->nrows;
  ncols = flat->ncols;
  nvals = flat->nvals;
  type = flat->type;

  /* Rows, cols, and vals are pointers into the vardata area */
  
  rows = PGGRB_MATRIX_DATA(flat);
  cols = rows + nrows;
  vals = (int64*)(cols + ncols);

  /* Initialize the new matrix */
  CHECKD(GrB_Matrix_new(&A->A,
                        type,
                        nrows,
                        ncols));

  /* Create a context callback to free matrix when context is cleared */
  ctxcb = (MemoryContextCallback*)MemoryContextAlloc(
          objcxt,
          sizeof(MemoryContextCallback));
  
  ctxcb->func = context_callback_matrix_free;
  ctxcb->arg = A;
  MemoryContextRegisterResetCallback(objcxt, ctxcb);

  /* If there's actual values, build the matrix */
  if (nvals > 0) {
    CHECKD(GrB_Matrix_build(A->A,
                            rows,
                            cols,
                            vals,
                            nvals,
                            GrB_SECOND_INT64));
  }

  A->flat_size = 0;
  A->flat_value = NULL;

  /* Switch back to old context */
  MemoryContextSwitchTo(oldcxt);
  PGGRB_RETURN_MATRIX(A);
}

/* Construct an empty flat matrix. */
pgGrB_FlatMatrix *
construct_empty_flat_matrix(GrB_Index nrows,
                       GrB_Index ncols,
                       GrB_Type type) {
  pgGrB_FlatMatrix *result;

  result = (pgGrB_FlatMatrix *) palloc0(sizeof(pgGrB_FlatMatrix));
  SET_VARSIZE(result, sizeof(pgGrB_FlatMatrix));
  result->nrows = ncols;
  result->ncols = nrows;
  result->nvals = 0;
  result->type = type;
  return result;
}

/* Construct an empty expanded matrix. */
pgGrB_Matrix *
construct_empty_expanded_matrix(GrB_Index nrows,
                                GrB_Index ncols,
                                GrB_Type type,
                                MemoryContext parentcontext) {
  pgGrB_FlatMatrix  *flat;
  Datum	d;
  flat = construct_empty_flat_matrix(nrows, ncols, type);
  d = expand_flat_matrix(PointerGetDatum(flat), parentcontext);
  pfree(flat);
  return (pgGrB_Matrix *) DatumGetEOHP(d);
}

/* Helper function to always expanded datum

This is used by PGGRB_GETARG_MATRIX */
pgGrB_Matrix *
DatumGetMatrix(Datum d) {
  pgGrB_Matrix *A;
  
  if (VARATT_IS_EXTERNAL_EXPANDED_RW(DatumGetPointer(d))) {
    A = (pgGrB_Matrix *) DatumGetEOHP(d);
    Assert(A->em_magic == EM_MAGIC);
    return A;
  }
  d = expand_flat_matrix(d, CurrentMemoryContext);
  return (pgGrB_Matrix *) DatumGetEOHP(d);
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
  MemoryContext oldcxt, resultcxt;

  pgGrB_Matrix_AggState *mstate = (pgGrB_Matrix_AggState*)PG_GETARG_POINTER(0);
  size_t n = 0, count = list_length(mstate->rows);
  GrB_Index *row_indices, *col_indices;
  int64 *matrix_vals;

  ListCell *li, *lj, *lv;

  if (PG_ARGISNULL(0))
    PG_RETURN_NULL();

  if (!AggCheckCallContext(fcinfo, &resultcxt)) {
    resultcxt = CurrentMemoryContext;
  }
  
  oldcxt = MemoryContextSwitchTo(resultcxt);
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

  retval = construct_empty_expanded_matrix(count,
                                           count,
                                           GrB_INT64,
                                           resultcxt);

  CHECKD(GrB_Matrix_build(retval->A,
                         row_indices,
                         col_indices,
                         matrix_vals,
                         count,
                         GrB_SECOND_INT64));

  MemoryContextSwitchTo(oldcxt);
  PGGRB_RETURN_MATRIX(retval);
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
    mat = (pgGrB_Matrix *) PGGRB_GETARG_MATRIX(0);

    state = (pgGrB_Matrix_ExtractState*)palloc(sizeof(pgGrB_Matrix_ExtractState));
    CHECKD(GrB_Matrix_nvals(&nvals, mat->A));

    state->rows = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);
    state->cols = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);
    state->vals = (int64*) palloc0(sizeof(int64) * nvals);

    CHECKD(GrB_Matrix_extractTuples(state->rows,
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

  retval = construct_empty_expanded_matrix(count,
                                           count,
                                           GrB_INT64,
                                           CurrentMemoryContext);

  CHECKD(GrB_Matrix_build(retval->A,
                         row_indices,
                         col_indices,
                         matrix_vals,
                         count,
                         GrB_PLUS_INT64));

  PGGRB_RETURN_MATRIX(retval);
}

Datum
matrix_out(PG_FUNCTION_ARGS)
{
  GrB_Info info;
  pgGrB_Matrix *mat = PGGRB_GETARG_MATRIX(0);
  char *result;
  GrB_Index nrows, ncols, nvals;
  GrB_Index *row_indices, *col_indices;
  int64 *matrix_vals;

  CHECKD(GrB_Matrix_nrows(&nrows, mat->A));
  CHECKD(GrB_Matrix_ncols(&ncols, mat->A));
  CHECKD(GrB_Matrix_nvals(&nvals, mat->A));

  row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);
  col_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * ncols);
  matrix_vals = (int64*) palloc0(sizeof(int64) * nvals);

  CHECKD(GrB_Matrix_extractTuples(row_indices,
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
  mat = (pgGrB_Matrix *) PGGRB_GETARG_MATRIX(0);
  CHECKD(GrB_Matrix_nrows(&count, mat->A));
  return Int64GetDatum(count);
}

Datum
matrix_ncols(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *mat;
  GrB_Index count;
  mat = (pgGrB_Matrix *) PGGRB_GETARG_MATRIX(0);
  CHECKD(GrB_Matrix_ncols(&count, mat->A));
  return Int64GetDatum(count);
}

Datum
matrix_nvals(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *mat;
  GrB_Index count;
  mat = (pgGrB_Matrix *) PGGRB_GETARG_MATRIX(0);
  CHECKD(GrB_Matrix_nvals(&count, mat->A));
  return Int64GetDatum(count);
}

Datum
matrix_eq(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A, *B;
  bool result;

  A = PGGRB_GETARG_MATRIX(0);
  B = PGGRB_GETARG_MATRIX(1);

  CHECKD(isequal(&result, A->A, B->A, NULL));
  PG_RETURN_BOOL(result);
}

Datum
matrix_neq(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A, *B;
  bool result;

  A = PGGRB_GETARG_MATRIX(0);
  B = PGGRB_GETARG_MATRIX(1);

  CHECKD(isequal(&result, A->A, B->A, NULL));
  PG_RETURN_BOOL(!result);
}

Datum
matrix_x_matrix(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A, *B, *C;
  GrB_Index m, n;
  A = PGGRB_GETARG_MATRIX(0);
  B = PGGRB_GETARG_MATRIX(1);

  CHECKD(GrB_Matrix_nrows(&m, A->A));
  CHECKD(GrB_Matrix_ncols(&n, B->A));

  C = construct_empty_expanded_matrix(m, n, GrB_INT64, CurrentMemoryContext);
  CHECKD(GrB_mxm(C->A, NULL, NULL, GxB_PLUS_TIMES_INT64, A->A, B->A, NULL));
  PGGRB_RETURN_MATRIX(C);
}

Datum
matrix_x_vector(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A;
  pgGrB_Vector *B, *C;
  GrB_Index size;

  A = (pgGrB_Matrix *) PGGRB_GETARG_MATRIX(0);
  B = (pgGrB_Vector *) PGGRB_GETARG_MATRIX(1);
  C = (pgGrB_Vector *) palloc0(sizeof(pgGrB_Vector));

  CHECKD(GrB_Vector_size(&size, B->V));
  CHECKD(GrB_Vector_new (&(C->V), GrB_INT64, size));

  CHECKD(GrB_mxv(C->V, NULL, NULL, GxB_PLUS_TIMES_INT64, A->A, B->V, NULL));
  PG_RETURN_POINTER(C);
}

Datum
vector_x_matrix(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *B;
  pgGrB_Vector *A, *C;
  GrB_Index size;

  A = (pgGrB_Vector *) PGGRB_GETARG_MATRIX(0);
  B = (pgGrB_Matrix *) PGGRB_GETARG_MATRIX(1);
  C = (pgGrB_Vector *) palloc0(sizeof(pgGrB_Vector));

  CHECKD(GrB_Vector_size(&size, A->V));
  CHECKD(GrB_Vector_new (&(C->V), GrB_INT64, size));

  CHECKD(GrB_vxm(C->V, NULL, NULL, GxB_PLUS_TIMES_INT64, A->V, B->A, NULL));
  PG_RETURN_POINTER(C);
}

Datum
matrix_ewise_mult(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A, *B, *C;
  GrB_Index m, n;
  A = PGGRB_GETARG_MATRIX(0);
  B = PGGRB_GETARG_MATRIX(1);

  CHECKD(GrB_Matrix_nrows(&m, A->A));
  CHECKD(GrB_Matrix_ncols(&n, A->A));

  C = construct_empty_expanded_matrix(m, n, GrB_INT64, CurrentMemoryContext);
  CHECKD(GrB_eWiseMult(C->A, NULL, NULL, GrB_TIMES_INT64, A->A, B->A, NULL));
  PGGRB_RETURN_MATRIX(C);
}

Datum
matrix_ewise_add(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A, *B, *C;
  GrB_Index m, n;
  A = PGGRB_GETARG_MATRIX(0);
  B = PGGRB_GETARG_MATRIX(1);

  CHECKD(GrB_Matrix_nrows(&m, A->A));
  CHECKD(GrB_Matrix_ncols(&n, A->A));

  C = construct_empty_expanded_matrix(m, n, GrB_INT64, CurrentMemoryContext);
  CHECKD(GrB_eWiseAdd(C->A, NULL, NULL, GrB_PLUS_INT64, A->A, B->A, NULL));
  PGGRB_RETURN_MATRIX(C);
}
