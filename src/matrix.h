/* This is a "header template" see matrix.c for specific instanciations */

/* Set Returning Function (matrix_elements) state for generating tuples
   from a matrix.
 */
typedef struct FN(pgGrB_Matrix_ExtractState) {
  pgGrB_Matrix *mat;
  GrB_Index *rows;
  GrB_Index *cols;
  PG_TYPE *vals;
} FN(pgGrB_Matrix_ExtractState);

/* Expanded Object Header "methods" for flattening matrices for storage */
static Size FN(matrix_get_flat_size)(ExpandedObjectHeader *eohptr);
static void FN(matrix_flatten_into)(ExpandedObjectHeader *eohptr,
                                    void *result, Size allocated_size);

static const ExpandedObjectMethods FN(matrix_methods) = {
  FN(matrix_get_flat_size),
  FN(matrix_flatten_into)
};

/* Utility function that expands a flattened matrix datum. */
Datum
FN(expand_flat_matrix)(pgGrB_FlatMatrix *flat,
                       MemoryContext parentcontext);

GrB_Index FN(extract_rowscols)(pgGrB_Matrix *A,
                               GrB_Index **rows,
                               GrB_Index **cols,
                               GrB_Index nvals);

pgGrB_Matrix *
FN(construct_empty_expanded_matrix)(GrB_Index nrows,
                                    GrB_Index ncols,
                                    MemoryContext parentcontext);

Datum FN(matrix_ewise_mult)(pgGrB_Matrix *A,
                            pgGrB_Matrix *B,
                            pgGrB_Matrix *C,
                            pgGrB_Matrix *mask,
                            GrB_BinaryOp binop);

Datum FN(matrix_ewise_add)(pgGrB_Matrix *A,
                           pgGrB_Matrix *B,
                           pgGrB_Matrix *C,
                           pgGrB_Matrix *mask,
                           GrB_BinaryOp binop);

Datum
FN(mxm)(pgGrB_Matrix *A,
        pgGrB_Matrix *B,
        pgGrB_Matrix *C,
        pgGrB_Matrix *mask,
        GrB_Semiring semiring,
        GrB_BinaryOp binop,
        GrB_Descriptor desc);

Datum
FN(matrix_kron)(pgGrB_Matrix *A,
                pgGrB_Matrix *B,
                pgGrB_Matrix *C,
                pgGrB_Matrix *mask,
                GrB_BinaryOp accum,
                GrB_BinaryOp mulop,
                GrB_Descriptor desc);

Datum
FN(mxv)(pgGrB_Matrix *A,
        pgGrB_Vector *B,
        pgGrB_Vector *C,
        pgGrB_Vector *mask,
        GrB_Semiring semiring,
        GrB_BinaryOp binop,
        GrB_Descriptor desc);

Datum
FN(vxm)(pgGrB_Vector *A,
        pgGrB_Matrix *B,
        pgGrB_Vector *C,
        pgGrB_Vector *mask,
        GrB_Semiring semiring,
        GrB_BinaryOp binop,
        GrB_Descriptor desc);


PG_FUNCTION_INFO_V1(FN(matrix));
PG_FUNCTION_INFO_V1(FN(matrix_empty));
PG_FUNCTION_INFO_V1(FN(matrix_elements));
PG_FUNCTION_INFO_V1(FN(matrix_reduce));
PG_FUNCTION_INFO_V1(FN(matrix_assign));
PG_FUNCTION_INFO_V1(FN(matrix_random));

/* Compute size of storage needed for matrix */
static Size
FN(matrix_get_flat_size)(ExpandedObjectHeader *eohptr) {
  GrB_Info info;
  pgGrB_Matrix *A = (pgGrB_Matrix *) eohptr;
  Size nbytes;
  GrB_Index nrows, ncols, nvals;

  Assert(A->em_magic == matrix_MAGIC);

  if (A->flat_size)
    return A->flat_size;

  CHECKD(GrB_Matrix_nrows(&nrows, A->M));
  CHECKD(GrB_Matrix_ncols(&ncols, A->M));
  CHECKD(GrB_Matrix_nvals(&nvals, A->M));

  nbytes = PGGRB_MATRIX_OVERHEAD();
#ifdef IMPORT_EXPORT
  nbytes += (nrows + 1) * sizeof(GrB_Index);
  nbytes += nvals * sizeof(GrB_Index);
  nbytes += nvals * sizeof(PG_TYPE);
#else
  nbytes += nvals * sizeof(GrB_Index);
  nbytes += nvals * sizeof(GrB_Index);
  nbytes += nvals * sizeof(PG_TYPE);
#endif

  A->flat_size = nbytes;
  return nbytes;
}

