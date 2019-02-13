
/* The same "header template" vector.h is used over and over to
   generate the various type specific functions. */

#define SUFFIX _int64            // suffix for names
#define T int64                  // postgres type
#define GT GrB_INT64             // graphblas vector type
#define GTT GrB_SECOND_INT64     // default duplicate index resolver
#define PGT PG_GETARG_INT64      // how to get value args
#define DGT DatumGetInt64        // datum get type
#define TGD Int64GetDatum        // type get datum
#define FMT(v) "%lu", v           // printf fmt
#include "vector.h"              // "call" template

#define SUFFIX _int32
#define T int32
#define GT GrB_INT32
#define GTT GrB_SECOND_INT32
#define PGT PG_GETARG_INT32
#define DGT DatumGetInt32
#define TGD Int32GetDatum
#define FMT(v) "%i", v
#include "vector.h"

#define SUFFIX _int16
#define T int16
#define GT GrB_INT16
#define GTT GrB_SECOND_INT16
#define PGT PG_GETARG_INT16
#define DGT DatumGetInt16
#define TGD Int16GetDatum
#define FMT(v) "%i", v
#include "vector.h"

#define SUFFIX _float8
#define T float8
#define GT GrB_FP64
#define GTT GrB_SECOND_FP64
#define PGT PG_GETARG_FLOAT8
#define DGT DatumGetFloat8
#define TGD Float8GetDatum
#define FMT(v) "%f", v
#include "vector.h"

#define SUFFIX _float4
#define T float4
#define GT GrB_FP32
#define GTT GrB_SECOND_FP32
#define PGT PG_GETARG_FLOAT4
#define DGT DatumGetFloat4
#define TGD Float4GetDatum
#define FMT(v) "%f", v
#include "vector.h"

#define SUFFIX _bool
#define T bool
#define GT GrB_BOOL
#define GTT GrB_SECOND_BOOL
#define PGT PG_GETARG_BOOL
#define DGT DatumGetBool
#define TGD BoolGetDatum
#define FMT(v) "%s", v ? "t" : "f"
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
