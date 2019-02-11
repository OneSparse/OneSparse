#ifndef PREFIX
    #define PREFIX
#endif
#define CCAT2(x, y) x ## y
#define CCAT(x, y) CCAT2(x, y)
#define FN(x) CCAT(x, SUFFIX)

#ifndef T
    #error Template argument missing.
#endif

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
FN(expand_flat_matrix)(Datum matrixdatum,
                   MemoryContext parentcontext);

/* Compute size of storage needed for matrix */
static Size
FN(matrix_get_flat_size)(ExpandedObjectHeader *eohptr) {
  GrB_Info info;
  pgGrB_Matrix *A = (pgGrB_Matrix *) eohptr;
  Size nbytes;
  GrB_Index nrows, ncols, nvals;

  Assert(A->em_magic == matrix_MAGIC);

  if (A->flat_value)
   return VARSIZE(A->flat_value);

  if (A->flat_size)
    return A->flat_size;

  CHECKD(GrB_Matrix_nrows(&nrows, A->M));
  CHECKD(GrB_Matrix_ncols(&ncols, A->M));
  CHECKD(GrB_Matrix_nvals(&nvals, A->M));

  nbytes = PGGRB_MATRIX_OVERHEAD();
  nbytes += nrows * sizeof(GrB_Index);
  nbytes += ncols * sizeof(GrB_Index);
  nbytes += nvals * sizeof(T);

  A->flat_size = nbytes;
  return nbytes;
}

/* Flatten matrix into allocated result buffer  */
static void
FN(matrix_flatten_into)(ExpandedObjectHeader *eohptr,
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

  Assert(A->em_magic == matrix_MAGIC);
  Assert(allocated_size == A->flat_size);

  memset(flat, 0, allocated_size);
  
  CHECKV(GrB_Matrix_nrows(&flat->nrows, A->M));
  CHECKV(GrB_Matrix_ncols(&flat->ncols, A->M));
  CHECKV(GrB_Matrix_nvals(&flat->nvals, A->M));
  
  start = PGGRB_MATRIX_DATA(flat);
  CHECKV(GrB_Matrix_extractTuples(start,
                                  start + flat->nrows,
                                  start + flat->nrows + flat->ncols,
                                  &flat->nvals,
                                  A->M));

  flat->type = GrB_INT64;

  SET_VARSIZE(flat, allocated_size);
}

/* Expand a flat matrix. */
Datum
FN(expand_flat_matrix)(Datum flatdatum,
                   MemoryContext parentcontext) {
  GrB_Info info;
  
  pgGrB_Matrix *A;
  pgGrB_FlatMatrix *flat;
  
  MemoryContext objcxt, oldcxt;
  MemoryContextCallback *ctxcb;
  
  GrB_Index ncols, nrows, nvals;
  GrB_Index *rows, *cols;
  T *vals;
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
  EOH_init_header(&A->hdr, &FN(matrix_methods), objcxt);

  /* Used for debugging checks */
  A->em_magic = matrix_MAGIC;

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
  vals = (T*)(cols + ncols);

  /* Initialize the new matrix */
  CHECKD(GrB_Matrix_new(&A->M,
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
    CHECKD(GrB_Matrix_build(A->M,
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

#undef T
#undef PREFIX
#undef CCAT2
#undef CCAT
#undef FN
