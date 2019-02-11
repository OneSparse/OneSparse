
#define T int64
#define GT GrB_INT64
#define GTT GrB_SECOND_INT64
#define PGT PG_GETARG_INT64
#define DGT DatumGetInt64
#define SUFFIX _int64
#include "vector.h"

/* MemoryContextCallback function to free matrices */
static void
context_callback_vector_free(void* a) {
  pgGrB_Vector *A = (pgGrB_Vector *) a;
  GrB_Vector_free(&A->V);
}


/* Construct an empty flat vector. */
pgGrB_FlatVector *
construct_empty_flat_vector(GrB_Index size,
                            GrB_Type type) {
  pgGrB_FlatVector *result;

  result = (pgGrB_FlatVector *) palloc0(sizeof(pgGrB_FlatVector));
  SET_VARSIZE(result, sizeof(pgGrB_FlatVector));
  result->size = size;
  result->nvals = 0;
  result->type = type;
  return result;
}

/* Helper function to always expanded datum

This is used by PGGRB_GETARG_VECTOR */
pgGrB_Vector *
DatumGetVector(Datum d) {
  pgGrB_Vector *A;
  
  if (VARATT_IS_EXTERNAL_EXPANDED(DatumGetPointer(d))) {
    A = (pgGrB_Vector *) DatumGetEOHP(d);
    Assert(A->ev_magic == vector_MAGIC);
    return A;
  }
  d = expand_flat_vector_int64(d, CurrentMemoryContext);
  return (pgGrB_Vector *) DatumGetEOHP(d);
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
    vec = (pgGrB_Vector *) PGGRB_GETARG_VECTOR(0);

    state = (pgGrB_Vector_ExtractState*)palloc(sizeof(pgGrB_Vector_ExtractState));
    CHECKD(GrB_Vector_size(&size, vec->V));

    state->indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * size);
    state->vals = (int64*) palloc0(sizeof(int64) * size);

    CHECKD(GrB_Vector_extractTuples(state->indices,
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
    values[0] = Int64GetDatum(state->indices[funcctx->call_cntr]);
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

  /* Datum arr; */
  /* ArrayType *vals; */
  /* FunctionCallInfoData locfcinfo; */
  /* int ndim, *dims; */
  /* int64 count, max; */

  /* GrB_Index *row_indices; */
  /* int64 *vector_vals, *data; */

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

  /* InitFunctionCallInfoData(locfcinfo, */
  /*                          fcinfo->flinfo, */
  /*                          3, */
  /*                          InvalidOid, */
  /*                          NULL, */
  /*                          NULL); */

  /* locfcinfo.arg[0] = PG_GETARG_DATUM(0); */
  /* locfcinfo.arg[1] = ObjectIdGetDatum(INT8OID); */
  /* locfcinfo.arg[2] = Int32GetDatum(-1); */
  /* locfcinfo.argnull[0] = false; */
  /* locfcinfo.argnull[1] = false; */
  /* locfcinfo.argnull[2] = false; */

  /* arr = array_in(&locfcinfo); */

  /* Assert(!locfcinfo.isnull); */

  /* vals = DatumGetArrayTypeP(arr); */

  /* if (ARR_HASNULL(vals)) */
  /*   ereport(ERROR, (errmsg("Array may not contain NULLs"))); */

  /* ndim = ARR_NDIM(vals); */

  /* dims = ARR_DIMS(vals); */

  /* count = dims[0]; */
  /* max = count; */

  /* row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * count); */
  /* vector_vals = (int64*) palloc0(sizeof(int64) * count); */

  /* data = (int64*)ARR_DATA_PTR(vals); */

  /* if (ndim == 1) { */
  /*   for (int64 i = 0; i < count; i++) { */
  /*     row_indices[i] = i; */
  /*     vector_vals[i] = data[i]; */
  /*   } */
  /* } else if (ndim == 2) { */
  /*   count = dims[1]; */
  /*   max = count; */
  /*   for (int64 i = 0; i < count; i++) { */
  /*     row_indices[i] = data[i]; */
  /*     vector_vals[i] = data[i+count]; */
  /*     if (data[i] > max) */
  /*       max = data[i]; */
  /*   } */
  /*   max++; */
  /* } else { */
  /*   ereport(ERROR, */
  /*           (errmsg("One (dense) or two (sparse) dimensional arrays required"))); */
  /* } */

  retval = construct_empty_expanded_vector_int64(0, GrB_INT64, CurrentMemoryContext);

  PGGRB_RETURN_VECTOR(retval);
}

/* cast implementation function. */
Datum
vector(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *retval;
  ArrayType *vals;
  int ndim, *dims;
  int64 count, max;
  
  GrB_Index *row_indices;
  int64 *vector_vals, *data;

  vals = PG_GETARG_ARRAYTYPE_P(0);

  if (ARR_HASNULL(vals))
    ereport(ERROR, (errmsg("Array may not contain NULLs")));

  ndim = ARR_NDIM(vals);
  dims = ARR_DIMS(vals);
  count = dims[0];
  
  row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * count);
  vector_vals = (int64*) palloc0(sizeof(int64) * count);

  data = (int64*)ARR_DATA_PTR(vals);

  for (int64 i = 0; i < count; i++) {
    row_indices[i] = i;
    vector_vals[i] = data[i];
  }

  retval = construct_empty_expanded_vector_int64(count, GrB_INT64, CurrentMemoryContext);

  CHECKD(GrB_Vector_build(retval->V,
                         row_indices,
                         vector_vals,
                         count,
                         GrB_PLUS_INT64));

  PGGRB_RETURN_VECTOR(retval);
}
