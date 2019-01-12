
static void
context_callback_vector_free(void* v) {
  pgGrB_Vector *vec = (pgGrB_Vector *) v;
  GrB_Vector_free(&vec->V);
}

Datum
vector_agg_acc(PG_FUNCTION_ARGS)
{
  pgGrB_Vector_AggState *mstate;
  MemoryContext aggcxt, oldcxt;
  Datum *row, *val;

  if (!AggCheckCallContext(fcinfo, &aggcxt))
    elog(ERROR, "aggregate function called in non-aggregate context");

  if (PG_ARGISNULL(1) || PG_ARGISNULL(2))
    elog(ERROR, "vectors cannot contain null values");

  oldcxt = MemoryContextSwitchTo(aggcxt);

  /* lazy create a new state */
  if (PG_ARGISNULL(0)) {
    mstate = palloc0(sizeof(pgGrB_Vector_AggState));
  }  else  {
    mstate = (pgGrB_Vector_AggState *)PG_GETARG_POINTER(0);
  }

  row = palloc(sizeof(int64));
  val = palloc(sizeof(int64));

  *row = PG_GETARG_INT64(1);
  *val = PG_GETARG_INT64(2);

  mstate->I = lappend(mstate->I, row);
  mstate->X = lappend(mstate->X, val);

  MemoryContextSwitchTo(oldcxt);

  PG_RETURN_POINTER(mstate);
}

Datum
vector_final_int4(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *retval;

  MemoryContext oldcxt;
  MemoryContextCallback *ctxcb;

  pgGrB_Vector_AggState *mstate = (pgGrB_Vector_AggState*)PG_GETARG_POINTER(0);
  size_t n = 0, count = list_length(mstate->I);
  GrB_Index *row_indices;
  int64 *vector_vals;

  ListCell *li, *lv;

  if (PG_ARGISNULL(0))
    PG_RETURN_NULL();

  mstate = (pgGrB_Vector_AggState *)PG_GETARG_POINTER(0);

  row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * count);
  vector_vals = (int64*) palloc0(sizeof(int64) * count);

  forboth (li, (mstate)->I, lv, (mstate)->X) {
    /* elog(NOTICE, "%lu", DatumGetInt64(*(Datum*)lfirst(li))); */
    /* elog(NOTICE, "%f", DatumGetFloat4(*(Datum*)lfirst(lv))); */
    row_indices[n] = DatumGetInt64(*(Datum*)lfirst(li));
    vector_vals[n] = DatumGetInt64(*(Datum*)lfirst(lv));
    n++;
  }

  //oldcxt = MemoryContextSwitchTo(CurTransactionContext);
  
  retval = (pgGrB_Vector*)palloc(sizeof(pgGrB_Vector));

  ctxcb = (MemoryContextCallback*) palloc(sizeof(MemoryContextCallback));
  ctxcb->func = context_callback_vector_free;
  ctxcb->arg = retval;
  MemoryContextRegisterResetCallback(CurrentMemoryContext, ctxcb);

  //MemoryContextSwitchTo(oldcxt);

  CHECK(GrB_Vector_new(&(retval->V),
                       GrB_INT64,
                       count));

  CHECK(GrB_Vector_build(retval->V,
                         row_indices,
                         vector_vals,
                         count,
                         GrB_SECOND_INT64));
  PG_RETURN_POINTER(retval);
}

