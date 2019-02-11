
#define T int64
#define SUFFIX _int64
#include "matrix.h"

/* MemoryContextCallback function to free matrices */
static void
context_callback_matrix_free(void* m) {
  pgGrB_Matrix *mat = (pgGrB_Matrix *) m;
  GrB_Matrix_free(&mat->M);
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
  d = expand_flat_matrix_int64(PointerGetDatum(flat), parentcontext);
  pfree(flat);
  return (pgGrB_Matrix *) DatumGetEOHP(d);
}

/* Helper function to always expanded datum

This is used by PGGRB_GETARG_MATRIX */
pgGrB_Matrix *
DatumGetMatrix(Datum d) {
  pgGrB_Matrix *A;
  
  if (VARATT_IS_EXTERNAL_EXPANDED(DatumGetPointer(d))) {
    A = (pgGrB_Matrix *) DatumGetEOHP(d);
    Assert(A->em_magic == matrix_MAGIC);
    return A;
  }
  d = expand_flat_matrix_int64(d, CurrentMemoryContext);
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

  CHECKD(GrB_Matrix_build(retval->M,
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
    CHECKD(GrB_Matrix_nvals(&nvals, mat->M));

    state->rows = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);
    state->cols = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);
    state->vals = (int64*) palloc0(sizeof(int64) * nvals);

    CHECKD(GrB_Matrix_extractTuples(state->rows,
                                   state->cols,
                                   state->vals,
                                   &nvals,
                                   mat->M));
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
matrix_in(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *retval;

  Datum arr;
  ArrayType *vals;
  FunctionCallInfoData locfcinfo;
  int ndim, *dims;

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
    ereport(ERROR, (errmsg("One-dimesional array required")));
  }

  dims = ARR_DIMS(vals);

  if (dims[0] != 3) {
    ereport(ERROR, (errmsg("First dimension must contain 3 elements")));
  }

  retval = construct_empty_expanded_matrix(0,
                                           0,
                                           GrB_INT64,
                                           CurrentMemoryContext);
  PGGRB_RETURN_MATRIX(retval);
}

Datum
matrix(PG_FUNCTION_ARGS) {
  pgGrB_Matrix *retval;

  GrB_Index nrows, ncols;

  nrows = PG_GETARG_INT64(0);
  ncols = PG_GETARG_INT64(1);

  retval = construct_empty_expanded_matrix(nrows,
                                           ncols,
                                           GrB_INT64,
                                           CurrentMemoryContext);

  PGGRB_RETURN_MATRIX(retval);
}
