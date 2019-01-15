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

#define M_MAGIC 689276813		/* ID for debugging crosschecks */

#define CHECK(method)                                       \
{                                                           \
    info = method ;                                         \
    if (! (info == GrB_SUCCESS || info == GrB_NO_VALUE))    \
    {                                                       \
      elog(ERROR, "%s", GrB_error());                       \
      return (Datum) 0;                                     \
    }                                                       \
}

typedef struct pgGrB_Matrix {
  GrB_Matrix A;
} pgGrB_Matrix;

typedef struct pgGrB_FlatMatrix {
    int32       vl_len_;
    int         nrows;
    int         ncols;
    Oid         elemtype;
} pgGrB_FlatMatrix;

typedef struct pgGrB_Matrix_AggState {
  /* Oid      elemtype; */
  List     *rows;
  List     *cols;
  List     *vals;
} pgGrB_Matrix_AggState;

typedef struct pgGrB_Matrix_ExtractState {
  pgGrB_Matrix *mat;
  GrB_Index *rows;
  GrB_Index *cols;
  int64 *vals;
} pgGrB_Matrix_ExtractState;

static void context_callback_matrix_free(void*);

typedef struct ExpandedMatrixHeader  {
  ExpandedObjectHeader hdr;
  int em_magic;

  GrB_Index nrows;
  GrB_Index ncols;

  Oid element_type;
  int16 typlen;
  bool typbyval;
  char typalign;
  
  GrB_Matrix A;
  
  Size flat_size;
  pgGrB_FlatMatrix *flat_value;

} ExpandedMatrixHeader;

PG_FUNCTION_INFO_V1(matrix_agg_acc);
PG_FUNCTION_INFO_V1(matrix_final_int8);

PG_FUNCTION_INFO_V1(matrix_tuples);

PG_FUNCTION_INFO_V1(matrix_in);
PG_FUNCTION_INFO_V1(matrix_out);

PG_FUNCTION_INFO_V1(matrix_ncols);
PG_FUNCTION_INFO_V1(matrix_nrows);
PG_FUNCTION_INFO_V1(matrix_nvals);

PG_FUNCTION_INFO_V1(matrix_x_matrix);
PG_FUNCTION_INFO_V1(matrix_x_vector);
PG_FUNCTION_INFO_V1(vector_x_matrix);

PG_FUNCTION_INFO_V1(matrix_eq);
PG_FUNCTION_INFO_V1(matrix_neq);

PG_FUNCTION_INFO_V1(matrix_ewise_mult);
PG_FUNCTION_INFO_V1(matrix_ewise_add);

/* Vectors */

typedef struct pgGrB_Vector {
  GrB_Vector V;
} pgGrB_Vector;

typedef struct pgGrB_Vector_AggState {
  /* Oid      elemtype; */
  List     *I;
  List     *X;
} pgGrB_Vector_AggState;

typedef struct pgGrB_Vector_ExtractState {
  pgGrB_Vector *vec;
  GrB_Index *rows;
  int64 *vals;
} pgGrB_Vector_ExtractState;

static void context_callback_vector_free(void*);

PG_FUNCTION_INFO_V1(vector_agg_acc);
PG_FUNCTION_INFO_V1(vector_final_int8);

PG_FUNCTION_INFO_V1(vector_tuples);

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

