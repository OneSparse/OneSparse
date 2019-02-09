
/* MemoryContextCallback function to free matrices */
static void
context_callback_vector_free(void* a) {
  pgGrB_Vector *A = (pgGrB_Vector *) a;
  GrB_Vector_free(&A->V);
}

/* Expanded Object Header "methods" for flattening matrices for storage */
static Size vector_get_flat_size(ExpandedObjectHeader *eohptr);
static void vector_flatten_into(ExpandedObjectHeader *eohptr,
                            void *result, Size allocated_size);

static const ExpandedObjectMethods vector_methods = {
  vector_get_flat_size,
  vector_flatten_into
};

/* Compute size of storage needed for vector */
static Size
vector_get_flat_size(ExpandedObjectHeader *eohptr) {
  GrB_Info info;
  pgGrB_Vector *A = (pgGrB_Vector *) eohptr;
  Size nbytes;
  GrB_Index size, nvals;

  Assert(A->ev_magic == vector_MAGIC);

  if (A->flat_value)
   return VARSIZE(A->flat_value);

  if (A->flat_size)
    return A->flat_size;

  CHECKD(GrB_Vector_size(&size, A->V));
  CHECKD(GrB_Vector_nvals(&nvals, A->V));

  nbytes = PGGRB_VECTOR_OVERHEAD();
  nbytes += size * sizeof(GrB_Index);
  nbytes += nvals * sizeof(int64);

  A->flat_size = nbytes;
  return nbytes;
}

/* Flatten vector into allocated result buffer  */
static void
vector_flatten_into(ExpandedObjectHeader *eohptr,
                void *result, Size allocated_size)  {
  GrB_Info info;
  GrB_Index *start;
  pgGrB_Vector *A = (pgGrB_Vector *) eohptr;
  pgGrB_FlatVector *flat = (pgGrB_FlatVector *) result;

  if (A->flat_value) {
    Assert(allocated_size == VARSIZE(A->flat_value));
    memcpy(flat, A->flat_value, allocated_size);
    return;
  }

  Assert(A->ev_magic == vector_MAGIC);
  Assert(allocated_size == A->flat_size);

  memset(flat, 0, allocated_size);
  
  CHECKV(GrB_Vector_size(&flat->size, A->V));
  CHECKV(GrB_Vector_nvals(&flat->nvals, A->V));
  
  start = PGGRB_VECTOR_DATA(flat);
  CHECKV(GrB_Vector_extractTuples(start,
                                  start + flat->size,
                                  &flat->nvals,
                                  A->V));
  flat->type = GrB_INT64;

  SET_VARSIZE(flat, allocated_size);
}