/* Flatten matrix into allocated result buffer  */
static void
FN(matrix_flatten_into)(ExpandedObjectHeader *eohptr,
                        void *result, Size allocated_size)  {
  GrB_Info info;
  GrB_Index *rows, nrows, ncols, nvals;
  pgGrB_Matrix *A = (pgGrB_Matrix *) eohptr;
  pgGrB_FlatMatrix *flat = (pgGrB_FlatMatrix *) result;
#ifdef IMPORT_EXPORT
  int64_t nonempty = -1;
  GrB_Index *cols;
  void *values;
  GrB_Type type;
#endif

  Assert(A->em_magic == matrix_MAGIC);
  Assert(allocated_size == A->flat_size);

  memset(flat, 0, allocated_size);

#ifdef IMPORT_EXPORT
  CHECKV(GxB_Matrix_export_CSR(&A->M,
                               &type,
                               &nrows,
                               &ncols,
                               &nvals,
                               &nonempty,
                               &rows,
                               &cols,
                               &values,
                               NULL));

  if (nvals > 0) {
    memcpy(PGGRB_MATRIX_DATA(flat),
           rows,
           (nrows+1)*sizeof(GrB_Index));
    memcpy((PGGRB_MATRIX_DATA(flat) + nrows+1),
           cols,
           nvals*sizeof(GrB_Index));
    memcpy((PGGRB_MATRIX_DATA(flat) + nrows+1 + nvals),
           values,
           nvals*sizeof(PG_TYPE));
    pfree(rows);
    pfree(cols);
    pfree(values);
  }
  flat->nonempty = nonempty;
#else
  CHECKV(GrB_Matrix_nrows(&nrows, A->M));
  CHECKV(GrB_Matrix_ncols(&ncols, A->M));
  CHECKV(GrB_Matrix_nvals(&nvals, A->M));

  rows = PGGRB_MATRIX_DATA(flat);
  CHECKV(GrB_Matrix_extractTuples(rows,
                                  rows + nvals,
                                  rows + nvals + nvals,
                                  &nvals,
                                  A->M));
#endif

  flat->type = GB_TYPE;
  flat->nrows = nrows;
  flat->ncols = ncols;
  flat->nvals = nvals;
  SET_VARSIZE(flat, allocated_size);
}

