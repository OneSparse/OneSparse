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
  ArrayType *arr;

  if (!AggCheckCallContext(fcinfo, &aggcxt))
    elog(ERROR, "aggregate function called in non-aggregate context");

  if (PG_ARGISNULL(1))
    elog(ERROR, "null-array was supplied");

  oldcxt = MemoryContextSwitchTo(aggcxt);
  arr = (ArrayType *)PG_GETARG_ARRAYTYPE_P_COPY(1);

  /* validation */
  if (ARR_ELEMTYPE(arr) != INT8OID) {
    elog(ERROR, "unsupported element type: %s",
       format_type_be(ARR_ELEMTYPE(arr)));
  }

  /* first one is null, create a new state */
  if (PG_ARGISNULL(0)) {
    mstate = palloc0(sizeof(pgGrB_Matrix_AggState));
    mstate->elemtype = arr->elemtype;
  }  else  {
    mstate = (pgGrB_Matrix_AggState *)PG_GETARG_POINTER(0);
    if (mstate->elemtype != ARR_ELEMTYPE(arr)) {
      elog(ERROR, "array has wrong data type");
    }
  }
  mstate->rows = lappend(mstate->rows, arr);

  MemoryContextSwitchTo(oldcxt);

  PG_RETURN_POINTER(mstate);
}

Datum
matrix_final_int4(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *retval;

  MemoryContextCallback *ctxcb;

  pgGrB_Matrix_AggState *mstate = (pgGrB_Matrix_AggState*)PG_GETARG_POINTER(0);
  size_t dimLength = list_length((mstate)->rows);
  int16 typlen;
  bool typbyval;
  ListCell *lc;
  GrB_Index *row_indices, *col_indices;
  int64_t *matrix_vals;

  if (PG_ARGISNULL(0))
    PG_RETURN_NULL();

  mstate = (pgGrB_Matrix_AggState *)PG_GETARG_POINTER(0);
  Assert(mstate->elemtype == INT4OID);

  get_typlenbyval((mstate)->elemtype,
                  &typlen,
                  &typbyval);

  foreach (lc, (mstate)->rows) {
      ArrayType *V = lfirst(lc);
  }
  row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * dimLength);
  col_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * dimLength);
  matrix_vals = (int64_t*) palloc0(sizeof(int64_t) * dimLength);

  /* memcpy(row_indices, ARR_DATA_PTR(vals), dimLength * sizeof(int64));   */
  /* memcpy(col_indices, ARR_DATA_PTR(vals) + dimLength, dimLength * sizeof(int64));   */
  /* memcpy(matrix_vals, ARR_DATA_PTR(vals) + (2*dimLength), dimLength * sizeof(int64)); */

  retval = (pgGrB_Matrix*)palloc(sizeof(pgGrB_Matrix));

  ctxcb = (MemoryContextCallback*) palloc(sizeof(MemoryContextCallback));
  ctxcb->func = context_callback_matrix_free;
  ctxcb->arg = retval;
  MemoryContextRegisterResetCallback(CurrentMemoryContext, ctxcb);

  CHECK(GrB_Matrix_new(&(retval->A),
                       GrB_INT64,
                       dimLength,
                       dimLength));

  CHECK(GrB_Matrix_build(retval->A,
                         row_indices,
                         col_indices,
                         matrix_vals,
                         dimLength,
                         GrB_PLUS_INT64));
  PG_RETURN_POINTER(retval);
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

  Oid valsType;
  /* int16 valsTypeWidth; */
  /* bool valsTypeByValue; */
  /* char valsTypeAlignmentCode; */
  /* Datum *valsContent; */
  int dimLength, valsLength;
  /* bool *valsNullFlags; */

  GrB_Index *row_indices, *col_indices;
  int64_t *matrix_vals;

  valsType = ObjectIdGetDatum(INT4OID);

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
  locfcinfo.arg[1] = ObjectIdGetDatum(INT4OID);
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
  dimLength = dims[0] + lb[0] - 1;

  /* get_typlenbyvalalign(valsType, */
  /*                      &valsTypeWidth, */
  /*                      &valsTypeByValue, */
  /*                      &valsTypeAlignmentCode); */

  /* // Extract the array contents (as Datum objects). */
  /* deconstruct_array(vals, */
  /*                   valsType, */
  /*                   valsTypeWidth, */
  /*                   valsTypeByValue, */
  /*                   valsTypeAlignmentCode, */
  /*                   &valsContent, */
  /*                   &valsNullFlags, */
  /*                   &valsLength); */

  row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * dimLength);
  col_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * dimLength);
  matrix_vals = (int64_t*) palloc0(sizeof(int64_t) * dimLength);

  /* memcpy(row_indices, ARR_DATA_PTR(vals), dimLength * sizeof(int64));   */
  /* memcpy(col_indices, ARR_DATA_PTR(vals) + dimLength, dimLength * sizeof(int64));   */
  /* memcpy(matrix_vals, ARR_DATA_PTR(vals) + (2*dimLength), dimLength * sizeof(int64)); */

  retval = (pgGrB_Matrix*) palloc(sizeof(pgGrB_Matrix));

  ctxcb = (MemoryContextCallback*) palloc(sizeof(MemoryContextCallback));
  ctxcb->func = context_callback_matrix_free;
  ctxcb->arg = retval;
  MemoryContextRegisterResetCallback(CurrentMemoryContext, ctxcb);

  CHECK(GrB_Matrix_new(&(retval->A), GrB_INT64, dimLength, dimLength));

  CHECK(GrB_Matrix_build(retval->A,
                         row_indices,
                         col_indices,
                         matrix_vals,
                         dimLength,
                         GrB_PLUS_INT64));

  PG_RETURN_POINTER(retval);
}

Datum
matrix_out(PG_FUNCTION_ARGS)
{
  pgGrB_Matrix *mat = (pgGrB_Matrix *) PG_GETARG_POINTER(0);
  char *result;
  GrB_Index nrows, ncols, nvals;

  GrB_Matrix_nrows(&nrows, mat->A);
  GrB_Matrix_ncols(&ncols, mat->A);
  GrB_Matrix_nvals(&nvals, mat->A);

  result = psprintf("{%i, %i, %i}::matrix", nrows, ncols, nvals);
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
