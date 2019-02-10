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

/* Set Returning Function (matrix_tuples) state for generating tuples
   from a matrix.
 */
typedef struct pgGrB_Matrix_ExtractState {
  pgGrB_Matrix *mat;
  GrB_Index *rows;
  GrB_Index *cols;
  int64 *vals;
} pgGrB_Matrix_ExtractState;


/* Callback function for freeing matrices. */
static void
context_callback_matrix_free(void*);

/* Utility function that expands a flattened matrix datum. */
Datum
expand_flat_matrix(Datum matrixdatum,
                   MemoryContext parentcontext);

/* Helper function that either detoasts or expands matrices. */
pgGrB_Matrix *
DatumGetMatrix(Datum d);

/* Helper function to create an empty flattened matrix. */
pgGrB_FlatMatrix *
construct_empty_flat_matrix(GrB_Index nrows,
                            GrB_Index ncols,
                            GrB_Type type);

/* Helper function that creates an expanded empty matrix. */
pgGrB_Matrix *
construct_empty_expanded_matrix(GrB_Index nrows,
                                GrB_Index ncols,
                                GrB_Type type,
                                MemoryContext parentcontext);

/* Helper to detoast and expand matrixs arguments */
#define PGGRB_GETARG_MATRIX(n)  DatumGetMatrix(PG_GETARG_DATUM(n))

/* Helper to return Expanded Object Header Pointer from matrix. */
#define PGGRB_RETURN_MATRIX(A) return EOHPGetRWDatum(&(A)->hdr)

/* Helper to compute flat matrix header size */
#define PGGRB_MATRIX_OVERHEAD() MAXALIGN(sizeof(pgGrB_FlatMatrix))

/* pointer to beginning of matrix data. */
#define PGGRB_MATRIX_DATA(a) (GrB_Index *)(((char *) (a)) + PGGRB_MATRIX_OVERHEAD())

/* Public API functions */

PG_FUNCTION_INFO_V1(matrix_agg_acc);
PG_FUNCTION_INFO_V1(matrix_final_int8);

PG_FUNCTION_INFO_V1(matrix);
PG_FUNCTION_INFO_V1(matrix_tuples);
PG_FUNCTION_INFO_V1(matrix_in);
PG_FUNCTION_INFO_V1(matrix_out);

PG_FUNCTION_INFO_V1(matrix_ncols);
PG_FUNCTION_INFO_V1(matrix_nrows);
PG_FUNCTION_INFO_V1(matrix_nvals);

PG_FUNCTION_INFO_V1(matrix_mxm);
PG_FUNCTION_INFO_V1(matrix_mxv);
PG_FUNCTION_INFO_V1(matrix_vxm);

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

typedef struct pgGrB_Vector_ExtractState {
  pgGrB_Vector *vec;
  GrB_Index *indices;
  int64 *vals;
} pgGrB_Vector_ExtractState;

static void context_callback_vector_free(void*);

/* Helper function that either detoasts or expands vectors. */
pgGrB_Vector *
DatumGetVector(Datum d);

/* Utility function that expands a flattened vector datum. */
Datum
expand_flat_vector(Datum vectordatum,
                   MemoryContext parentcontext);

/* Helper function to create an empty flattened vector. */
pgGrB_FlatVector *
construct_empty_flat_vector(GrB_Index size,
                            GrB_Type type);

/* Helper function that creates an expanded empty vector. */
pgGrB_Vector *
construct_empty_expanded_vector(GrB_Index size,
                                GrB_Type type,
                                MemoryContext parentcontext);

/* function to choose implicit semirings for overloaded operators */
GrB_Semiring mxm_semiring(pgGrB_Matrix *left, pgGrB_Matrix *right);
GrB_Semiring mxv_semiring(pgGrB_Matrix *left, pgGrB_Vector *right);
GrB_Semiring vxm_semiring(pgGrB_Vector *left, pgGrB_Matrix *right);

/* Helper to detoast and expand vectors arguments */
#define PGGRB_GETARG_VECTOR(n)  DatumGetVector(PG_GETARG_DATUM(n))

/* Helper to return Expanded Object Header Pointer from vector. */
#define PGGRB_RETURN_VECTOR(A) return EOHPGetRWDatum(&(A)->hdr)

/* Helper to compute flat vector header size */
#define PGGRB_VECTOR_OVERHEAD() MAXALIGN(sizeof(pgGrB_FlatVector))

#define PGGRB_VECTOR_DATA(a) (GrB_Index *)(((char *) (a)) + PGGRB_VECTOR_OVERHEAD())

PG_FUNCTION_INFO_V1(vector_agg_acc);
PG_FUNCTION_INFO_V1(vector_final_int8);

PG_FUNCTION_INFO_V1(vector_tuples);

PG_FUNCTION_INFO_V1(vector);
PG_FUNCTION_INFO_V1(vector_in);
PG_FUNCTION_INFO_V1(vector_out);

PG_FUNCTION_INFO_V1(vector_ewise_mult);
PG_FUNCTION_INFO_V1(vector_ewise_add);

PG_FUNCTION_INFO_V1(vector_eq);
PG_FUNCTION_INFO_V1(vector_neq);

PG_FUNCTION_INFO_V1(vector_nvals);
PG_FUNCTION_INFO_V1(vector_size);

char grb_type_to_name(GrB_Type t);
GrB_Type grb_name_to_type(char* name);

void _PG_init(void);

#endif /* PGGRAPHBLAS_H */
