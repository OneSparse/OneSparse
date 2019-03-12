/* This is a "header template" see vector.c for specific instanciations */

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
FN(expand_flat_vector)(pgGrB_FlatVector *flat,
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

Datum FN(vector_eq)(pgGrB_Vector *A,
                    pgGrB_Vector *B);

Datum FN(vector_ne)(pgGrB_Vector *A,
                    pgGrB_Vector *B);

GrB_Index *FN(extract_indexes)(pgGrB_Vector *A, GrB_Index size);

PG_FUNCTION_INFO_V1(FN(vector));
PG_FUNCTION_INFO_V1(FN(vector_empty));
PG_FUNCTION_INFO_V1(FN(vector_elements));
PG_FUNCTION_INFO_V1(FN(vector_reduce));
PG_FUNCTION_INFO_V1(FN(vector_assign));
PG_FUNCTION_INFO_V1(FN(vector_set_element));

/* Compute size of storage needed for vector */
static Size
FN(vector_get_flat_size)(ExpandedObjectHeader *eohptr) {
  GrB_Info info;
  pgGrB_Vector *A = (pgGrB_Vector *) eohptr;
  Size nbytes;
  GrB_Index size, nvals;

  Assert(A->ev_magic == vector_MAGIC);

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
  GrB_Index *start, size, nvals;
  pgGrB_Vector *A = (pgGrB_Vector *) eohptr;
  pgGrB_FlatVector *flat = (pgGrB_FlatVector *) result;
#ifdef IMPORT_EXPORT
  void *values;
  GrB_Type type;
#endif

  Assert(A->ev_magic == vector_MAGIC);
  Assert(allocated_size == A->flat_size);

  memset(flat, 0, allocated_size);

  CHECKV(GrB_Vector_nvals(&nvals, A->V));
  CHECKV(GrB_Vector_size(&size, A->V));

#ifdef IMPORT_EXPORT
  CHECKV(GxB_Vector_export(&A->V,
                           &type,
                           &size,
                           &nvals,
                           &start,
                           &values,
                           NULL));

  if (nvals > 0) {
    memcpy(PGGRB_VECTOR_DATA(flat), start, nvals*sizeof(GrB_Index));
    memcpy((PGGRB_VECTOR_DATA(flat) + nvals), values, nvals*sizeof(PG_TYPE));
    pfree(start);
    pfree(values);
  }
#else
  start = PGGRB_VECTOR_DATA(flat);
  CHECKV(GrB_Vector_extractTuples(start,
                                  start + size,
                                  &nvals,
                                  A->V));
#endif

  flat->size = size;
  flat->nvals = nvals;
  flat->type = GB_TYPE;

  SET_VARSIZE(flat, allocated_size);
}

/* Expand a flat vector. */
Datum
FN(expand_flat_vector)(pgGrB_FlatVector *flat,
                       MemoryContext parentcontext) {
  GrB_Info info;

  pgGrB_Vector *A;

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
  d = FN(expand_flat_vector)(flat, parentcontext);
  pfree(flat);
  return (pgGrB_Vector *) DatumGetEOHP(d);
}

Datum
FN(vector_empty)(PG_FUNCTION_ARGS) {
  pgGrB_Vector *retval;
  GrB_Index max_index;
  max_index = PG_GETARG_INT64(0);
  retval = FN(construct_empty_expanded_vector)(max_index, CurrentMemoryContext);
  PGGRB_RETURN_VECTOR(retval);
  }

/* cast implementation function. */
Datum
FN(vector)(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *retval;
  ArrayType *indexes, *vals;
  int *idims, *vdims;
  int i;
  GrB_Index vcount, size;

  GrB_Index *row_indices;
  PG_TYPE *values;
  ArrayIterator array_iterator;
  Datum	value;
  bool isnull;

  if (PG_NARGS() == 1) {
    indexes = NULL;
    vals = PG_GETARG_ARRAYTYPE_P(0);
    if (ARR_HASNULL(vals))
      ereport(ERROR, (errmsg("Value array may not contain NULLs")));
    vdims = ARR_DIMS(vals);
  } else {
    indexes = PG_GETARG_ARRAYTYPE_P(0);
    vals = PG_GETARG_ARRAYTYPE_P(1);
    if (ARR_HASNULL(vals) || ARR_HASNULL(indexes))
      ereport(ERROR, (errmsg("Neither value or indexc arrays may not contain NULLs")));
    
    idims = ARR_DIMS(indexes);
    vdims = ARR_DIMS(vals);
    
    if (idims[0] != vdims[0])
      ereport(ERROR, (errmsg("Index and Value arrays must be same size.")));
  }

  size = vcount = vdims[0];

  if (PG_NARGS() > 2 && !PG_ARGISNULL(2))
    size = PG_GETARG_INT64(2);

  row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * vcount);
  values = (PG_TYPE*) palloc0(sizeof(PG_TYPE) * vcount);

  i = 0;
  array_iterator = array_create_iterator(vals, 0, NULL);
  while (array_iterate(array_iterator, &value, &isnull))
	{
      if (indexes == NULL)
        row_indices[i] = i;
      values[i] = PG_DGT(value);
      i++;
  }

  i = 0;
  if (indexes != NULL) {
    array_iterator = array_create_iterator(indexes, 0, NULL);
    while (array_iterate(array_iterator, &value, &isnull)) {
      row_indices[i] = DatumGetInt64(value);
      if (row_indices[i] > size)
        size = row_indices[i] + 1;
      i++;
    }
  }

  retval = FN(construct_empty_expanded_vector)(size, CurrentMemoryContext);

  CHECKD(GrB_Vector_build(retval->V,
                         row_indices,
                         values,
                         vcount,
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
  CHECKD(GrB_eWiseAdd(C->V, mask ? mask->V : NULL, NULL, GB_ADD, A->V, B->V, NULL));
  PGGRB_RETURN_VECTOR(C);
}

Datum FN(vector_eq)(pgGrB_Vector *A,
                    pgGrB_Vector *B) {
  GrB_Info info;
  pgGrB_Vector *C;
  GrB_Index size;
  bool result = 0;

  CHECKD(GrB_Vector_size(&size, A->V));
  C = FN(construct_empty_expanded_vector)(size, CurrentMemoryContext);

  CHECKD(GrB_eWiseMult(C->V, NULL, NULL, GB_EQ, A->V, B->V, NULL));
  CHECKD(GrB_Vector_reduce_BOOL(&result, NULL, GxB_LAND_BOOL_MONOID, C->V, NULL));
  PG_RETURN_BOOL(result);

}

Datum FN(vector_ne)(pgGrB_Vector *A,
                     pgGrB_Vector *B) {
  GrB_Info info;
  pgGrB_Vector *C;
  GrB_Index size;
  bool result = 1;

  CHECKD(GrB_Vector_size(&size, A->V));
  C = FN(construct_empty_expanded_vector)(size, CurrentMemoryContext);

  CHECKD(GrB_eWiseMult(C->V, NULL, NULL, GB_NE, A->V, B->V, NULL));
  CHECKD(GrB_Vector_reduce_BOOL(&result, NULL, GxB_LAND_BOOL_MONOID, C->V, NULL));
  PG_RETURN_BOOL(result);
}

GrB_Index *FN(extract_indexes)(pgGrB_Vector *A,
                               GrB_Index size) {
  GrB_Index *row_indices;
  PG_TYPE *values;
  GrB_Info info;

  row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * size);
  values = (PG_TYPE*) palloc0(sizeof(PG_TYPE) * size);

  CHECKD(GrB_Vector_extractTuples(row_indices,
                                 values,
                                 &size,
                                 A->V));
  return row_indices;
}

Datum
FN(vector_reduce)(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *A;
  GrB_Semiring semiring;
  GrB_Monoid monoid;
  char *semiring_name;
  PG_TYPE val;

  A = PGGRB_GETARG_VECTOR(0);
  semiring_name = PG_ARGISNULL(1) ? NULL : text_to_cstring(PG_GETARG_TEXT_PP(1));

  semiring = semiring_name ? lookup_semiring(semiring_name) : GB_RNG;
  CHECKD(GxB_Semiring_add(&monoid, semiring));
  CHECKD(GrB_reduce(&val, NULL, monoid, A->V, NULL));
  PG_RET(val);
}

Datum
FN(vector_assign)(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *A, *B, *mask;
  GrB_Index size, *indexes = NULL;
  PG_TYPE val;

  A = PGGRB_GETARG_VECTOR(0);
  val = PG_GET(1);
  B = PG_ARGISNULL(2) ? NULL : PGGRB_GETARG_VECTOR(2);
  mask = PG_ARGISNULL(3)? NULL : PGGRB_GETARG_VECTOR(3);

  CHECKD(GrB_Vector_size(&size, A->V));

  if (B != NULL)
    indexes = FN(extract_indexes)(B, size);

  CHECKD(GrB_assign(A->V, mask? mask->V: NULL, NULL, val, indexes ? indexes : GrB_ALL, size, NULL));
  PGGRB_RETURN_VECTOR(A);
}

Datum
FN(vector_set_element)(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *A;
  GrB_Index index;
  PG_TYPE val;

  A = PGGRB_GETARG_VECTOR(0);
  index = PG_GETARG_INT64(1);
  val = PG_GET(2);

  CHECKD(GrB_Vector_setElement(A->V, val, index));
  PGGRB_RETURN_VECTOR(A);
}

#undef SUFFIX
#undef PG_TYPE
#undef GB_TYPE
#undef GB_DUP
#undef GB_MUL
#undef GB_ADD
#undef GB_EQ
#undef GB_NE
#undef GB_RNG
#undef PG_GET
#undef PG_RET
#undef PG_DGT
#undef PG_TGD
#undef PRINT_FMT
