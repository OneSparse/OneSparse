#ifndef PGGRAPHBLAS_H
#define PGGRAPHBLAS_H

#include "suitesparse/GraphBLAS.h"
#include "postgres.h"
#include "utils/builtins.h"
#include "libpq/pqformat.h"
#include "funcapi.h"
#include "access/htup_details.h"
#include "utils/array.h"
#include "utils/arrayaccess.h"
#include "catalog/pg_type_d.h"
#include "catalog/pg_type.h"
#include "utils/lsyscache.h"
#include "nodes/pg_list.h"
#include "utils/varlena.h"

#define CCAT2(x, y) x ## y
#define CCAT(x, y) CCAT2(x, y)
#define FN(x) CCAT(x, SUFFIX)

#define TYPE_APPLY(D, T, F, ...) do {       \
  if ((T) == GrB_INT64)                           \
    D = F ## _int64(__VA_ARGS__);                 \
  else if ((T)  == GrB_INT32)                     \
    D = F ## _int32(__VA_ARGS__);                 \
  else if ((T)  == GrB_INT16)                     \
    D = F ## _int16(__VA_ARGS__);                 \
  else if ((T)  == GrB_INT8)                      \
    D = F ## _int8(__VA_ARGS__);                  \
  else if ((T)  == GrB_BOOL)                      \
    D = F ## _bool(__VA_ARGS__);                  \
  else if ((T)  == GrB_FP64)                      \
    D = F ## _float8(__VA_ARGS__);                \
  else if((T)  == GrB_FP32)                       \
    D = F ## _float4(__VA_ARGS__);                \
  else                                            \
    elog(ERROR, "unsupported type");             \
  } while(0)

#define DEFAULT_SEMIRING(T)                       \
  (T) == GrB_INT64?                               \
    "PLUS_TIMES_INT64" :                          \
    (T)  == GrB_INT32?                            \
    "PLUS_TIMES_INT32" :                          \
    (T)  == GrB_INT16?                            \
    "PLUS_TIMES_INT16" :                          \
    (T)  == GrB_INT8?                             \
    "PLUS_TIMES_INT8" :                           \
    (T)  == GrB_BOOL?                             \
    "LOR_LAND_BOOL" :                             \
    (T)  == GrB_FP64?                             \
    "PLUS_TIMES_FP64" :                           \
    (T)  == GrB_FP32?                             \
    "PLUS_TIMES_FP32" :                           \
    "unknown"

#define DEFAULT_TIMES_BINOP(T)                    \
  (T) == GrB_INT64?                               \
    "TIMES_INT64" :                               \
    (T)  == GrB_INT32?                            \
    "TIMES_INT32" :                               \
    (T)  == GrB_INT16?                            \
    "TIMES_INT16" :                               \
    (T)  == GrB_INT8?                             \
    "TIMES_INT8" :                               \
    (T)  == GrB_BOOL?                             \
    "TIMES_BOOL" :                                 \
    (T)  == GrB_FP64?                             \
    "TIMES_FP64" :                                \
    (T)  == GrB_FP32?                             \
    "TIMES_FP32" :                                \
    "unknown"

#define DEFAULT_PLUS_BINOP(T)                     \
  (T) == GrB_INT64?                               \
    "PLUS_INT64" :                                \
    (T)  == GrB_INT32?                            \
    "PLUS_INT32" :                                \
    (T)  == GrB_INT16?                            \
    "PLUS_INT16" :                                \
    (T)  == GrB_INT8?                             \
    "PLUS_INT8" :                                \
    (T)  == GrB_BOOL?                             \
    "PLUS_BOOL" :                                 \
    (T)  == GrB_FP64?                             \
    "PLUS_FP64" :                                 \
    (T)  == GrB_FP32?                             \
    "PLUS_FP32" :                                 \
    "unknown"

/* dumb debug helper */
#define elogn(s) elog(NOTICE, "%s", (s))
#define elogn1(s, v) elog(NOTICE, "%s: %lu", (s), (v))

/* GraphBLAS API checking macros */