/* Expand a flat matrix. */
Datum
FN(expand_flat_matrix)(pgGrB_FlatMatrix *flat,
                       MemoryContext parentcontext) {
  GrB_Info info;

  pgGrB_Matrix *A;

  MemoryContext objcxt, oldcxt;
  MemoryContextCallback *ctxcb;

  GrB_Index ncols, nrows, nvals;
  GrB_Index *rows, *cols;
  void *vals;
  GrB_Type type;
#ifdef IMPORT_EXPORT
  int64_t nonempty;
#endif

  /* Create a new context that will hold the expanded object. */
  objcxt = AllocSetContextCreate(parentcontext,
                                 "expanded matrix",
                                 ALLOCSET_DEFAULT_SIZES);

  /* Allocate a new expanded matrix */
  A = (pgGrB_Matrix*)MemoryContextAlloc(objcxt,
                                        sizeof(pgGrB_Matrix));

  /* Initialize the ExpandedObjectHeader member with flattening
   * methods and new object context */
  EOH_init_header(&A->hdr, &FN(matrix_methods), objcxt);

  /* Used for debugging checks */
  A->em_magic = matrix_MAGIC;

  /* Switch to new object context */
  oldcxt = MemoryContextSwitchTo(objcxt);

  /* Get dimensional information from flat */
  nrows = flat->nrows;
  ncols = flat->ncols;
  nvals = flat->nvals;
  type = flat->type;

  /* If there's actual values, build the matrix */
#ifdef IMPORT_EXPORT
  nonempty = flat->nonempty;

  if (nvals > 0) {
    rows = malloc_function((nrows+1)*sizeof(GrB_Index));
    cols = malloc_function(nvals*sizeof(GrB_Index));
    vals = malloc_function(nvals*sizeof(PG_TYPE));
    memcpy(rows,
           PGGRB_MATRIX_DATA(flat),
           (nrows+1)*sizeof(GrB_Index));
    memcpy(cols,
           PGGRB_MATRIX_DATA(flat) + nrows+1,
           nvals*sizeof(GrB_Index));
    memcpy(vals,
           PGGRB_MATRIX_DATA(flat) + nrows+1 + nvals,
           nvals*sizeof(PG_TYPE));

    CHECKD(GxB_Matrix_import_CSR(&A->M,
                                 type,
                                 nrows,
                                 ncols,
                                 nvals,
                                 nonempty,
                                 &rows,
                                 &cols,
                                 &vals,
                                 NULL
                                 ));
    if (A->M == NULL) {
      pfree(rows);
      pfree(cols);
      pfree(vals);
      ereport(ERROR, (errmsg("Matrix import failed")));
    }
  } else {
    CHECKD(GrB_Matrix_new(&A->M,
                          type,
                          nrows,
                          ncols));
  }

#else
  /* Initialize the new matrix */
  CHECKD(GrB_Matrix_new(&A->M,
                        type,
                        nrows,
                        ncols));

  if (nvals > 0) {
    /* Rows, cols, and vals are pointers into the vardata area */
    rows = PGGRB_MATRIX_DATA(flat);
    cols = rows + nrows;
    vals = (PG_TYPE*)(cols + ncols);

    CHECKD(GrB_Matrix_build(A->M,
                            rows,
                            cols,
                            vals,
                            nvals,
                            GB_DUP));
  }
#endif

  A->type = type;
  A->flat_size = 0;

  /* Create a context callback to free matrix when context is cleared */
  ctxcb = MemoryContextAlloc(objcxt,
                             sizeof(MemoryContextCallback));

  ctxcb->func = context_callback_matrix_free;
  ctxcb->arg = A;
  MemoryContextRegisterResetCallback(objcxt, ctxcb);

  /* Switch back to old context */
  MemoryContextSwitchTo(oldcxt);
  PGGRB_RETURN_MATRIX(A);
}

/* Construct an empty expanded matrix. */
pgGrB_Matrix *
FN(construct_empty_expanded_matrix)(GrB_Index nrows,
                                GrB_Index ncols,
                                MemoryContext parentcontext) {
  pgGrB_FlatMatrix  *flat;
  Datum	d;
  flat = construct_empty_flat_matrix(nrows, ncols, GB_TYPE);
  d = FN(expand_flat_matrix)(flat, parentcontext);
  pfree(flat);
  return (pgGrB_Matrix *) DatumGetEOHP(d);
}

Datum
FN(matrix_empty)(PG_FUNCTION_ARGS) {
  pgGrB_Matrix *retval;
  GrB_Index nrows, ncols;

  nrows = PG_GETARG_INT64(0);
  ncols = PG_GETARG_INT64(1);

  retval = FN(construct_empty_expanded_matrix)(nrows,
                                               ncols,
                                               CurrentMemoryContext);
  PGGRB_RETURN_MATRIX(retval);
}

Datum
FN(matrix_elements)(PG_FUNCTION_ARGS) {
  GrB_Info info;
  FuncCallContext  *funcctx;
  TupleDesc tupdesc;
  Datum result;

  Datum values[3];
  bool nulls[3] = {false, false, false};
  HeapTuple tuple;
  GrB_Index nvals = 0;
  pgGrB_Matrix *mat;
  FN(pgGrB_Matrix_ExtractState) *state;

  if (SRF_IS_FIRSTCALL()) {
    MemoryContext oldcontext;

    funcctx = SRF_FIRSTCALL_INIT();
    oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
    mat = PGGRB_GETARG_MATRIX(0);

    state = (FN(pgGrB_Matrix_ExtractState)*)palloc(sizeof(FN(pgGrB_Matrix_ExtractState)));
    CHECKD(GrB_Matrix_nvals(&nvals, mat->M));

    state->rows = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);
    state->cols = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);
    state->vals = (PG_TYPE*) palloc0(sizeof(PG_TYPE) * nvals);

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
  state = (FN(pgGrB_Matrix_ExtractState)*)funcctx->user_fctx;
  mat = state->mat;

  if (funcctx->call_cntr < funcctx->max_calls) {
    values[0] = Int64GetDatum(state->rows[funcctx->call_cntr]);
    values[1] = Int64GetDatum(state->cols[funcctx->call_cntr]);
    values[2] = PG_TGD(state->vals[funcctx->call_cntr]);

    tuple = heap_form_tuple(funcctx->tuple_desc, values, nulls);
    result = HeapTupleGetDatum(tuple);
    SRF_RETURN_NEXT(funcctx, result);
  } else {
    SRF_RETURN_DONE(funcctx);
  }
}

