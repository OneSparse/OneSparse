
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
  OS_Vector *A = (OS_Vector *) a;
  GrB_Vector_free(&A->V);
}

/* Construct an empty flat vector. */
OS_FlatVector *
construct_empty_flat_vector(GrB_Index size, GrB_Type type) {
  OS_FlatVector *result;

  result = (OS_FlatVector *) palloc0(sizeof(OS_FlatVector));
  SET_VARSIZE(result, sizeof(OS_FlatVector));
  result->size = size;
  result->nvals = 0;
  result->type = type;
  return result;
}

/* Helper function to always expanded datum

This is used by OS_GETARG_VECTOR */
OS_Vector *
DatumGetVector(Datum d) {
  OS_Vector *A;
  OS_FlatVector *flat;

  if (VARATT_IS_EXTERNAL_EXPANDED(DatumGetPointer(d))) {
    A = VectorGetEOHP(d);
    Assert(A->ev_magic == vector_MAGIC);
    return A;
  }
  flat = (OS_FlatVector*)PG_DETOAST_DATUM(d);
  TYPE_APPLY(d, flat->type, expand_flat_vector, flat, CurrentMemoryContext);
  return VectorGetEOHP(d);
}

Datum
vector_in(PG_FUNCTION_ARGS)
{
  OS_FlatVector *flat;
  char *input;
  size_t len;
  int bc;
  Datum d;

  input = PG_GETARG_CSTRING(0);
  len = strlen(input);
  bc = (len) / 2 + VARHDRSZ;
  flat = palloc(bc);
  hex_decode(input, len, (char*)flat);
  TYPE_APPLY(d, flat->type, expand_flat_vector, flat, CurrentMemoryContext);
  return d;
}

Datum
vector_out(PG_FUNCTION_ARGS)
{
  Size size;
  char *rp, *result, *buf;
  OS_Vector *A = OS_GETARG_VECTOR(0);
  size = EOH_get_flat_size(&A->hdr);
  buf = palloc(size);
  EOH_flatten_into(&A->hdr, buf, size);
  rp = result = palloc((size * 2) + 1);
  rp += hex_encode(buf, size, rp);
  *rp = '\0';
  PG_RETURN_CSTRING(result);
}

Datum
vector_ewise_mult(PG_FUNCTION_ARGS) {
  OS_Vector *A, *B;
  OS_Vector *C = NULL, *mask = NULL;
  Datum d;
  char *binop_name, *accum_name;
  GrB_BinaryOp binop = NULL, accum = NULL;
  GrB_Descriptor desc = NULL;
  GrB_Info info;

  A = OS_GETARG_VECTOR(0);
  B = OS_GETARG_VECTOR(1);

  accum_name = NULL;
  binop_name = vector_times_binop(A, B);

  if (PG_NARGS() > 2) {
    C = PG_ARGISNULL(2) ? NULL : OS_GETARG_VECTOR(2);
   mask = PG_ARGISNULL(3) ? NULL : OS_GETARG_VECTOR(3);
    binop_name = PG_ARGISNULL(4) ?
      binop_name : text_to_cstring(PG_GETARG_TEXT_PP(4));
    accum_name = PG_ARGISNULL(5) ?
      accum_name : text_to_cstring(PG_GETARG_TEXT_PP(5));
    if (PG_NARGS() > 6) {
      GET_DESCRIPTOR(6, desc);
    }
  }

  binop = lookup_binop(binop_name);
  if (binop == NULL)
    elog(ERROR, "unknown binop name %s", binop_name);
  if (accum_name != NULL)
    accum = lookup_binop(accum_name);


  TYPE_APPLY(d, A->type, vector_ewise_mult, A, B, C, mask, binop, accum, desc);
  return d;
}