/* Expand a flat vector. */
Datum
expand_flat_vector(Datum flatdatum,
                   MemoryContext parentcontext) {
  GrB_Info info;
  
  pgGrB_Vector *A;
  pgGrB_FlatVector *flat;
  
  MemoryContext objcxt, oldcxt;
  MemoryContextCallback *ctxcb;
  
  GrB_Index size, nvals;
  GrB_Index *indices;
  int64 *vals;
  GrB_Type type;

  /* Create a new context that will hold the expanded object. */
  objcxt = AllocSetContextCreate(
          parentcontext,
          "expanded vector",
          ALLOCSET_DEFAULT_SIZES);

  /* Allocate a new expanded vector */
  A = (pgGrB_Vector*)MemoryContextAlloc(
          objcxt,
          sizeof(pgGrB_Vector));

  /* Initialize the ExpandedObjectHeader member with flattening
   * methods and new object context */
  EOH_init_header(&A->hdr, &vector_methods, objcxt);

  /* Used for debugging checks */
  A->ev_magic = vector_MAGIC;

  /* Switch to new object context */
  oldcxt = MemoryContextSwitchTo(objcxt);

  /* Copy the flat datum into our context */
  flat = (pgGrB_FlatVector*)flatdatum;

  /* Get dimensional information from flat */
  size = flat->size;
  nvals = flat->nvals;
  type = flat->type;

  /* indices and vals are pointers into the vardata area */
  
  indices = PGGRB_VECTOR_DATA(flat);
  vals = (int64*)(indices + size);

  /* Initialize the new vector */
  CHECKD(GrB_Vector_new(&A->V,
                        type,
                        size));

  /* Create a context callback to free vector when context is cleared */
  ctxcb = (MemoryContextCallback*)MemoryContextAlloc(
          objcxt,
          sizeof(MemoryContextCallback));
  
  ctxcb->func = context_callback_vector_free;
  ctxcb->arg = A;
  MemoryContextRegisterResetCallback(objcxt, ctxcb);

  /* If there's actual values, build the vector */
  if (nvals > 0) {
    CHECKD(GrB_Vector_build(A->V,
                            indices,
                            vals,
                            nvals,
                            GrB_SECOND_INT64));
  }

  A->flat_size = 0;
  A->flat_value = NULL;

  /* Switch back to old context */
  MemoryContextSwitchTo(oldcxt);
  PGGRB_RETURN_VECTOR(A);
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

/* Construct an empty expanded vector. */
pgGrB_Vector *
construct_empty_expanded_vector(GrB_Index size,
                                GrB_Type type,
                                MemoryContext parentcontext) {
  pgGrB_FlatVector  *flat;
  Datum	d;
  flat = construct_empty_flat_vector(size, type);
  d = expand_flat_vector(PointerGetDatum(flat), parentcontext);
  pfree(flat);
  return (pgGrB_Vector *) DatumGetEOHP(d);
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
  d = expand_flat_vector(d, CurrentMemoryContext);
  return (pgGrB_Vector *) DatumGetEOHP(d);
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
vector_final_int8(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *retval;

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

  retval = (pgGrB_Vector*)palloc(sizeof(pgGrB_Vector));

  ctxcb = (MemoryContextCallback*) palloc(sizeof(MemoryContextCallback));
  ctxcb->func = context_callback_vector_free;
  ctxcb->arg = retval;
  MemoryContextRegisterResetCallback(CurrentMemoryContext, ctxcb);

  CHECKD(GrB_Vector_new(&retval->V,
                       GrB_INT64,
                       count));

  CHECKD(GrB_Vector_build(retval->V,
                         row_indices,
                         vector_vals,
                         count,
                         GrB_SECOND_INT64));
  PGGRB_RETURN_VECTOR(retval);
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

  Datum arr;
  ArrayType *vals;
  FunctionCallInfoData locfcinfo;
  int ndim, *dims;
  int64 count, max;

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

  if (ARR_HASNULL(vals))
    ereport(ERROR, (errmsg("Array may not contain NULLs")));

  ndim = ARR_NDIM(vals);

  dims = ARR_DIMS(vals);

  count = dims[0];
  max = count;

  row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * count);
  vector_vals = (int64*) palloc0(sizeof(int64) * count);

  data = (int64*)ARR_DATA_PTR(vals);

  if (ndim == 1) {
    for (int64 i = 0; i < count; i++) {
      row_indices[i] = i;
      vector_vals[i] = data[i];
    }
  } else if (ndim == 2) {
    count = dims[1];
    max = count;
    for (int64 i = 0; i < count; i++) {
      row_indices[i] = data[i];
      vector_vals[i] = data[i+count];
      if (data[i] > max)
        max = data[i];
    }
    max++;
  } else {
    ereport(ERROR,
            (errmsg("One (dense) or two (sparse) dimensional arrays required")));
  }

  retval = construct_empty_expanded_vector(max, GrB_INT64, CurrentMemoryContext);

  CHECKD(GrB_Vector_build(retval->V,
                         row_indices,
                         vector_vals,
                         count,
                         GrB_PLUS_INT64));

  PGGRB_RETURN_VECTOR(retval);
}

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
  C = construct_empty_expanded_vector(size, GrB_INT64, CurrentMemoryContext);
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
  C = construct_empty_expanded_vector(size, GrB_INT64, CurrentMemoryContext);
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
