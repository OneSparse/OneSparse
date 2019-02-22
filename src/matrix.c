/* The same "header template" vector.h is used to generate the various
   type specific functions. */

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
#include "matrix.h"

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
#include "matrix.h"

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
#include "matrix.h"

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
#include "matrix.h"

#define SUFFIX _int64                // suffix for names
#define PG_TYPE int64                // postgres type
#define GB_TYPE GrB_INT64            // graphblas vector type
#define GB_DUP GrB_SECOND_INT64      // default duplicate index resolver
#define GB_MUL GrB_TIMES_INT64       // times bin op
#define GB_ADD GrB_PLUS_INT64       // times bin op
#define PG_GET PG_GETARG_INT64       // how to get value args
#define PG_DGT DatumGetInt64         // datum get type
#define PG_TGD Int64GetDatum         // type get datum
#define PRINT_FMT(v) "%lu", v        // printf fmt
#include "matrix.h"                  // "call" template

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
#include "matrix.h"

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
#include "matrix.h"

/* MemoryContextCallback function to free matrices */
static void
context_callback_matrix_free(void* m) {
  pgGrB_Matrix *mat = (pgGrB_Matrix *) m;
  GrB_Matrix_free(&mat->M);
}

/* Construct an empty flat matrix. */
pgGrB_FlatMatrix *
construct_empty_flat_matrix(GrB_Index nrows,
                       GrB_Index ncols,
                       GrB_Type type) {
  pgGrB_FlatMatrix *result;

  result = (pgGrB_FlatMatrix *) palloc0(sizeof(pgGrB_FlatMatrix));
  SET_VARSIZE(result, sizeof(pgGrB_FlatMatrix));
  result->nrows = ncols;
  result->ncols = nrows;
  result->nvals = 0;
  result->type = type;
  return result;
}

/* Helper function to always expanded datum

This is used by PGGRB_GETARG_MATRIX */
pgGrB_Matrix *
DatumGetMatrix(Datum d) {
  pgGrB_Matrix *A;
  pgGrB_FlatMatrix *flat;

  if (VARATT_IS_EXTERNAL_EXPANDED(DatumGetPointer(d))) {
    A = MatrixGetEOHP(d);
    Assert(A->em_magic == matrix_MAGIC);
    return A;
  }
  flat = (pgGrB_FlatMatrix*)d;
  DATUM_TYPE_APPLY(d, flat->type, expand_flat_matrix, d, CurrentMemoryContext);
  return MatrixGetEOHP(d);
}

Datum
matrix_in(PG_FUNCTION_ARGS) {
  pgGrB_Matrix *retval;

  retval = construct_empty_expanded_matrix_int64(0,
                                           0,
                                           CurrentMemoryContext);
  PGGRB_RETURN_MATRIX(retval);
}

Datum
matrix_out(PG_FUNCTION_ARGS)
{
  Datum d;
  GrB_Info info;
  GrB_Type type;
  pgGrB_Matrix *mat = PGGRB_GETARG_MATRIX(0);

  CHECKD(GxB_Matrix_type(&type, mat->M));

  DATUM_TYPE_APPLY(d, type, matrix_out, mat);
  return d;
}

Datum
matrix_nrows(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *mat;
  GrB_Index count;
  mat = (pgGrB_Matrix *) PGGRB_GETARG_MATRIX(0);
  CHECKD(GrB_Matrix_nrows(&count, mat->M));
  return Int64GetDatum(count);
}

Datum
matrix_ncols(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *mat;
  GrB_Index count;
  mat = (pgGrB_Matrix *) PGGRB_GETARG_MATRIX(0);
  CHECKD(GrB_Matrix_ncols(&count, mat->M));
  return Int64GetDatum(count);
}

Datum
matrix_nvals(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *mat;
  GrB_Index count;
  mat = (pgGrB_Matrix *) PGGRB_GETARG_MATRIX(0);
  CHECKD(GrB_Matrix_nvals(&count, mat->M));
  return Int64GetDatum(count);
}

Datum
matrix_eq(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A, *B;
  bool result;

  A = PGGRB_GETARG_MATRIX(0);
  B = PGGRB_GETARG_MATRIX(1);

  CHECKD(isequal(&result, A->M, B->M, NULL));
  PG_RETURN_BOOL(result);
}

Datum
matrix_ne(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A, *B;
  bool result;

  A = PGGRB_GETARG_MATRIX(0);
  B = PGGRB_GETARG_MATRIX(1);

  CHECKD(isequal(&result, A->M, B->M, NULL));
  PG_RETURN_BOOL(!result);
}

Datum
matrix_ewise_mult(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A, *B, *C = NULL, *mask = NULL;
  GrB_Type type;
  Datum d;
  char *binop_name;
  GrB_BinaryOp binop;
  
  A = PGGRB_GETARG_MATRIX(0);
  B = PGGRB_GETARG_MATRIX(1);

  binop_name = matrix_times_binop(A, B);
  
  if (PG_NARGS() > 2)
    C = PG_ARGISNULL(2) ? NULL : PGGRB_GETARG_MATRIX(2);
  if (PG_NARGS() > 3)
    mask = PG_ARGISNULL(3) ? NULL : PGGRB_GETARG_MATRIX(3);
  if (PG_NARGS() > 4)
    binop_name = PG_ARGISNULL(4) ?
      binop_name : text_to_cstring(PG_GETARG_TEXT_PP(4));

  binop = lookup_binop(binop_name);
  if (binop == NULL)
    elog(ERROR, "unknown binop name %s", binop_name);

  CHECKD(GxB_Matrix_type(&type, A->M));

  DATUM_TYPE_APPLY(d, type, matrix_ewise_mult, A, B, C, mask, binop);
  return d;
}

