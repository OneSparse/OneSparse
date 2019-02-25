
/* The same "header template" vector.h is used over and over to
   generate the various type specific functions. */

#define SUFFIX _bool
#define PG_TYPE bool
#define GB_TYPE GrB_BOOL
#define GB_DUP GrB_SECOND_BOOL
#define GB_MUL GrB_TIMES_BOOL
#define GB_ADD GrB_PLUS_BOOL
#define GB_EQ  GxB_ISEQ_BOOL
#define GB_NE  GxB_ISNE_BOOL
#define GB_RNG GxB_LOR_LAND_BOOL
#define PG_GET PG_GETARG_BOOL
#define PG_RET PG_RETURN_BOOL
#define PG_DGT DatumGetBool
#define PG_TGD BoolGetDatum
#define PRINT_FMT(v) "%s", v ? "t" : "f"
#include "vector.h"

#define SUFFIX _int8
#define PG_TYPE int8
#define GB_TYPE GrB_INT8
#define GB_DUP GrB_SECOND_INT8
#define GB_MUL GrB_TIMES_INT8
#define GB_ADD GrB_PLUS_INT8
#define GB_EQ  GxB_ISEQ_INT8
#define GB_NE  GxB_ISNE_INT8
#define GB_RNG GxB_PLUS_TIMES_INT8
#define PG_GET PG_GETARG_CHAR
#define PG_RET PG_RETURN_CHAR
#define PG_DGT DatumGetChar
#define PG_TGD CharGetDatum
#define PRINT_FMT(v) "%i", v
#include "vector.h"

#define SUFFIX _int16
#define PG_TYPE int16
#define GB_TYPE GrB_INT16
#define GB_DUP GrB_SECOND_INT16
#define GB_MUL GrB_TIMES_INT16
#define GB_ADD GrB_PLUS_INT16
#define GB_EQ  GxB_ISEQ_INT16
#define GB_NE  GxB_ISNE_INT16
#define GB_RNG GxB_PLUS_TIMES_INT16
#define PG_GET PG_GETARG_INT16
#define PG_RET PG_RETURN_INT16
#define PG_DGT DatumGetInt16
#define PG_TGD Int16GetDatum
#define PRINT_FMT(v) "%i", v
#include "vector.h"

#define SUFFIX _int32
#define PG_TYPE int32
#define GB_TYPE GrB_INT32
#define GB_DUP GrB_SECOND_INT32
#define GB_MUL GrB_TIMES_INT32
#define GB_ADD GrB_PLUS_INT32
#define GB_EQ  GxB_ISEQ_INT32
#define GB_NE  GxB_ISNE_INT32
#define GB_RNG GxB_PLUS_TIMES_INT32
#define PG_GET PG_GETARG_INT32
#define PG_RET PG_RETURN_INT32
#define PG_DGT DatumGetInt32
#define PG_TGD Int32GetDatum
#define PRINT_FMT(v) "%i", v
#include "vector.h"

#define SUFFIX _int64                // suffix for names
#define PG_TYPE int64                // postgres type
#define GB_TYPE GrB_INT64            // graphblas vector type
#define GB_DUP GrB_SECOND_INT64      // default duplicate index resolver
#define GB_MUL GrB_TIMES_INT64       // times bin op
#define GB_ADD GrB_PLUS_INT64         // times bin op
#define GB_EQ  GxB_ISEQ_INT64        // equality operator
#define GB_NE  GxB_ISNE_INT64
#define GB_RNG GxB_PLUS_TIMES_INT64
#define PG_GET PG_GETARG_INT64       // how to get value args
#define PG_RET PG_RETURN_INT64
#define PG_DGT DatumGetInt64         // datum get type
#define PG_TGD Int64GetDatum         // type get datum
#define PRINT_FMT(v) "%lu", v        // printf fmt
#include "vector.h"                  // "call" template

#define SUFFIX _float4
#define PG_TYPE float4
#define GB_TYPE GrB_FP32
#define GB_DUP GrB_SECOND_FP32
#define GB_MUL GrB_TIMES_FP32
#define GB_ADD GrB_PLUS_FP32
#define GB_EQ  GxB_ISEQ_FP32
#define GB_NE  GxB_ISNE_FP32
#define GB_RNG GxB_PLUS_TIMES_FP32
#define PG_GET PG_GETARG_FLOAT4
#define PG_RET PG_RETURN_FLOAT4
#define PG_DGT DatumGetFloat4
#define PG_TGD Float4GetDatum
#define PRINT_FMT(v) "%f", v
#include "vector.h"

#define SUFFIX _float8
#define PG_TYPE float8
#define GB_TYPE GrB_FP64
#define GB_DUP GrB_SECOND_FP64
#define GB_MUL GrB_TIMES_FP64
#define GB_ADD GrB_PLUS_FP64
#define GB_EQ  GxB_ISEQ_FP64
#define GB_NE  GxB_ISNE_FP64
#define GB_RNG GxB_PLUS_TIMES_FP64
#define PG_GET PG_GETARG_FLOAT8
#define PG_RET PG_RETURN_FLOAT8
#define PG_DGT DatumGetFloat8
#define PG_TGD Float8GetDatum
#define PRINT_FMT(v) "%f", v
#include "vector.h"