#define CHECKD(method, obj)                                 \
  {                                                         \
    info = method ;                                         \
    if (! (info == GrB_SUCCESS || info == GrB_NO_VALUE))    \
    {                                                       \
        const char *emsg;                                   \
        GrB_error(&emsg, obj);                              \
        elog(ERROR, "%s", emsg);                            \
        return (Datum) 0;                                   \
    }                                                       \
  }                                                         \

#define CHECKV(method, obj)                                 \
{                                                           \
    info = method ;                                         \
    if (! (info == GrB_SUCCESS || info == GrB_NO_VALUE))    \
    {                                                       \
        const char *emsg;                                   \
        GrB_error(&emsg, obj);                              \
        elog(ERROR, "%s", emsg);                            \
      return;                                               \
    }                                                       \
}                                                           \

GrB_Info isequal_type
(
    bool *result,
    GrB_Matrix A,
    GrB_Matrix B,
    GrB_BinaryOp op
 );

GrB_Info isequal
(
    bool *result,
    GrB_Matrix A,
    GrB_Matrix B,
    GrB_BinaryOp userop
 );

GrB_Info visequal_type
(
    bool *result,
    GrB_Vector A,
    GrB_Vector B,
    GrB_BinaryOp op
 );

GrB_Info visequal
(
    bool *result,
    GrB_Vector A,
    GrB_Vector B,
    GrB_BinaryOp userop
 );
/* Flattened representation of matrix, used to store to disk.

*/
typedef struct pgGrB_FlatMatrix {
  int32 vl_len_;
  GrB_Index nrows;
  GrB_Index ncols;
  GrB_Index nvals;
  GrB_Type type;
} pgGrB_FlatMatrix;

/* ID for debugging crosschecks */
#define matrix_MAGIC 689276813

/* Expanded representation of matrix.

When loaded from storage, the flattened representation is used to
build the matrix with GrB_Matrix_build.
*/
typedef struct pgGrB_Matrix  {
  ExpandedObjectHeader hdr;
  int em_magic;
  GrB_Type type;
  GrB_Matrix M;
  Size flat_size;
} pgGrB_Matrix;

/* helper to turn types into names. */
char* grb_type_to_name(GrB_Type t);
GrB_Type grb_name_to_type(char* n);

/* Callback function for freeing matrices. */
static void
context_callback_matrix_free(void*);

/* Helper function that either detoasts or expands matrices. */
pgGrB_Matrix* DatumGetMatrix(Datum d);

/* Helper function to create an empty flattened matrix. */
pgGrB_FlatMatrix *
construct_empty_flat_matrix(GrB_Index nrows,
                            GrB_Index ncols,
                            GrB_Type type);

/* Helper to detoast and expand matrix arguments */
#define PGGRB_GETARG_MATRIX(n)  DatumGetMatrix(PG_GETARG_DATUM(n))

/* Helper to return Expanded Object Header Pointer from matrix. */
#define PGGRB_RETURN_MATRIX(A) return EOHPGetRWDatum(&(A)->hdr)

/* Helper to compute flat matrix header size */
#define PGGRB_MATRIX_OVERHEAD() MAXALIGN(sizeof(pgGrB_FlatMatrix))

/* pointer to beginning of matrix data. */
#define PGGRB_MATRIX_DATA(a) ((GrB_Index *)(((char *) (a)) + PGGRB_MATRIX_OVERHEAD()))

/* Public API functions */

PG_FUNCTION_INFO_V1(matrix);
PG_FUNCTION_INFO_V1(matrix_in);
PG_FUNCTION_INFO_V1(matrix_out);

PG_FUNCTION_INFO_V1(matrix_ncols);
PG_FUNCTION_INFO_V1(matrix_nrows);
PG_FUNCTION_INFO_V1(matrix_nvals);

PG_FUNCTION_INFO_V1(mxm);
PG_FUNCTION_INFO_V1(mxv);
PG_FUNCTION_INFO_V1(vxm);
PG_FUNCTION_INFO_V1(matrix_kron);
PG_FUNCTION_INFO_V1(matrix_xtract);
PG_FUNCTION_INFO_V1(matrix_print);
PG_FUNCTION_INFO_V1(matrix_mmwrite);
PG_FUNCTION_INFO_V1(matrix_mmread);

