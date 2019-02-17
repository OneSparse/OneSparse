
/* The same "header template" vector.h is used over and over to
   generate the various type specific functions. */

#define SUFFIX _bool
#define PG_TYPE bool
#define GB_TYPE GrB_BOOL
#define GB_DUP GrB_SECOND_BOOL
#define GB_MUL GrB_TIMES_BOOL
#define GB_ADD GrB_PLUS_BOOL
#define PG_GET PG_GETARG_BOOL
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
#define PG_GET PG_GETARG_CHAR
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
#define PG_GET PG_GETARG_INT16
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
#define PG_GET PG_GETARG_INT32
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
#define PG_GET PG_GETARG_INT64       // how to get value args
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
#define PG_GET PG_GETARG_FLOAT4
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
#define PG_GET PG_GETARG_FLOAT8
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
  d = DATUM_TYPE_APPLY(flat->type, expand_flat_vector, d, CurrentMemoryContext);
  if (d == (Datum)0)
    elog(ERROR, "Unknown graphblas type");
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

  d = DATUM_TYPE_APPLY(typ, vector_out, vec);
  if (d == (Datum)0)
    elog(ERROR, "Unknown graphblas type");
  return d;
}

Datum
vector_ewise_mult(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *A, *B;
  GrB_Type type;
  Datum d;
  A = PGGRB_GETARG_VECTOR(0);
  B = PGGRB_GETARG_VECTOR(1);
  CHECKD(GxB_Vector_type(&type, A->V));

  d = DATUM_TYPE_APPLY(type, vector_ewise_mult, A, B);
  if (d == (Datum)0)
    elog(ERROR, "Unknown graphblas type");
  return d;
}

Datum
vector_ewise_add(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Vector *A, *B;
  GrB_Type type;
  Datum d;
  A = PGGRB_GETARG_VECTOR(0);
  B = PGGRB_GETARG_VECTOR(1);
  CHECKD(GxB_Vector_type(&type, A->V));

  d = DATUM_TYPE_APPLY(type, vector_ewise_add, A, B);
  if (d == (Datum)0)
    elog(ERROR, "Unknown graphblas type");
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
  pgGrB_Vector *A, *B, *C;
  GrB_Index asize, bsize, anvals, bnvals;
  GrB_Type atype, btype;
  bool result = 0;

  VECTOR_BINOP_PREAMBLE();

  CHECKD(GxB_Vector_type(&atype, A->V));
  CHECKD(GxB_Vector_type(&btype, B->V));

  if (atype != btype)
    PG_RETURN_BOOL(result);

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
  GrB_Type atype, btype;
  GrB_Index asize, bsize, anvals, bnvals;
  bool result = 1;

  VECTOR_BINOP_PREAMBLE();
  CHECKD(GxB_Vector_type(&atype, A->V));
  CHECKD(GxB_Vector_type(&btype, B->V));

  if (atype != btype)
    PG_RETURN_BOOL(result);

  CHECKD(GrB_Vector_size(&asize, A->V));
  CHECKD(GrB_Vector_size(&bsize, B->V));

  if (asize != bsize)
    PG_RETURN_BOOL(result);

  CHECKD(GrB_Vector_nvals(&anvals, A->V));
  CHECKD(GrB_Vector_nvals(&bnvals, B->V));

  if (anvals != bnvals)
    PG_RETURN_BOOL(result);

  CHECKD(GrB_Vector_new (&(C->V), GrB_BOOL, asize));

  CHECKD(GrB_eWiseMult(C->V, NULL, NULL, GxB_ISNE_INT64, A->V, B->V, NULL));
  CHECKD(GrB_Vector_reduce_BOOL(&result, NULL, GxB_LAND_BOOL_MONOID, C->V, NULL));
  PG_RETURN_BOOL(result);
}