Datum
FN(matrix_ewise_mult)(pgGrB_Matrix *A,
                      pgGrB_Matrix *B,
                      pgGrB_Matrix *C,
                      pgGrB_Matrix *mask,
                      GrB_BinaryOp binop) {
  GrB_Info info;
  GrB_Index m, n;

  if (C == NULL) {
    CHECKD(GrB_Matrix_nrows(&m, A->M));
    CHECKD(GrB_Matrix_ncols(&n, A->M));
    C = FN(construct_empty_expanded_matrix)(m, n, CurrentMemoryContext);
  }
  CHECKD(GrB_eWiseMult(C->M, mask ? mask->M : NULL, NULL, binop, A->M, B->M, NULL));
  PGGRB_RETURN_MATRIX(C);
}

Datum
FN(matrix_ewise_add)(pgGrB_Matrix *A,
                     pgGrB_Matrix *B,
                     pgGrB_Matrix *C,
                     pgGrB_Matrix *mask,
                     GrB_BinaryOp binop
                     ) {
  GrB_Info info;
  GrB_Index m, n;

  if (C == NULL) {
    CHECKD(GrB_Matrix_nrows(&m, A->M));
    CHECKD(GrB_Matrix_ncols(&n, A->M));
    C = FN(construct_empty_expanded_matrix)(m, n, CurrentMemoryContext);
  }

  CHECKD(GrB_eWiseAdd(C->M, mask ? mask->M : NULL, NULL, binop, A->M, B->M, NULL));
  PGGRB_RETURN_MATRIX(C);
}

Datum
FN(mxm)(pgGrB_Matrix *A,
        pgGrB_Matrix *B,
        pgGrB_Matrix *C,
        pgGrB_Matrix *mask,
        GrB_Semiring semiring,
        GrB_BinaryOp binop,
        GrB_Descriptor desc) {
  GrB_Info info;

  if (C == NULL) {
    GrB_Index m, n;
    CHECKD(GrB_Matrix_nrows(&m, A->M));
    CHECKD(GrB_Matrix_ncols(&n, B->M));
    C = FN(construct_empty_expanded_matrix)(m, n, CurrentMemoryContext);
  }
  CHECKD(GrB_mxm(C->M, mask ? mask->M : NULL, binop, semiring, A->M, B->M, desc));
  PGGRB_RETURN_MATRIX(C);
}

Datum
FN(mxv)(pgGrB_Matrix *A,
        pgGrB_Vector *B,
        pgGrB_Vector *C,
        pgGrB_Vector *mask,
        GrB_Semiring semiring,
        GrB_BinaryOp binop,
        GrB_Descriptor desc) {
  GrB_Info info;
  GrB_Index size;

  if (C == NULL) {
    CHECKD(GrB_Matrix_ncols(&size, A->M));
    C = FN(construct_empty_expanded_vector)(size, CurrentMemoryContext);
  }
  CHECKD(GrB_mxv(C->V, mask ? mask->V : NULL, binop, semiring, A->M, B->V, desc));
  PGGRB_RETURN_VECTOR(C);
}

Datum
FN(vxm)(pgGrB_Vector *A,
        pgGrB_Matrix *B,
        pgGrB_Vector *C,
        pgGrB_Vector *mask,
        GrB_Semiring semiring,
        GrB_BinaryOp binop,
        GrB_Descriptor desc) {
  GrB_Info info;
  GrB_Index size;

  if (C == NULL) {
    CHECKD(GrB_Vector_size(&size, A->V));
    C = FN(construct_empty_expanded_vector)(size, CurrentMemoryContext);
  }
  CHECKD(GrB_vxm(C->V, mask ? mask->V : NULL, binop, semiring, A->V, B->M, desc));
  PGGRB_RETURN_VECTOR(C);
}