Datum
vector_tuples(PG_FUNCTION_ARGS) {
  GrB_Info info;
  FuncCallContext  *funcctx;
  TupleDesc tupdesc;
  Datum result;

  Datum values[3];
  bool nulls[2] = {false, false};
  HeapTuple tuple;
  GrB_Index size = 0;
  pgGrB_Vector *vec;
  pgGrB_Vector_ExtractState *state;

  if (SRF_IS_FIRSTCALL()) {
    MemoryContext oldcontext;

    funcctx = SRF_FIRSTCALL_INIT();
    oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
    vec = (pgGrB_Vector *) PG_GETARG_POINTER(0);

    state = (pgGrB_Vector_ExtractState*)palloc(sizeof(pgGrB_Vector_ExtractState));
    CHECK(GrB_Vector_size(&size, vec->V));

    state->rows = (GrB_Index*) palloc0(sizeof(GrB_Index) * size);
    state->vals = (int64*) palloc0(sizeof(int64) * size);

    CHECK(GrB_Vector_extractTuples(state->rows,
                                   state->vals,
                                   &size,
                                   vec->V));
    state->vec = vec;
    funcctx->max_calls = size;
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
  state = (pgGrB_Vector_ExtractState*)funcctx->user_fctx;
  vec = state->vec;

  if (funcctx->call_cntr < funcctx->max_calls) {
    values[0] = Int64GetDatum(state->rows[funcctx->call_cntr]);
    values[1] = Int64GetDatum(state->vals[funcctx->call_cntr]);

    tuple = heap_form_tuple(funcctx->tuple_desc, values, nulls);
    result = HeapTupleGetDatum(tuple);
    SRF_RETURN_NEXT(funcctx, result);
  } else {
    SRF_RETURN_DONE(funcctx);
  }
}

Datum
vector_in(PG_FUNCTION_ARGS)
{

  /* This is all wrong and provisional until I can get my head around
     the ::array API. */
  
  GrB_Info info;
  pgGrB_Vector *retval;

  MemoryContext oldcxt;
  
  MemoryContextCallback *ctxcb;

  Datum arr;
  ArrayType *vals;
  FunctionCallInfoData locfcinfo;
  int ndim, *dims, *lb;
  int count;

  GrB_Index *row_indices;
  int64 *vector_vals, *data;

  /* A comment from the pg source...
   *
   * Normally one would call array_recv() using DirectFunctionCall3, but
   * that does not work since array_recv wants to cache some data using
   * fcinfo->flinfo->fn_extra.  So we need to pass it our own flinfo
   * parameter.
   */

  /* arr = DirectFunctionCall3(array_in, */
  /*                           PG_GETARG_DATUM(0), */
  /*                           ObjectIdGetDatum(INT2OID), */
  /*                           Int32GetDatum(-1)); */

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
  if (ndim != 1) {
    ereport(ERROR, (errmsg("One-dimesional array is required")));
  }

  dims = ARR_DIMS(vals);

  lb = ARR_LBOUND(vals);
  count = dims[0] + lb[0] - 1;

  row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * count);
  vector_vals = (int64*) palloc0(sizeof(int64) * count);

  data = (int64*)ARR_DATA_PTR(vals);

  for (int64 i = 0; i < count; i++) {
    row_indices[i] = i;
    vector_vals[i] = data[i];
  }

  //  oldcxt = MemoryContextSwitchTo(CurTransactionContext);
  retval = (pgGrB_Vector*) palloc(sizeof(pgGrB_Vector));

  ctxcb = (MemoryContextCallback*) palloc(sizeof(MemoryContextCallback));
  ctxcb->func = context_callback_vector_free;
  ctxcb->arg = retval;
  MemoryContextRegisterResetCallback(CurTransactionContext, ctxcb);
  //  MemoryContextSwitchTo(oldcxt);

  CHECK(GrB_Vector_new(&(retval->V), GrB_INT64, count + 1));

  CHECK(GrB_Vector_build(retval->V,
                         row_indices,
                         vector_vals,
                         count,
                         GrB_PLUS_INT64));

  PG_RETURN_POINTER(retval);
}

Datum
vector_out(PG_FUNCTION_ARGS)
{

  /* This is all wrong and provisional until I can get my head around
     the ::array API. */
  
  GrB_Info info;
  pgGrB_Vector *vec = (pgGrB_Vector *) PG_GETARG_POINTER(0);
  char *result;
  GrB_Index size, nvals;

  GrB_Index *row_indices;
  int64 *vector_vals;
  ArrayType *outarray;

  CHECK(GrB_Vector_size(&size, vec->V));
  CHECK(GrB_Vector_nvals(&nvals, vec->V));

  row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * size);
  vector_vals = (int64*) palloc0(sizeof(int64) * size);

  CHECK(GrB_Vector_extractTuples(row_indices,
                                 vector_vals,
                                 &size,
                                 vec->V));

  result = psprintf("{");
  for (int i = 0; i < size; i++) {
    result = strcat(result, psprintf("%lu", vector_vals[i]));
    if (i != size - 1)
      result = strcat(result, ",");
  }
  result = strcat(result, "}");
  PG_RETURN_CSTRING(result);
}

#define VECTOR_BINOP_PREAMBLE()                         \
  do {                                                  \
    A = (pgGrB_Vector *) PG_GETARG_POINTER(0);          \
    B = (pgGrB_Vector *) PG_GETARG_POINTER(1);          \
    C = (pgGrB_Vector *) palloc0(sizeof(pgGrB_Vector)); \
  } while (0)


Datum
vector_ewise_mult(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *A, *B, *C;
  GrB_Index m;
  VECTOR_BINOP_PREAMBLE();

  CHECK(GrB_Vector_size(&m, A->V));
  CHECK(GrB_Vector_new (&(C->V), GrB_INT64, m));

  CHECK(GrB_eWiseMult(C->V, NULL, NULL, GrB_TIMES_INT64, A->V, B->V, NULL));
  PG_RETURN_POINTER(C);
}

Datum
vector_ewise_add(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *A, *B, *C;
  GrB_Index m;
  VECTOR_BINOP_PREAMBLE();
  
  CHECK(GrB_Vector_size(&m, A->V));
  CHECK(GrB_Vector_new (&(C->V), GrB_INT64, m));

  CHECK(GrB_eWiseAdd(C->V, NULL, NULL, GrB_PLUS_INT64, A->V, B->V, NULL));
  PG_RETURN_POINTER(C);
}