PG_FUNCTION_INFO_V1(matrix_eq);
PG_FUNCTION_INFO_V1(matrix_ne);

PG_FUNCTION_INFO_V1(matrix_ewise_mult);
PG_FUNCTION_INFO_V1(matrix_ewise_add);
PG_FUNCTION_INFO_V1(matrix_reduce_vector);
PG_FUNCTION_INFO_V1(matrix_transpose);
PG_FUNCTION_INFO_V1(matrix_assign_matrix);
PG_FUNCTION_INFO_V1(matrix_bfs);

/* Vectors */

/* Flattened representation of vector, used to store to disk.

Actual array data is appended and for the moment cannot exceed 1GB.
*/
typedef struct pgGrB_FlatVector {
  int32 vl_len_;
  GrB_Index size;
  GrB_Index nvals;
  GrB_Type type;
} pgGrB_FlatVector;

/* ID for debugging crosschecks */
#define vector_MAGIC 681276813

/* Expanded representation of vector.

When loaded from storage, the flattened representation is used to
build the vector with GrB_Vector_build.
*/
typedef struct pgGrB_Vector  {
  ExpandedObjectHeader hdr;
  int ev_magic;
  GrB_Type type;
  GrB_Vector V;
  Size flat_size;
} pgGrB_Vector;

static void context_callback_vector_free(void*);

/* Helper function that either detoasts or expands vectors. */
pgGrB_Vector* DatumGetVector(Datum d);

/* Helper function to create an empty flattened vector. */
pgGrB_FlatVector* construct_empty_flat_vector(GrB_Index size, GrB_Type type);

GrB_Type mxm_type(pgGrB_Matrix *left, pgGrB_Matrix *right);
GrB_Type mxv_type(pgGrB_Matrix *left, pgGrB_Vector *right);
GrB_Type vxm_type(pgGrB_Vector *left, pgGrB_Matrix *right);

/* function to choose implicit semirings for overloaded operators */
char* mxm_semiring(pgGrB_Matrix *left, pgGrB_Matrix *right);
char* mxv_semiring(pgGrB_Matrix *left, pgGrB_Vector *right);
char* vxm_semiring(pgGrB_Vector *left, pgGrB_Matrix *right);

char* matrix_times_binop(pgGrB_Matrix *left, pgGrB_Matrix *right);
char* matrix_plus_binop(pgGrB_Matrix *left, pgGrB_Matrix *right);

char* vector_times_binop(pgGrB_Vector *left, pgGrB_Vector *right);
char* vector_plus_binop(pgGrB_Vector *left, pgGrB_Vector *right);

GrB_Semiring lookup_semiring(char *name);
GrB_BinaryOp lookup_binop(char *name);

#define VectorGetEOHP(d) (pgGrB_Vector *) DatumGetEOHP(d)
#define MatrixGetEOHP(d) (pgGrB_Matrix *) DatumGetEOHP(d);

/* Helper to detoast and expand vector arguments */
#define PGGRB_GETARG_VECTOR(n)  DatumGetVector(PG_GETARG_DATUM(n))

/* Helper to return Expanded Object Header Pointer from vector. */
#define PGGRB_RETURN_VECTOR(A) return EOHPGetRWDatum(&(A)->hdr)

/* Helper to compute flat vector header size */
#define PGGRB_VECTOR_OVERHEAD() MAXALIGN(sizeof(pgGrB_FlatVector))

#define PGGRB_VECTOR_DATA(a) ((GrB_Index *)(((char *) (a)) + PGGRB_VECTOR_OVERHEAD()))

PG_FUNCTION_INFO_V1(vector_in);
PG_FUNCTION_INFO_V1(vector_out);

PG_FUNCTION_INFO_V1(vector_ewise_mult);
PG_FUNCTION_INFO_V1(vector_ewise_add);

