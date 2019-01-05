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

#define GRB_CHECK(method)                                   \
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


static void context_callback_matrix_free(void*);

PG_FUNCTION_INFO_V1(matrix_in);
PG_FUNCTION_INFO_V1(matrix_out);

void _PG_init(void);

#endif /* PGGRAPHBLAS_H */
