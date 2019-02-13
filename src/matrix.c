
#define SUFFIX _int64
#define T int64
#define GT GrB_INT64             // graphblas vector type
#define GTT GrB_SECOND_INT64     // default duplicate index resolver
#define PGT PG_GETARG_INT64      // how to get value args
#define DGT DatumGetInt64        // datum get type
#define TGD Int64GetDatum        // type get datum
#define FMT(v) "%lu", v           // printf fmt
#include "matrix.h"

#define SUFFIX _int32
#define T int32
#define GT GrB_INT32
#define GTT GrB_SECOND_INT32
#define PGT PG_GETARG_INT32
#define DGT DatumGetInt32
#define TGD Int32GetDatum
#define FMT(v) "%i", v
#include "matrix.h"

#define SUFFIX _int16
#define T int16
#define GT GrB_INT16
#define GTT GrB_SECOND_INT16
#define PGT PG_GETARG_INT16
#define DGT DatumGetInt16
#define TGD Int16GetDatum
#define FMT(v) "%i", v
#include "matrix.h"

#define SUFFIX _float8
#define T float8
#define GT GrB_FP64
#define GTT GrB_SECOND_FP64
#define PGT PG_GETARG_FLOAT8
#define DGT DatumGetFloat8
#define TGD Float8GetDatum
#define FMT(v) "%f", v
#include "matrix.h"

#define SUFFIX _bool
#define T bool
#define GT GrB_BOOL
#define GTT GrB_SECOND_BOOL
#define PGT PG_GETARG_BOOL
#define DGT DatumGetBool
#define TGD BoolGetDatum
#define FMT(v) "%s", v ? "t" : "f"
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
  d = DATUM_TYPE_APPLY(flat->type, expand_flat_matrix, d, CurrentMemoryContext);
  if (d == (Datum)0)
    elog(ERROR, "Unknown graphblas type");
  
  return MatrixGetEOHP(d);
}

Datum
matrix_in(PG_FUNCTION_ARGS) {
  pgGrB_Matrix *retval;

  Datum arr;
  ArrayType *vals;
  FunctionCallInfoData locfcinfo;
  int ndim, *dims;

  InitFunctionCallInfoData(locfcinfo,
                           fcinfo->flinfo,
                           3,
                           InvalidOid,
                           NULL,
                           NULL);

  locfcinfo.arg[0] = PG_GETARG_DATUM(0);
  locfcinfo.arg[1] = ObjectIdGetDatum(INT8OID);
  locfcinfo.arg[2] = Int32GetDatum(-1);
  locfcinfo.argnull[0] = false;
  locfcinfo.argnull[1] = false;
  locfcinfo.argnull[2] = false;

  arr = array_in(&locfcinfo);

  Assert(!locfcinfo.isnull);

  vals = DatumGetArrayTypeP(arr);

  if (ARR_HASNULL(vals)) {
    ereport(ERROR, (errmsg("Array may not contain NULLs")));
  }

  ndim = ARR_NDIM(vals);
  if (ndim != 1) {
    ereport(ERROR, (errmsg("One-dimesional array required")));
  }

  dims = ARR_DIMS(vals);

  if (dims[0] != 3) {
    ereport(ERROR, (errmsg("First dimension must contain 3 elements")));
  }

  retval = construct_empty_expanded_matrix_int64(0,
                                           0,
                                           CurrentMemoryContext);
  PGGRB_RETURN_MATRIX(retval);
}