PG_FUNCTION_INFO_V1(vector_eq);
PG_FUNCTION_INFO_V1(vector_ne);

PG_FUNCTION_INFO_V1(vector_nvals);
PG_FUNCTION_INFO_V1(vector_size);

PG_FUNCTION_INFO_V1(vector_xtract);
PG_FUNCTION_INFO_V1(vector_print);

typedef struct pgGrB_Semiring  {
  char name[255];
  GrB_Semiring R;
} pgGrB_Semiring;

typedef struct pgGrB_BinaryOp  {
  char name[255];
  GrB_BinaryOp B;
} pgGrB_BinaryOp;

typedef struct pgGrB_UnaryOp  {
  char name[255];
  GrB_UnaryOp U;
} pgGrB_UnaryOp;

#define SEMIRING(N, PLS, TMS, TYP) do {                             \
    strlcpy(semirings[(N)].name, #PLS #TMS #TYP, 255);              \
    semirings[(N)].R = GxB_ ## PLS ## TMS ## TYP;                   \
    } while (0)

#define SEMIRING_TYPES(N, PLS, TMS) do {          \
    SEMIRING (N, PLS, TMS, INT8);                 \
    SEMIRING(N+1, PLS, TMS, UINT8);               \
    SEMIRING(N+2, PLS, TMS, INT16);               \
    SEMIRING(N+3, PLS, TMS, UINT16);              \
    SEMIRING(N+4, PLS, TMS, INT32);               \
    SEMIRING(N+5, PLS, TMS, UINT32);              \
    SEMIRING(N+6, PLS, TMS, INT64);               \
    SEMIRING(N+7, PLS, TMS, UINT64);              \
    SEMIRING(N+8, PLS, TMS, FP32);                \
    SEMIRING(N+9, PLS, TMS, FP64);                \
    } while(0)

#define SEMIRING_PURE_BOOL(N, PLS) do {             \
  SEMIRING(N, PLS, FIRST_, BOOL);                   \
    SEMIRING(N+1, PLS, SECOND_, BOOL);              \
    SEMIRING(N+2, PLS, LOR_, BOOL);                 \
    SEMIRING(N+3, PLS, LAND_, BOOL);                \
    SEMIRING(N+4, PLS, LXOR_, BOOL);                \
    SEMIRING(N+5, PLS, EQ_, BOOL);                  \
    SEMIRING(N+6, PLS, GT_, BOOL);                  \
    SEMIRING(N+7, PLS, LT_, BOOL);                  \
    SEMIRING(N+8, PLS, GE_, BOOL);                  \
    SEMIRING(N+9, PLS, LE_, BOOL);                  \
  } while(0)

#define SEMIRING_GROUP(N, TMS) do {                    \
    SEMIRING_TYPES((N), MIN_, TMS);                    \
    SEMIRING_TYPES((N)+10, MAX_, TMS);                 \
    SEMIRING_TYPES((N)+20, PLUS_, TMS);                \
    SEMIRING_TYPES((N)+30, TIMES_, TMS);               \
  } while(0)

#define SEMIRING_BOOL_GROUP(N, TMS) do {                 \
    SEMIRING_TYPES((N), LOR_, TMS);                      \
    SEMIRING_TYPES((N)+10, LAND_, TMS);                  \
    SEMIRING_TYPES((N)+20, LXOR_, TMS);                  \
    SEMIRING_TYPES((N)+30, EQ_, TMS);                    \
  } while(0)

#define SEMIRING_PURE_BOOL_GROUP(N) do {                \
    SEMIRING_PURE_BOOL((N), LOR_);                      \
    SEMIRING_PURE_BOOL((N)+10, LAND_);                  \
    SEMIRING_PURE_BOOL((N)+20, LXOR_);                  \
    SEMIRING_PURE_BOOL((N)+30, EQ_);                    \
  } while(0)

#define BINOP(N, PRE, OP, TYP) do {                                 \
    strlcpy(binops[(N)].name, #OP #TYP, 255);                       \
    binops[(N)].B = PRE ## OP ## TYP;                               \
  } while (0)

