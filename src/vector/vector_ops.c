#include "../onesparse.h"

#define SUFFIX _int64                // suffix for names
#define PG_TYPE int64                // postgres type
#define GB_TYPE GrB_INT64            // graphblas vector type
#define PG_GETARG PG_GETARG_INT64       // how to get value args
#define PG_RETURN PG_RETURN_INT64
#include "vector_ops.h"

#define SUFFIX _int32                // suffix for names
#define PG_TYPE int32                // postgres type
#define GB_TYPE GrB_INT32            // graphblas vector type
#define PG_GETARG PG_GETARG_INT32       // how to get value args
#define PG_RETURN PG_RETURN_INT32
#include "vector_ops.h"

#define SUFFIX _int16                // suffix for names
#define PG_TYPE int16                // postgres type
#define GB_TYPE GrB_INT16            // graphblas vector type
#define PG_GETARG PG_GETARG_INT16       // how to get value args
#define PG_RETURN PG_RETURN_INT16
#include "vector_ops.h"

#define SUFFIX _fp64                // suffix for names
#define PG_TYPE float8                // postgres type
#define GB_TYPE GrB_FP64            // graphblas vector type
#define PG_GETARG PG_GETARG_FLOAT8       // how to get value args
#define PG_RETURN PG_RETURN_FLOAT8
#include "vector_ops.h"

#define SUFFIX _fp32                // suffix for names
#define PG_TYPE float4                // postgres type
#define GB_TYPE GrB_FP32            // graphblas vector type
#define PG_GETARG PG_GETARG_FLOAT4       // how to get value args
#define PG_RETURN PG_RETURN_FLOAT4
#include "vector_ops.h"

#define SUFFIX _bool                // suffix for names
#define PG_TYPE bool                // postgres type
#define GB_TYPE GrB_BOOL            // graphblas vector type
#define PG_GETARG PG_GETARG_BOOL       // how to get value args
#define PG_RETURN PG_RETURN_BOOL
#include "vector_ops.h"

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
