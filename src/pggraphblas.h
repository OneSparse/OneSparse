#ifndef PGGRAPHBLAS_H
#define PGGRAPHBLAS_H

#include "GraphBLAS.h"
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

#define CCAT2(x, y) x ## y
#define CCAT(x, y) CCAT2(x, y)
#define FN(x) CCAT(x, SUFFIX)

#define DATUM_TYPE_APPLY(T, F, ...)               \
  (T) == GrB_INT64?                               \
    F ## _int64(__VA_ARGS__) :                    \
    (T)  == GrB_INT32?                            \
    F ## _int32(__VA_ARGS__) :                    \
    (T)  == GrB_INT16?                            \
    F ## _int16(__VA_ARGS__) :                    \
    (T)  == GrB_BOOL?                             \
    F ## _bool(__VA_ARGS__) :                     \
    (T)  == GrB_FP64?                             \
    F ## _float8(__VA_ARGS__) :                   \
    (T)  == GrB_FP32?                             \
    F ## _float4(__VA_ARGS__) :                   \
    (Datum)0

#define DEFAULT_SEMIRING(T)                       \
  (T) == GrB_INT64?                               \
    GxB_PLUS_TIMES_INT64 :                        \
    (T)  == GrB_INT32?                            \
    GxB_PLUS_TIMES_INT32 :                        \
    (T)  == GrB_INT16?                            \
    GxB_PLUS_TIMES_INT16 :                        \
    (T)  == GrB_BOOL?                             \
    GxB_LOR_LAND_BOOL :                           \
    (T)  == GrB_FP64?                             \
    GxB_PLUS_TIMES_FP64 :                         \
    (T)  == GrB_FP32?                             \
    GxB_PLUS_TIMES_FP32 :                         \
    NULL


/* dumb debug helper */
#define elogn(s) elog(NOTICE, "%s", (s))
#define elogn1(s, v) elog(NOTICE, "%s: %lu", (s), (v))

/* GraphBLAS API checking macros */

#define CHECKD(method)                                      \
  {                                                         \
    info = method ;                                         \
    if (! (info == GrB_SUCCESS || info == GrB_NO_VALUE))    \
    {                                                       \
      elog(ERROR, "%s", GrB_error());                       \
      return (Datum) 0;                                     \
    }                                                       \
}

#define CHECKV(method)                                      \
{                                                           \
    info = method ;                                         \
    if (! (info == GrB_SUCCESS || info == GrB_NO_VALUE))    \
    {                                                       \
      elog(ERROR, "%s", GrB_error());                       \
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

/* Flattened representation of matrix, used to store to disk.

Actual array data is appended and for the moment cannot exceed 1GB.
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
  pgGrB_FlatMatrix *flat_value;
} pgGrB_Matrix;


/* Aggregate function state (matrix_agg) for accumulating tuples into a matrix.
 */
typedef struct pgGrB_Matrix_AggState {
  /* Oid      elemtype; */
  List *rows;
  List *cols;
  List *vals;
} pgGrB_Matrix_AggState;

/* helper to turn types into names. */
char* grb_type_to_name(GrB_Type t);
GrB_Type grb_name_to_type(char* n);

/* Callback function for freeing matrices. */
static void
context_callback_matrix_free(void*);

/* Helper function that either detoasts or expands matrices. */
pgGrB_Matrix *
DatumGetMatrix(Datum d);

/* Helper function to create an empty flattened matrix. */
pgGrB_FlatMatrix *
construct_empty_flat_matrix(GrB_Index nrows,
                            GrB_Index ncols,
                            GrB_Type type);

/* Helper to detoast and expand matrixs arguments */
#define PGGRB_GETARG_MATRIX(n)  DatumGetMatrix(PG_GETARG_DATUM(n))

/* Helper to return Expanded Object Header Pointer from matrix. */
#define PGGRB_RETURN_MATRIX(A) return EOHPGetRWDatum(&(A)->hdr)

/* Helper to compute flat matrix header size */
#define PGGRB_MATRIX_OVERHEAD() MAXALIGN(sizeof(pgGrB_FlatMatrix))

/* pointer to beginning of matrix data. */
#define PGGRB_MATRIX_DATA(a) (GrB_Index *)(((char *) (a)) + PGGRB_MATRIX_OVERHEAD())

/* Public API functions */

PG_FUNCTION_INFO_V1(matrix_in);
PG_FUNCTION_INFO_V1(matrix_out);

PG_FUNCTION_INFO_V1(matrix_ncols);
PG_FUNCTION_INFO_V1(matrix_nrows);
PG_FUNCTION_INFO_V1(matrix_nvals);

PG_FUNCTION_INFO_V1(mxm);
PG_FUNCTION_INFO_V1(mxv);
PG_FUNCTION_INFO_V1(vxm);

PG_FUNCTION_INFO_V1(matrix_eq);
PG_FUNCTION_INFO_V1(matrix_neq);

PG_FUNCTION_INFO_V1(matrix_ewise_mult);
PG_FUNCTION_INFO_V1(matrix_ewise_add);

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
  pgGrB_FlatVector *flat_value;
} pgGrB_Vector;

typedef struct pgGrB_Vector_AggState {
  /* Oid      elemtype; */
  List     *I;
  List     *X;
} pgGrB_Vector_AggState;

static void context_callback_vector_free(void*);

/* Helper function that either detoasts or expands vectors. */
pgGrB_Vector *
DatumGetVector(Datum d);

/* Helper function to create an empty flattened vector. */
pgGrB_FlatVector *
construct_empty_flat_vector(GrB_Index size,
                            GrB_Type type);

/* function to choose implicit semirings for overloaded operators */
GrB_Semiring mxm_semiring(pgGrB_Matrix *left, pgGrB_Matrix *right);
GrB_Semiring mxv_semiring(pgGrB_Matrix *left, pgGrB_Vector *right);
GrB_Semiring vxm_semiring(pgGrB_Vector *left, pgGrB_Matrix *right);

#define VectorGetEOHP(d) (pgGrB_Vector *) DatumGetEOHP(d)
#define MatrixGetEOHP(d) (pgGrB_Matrix *) DatumGetEOHP(d);

/* Helper to detoast and expand vectors arguments */
#define PGGRB_GETARG_VECTOR(n)  DatumGetVector(PG_GETARG_DATUM(n))

/* Helper to return Expanded Object Header Pointer from vector. */
#define PGGRB_RETURN_VECTOR(A) return EOHPGetRWDatum(&(A)->hdr)

/* Helper to compute flat vector header size */
#define PGGRB_VECTOR_OVERHEAD() MAXALIGN(sizeof(pgGrB_FlatVector))

#define PGGRB_VECTOR_DATA(a) (GrB_Index *)(((char *) (a)) + PGGRB_VECTOR_OVERHEAD())

PG_FUNCTION_INFO_V1(vector_in);
PG_FUNCTION_INFO_V1(vector_out);

PG_FUNCTION_INFO_V1(vector_ewise_mult);
PG_FUNCTION_INFO_V1(vector_ewise_add);

PG_FUNCTION_INFO_V1(vector_eq);
PG_FUNCTION_INFO_V1(vector_neq);

PG_FUNCTION_INFO_V1(vector_nvals);
PG_FUNCTION_INFO_V1(vector_size);

void _PG_init(void);

#endif /* PGGRAPHBLAS_H */