Datum
matrix_ewise_add(PG_FUNCTION_ARGS) {
  GrB_Info info;
  pgGrB_Matrix *A, *B, *C = NULL, *mask = NULL;
  GrB_Type type;
  Datum d;
  char *binop_name;
  GrB_BinaryOp binop;
  
  A = PGGRB_GETARG_MATRIX(0);
  B = PGGRB_GETARG_MATRIX(1);

  binop_name = matrix_plus_binop(A, B);
  
  if (PG_NARGS() > 2)
    C = PG_ARGISNULL(2) ? NULL : PGGRB_GETARG_MATRIX(2);
  if (PG_NARGS() > 3)
    mask = PG_ARGISNULL(3) ? NULL : PGGRB_GETARG_MATRIX(3);
  if (PG_NARGS() > 4)
    binop_name = PG_ARGISNULL(4) ?
      binop_name : text_to_cstring(PG_GETARG_TEXT_PP(4));

  binop = lookup_binop(binop_name);
  if (binop == NULL)
    elog(ERROR, "unknown binop name %s", binop_name);

  CHECKD(GxB_Matrix_type(&type, A->M));

  DATUM_TYPE_APPLY(d, type, matrix_ewise_add, A, B, C, mask, binop);
  return d;
}

Datum
mxm(PG_FUNCTION_ARGS) {
  pgGrB_Matrix *A, *B, *C = NULL, *mask = NULL;
  GrB_Type type;
  Datum d;
  char *semiring_name;
  GrB_Semiring semiring;

  A = PGGRB_GETARG_MATRIX(0);
  B = PGGRB_GETARG_MATRIX(1);

  semiring_name = mxm_semiring(A, B);
  
  if (PG_NARGS() > 2)
    C = PG_ARGISNULL(2) ? NULL : PGGRB_GETARG_MATRIX(2);
  if (PG_NARGS() > 3)
    mask = PG_ARGISNULL(3) ? NULL : PGGRB_GETARG_MATRIX(3);
  if (PG_NARGS() > 4)
    semiring_name = PG_ARGISNULL(4) ? 
      semiring_name : 
      text_to_cstring(PG_GETARG_TEXT_PP(4));

  semiring = lookup_semiring(semiring_name);
  if (semiring == NULL)
    elog(ERROR, "unknown semiring name %s", semiring_name);

  type = mxm_type(A, B);

  DATUM_TYPE_APPLY(d, type, mxm, A, B, C, mask, semiring);
  return d;
}

Datum
mxv(PG_FUNCTION_ARGS) {
  pgGrB_Matrix *A;
  pgGrB_Vector *B, *C = NULL, *mask = NULL;
  GrB_Type type;
  Datum d;
  char *semiring_name;
  GrB_Semiring semiring;

  A = (pgGrB_Matrix *) PGGRB_GETARG_MATRIX(0);
  B = (pgGrB_Vector *) PGGRB_GETARG_VECTOR(1);

  semiring_name = mxv_semiring(A, B);
  
  if (PG_NARGS() > 2)
    C = PG_ARGISNULL(2) ? NULL : PGGRB_GETARG_VECTOR(2);
  if (PG_NARGS() > 3)
    mask = PG_ARGISNULL(3) ? NULL : PGGRB_GETARG_VECTOR(3);
  if (PG_NARGS() > 4)
    semiring_name = PG_ARGISNULL(4) ? 
      semiring_name : 
      text_to_cstring(PG_GETARG_TEXT_PP(4));
  
  semiring = lookup_semiring(semiring_name);
  if (semiring == NULL)
    elog(ERROR, "unknown semiring name %s", semiring_name);

  type = mxv_type(A, B);

  DATUM_TYPE_APPLY(d, type, mxv, A, B, C, mask, semiring);
  return d;
}

Datum
vxm(PG_FUNCTION_ARGS) {
  pgGrB_Matrix *B;
  pgGrB_Vector *A, *C = NULL, *mask = NULL;
  GrB_Type type;
  Datum d;
  char *semiring_name;
  GrB_Semiring semiring;

  A = (pgGrB_Vector *) PGGRB_GETARG_VECTOR(0);
  B = (pgGrB_Matrix *) PGGRB_GETARG_MATRIX(1);

  semiring_name = vxm_semiring(A, B);

  if (PG_NARGS() > 2)
    C = PG_ARGISNULL(2) ? NULL : PGGRB_GETARG_VECTOR(2);
  if (PG_NARGS() > 3)
    mask = PG_ARGISNULL(3) ? NULL : PGGRB_GETARG_VECTOR(3);
  if (PG_NARGS() > 4)
    semiring_name = PG_ARGISNULL(4) ? semiring_name : PG_GETARG_CSTRING(4);

  semiring = lookup_semiring(semiring_name);
  if (semiring == NULL)
    elog(ERROR, "unknown semiring name %s", semiring_name);

  type = vxm_type(A, B);

  DATUM_TYPE_APPLY(d, type, vxm, A, B, C, mask, semiring);
  return d;
}

/* Datum */
/* matrix_new(PG_FUNCTION_ARGS) { */
/*   GrB_Info info; */
/*   pgGrB_Matrix *result; */
/*   ExpandedArrayHeader *I, *J, *V; */
/*   GrB_Type type; */
/*   Datum d; */

/*   I = PG_GETARG_EXPANDED_ARRAY(0); */
/*   J = PG_GETARG_EXPANDED_ARRAY(1); */
/*   V = PG_GETARG_EXPANDED_ARRAY(1); */
  
/*   d = DATUM_TYPE_APPLY(type, matrix_new, I, J, V); */
/*   if (d == (Datum)0) */
/*     elog(ERROR, "Unknown graphblas type"); */
/*   return d; */
/* } */

