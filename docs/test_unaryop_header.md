```
\pset linestyle unicode
\pset border 2
```
# UnaryOp

UnaryOps apply a function to an element, returning an element.

| OneSparse Name | SuiteSparse Name |
|----------------|------------------|
| identity_bool | GrB_IDENTITY_BOOL |
| ainv_bool | GrB_AINV_BOOL |
| minv_bool | GrB_MINV_BOOL |
| lnot_bool | GxB_LNOT_BOOL |
| identity_int8 | GrB_IDENTITY_INT8 |
| ainv_int8 | GrB_AINV_INT8 |
| minv_int8 | GrB_MINV_INT8 |
| lnot_int8 | GxB_LNOT_INT8 |
| identity_int16 | GrB_IDENTITY_INT16 |
| ainv_int16 | GrB_AINV_INT16 |
| minv_int16 | GrB_MINV_INT16 |
| lnot_int16 | GxB_LNOT_INT16 |
| identity_int32 | GrB_IDENTITY_INT32 |
| ainv_int32 | GrB_AINV_INT32 |
| minv_int32 | GrB_MINV_INT32 |
| lnot_int32 | GxB_LNOT_INT32 |
| identity_int64 | GrB_IDENTITY_INT64 |
| ainv_int64 | GrB_AINV_INT64 |
| minv_int64 | GrB_MINV_INT64 |
| lnot_int64 | GxB_LNOT_INT64 |
| identity_uint8 | GrB_IDENTITY_UINT8 |
| ainv_uint8 | GrB_AINV_UINT8 |
| minv_uint8 | GrB_MINV_UINT8 |
| lnot_uint8 | GxB_LNOT_UINT8 |
| identity_uint16 | GrB_IDENTITY_UINT16 |
| ainv_uint16 | GrB_AINV_UINT16 |
| minv_uint16 | GrB_MINV_UINT16 |
| lnot_uint16 | GxB_LNOT_UINT16 |
| identity_uint32 | GrB_IDENTITY_UINT32 |
| ainv_uint32 | GrB_AINV_UINT32 |
| minv_uint32 | GrB_MINV_UINT32 |
| lnot_uint32 | GxB_LNOT_UINT32 |
| identity_uint64 | GrB_IDENTITY_UINT64 |
| ainv_uint64 | GrB_AINV_UINT64 |
| minv_uint64 | GrB_MINV_UINT64 |
| lnot_uint64 | GxB_LNOT_UINT64 |
| identity_fp32 | GrB_IDENTITY_FP32 |
| ainv_fp32 | GrB_AINV_FP32 |
| minv_fp32 | GrB_MINV_FP32 |
| lnot_fp32 | GxB_LNOT_FP32 |
| identity_fp64 | GrB_IDENTITY_FP64 |
| ainv_fp64 | GrB_AINV_FP64 |
| minv_fp64 | GrB_MINV_FP64 |
| lnot_fp64 | GxB_LNOT_FP64 |
| identity_fc32 | GxB_IDENTITY_FC32 |
| ainv_fc32 | GxB_AINV_FC32 |
| minv_fc32 | GxB_MINV_FC32 |
| identity_fc64 | GxB_IDENTITY_FC64 |
| ainv_fc64 | GxB_AINV_FC64 |
| minv_fc64 | GxB_MINV_FC64 |
| one_bool | GxB_ONE_BOOL |
| abs_bool | GrB_ABS_BOOL |
| one_int8 | GxB_ONE_INT8 |
| abs_int8 | GrB_ABS_INT8 |
| bnot_int8 | GrB_BNOT_INT8 |
| one_int16 | GxB_ONE_INT16 |
| abs_int16 | GrB_ABS_INT16 |
| bnot_int16 | GrB_BNOT_INT16 |
| one_int32 | GxB_ONE_INT32 |
| abs_int32 | GrB_ABS_INT32 |
| bnot_int32 | GrB_BNOT_INT32 |
| one_int64 | GxB_ONE_INT64 |
| abs_int64 | GrB_ABS_INT64 |
| bnot_int64 | GrB_BNOT_INT64 |
| one_uint8 | GxB_ONE_UINT8 |
| abs_uint8 | GrB_ABS_UINT8 |
| bnot_uint8 | GrB_BNOT_UINT8 |
| one_uint16 | GxB_ONE_UINT16 |
| abs_uint16 | GrB_ABS_UINT16 |
| bnot_uint16 | GrB_BNOT_UINT16 |
| one_uint32 | GxB_ONE_UINT32 |
| abs_uint32 | GrB_ABS_UINT32 |
| bnot_uint32 | GrB_BNOT_UINT32 |
| one_uint64 | GxB_ONE_UINT64 |
| abs_uint64 | GrB_ABS_UINT64 |
| bnot_uint64 | GrB_BNOT_UINT64 |
| one_fp32 | GxB_ONE_FP32 |
| abs_fp32 | GrB_ABS_FP32 |
| one_fp64 | GxB_ONE_FP64 |
| abs_fp64 | GrB_ABS_FP64 |
| one_fc32 | GxB_ONE_FC32 |
| one_fc64 | GxB_ONE_FC64 |
| lnot | GrB_LNOT |
| abs_bool | GxB_ABS_BOOL |
| abs_int8 | GxB_ABS_INT8 |
| abs_int16 | GxB_ABS_INT16 |
| abs_int32 | GxB_ABS_INT32 |
| abs_int64 | GxB_ABS_INT64 |
| abs_uint8 | GxB_ABS_UINT8 |
| abs_uint16 | GxB_ABS_UINT16 |
| abs_uint32 | GxB_ABS_UINT32 |
| abs_uint64 | GxB_ABS_UINT64 |
| abs_fp32 | GxB_ABS_FP32 |
| abs_fp64 | GxB_ABS_FP64 |
| sqrt_fp32 | GxB_SQRT_FP32 |
| log_fp32 | GxB_LOG_FP32 |
| exp_fp32 | GxB_EXP_FP32 |
| log2_fp32 | GxB_LOG2_FP32 |
| sqrt_fp64 | GxB_SQRT_FP64 |
| log_fp64 | GxB_LOG_FP64 |
| exp_fp64 | GxB_EXP_FP64 |
| log2_fp64 | GxB_LOG2_FP64 |
| sqrt_fc32 | GxB_SQRT_FC32 |
| log_fc32 | GxB_LOG_FC32 |
| exp_fc32 | GxB_EXP_FC32 |
| log2_fc32 | GxB_LOG2_FC32 |
| sqrt_fc64 | GxB_SQRT_FC64 |
| log_fc64 | GxB_LOG_FC64 |
| exp_fc64 | GxB_EXP_FC64 |
| log2_fc64 | GxB_LOG2_FC64 |
| sin_fp32 | GxB_SIN_FP32 |
| cos_fp32 | GxB_COS_FP32 |
| tan_fp32 | GxB_TAN_FP32 |
| sin_fp64 | GxB_SIN_FP64 |
| cos_fp64 | GxB_COS_FP64 |
| tan_fp64 | GxB_TAN_FP64 |
| sin_fc32 | GxB_SIN_FC32 |
| cos_fc32 | GxB_COS_FC32 |
| tan_fc32 | GxB_TAN_FC32 |
| sin_fc64 | GxB_SIN_FC64 |
| cos_fc64 | GxB_COS_FC64 |
| tan_fc64 | GxB_TAN_FC64 |
| acos_fp32 | GxB_ACOS_FP32 |
| asin_fp32 | GxB_ASIN_FP32 |
| atan_fp32 | GxB_ATAN_FP32 |
| acos_fp64 | GxB_ACOS_FP64 |
| asin_fp64 | GxB_ASIN_FP64 |
| atan_fp64 | GxB_ATAN_FP64 |
| acos_fc32 | GxB_ACOS_FC32 |
| asin_fc32 | GxB_ASIN_FC32 |
| atan_fc32 | GxB_ATAN_FC32 |
| acos_fc64 | GxB_ACOS_FC64 |
| asin_fc64 | GxB_ASIN_FC64 |
| atan_fc64 | GxB_ATAN_FC64 |
| sinh_fp32 | GxB_SINH_FP32 |
| cosh_fp32 | GxB_COSH_FP32 |
| tanh_fp32 | GxB_TANH_FP32 |
| sinh_fp64 | GxB_SINH_FP64 |
| cosh_fp64 | GxB_COSH_FP64 |
| tanh_fp64 | GxB_TANH_FP64 |
| sinh_fc32 | GxB_SINH_FC32 |
| cosh_fc32 | GxB_COSH_FC32 |
| tanh_fc32 | GxB_TANH_FC32 |
| sinh_fc64 | GxB_SINH_FC64 |
| cosh_fc64 | GxB_COSH_FC64 |
| tanh_fc64 | GxB_TANH_FC64 |
| acosh_fp32 | GxB_ACOSH_FP32 |
| asinh_fp32 | GxB_ASINH_FP32 |
| atanh_fp32 | GxB_ATANH_FP32 |
| signum_fp32 | GxB_SIGNUM_FP32 |
| acosh_fp64 | GxB_ACOSH_FP64 |
| asinh_fp64 | GxB_ASINH_FP64 |
| atanh_fp64 | GxB_ATANH_FP64 |
| signum_fp64 | GxB_SIGNUM_FP64 |
| acosh_fc32 | GxB_ACOSH_FC32 |
| asinh_fc32 | GxB_ASINH_FC32 |
| atanh_fc32 | GxB_ATANH_FC32 |
| signum_fc32 | GxB_SIGNUM_FC32 |
| acosh_fc64 | GxB_ACOSH_FC64 |
| asinh_fc64 | GxB_ASINH_FC64 |
| atanh_fc64 | GxB_ATANH_FC64 |
| signum_fc64 | GxB_SIGNUM_FC64 |
| ceil_fp32 | GxB_CEIL_FP32 |
| floor_fp32 | GxB_FLOOR_FP32 |
| round_fp32 | GxB_ROUND_FP32 |
| trunc_fp32 | GxB_TRUNC_FP32 |
| ceil_fp64 | GxB_CEIL_FP64 |
| floor_fp64 | GxB_FLOOR_FP64 |
| round_fp64 | GxB_ROUND_FP64 |
| trunc_fp64 | GxB_TRUNC_FP64 |
| ceil_fc32 | GxB_CEIL_FC32 |
| floor_fc32 | GxB_FLOOR_FC32 |
| round_fc32 | GxB_ROUND_FC32 |
| trunc_fc32 | GxB_TRUNC_FC32 |
| ceil_fc64 | GxB_CEIL_FC64 |
| floor_fc64 | GxB_FLOOR_FC64 |
| round_fc64 | GxB_ROUND_FC64 |
| trunc_fc64 | GxB_TRUNC_FC64 |
| exp2_fp32 | GxB_EXP2_FP32 |
| expm1_fp32 | GxB_EXPM1_FP32 |
| log10_fp32 | GxB_LOG10_FP32 |
| log1p_fp32 | GxB_LOG1P_FP32 |
| exp2_fp64 | GxB_EXP2_FP64 |
| expm1_fp64 | GxB_EXPM1_FP64 |
| log10_fp64 | GxB_LOG10_FP64 |
| log1p_fp64 | GxB_LOG1P_FP64 |
| exp2_fc32 | GxB_EXP2_FC32 |
| expm1_fc32 | GxB_EXPM1_FC32 |
| log10_fc32 | GxB_LOG10_FC32 |
| log1p_fc32 | GxB_LOG1P_FC32 |
| exp2_fc64 | GxB_EXP2_FC64 |
| expm1_fc64 | GxB_EXPM1_FC64 |
| log10_fc64 | GxB_LOG10_FC64 |
| log1p_fc64 | GxB_LOG1P_FC64 |
| lgamma_fp32 | GxB_LGAMMA_FP32 |
| tgamma_fp32 | GxB_TGAMMA_FP32 |
| erf_fp32 | GxB_ERF_FP32 |
| erfc_fp32 | GxB_ERFC_FP32 |
| lgamma_fp64 | GxB_LGAMMA_FP64 |
| tgamma_fp64 | GxB_TGAMMA_FP64 |
| erf_fp64 | GxB_ERF_FP64 |
| erfc_fp64 | GxB_ERFC_FP64 |
| cbrt_fp32 | GxB_CBRT_FP32 |
| cbrt_fp64 | GxB_CBRT_FP64 |
| frexpx_fp32 | GxB_FREXPX_FP32 |
| frexpe_fp32 | GxB_FREXPE_FP32 |
| frexpx_fp64 | GxB_FREXPX_FP64 |
| frexpe_fp64 | GxB_FREXPE_FP64 |
| conj_fc32 | GxB_CONJ_FC32 |
| conj_fc64 | GxB_CONJ_FC64 |
| creal_fc32 | GxB_CREAL_FC32 |
| cimag_fc32 | GxB_CIMAG_FC32 |
| carg_fc32 | GxB_CARG_FC32 |
| abs_fc32 | GxB_ABS_FC32 |
| creal_fc64 | GxB_CREAL_FC64 |
| cimag_fc64 | GxB_CIMAG_FC64 |
| carg_fc64 | GxB_CARG_FC64 |
| abs_fc64 | GxB_ABS_FC64 |
| isinf_fp32 | GxB_ISINF_FP32 |
| isinf_fp64 | GxB_ISINF_FP64 |
| isinf_fc32 | GxB_ISINF_FC32 |
| isinf_fc64 | GxB_ISINF_FC64 |
| isnan_fp32 | GxB_ISNAN_FP32 |
| isnan_fp64 | GxB_ISNAN_FP64 |
| isnan_fc32 | GxB_ISNAN_FC32 |
| isnan_fc64 | GxB_ISNAN_FC64 |
| isfinite_fp32 | GxB_ISFINITE_FP32 |
| isfinite_fp64 | GxB_ISFINITE_FP64 |
| isfinite_fc32 | GxB_ISFINITE_FC32 |
| isfinite_fc64 | GxB_ISFINITE_FC64 |
| positioni_int32 | GxB_POSITIONI_INT32 |
| positioni_int64 | GxB_POSITIONI_INT64 |
| positioni1_int32 | GxB_POSITIONI1_INT32 |
| positioni1_int64 | GxB_POSITIONI1_INT64 |
| positionj_int32 | GxB_POSITIONJ_INT32 |
| positionj_int64 | GxB_POSITIONJ_INT64 |
| positionj1_int32 | GxB_POSITIONJ1_INT32 |
| positionj1_int64 | GxB_POSITIONJ1_INT64 |