/* MemoryContextCallback function to free matrices */
static void
context_callback_vector_free(void* a) {
  pgGrB_Vector *A = (pgGrB_Vector *) a;
  GrB_Vector_free(&A->V);
}

/* Construct an empty flat vector. */
pgGrB_FlatVector *
construct_empty_flat_vector(GrB_Index size, GrB_Type type) {
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
  pgGrB_FlatVector *flat;

  if (VARATT_IS_EXTERNAL_EXPANDED(DatumGetPointer(d))) {
    A = VectorGetEOHP(d);
    Assert(A->ev_magic == vector_MAGIC);
    return A;
  }
  flat = (pgGrB_FlatVector*)d;
  TYPE_APPLY(d, flat->type, expand_flat_vector, d, CurrentMemoryContext);
  return VectorGetEOHP(d);
}

Datum
vector_in(PG_FUNCTION_ARGS)
{
  pgGrB_Vector *retval;

  retval = construct_empty_expanded_vector_int64(0, CurrentMemoryContext);

  PGGRB_RETURN_VECTOR(retval);
}

Datum
vector_out(PG_FUNCTION_ARGS)
{
  GrB_Info info;
  GrB_Type typ;
  Datum d;
  pgGrB_Vector *vec = PGGRB_GETARG_VECTOR(0);
  CHECKD(GxB_Vector_type(&typ, vec->V));

  TYPE_APPLY(d, typ, vector_out, vec);
  return d;
}

Datum
vector_ewise_mult(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *A, *B;
  pgGrB_Vector *C = NULL, *mask = NULL;
  GrB_Type type;
  Datum d;
  char *binop_name;
  GrB_BinaryOp binop;

  A = PGGRB_GETARG_VECTOR(0);
  B = PGGRB_GETARG_VECTOR(1);

  binop_name = vector_times_binop(A, B);

  if (PG_NARGS() > 2) {
    C = PG_ARGISNULL(2) ? NULL : PGGRB_GETARG_VECTOR(2);
    mask = PG_ARGISNULL(3) ? NULL : PGGRB_GETARG_VECTOR(3);
    binop_name = PG_ARGISNULL(4) ?
      binop_name : text_to_cstring(PG_GETARG_TEXT_PP(4));
  }

  binop = lookup_binop(binop_name);
  if (binop == NULL)
    elog(ERROR, "unknown binop name %s", binop_name);

  CHECKD(GxB_Vector_type(&type, A->V));

  TYPE_APPLY(d, type, vector_ewise_mult, A, B, C, mask, binop);
  return d;
}

Datum
vector_ewise_add(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *A, *B;
  pgGrB_Vector *C = NULL, *mask = NULL;
  GrB_Type type;
  Datum d;
  char *binop_name;
  GrB_BinaryOp binop;

  A = PGGRB_GETARG_VECTOR(0);
  B = PGGRB_GETARG_VECTOR(1);

  binop_name = vector_times_binop(A, B);

  if (PG_NARGS() > 2) {
    C = PG_ARGISNULL(2) ? NULL : PGGRB_GETARG_VECTOR(2);
    mask = PG_ARGISNULL(3) ? NULL : PGGRB_GETARG_VECTOR(3);
    binop_name = PG_ARGISNULL(4) ?
      binop_name : text_to_cstring(PG_GETARG_TEXT_PP(4));
  }

  binop = lookup_binop(binop_name);
  if (binop == NULL)
    elog(ERROR, "unknown binop name %s", binop_name);

  CHECKD(GxB_Vector_type(&type, A->V));

  TYPE_APPLY(d, type, vector_ewise_add, A, B, C, mask, binop);
  return d;
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


Datum
vector_eq(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *A, *B;
  bool result;

  A = PGGRB_GETARG_VECTOR(0);
  B = PGGRB_GETARG_VECTOR(1);

  CHECKD(visequal(&result, A->V, B->V, NULL));
  PG_RETURN_BOOL(result);
}

Datum
vector_ne(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *A, *B;
  bool result;

  A = PGGRB_GETARG_VECTOR(0);
  B = PGGRB_GETARG_VECTOR(1);

  CHECKD(visequal(&result, A->V, B->V, NULL));
  PG_RETURN_BOOL(!result);
}

Datum
vector_xtract(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *A, *B, *C = NULL;
  GrB_Index size, *indexes;
  GrB_Type type;

  A = PGGRB_GETARG_VECTOR(0);
  B = PGGRB_GETARG_VECTOR(1);

  CHECKD(GxB_Vector_type(&type, A->V));
  CHECKD(GrB_Vector_size(&size, A->V));
  if (C == NULL) {
    TYPE_APPLY(C, type, construct_empty_expanded_vector, size, CurrentMemoryContext);
  }

  TYPE_APPLY(indexes, type, extract_indexes, B, size);

  CHECKD(GrB_Vector_extract(C->V, NULL, NULL, A->V, indexes, size, NULL));
  PGGRB_RETURN_VECTOR(C);
}
