/* This is a "header template" see vector.c for specific instanciations */

#ifndef PG_TYPE
    #error Template argument missing.
#endif
#ifndef SUFFIX
    #error Suffix argument missing.
#endif

#define VECTOR_BINOP_PREAMBLE()                           \
  do {                                                    \
    A = PGGRB_GETARG_VECTOR(0);                           \
    B = PGGRB_GETARG_VECTOR(1);                           \
    C = (pgGrB_Vector *) palloc0(sizeof(pgGrB_Vector));   \
  } while (0)


typedef struct FN(pgGrB_Vector_ExtractState) {
  pgGrB_Vector *vec;
  GrB_Index *indices;
  PG_TYPE *vals;
} FN(pgGrB_Vector_ExtractState);

/* Expanded Object Header "methods" for flattening vectors for storage */
static Size FN(vector_get_flat_size)(ExpandedObjectHeader *eohptr);
static void FN(vector_flatten_into)(ExpandedObjectHeader *eohptr,
                            void *result, Size allocated_size);

static const ExpandedObjectMethods FN(vector_methods) = {
  FN(vector_get_flat_size),
  FN(vector_flatten_into)
};

/* Utility function that expands a flattened vector datum. */
Datum
FN(expand_flat_vector)(Datum vectordatum,
                   MemoryContext parentcontext);

/* Helper function that creates an expanded empty vector. */
pgGrB_Vector *
FN(construct_empty_expanded_vector)(GrB_Index size,
                                    MemoryContext parentcontext);

Datum FN(vector_out)(pgGrB_Vector *vec);
Datum FN(vector_ewise_mult)(pgGrB_Vector *A,
                            pgGrB_Vector *B,
                            pgGrB_Vector *C,
                            pgGrB_Vector *mask,
                            GrB_BinaryOp binop);
Datum FN(vector_ewise_add)(pgGrB_Vector *A,
                           pgGrB_Vector *B,
                            pgGrB_Vector *C,
                            pgGrB_Vector *mask,
                            GrB_BinaryOp binop);

PG_FUNCTION_INFO_V1(FN(vector));
PG_FUNCTION_INFO_V1(FN(vector_agg_acc));
PG_FUNCTION_INFO_V1(FN(vector_final));
PG_FUNCTION_INFO_V1(FN(vector_elements));

/* Compute size of storage needed for vector */
static Size
FN(vector_get_flat_size)(ExpandedObjectHeader *eohptr) {
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
  nbytes += nvals * sizeof(PG_TYPE);

  A->flat_size = nbytes;
  return nbytes;
}

/* Flatten vector into allocated result buffer  */
static void
FN(vector_flatten_into)(ExpandedObjectHeader *eohptr,
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
  flat->type = GB_TYPE;

  SET_VARSIZE(flat, allocated_size);
}

/* Expand a flat vector. */
Datum
FN(expand_flat_vector)(Datum flatdatum,
                       MemoryContext parentcontext) {
  GrB_Info info;

  pgGrB_Vector *A;
  pgGrB_FlatVector *flat;

  MemoryContext objcxt, oldcxt;
  MemoryContextCallback *ctxcb;

  GrB_Index size, nvals;
  GrB_Index *indices;
  PG_TYPE *vals;
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
  EOH_init_header(&A->hdr, &FN(vector_methods), objcxt);

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
  vals = (PG_TYPE*)(indices + size);

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
                            GB_DUP));
  }

  A->type = type;
  A->flat_size = 0;
  A->flat_value = NULL;

  /* Switch back to old context */
  MemoryContextSwitchTo(oldcxt);
  PGGRB_RETURN_VECTOR(A);
}

/* Construct an empty expanded vector. */
pgGrB_Vector *
FN(construct_empty_expanded_vector)(GrB_Index size,
                                    MemoryContext parentcontext) {
  pgGrB_FlatVector  *flat;
  Datum	d;
  flat = construct_empty_flat_vector(size, GB_TYPE);
  d = FN(expand_flat_vector)(PointerGetDatum(flat), parentcontext);
  pfree(flat);
  return (pgGrB_Vector *) DatumGetEOHP(d);
}

Datum
FN(vector_agg_acc)(PG_FUNCTION_ARGS)
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

  row = palloc(sizeof(PG_TYPE));
  val = palloc(sizeof(PG_TYPE));

  *row = PG_GET(1);
  *val = PG_GET(2);

  mstate->I = lappend(mstate->I, row);
  mstate->X = lappend(mstate->X, val);

  MemoryContextSwitchTo(oldcxt);

  PG_RETURN_POINTER(mstate);
}

Datum
FN(vector_final)(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *retval;

  MemoryContextCallback *ctxcb;

  pgGrB_Vector_AggState *mstate = (pgGrB_Vector_AggState*)PG_GETARG_POINTER(0);
  size_t n = 0, count = list_length(mstate->I);
  GrB_Index *row_indices;
  PG_TYPE *values;

  ListCell *li, *lv;

  if (PG_ARGISNULL(0))
    PG_RETURN_NULL();

  mstate = (pgGrB_Vector_AggState *)PG_GETARG_POINTER(0);

  row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * count);
  values = (PG_TYPE*) palloc0(sizeof(PG_TYPE) * count);

  forboth (li, (mstate)->I, lv, (mstate)->X) {
    row_indices[n] = DatumGetInt64(*(Datum*)lfirst(li));
    values[n] = PG_DGT(*(Datum*)lfirst(lv));
    n++;
  }

  retval = (pgGrB_Vector*)palloc(sizeof(pgGrB_Vector));

  ctxcb = (MemoryContextCallback*) palloc(sizeof(MemoryContextCallback));
  ctxcb->func = context_callback_vector_free;
  ctxcb->arg = retval;
  MemoryContextRegisterResetCallback(CurrentMemoryContext, ctxcb);

  CHECKD(GrB_Vector_new(&retval->V,
                       GB_TYPE,
                       count));

  CHECKD(GrB_Vector_build(retval->V,
                         row_indices,
                         values,
                         count,
                         GB_DUP));
  PGGRB_RETURN_VECTOR(retval);
}