#define BINOP_TYPES(N, PRE, OP) do {          \
    BINOP (N, PRE, OP, INT8);                 \
    BINOP(N+1, PRE, OP, UINT8);               \
    BINOP(N+2, PRE, OP, INT16);               \
    BINOP(N+3, PRE, OP, UINT16);              \
    BINOP(N+4, PRE, OP, INT32);               \
    BINOP(N+5, PRE, OP, UINT32);              \
    BINOP(N+6, PRE, OP, INT64);               \
    BINOP(N+7, PRE, OP, UINT64);              \
    BINOP(N+8, PRE, OP, FP32);                \
    BINOP(N+9, PRE, OP, FP64);                \
    BINOP (N+10, PRE, OP, BOOL);              \
    } while(0)


#define UOP(N, PRE, OP, TYP) do {                                 \
    strlcpy(uops[(N)].name, #OP #TYP, 255);                       \
    uops[(N)].U = PRE ## OP ## TYP;                               \
  } while (0)

#define UOP_TYPES(N, PRE, OP) do {          \
    UOP (N, PRE, OP, INT8);                 \
    UOP(N+1, PRE, OP, UINT8);               \
    UOP(N+2, PRE, OP, INT16);               \
    UOP(N+3, PRE, OP, UINT16);              \
    UOP(N+4, PRE, OP, INT32);               \
    UOP(N+5, PRE, OP, UINT32);              \
    UOP(N+6, PRE, OP, INT64);               \
    UOP(N+7, PRE, OP, UINT64);              \
    UOP(N+8, PRE, OP, FP32);                \
    UOP(N+9, PRE, OP, FP64);                \
    } while(0)

#define RETURN_TYPECHCK1(A, B, OP, TYP)     \
  if ((A) == GrB_##TYP || (B) == GrB_##TYP) \
    return #OP "_" #TYP

#define RETURN_TYPECHCK2(A, B, TYP)     \
  if ((A) == GrB_##TYP || (B) == GrB_##TYP) \
    return GrB_##TYP

#define GETARG_DESCRIPTOR_VAL(ARG, DESC, FIELD, VAL) do {   \
    char *desc_val;                                                     \
    if (!PG_ARGISNULL(ARG)) {                                           \
      (desc_val) = text_to_cstring(PG_GETARG_TEXT_PP(ARG));             \
      if (strcmp((desc_val), "default") == 0) {                         \
          CHECKD(GrB_Descriptor_set((DESC), GrB_##FIELD, GxB_DEFAULT), DESC); \
      } else if (strcmp((desc_val), "replace") == 0) {                  \
          CHECKD(GrB_Descriptor_set((DESC), GrB_##FIELD, GrB_##VAL), DESC);  \
      } else if (strcmp((desc_val), "scmp") == 0) {                     \
          CHECKD(GrB_Descriptor_set((DESC), GrB_##FIELD, GrB_##VAL), DESC);  \
      } else if (strcmp((desc_val), "tran") == 0) {                     \
          CHECKD(GrB_Descriptor_set((DESC), GrB_##FIELD, GrB_##VAL), DESC);  \
      } else                                                            \
        elog(ERROR, "unknown outp descriptor value %s", desc_val);      \
    }                                                                   \
  } while(0)

#define GET_DESCRIPTOR(N, D) do {                               \
    CHECKD(GrB_Descriptor_new(&D), D);                        \
    GETARG_DESCRIPTOR_VAL(N, D, OUTP, REPLACE);                 \
    GETARG_DESCRIPTOR_VAL(N+1, D, MASK, COMP_STRUCTURE);        \
    GETARG_DESCRIPTOR_VAL(N+2, D, INP0, TRAN);                  \
    GETARG_DESCRIPTOR_VAL(N+3, D, INP1, TRAN);                  \
  } while (0)

void _PG_init(void);

void *malloc_function(size_t);
void *calloc_function(size_t, size_t);
void *realloc_function(void*, size_t);
void free_function(void*);

#define IMPORT_EXPORT // Use import export

#endif /* PGGRAPHBLAS_H */
