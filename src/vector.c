
static void
context_callback_vector_free(void* v) {
  pgGrB_Vector *vec = (pgGrB_Vector *) v;
  GrB_Vector_free(&vec->V);
}