Datum
FN(matrix)(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *retval;
  ArrayType *rows, *cols, *vals;
  int *idims, *jdims, *vdims;
  int i, count;

  GrB_Index *row_indices, *col_indices;
  GrB_Index max_row_index, max_col_index;
  PG_TYPE *values;
  ArrayIterator array_iterator;
  Datum	value;
  bool isnull;

  if (PG_NARGS() == 2) {
    max_row_index = PG_GETARG_INT64(0);
    max_col_index = PG_GETARG_INT64(1);
    retval = FN(construct_empty_expanded_matrix)(max_row_index,
                                                 max_col_index,
                                                 CurrentMemoryContext);
    PGGRB_RETURN_MATRIX(retval);
  }

  rows = PG_GETARG_ARRAYTYPE_P(0);
  cols = PG_GETARG_ARRAYTYPE_P(1);
  vals = PG_GETARG_ARRAYTYPE_P(2);

  if (ARR_HASNULL(rows) || ARR_HASNULL(cols) || ARR_HASNULL(vals))
    ereport(ERROR, (errmsg("Matrix values may not be null")));

  idims = ARR_DIMS(rows);
  jdims = ARR_DIMS(cols);
  vdims =  ARR_DIMS(vals);

  if (!PG_ARGISNULL(3))
    max_row_index = PG_GETARG_INT64(3);
  else
    max_row_index = idims[0];

  if (!PG_ARGISNULL(4))
    max_col_index = PG_GETARG_INT64(4);
  else
    max_col_index = jdims[0];

  count = vdims[0];

  if ((idims[0] != jdims[0]) || (jdims[0] != vdims[0]))
    ereport(ERROR, (errmsg("Row, column, and value arrays must be same size.")));

  row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * max_row_index);
  col_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * max_col_index);
  values = (PG_TYPE*) palloc0(sizeof(PG_TYPE) * count);

  i = 0;
  array_iterator = array_create_iterator(rows, 0, NULL);
  while (array_iterate(array_iterator, &value, &isnull))
	{
      row_indices[i] = DatumGetInt64(value);
      if (row_indices[i] > max_row_index)
        max_row_index = row_indices[i] + 1;
      i++;
  }
  array_free_iterator(array_iterator);

  i = 0;
  array_iterator = array_create_iterator(cols, 0, NULL);
  while (array_iterate(array_iterator, &value, &isnull))
	{
      col_indices[i] = DatumGetInt64(value);
      if (col_indices[i] > max_col_index)
        max_col_index = col_indices[i] + 1;
      i++;
  }
  array_free_iterator(array_iterator);

  i = 0;
  array_iterator = array_create_iterator(vals, 0, NULL);
  while (array_iterate(array_iterator, &value, &isnull))
	{
      values[i] = PG_DGT(value);
      i++;
  }
  array_free_iterator(array_iterator);

  retval = FN(construct_empty_expanded_matrix)(max_row_index,
                                               max_col_index,
                                               CurrentMemoryContext);

  CHECKD(GrB_Matrix_build(retval->M,
                          row_indices,
                          col_indices,
                          values,
                          count,
                          GB_DUP));

  PGGRB_RETURN_MATRIX(retval);
}

Datum
FN(matrix_reduce)(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A;
  GrB_Semiring semiring;
  GrB_Monoid monoid;
  char *semiring_name;
  PG_TYPE val;

  A = PGGRB_GETARG_MATRIX(0);
  semiring_name = text_to_cstring(PG_GETARG_TEXT_PP(1));
  semiring = lookup_semiring(semiring_name);
  CHECKD(GxB_Semiring_add(&monoid, semiring));
  CHECKD(GrB_reduce(&val, NULL, monoid, A->M, NULL));
  PG_RET(val);
}

GrB_Index FN(extract_rowscols)(pgGrB_Matrix *A,
                         GrB_Index **rows,
                         GrB_Index **cols,
                         GrB_Index nvals) {
  PG_TYPE *values;
  GrB_Info info;

  *rows = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);
  *cols = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);
  values = (PG_TYPE*) palloc0(sizeof(PG_TYPE) * nvals);

  CHECKD(GrB_Matrix_extractTuples(*rows,
                                  *cols,
                                  values,
                                  &nvals,
                                  A->M));
  return nvals;
}