/* cast implementation function. */
Datum
FN(vector)(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *retval;
  ArrayType *vals;
  int *dims;
  int count;

  GrB_Index *row_indices;
  PG_TYPE *values, *data;

  vals = PG_GETARG_ARRAYTYPE_P(0);

  if (ARR_HASNULL(vals))
    ereport(ERROR, (errmsg("Array may not contain NULLs")));

  dims = ARR_DIMS(vals);
  count = dims[0];

  row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * count);
  values = (PG_TYPE*) palloc0(sizeof(PG_TYPE) * count);

  data = (PG_TYPE*)ARR_DATA_PTR(vals);

  for (int i = 0; i < count; i++) {
    row_indices[i] = i;
    values[i] = data[i];
  }

  retval = FN(construct_empty_expanded_vector)(count, CurrentMemoryContext);

  CHECKD(GrB_Vector_build(retval->V,
                         row_indices,
                         values,
                         count,
                         GB_DUP));

  PGGRB_RETURN_VECTOR(retval);
}

Datum
FN(vector_elements)(PG_FUNCTION_ARGS) {
  GrB_Info info;
  FuncCallContext  *funcctx;
  TupleDesc tupdesc;
  Datum result;

  Datum values[3];
  bool nulls[2] = {false, false};
  HeapTuple tuple;
  GrB_Index size = 0;
  pgGrB_Vector *vec;
  FN(pgGrB_Vector_ExtractState) *state;

  if (SRF_IS_FIRSTCALL()) {
    MemoryContext oldcontext;

    funcctx = SRF_FIRSTCALL_INIT();
    oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
    vec = (pgGrB_Vector *) PGGRB_GETARG_VECTOR(0);

    state = (FN(pgGrB_Vector_ExtractState)*)palloc(sizeof(FN(pgGrB_Vector_ExtractState)));
    CHECKD(GrB_Vector_size(&size, vec->V));

    state->indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * size);
    state->vals = (PG_TYPE*) palloc0(sizeof(PG_TYPE) * size);

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
  state = (FN(pgGrB_Vector_ExtractState)*)funcctx->user_fctx;
  vec = state->vec;

  if (funcctx->call_cntr < funcctx->max_calls) {
    values[0] = Int64GetDatum(state->indices[funcctx->call_cntr]);
    values[1] = PG_TGD(state->vals[funcctx->call_cntr]);

    tuple = heap_form_tuple(funcctx->tuple_desc, values, nulls);
    result = HeapTupleGetDatum(tuple);
    SRF_RETURN_NEXT(funcctx, result);
  } else {
    SRF_RETURN_DONE(funcctx);
  }
}

Datum
FN(vector_out)(pgGrB_Vector *vec)
{
  GrB_Info info;
  char *result;
  GrB_Index size, nvals;
  GrB_Index *row_indices;
  PG_TYPE *values;

  CHECKD(GrB_Vector_size(&size, vec->V));
  CHECKD(GrB_Vector_nvals(&nvals, vec->V));

  row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * size);
  values = (PG_TYPE*) palloc0(sizeof(PG_TYPE) * size);

  CHECKD(GrB_Vector_extractTuples(row_indices,
                                 values,
                                 &size,
                                 vec->V));

  result = psprintf("<vector_%s(%lu):[", grb_type_to_name(vec->type), size);

  for (int i = 0; i < size; i++) {
    result = strcat(result, psprintf("%lu", row_indices[i]));
    if (i != size - 1)
      result = strcat(result, ",");
  }
  result = strcat(result, "][");

  for (int i = 0; i < nvals; i++) {
    result = strcat(result, psprintf(PRINT_FMT(values[i])));
    if (i != nvals - 1)
      result = strcat(result, ",");
  }
  result = strcat(result, "]>");

  PG_RETURN_CSTRING(result);
}

Datum
FN(vector_ewise_mult)(pgGrB_Vector *A,
                      pgGrB_Vector *B,
                      pgGrB_Vector *C,
                      pgGrB_Vector *mask,
                      GrB_BinaryOp binop) {
  GrB_Info info;
  GrB_Index size;

  if (C == NULL) {
    CHECKD(GrB_Vector_size(&size, A->V));
    C = FN(construct_empty_expanded_vector)(size, CurrentMemoryContext);
  }
  CHECKD(GrB_eWiseMult(C->V, mask ? mask->V : NULL, NULL, binop, A->V, B->V, NULL));
  PGGRB_RETURN_VECTOR(C);
}

Datum
FN(vector_ewise_add)(pgGrB_Vector *A,
                     pgGrB_Vector *B,
                      pgGrB_Vector *C,
                      pgGrB_Vector *mask,
                      GrB_BinaryOp binop) {
  GrB_Info info;
  GrB_Index size;

  if (C == NULL) {
    CHECKD(GrB_Vector_size(&size, A->V));
    C = FN(construct_empty_expanded_vector)(size, CurrentMemoryContext);
  }
  CHECKD(GrB_eWiseAdd(C->V, NULL, NULL, GB_ADD, A->V, B->V, NULL));
  PGGRB_RETURN_VECTOR(C);
}

#undef SUFFIX
#undef PG_TYPE
#undef GB_TYPE
#undef GB_DUP
#undef GB_MUL
#undef GB_ADD
#undef PG_GET
#undef PG_DGT
#undef PG_TGD
#undef PRINT_FMT
