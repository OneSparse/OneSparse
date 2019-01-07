#include "pggraphblas.h"
PG_MODULE_MAGIC;

static void
context_callback_matrix_free(void* m) {
  pgGrB_Matrix *mat = (pgGrB_Matrix *) m;
  GrB_Matrix_free(&mat->A);
}

Datum
matrix_agg_acc(PG_FUNCTION_ARGS)
{
  pgGrB_Matrix_AggState *mstate;
  MemoryContext aggcxt;
  MemoryContext oldcxt;
  Datum row, col, val;

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

  row = PG_GETARG_INT64(1);
  col = PG_GETARG_INT64(2);
  val = PG_GETARG_FLOAT4(3);

  mstate->rows = lappend(mstate->rows, &row);
  mstate->cols = lappend(mstate->cols, &col);
  mstate->vals = lappend(mstate->vals, &val);

  MemoryContextSwitchTo(oldcxt);

  PG_RETURN_POINTER(mstate);
}

Datum
matrix_final_int4(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *retval;

  MemoryContextCallback *ctxcb;

  pgGrB_Matrix_AggState *mstate = (pgGrB_Matrix_AggState*)PG_GETARG_POINTER(0);
  size_t count = list_length(mstate->rows);
  GrB_Index *row_indices, *col_indices;
  float4 *matrix_vals;

  ListCell *li, *lj, *lv;

  if (PG_ARGISNULL(0))
    PG_RETURN_NULL();

  mstate = (pgGrB_Matrix_AggState *)PG_GETARG_POINTER(0);

  retval = (pgGrB_Matrix*)palloc(sizeof(pgGrB_Matrix));

  ctxcb = (MemoryContextCallback*) palloc(sizeof(MemoryContextCallback));
  ctxcb->func = context_callback_matrix_free;
  ctxcb->arg = retval;
  MemoryContextRegisterResetCallback(CurrentMemoryContext, ctxcb);

  row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * count);
  col_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * count);
  matrix_vals = (float4*) palloc0(sizeof(float4) * count);

  forthree (li, (mstate)->rows, lj, (mstate)->cols, lv, (mstate)->vals) {
    row_indices[count] = DatumGetInt64(lfirst(li));
    col_indices[count] = DatumGetInt64(lfirst(lj));
    matrix_vals[count] = DatumGetInt64(lfirst(lv));
  }

  elog(NOTICE, "%lu", count);

  CHECK(GrB_Matrix_new(&(retval->A),
                       GrB_FP32,
                       count,
                       count));

  CHECK(GrB_Matrix_build(retval->A,
                         row_indices,
                         col_indices,
                         matrix_vals,
                         count,
                         GrB_SECOND_FP32));
  PG_RETURN_POINTER(retval);
}


Datum
matrix_extract(PG_FUNCTION_ARGS) {
  GrB_Info info;
  FuncCallContext  *funcctx;
  TupleDesc tupdesc;
  Datum result;

  Datum values[3];
  bool nulls[3] = {false, false, false};
  HeapTuple tuple;
  GrB_Index nvals = 0;
  pgGrB_Matrix *mat;

  if (SRF_IS_FIRSTCALL()) {
    MemoryContext oldcontext;

    funcctx = SRF_FIRSTCALL_INIT();
    oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
    mat = (pgGrB_Matrix *) PG_GETARG_POINTER(0);
    funcctx->user_fctx = (void*)mat;
    CHECK(GrB_Matrix_nvals(&nvals, mat->A));
    funcctx->max_calls = nvals;

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
  mat = (pgGrB_Matrix*)funcctx->user_fctx;

  if (funcctx->call_cntr < funcctx->max_calls) {
    values[0] = Int64GetDatum(1);
    values[1] = Int64GetDatum(1);
    values[2] = Float4GetDatum(1.0);

    tuple = heap_form_tuple(tupdesc, values, nulls);
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
  int ndim, *dims, *lb;
  int count;

  GrB_Index *row_indices, *col_indices;
  float4 *matrix_vals;

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
  locfcinfo.arg[1] = ObjectIdGetDatum(FLOAT4OID);
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

  lb = ARR_LBOUND(vals);
  count = dims[0] + lb[0] - 1;

  row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * count);
  col_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * count);
  matrix_vals = (float4*) palloc0(sizeof(float4) * count);

  retval = (pgGrB_Matrix*) palloc(sizeof(pgGrB_Matrix));

  ctxcb = (MemoryContextCallback*) palloc(sizeof(MemoryContextCallback));
  ctxcb->func = context_callback_matrix_free;
  ctxcb->arg = retval;
  MemoryContextRegisterResetCallback(CurrentMemoryContext, ctxcb);

  CHECK(GrB_Matrix_new(&(retval->A), GrB_FP32, count, count));

  CHECK(GrB_Matrix_build(retval->A,
                         row_indices,
                         col_indices,
                         matrix_vals,
                         count,
                         GrB_PLUS_FP32));

  PG_RETURN_POINTER(retval);
}

Datum
matrix_out(PG_FUNCTION_ARGS)
{
  GrB_Info info;
  pgGrB_Matrix *mat = (pgGrB_Matrix *) PG_GETARG_POINTER(0);
  char *result;
  GrB_Index nrows, ncols, nvals;
  GrB_Index n;

  GrB_Index *row_indices, *col_indices;
  float4 *matrix_vals;

  CHECK(GrB_Matrix_nrows(&nrows, mat->A));
  CHECK(GrB_Matrix_ncols(&ncols, mat->A));
  CHECK(GrB_Matrix_nvals(&nvals, mat->A));

  row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * nrows);
  col_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * ncols);
  matrix_vals = (float4*) palloc0(sizeof(float4) * nvals);

  CHECK(GrB_Matrix_extractTuples(row_indices,
                                 col_indices,
                                 matrix_vals,
                                 &n,
                                 mat->A));

  result = psprintf("{%lu, %lu, %lu}::matrix", nrows, ncols, nvals);
  PG_RETURN_CSTRING(result);
}

void
_PG_init(void)
{
  GrB_Info info;
  info = GrB_init (GrB_BLOCKING);
  if (! (info == GrB_SUCCESS || info == GrB_NO_VALUE))
    {
      elog(ERROR, "%s", GrB_error());
      return;
    }
}