Datum
FN(matrix_assign)(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A, *B=NULL, *mask=NULL;
  GrB_Index nrows, ncols, nvals;
  GrB_Index *rows = NULL, *cols = NULL;
  PG_TYPE val;

  A = PGGRB_GETARG_MATRIX(0);
  val = PG_GET(1);
  B = PG_ARGISNULL(2) ? NULL : PGGRB_GETARG_MATRIX(2);
  mask = PG_ARGISNULL(3)? NULL : PGGRB_GETARG_MATRIX(3);

  if (B != NULL) {
    CHECKD(GrB_Matrix_nvals(&nvals, B->M));
    nrows = ncols = nvals = FN(extract_rowscols)(B, &rows, &cols, nvals);
  } else {
    CHECKD(GrB_Matrix_nrows(&nrows, A->M));
    CHECKD(GrB_Matrix_ncols(&ncols, A->M));
  }

  CHECKD(GrB_assign(A->M,
                    mask? mask->M: NULL,
                    NULL,
                    val,
                    rows?rows:GrB_ALL,
                    nrows,
                    cols?cols:GrB_ALL,
                    ncols,
                    NULL));

  PGGRB_RETURN_MATRIX(A);
}

Datum
FN(matrix_kron)(pgGrB_Matrix *A,
        pgGrB_Matrix *B,
        pgGrB_Matrix *C,
        pgGrB_Matrix *mask,
        GrB_BinaryOp accum,
        GrB_BinaryOp mulop,
        GrB_Descriptor desc) {
  GrB_Info info;

  if (C == NULL) {
    GrB_Index m, n, p, q;
    CHECKD(GrB_Matrix_nrows(&m, A->M));
    CHECKD(GrB_Matrix_ncols(&n, A->M));
    CHECKD(GrB_Matrix_nrows(&p, B->M));
    CHECKD(GrB_Matrix_ncols(&q, B->M));
    C = FN(construct_empty_expanded_matrix)(m*p, n*q, CurrentMemoryContext);
  }
  CHECKD(GxB_kron(C->M, mask ? mask->M : NULL, accum, mulop, A->M, B->M, desc));
  PGGRB_RETURN_MATRIX(C);
}

Datum
FN(matrix_random)(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A;
  GrB_Index nrows, ncols, nvals;
  bool make_pattern, make_symmetric, make_skew_symmetric, make_hermitian, no_diagonal;
  uint64_t seed;
  MemoryContext objcxt, oldcontext;
  
  nrows = PG_GETARG_INT64(0);
  ncols = PG_GETARG_INT64(1);
  nvals = PG_GETARG_INT64(2);
  make_pattern = PG_GETARG_BOOL(3);
  make_symmetric = PG_GETARG_BOOL(4);
  make_skew_symmetric = PG_GETARG_BOOL(5);
  make_hermitian = PG_GETARG_BOOL(6);
  no_diagonal = PG_GETARG_BOOL(7);
  /* Create a new context that will hold the expanded object. */
  objcxt = AllocSetContextCreate(CurrentMemoryContext,
                                 "expanded matrix",
                                 ALLOCSET_DEFAULT_SIZES);

  /* Allocate a new expanded matrix */
  A = (pgGrB_Matrix*)MemoryContextAlloc(objcxt,
                                        sizeof(pgGrB_Matrix));

  /* Initialize the ExpandedObjectHeader member with flattening
   * methods and new object context */
  EOH_init_header(&A->hdr, &FN(matrix_methods), objcxt);

  /* Used for debugging checks */
  A->em_magic = matrix_MAGIC;
  A->flat_size = 0;

  oldcontext = MemoryContextSwitchTo(objcxt);
  
  CHECKD(LAGraph_random(&A->M,
                        GB_TYPE,
                        nrows,
                        ncols,
                        nvals,
                        make_pattern,
                        make_symmetric,
                        make_skew_symmetric,
                        make_hermitian,
                        no_diagonal,
                        &seed));

  MemoryContextSwitchTo(oldcontext);
  PGGRB_RETURN_MATRIX(A);
}

#undef SUFFIX
#undef PG_TYPE
#undef GB_TYPE
#undef GB_DUP
#undef GB_MUL
#undef GB_ADD
#undef PG_GET
#undef PG_RET
#undef PG_DGT
#undef PG_TGD
#undef PRINT_FMT
