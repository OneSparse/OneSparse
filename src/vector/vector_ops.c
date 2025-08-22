#include "../onesparse.h"

#define SUFFIX _int64                // suffix for names
#define PG_TYPE int64                // postgres type
#define PG_TYPEOID  INT8OID              // postgres oid for type
#define GB_TYPE GrB_INT64            // graphblas vector type
#define PG_GETARG PG_GETARG_INT64       // how to get value args
#define PG_RETURN PG_RETURN_INT64
#define PG_GET_DATUM Int64GetDatum
#define PG_DATUM_GET DatumGetInt64
#include "vector_ops.h"

#define SUFFIX _int32                // suffix for names
#define PG_TYPE int32                // postgres type
#define PG_TYPEOID  INT4OID              // postgres oid for type
#define GB_TYPE GrB_INT32            // graphblas vector type
#define PG_GETARG PG_GETARG_INT32       // how to get value args
#define PG_RETURN PG_RETURN_INT32
#define PG_GET_DATUM Int32GetDatum
#define PG_DATUM_GET DatumGetInt32
#include "vector_ops.h"

#define SUFFIX _int16                // suffix for names
#define PG_TYPE int16                // postgres type
#define PG_TYPEOID  INT2OID              // postgres oid for type
#define GB_TYPE GrB_INT16            // graphblas vector type
#define PG_GETARG PG_GETARG_INT16       // how to get value args
#define PG_RETURN PG_RETURN_INT16
#define PG_GET_DATUM Int16GetDatum
#define PG_DATUM_GET DatumGetInt16
#include "vector_ops.h"

#define SUFFIX _fp64                // suffix for names
#define PG_TYPE float8                // postgres type
#define PG_TYPEOID  FLOAT8OID              // postgres oid for type
#define GB_TYPE GrB_FP64            // graphblas vector type
#define PG_GETARG PG_GETARG_FLOAT8       // how to get value args
#define PG_RETURN PG_RETURN_FLOAT8
#define PG_GET_DATUM Float8GetDatum
#define PG_DATUM_GET DatumGetFloat8
#include "vector_ops.h"

#define SUFFIX _fp32                // suffix for names
#define PG_TYPE float4                // postgres type
#define PG_TYPEOID  FLOAT4OID              // postgres oid for type
#define GB_TYPE GrB_FP32            // graphblas vector type
#define PG_GETARG PG_GETARG_FLOAT4       // how to get value args
#define PG_RETURN PG_RETURN_FLOAT4
#define PG_GET_DATUM Float4GetDatum
#define PG_DATUM_GET DatumGetFloat4
#include "vector_ops.h"

#define SUFFIX _bool                // suffix for names
#define PG_TYPE bool                // postgres type
#define GB_TYPE GrB_BOOL            // graphblas vector type
#define PG_TYPEOID  BOOLOID              // postgres oid for type
#define PG_GETARG PG_GETARG_BOOL       // how to get value args
#define PG_RETURN PG_RETURN_BOOL
#define PG_GET_DATUM BoolGetDatum
#define PG_DATUM_GET DatumGetBool
#include "vector_ops.h"