Datum
vector_ewise_add(PG_FUNCTION_ARGS) {
  OS_Vector *A, *B;
  OS_Vector *C = NULL, *mask = NULL;
  Datum d;
  char *binop_name, *accum_name;
  GrB_BinaryOp binop = NULL, accum = NULL;
  GrB_Descriptor desc = NULL;
  GrB_Info info;

  A = OS_GETARG_VECTOR(0);
  B = OS_GETARG_VECTOR(1);

  accum_name = NULL;
  binop_name = vector_times_binop(A, B);

  if (PG_NARGS() > 2) {
    C = PG_ARGISNULL(2) ? NULL : OS_GETARG_VECTOR(2);
    mask = PG_ARGISNULL(3) ? NULL : OS_GETARG_VECTOR(3);
    binop_name = PG_ARGISNULL(4) ?
      binop_name : text_to_cstring(PG_GETARG_TEXT_PP(4));
    accum_name = PG_ARGISNULL(5) ?
      accum_name : text_to_cstring(PG_GETARG_TEXT_PP(5));
    if (PG_NARGS() > 6) {
      GET_DESCRIPTOR(6, desc);
    }
  }

  binop = lookup_binop(binop_name);
  if (binop == NULL)
    elog(ERROR, "unknown binop name %s", binop_name);
  if (accum_name != NULL)
    accum = lookup_binop(accum_name);


  TYPE_APPLY(d, A->type, vector_ewise_add, A, B, C, mask, binop, accum, desc);
  return d;
}

Datum
vector_nvals(PG_FUNCTION_ARGS) {
  GrB_Info info;
  OS_Vector *vec;
  GrB_Index nvals;
  vec = OS_GETARG_VECTOR(0);
  CHECKD(GrB_Vector_nvals(&nvals, vec->V), vec->V);
  return Int64GetDatum(nvals);
}

Datum
vector_size(PG_FUNCTION_ARGS) {
  GrB_Info info;
  OS_Vector *vec;
  GrB_Index size;
  vec = OS_GETARG_VECTOR(0);
  CHECKD(GrB_Vector_size(&size, vec->V), vec->V);
  return Int64GetDatum(size);
}


Datum
vector_eq(PG_FUNCTION_ARGS) {
  GrB_Info info;
  OS_Vector *A, *B;
  bool result;

  A = OS_GETARG_VECTOR(0);
  B = OS_GETARG_VECTOR(1);

  result = true;
  PG_RETURN_BOOL(result);
}

Datum
vector_ne(PG_FUNCTION_ARGS) {
  GrB_Info info;
  OS_Vector *A, *B;
  bool result;

  A = OS_GETARG_VECTOR(0);
  B = OS_GETARG_VECTOR(1);

  result = true;
  PG_RETURN_BOOL(!result);
}

Datum
vector_xtract(PG_FUNCTION_ARGS) {
  GrB_Info info;
  OS_Vector *A, *B, *C = NULL;
  GrB_Index size, *indexes;

  A = OS_GETARG_VECTOR(0);
  B = OS_GETARG_VECTOR(1);
  C = PG_ARGISNULL(2) ? NULL : OS_GETARG_VECTOR(2);

  CHECKD(GrB_Vector_size(&size, A->V), A->V);
  if (C == NULL) {
    TYPE_APPLY(C, A->type, construct_empty_expanded_vector, size, CurrentMemoryContext);
  }

  TYPE_APPLY(indexes, A->type, extract_indexes, B, size);

  CHECKD(GrB_Vector_extract(C->V, NULL, NULL, A->V, indexes, size, NULL), A->V);
  OS_RETURN_VECTOR(C);
}

Datum
vector_print(PG_FUNCTION_ARGS) {
  OS_Vector *A;
  char *result, *buf;
  size_t size;
  FILE *fp;
  int level;
  A = OS_GETARG_VECTOR(0);
  level = PG_GETARG_INT32(1);
  
  fp = open_memstream(&buf, &size);
  if (fp == NULL)
    elog(ERROR, "unable to open memstream for vector_print");
  GxB_fprint(A->V, level, fp);
  fflush(fp);
  result = palloc(size + 1);
  memcpy(result, buf, size+1);
  free(buf);
  PG_RETURN_TEXT_P(cstring_to_text_with_len(result, size+1));
}
