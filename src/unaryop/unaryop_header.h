typedef struct unaryop_entry
{
	uint32 status;
	char *name;
    GrB_UnaryOp unaryop;
} unaryop_entry;

static uint32 hash_string_pointer(char *s);

static uint32
hash_string_pointer(char *s)
{
	unsigned char *ss = (unsigned char *) s;

	return hash_bytes(ss, strlen(s));
    }

#define UNARYOPHASH_INITIAL_SIZE 2000

#define SH_PREFIX		unaryophash
#define SH_ELEMENT_TYPE unaryop_entry
#define SH_KEY_TYPE		char *
#define	SH_KEY			name
#define SH_HASH_KEY(tb, key)	hash_string_pointer(key)
#define SH_EQUAL(tb, a, b)		(strcmp(a, b) == 0)
#define	SH_SCOPE		static inline
#define SH_DECLARE
#define SH_DEFINE
#include "lib/simplehash.h"

static unaryophash_hash *unaryophash;

void initialize_unaryops()
{
    bool found;
    unaryop_entry *entry;

    unaryophash = unaryophash_create(TopMemoryContext, UNARYOPHASH_INITIAL_SIZE, NULL);
    
    entry = unaryophash_insert(unaryophash, "identity_bool", &found);
    entry->name = strdup("identity_bool");
    entry->unaryop = GrB_IDENTITY_BOOL;

    entry = unaryophash_insert(unaryophash, "ainv_bool", &found);
    entry->name = strdup("ainv_bool");
    entry->unaryop = GrB_AINV_BOOL;

    entry = unaryophash_insert(unaryophash, "minv_bool", &found);
    entry->name = strdup("minv_bool");
    entry->unaryop = GrB_MINV_BOOL;

    entry = unaryophash_insert(unaryophash, "identity_int8", &found);
    entry->name = strdup("identity_int8");
    entry->unaryop = GrB_IDENTITY_INT8;

    entry = unaryophash_insert(unaryophash, "ainv_int8", &found);
    entry->name = strdup("ainv_int8");
    entry->unaryop = GrB_AINV_INT8;

    entry = unaryophash_insert(unaryophash, "minv_int8", &found);
    entry->name = strdup("minv_int8");
    entry->unaryop = GrB_MINV_INT8;

    entry = unaryophash_insert(unaryophash, "identity_int16", &found);
    entry->name = strdup("identity_int16");
    entry->unaryop = GrB_IDENTITY_INT16;

    entry = unaryophash_insert(unaryophash, "ainv_int16", &found);
    entry->name = strdup("ainv_int16");
    entry->unaryop = GrB_AINV_INT16;

    entry = unaryophash_insert(unaryophash, "minv_int16", &found);
    entry->name = strdup("minv_int16");
    entry->unaryop = GrB_MINV_INT16;

    entry = unaryophash_insert(unaryophash, "identity_int32", &found);
    entry->name = strdup("identity_int32");
    entry->unaryop = GrB_IDENTITY_INT32;

    entry = unaryophash_insert(unaryophash, "ainv_int32", &found);
    entry->name = strdup("ainv_int32");
    entry->unaryop = GrB_AINV_INT32;

    entry = unaryophash_insert(unaryophash, "minv_int32", &found);
    entry->name = strdup("minv_int32");
    entry->unaryop = GrB_MINV_INT32;

    entry = unaryophash_insert(unaryophash, "identity_int64", &found);
    entry->name = strdup("identity_int64");
    entry->unaryop = GrB_IDENTITY_INT64;

    entry = unaryophash_insert(unaryophash, "ainv_int64", &found);
    entry->name = strdup("ainv_int64");
    entry->unaryop = GrB_AINV_INT64;

    entry = unaryophash_insert(unaryophash, "minv_int64", &found);
    entry->name = strdup("minv_int64");
    entry->unaryop = GrB_MINV_INT64;

    entry = unaryophash_insert(unaryophash, "identity_uint8", &found);
    entry->name = strdup("identity_uint8");
    entry->unaryop = GrB_IDENTITY_UINT8;

    entry = unaryophash_insert(unaryophash, "ainv_uint8", &found);
    entry->name = strdup("ainv_uint8");
    entry->unaryop = GrB_AINV_UINT8;

    entry = unaryophash_insert(unaryophash, "minv_uint8", &found);
    entry->name = strdup("minv_uint8");
    entry->unaryop = GrB_MINV_UINT8;

    entry = unaryophash_insert(unaryophash, "identity_uint16", &found);
    entry->name = strdup("identity_uint16");
    entry->unaryop = GrB_IDENTITY_UINT16;

    entry = unaryophash_insert(unaryophash, "ainv_uint16", &found);
    entry->name = strdup("ainv_uint16");
    entry->unaryop = GrB_AINV_UINT16;

    entry = unaryophash_insert(unaryophash, "minv_uint16", &found);
    entry->name = strdup("minv_uint16");
    entry->unaryop = GrB_MINV_UINT16;

    entry = unaryophash_insert(unaryophash, "identity_uint32", &found);
    entry->name = strdup("identity_uint32");
    entry->unaryop = GrB_IDENTITY_UINT32;

    entry = unaryophash_insert(unaryophash, "ainv_uint32", &found);
    entry->name = strdup("ainv_uint32");
    entry->unaryop = GrB_AINV_UINT32;

    entry = unaryophash_insert(unaryophash, "minv_uint32", &found);
    entry->name = strdup("minv_uint32");
    entry->unaryop = GrB_MINV_UINT32;

    entry = unaryophash_insert(unaryophash, "identity_uint64", &found);
    entry->name = strdup("identity_uint64");
    entry->unaryop = GrB_IDENTITY_UINT64;

    entry = unaryophash_insert(unaryophash, "ainv_uint64", &found);
    entry->name = strdup("ainv_uint64");
    entry->unaryop = GrB_AINV_UINT64;

    entry = unaryophash_insert(unaryophash, "minv_uint64", &found);
    entry->name = strdup("minv_uint64");
    entry->unaryop = GrB_MINV_UINT64;

    entry = unaryophash_insert(unaryophash, "identity_fp32", &found);
    entry->name = strdup("identity_fp32");
    entry->unaryop = GrB_IDENTITY_FP32;

    entry = unaryophash_insert(unaryophash, "ainv_fp32", &found);
    entry->name = strdup("ainv_fp32");
    entry->unaryop = GrB_AINV_FP32;

    entry = unaryophash_insert(unaryophash, "minv_fp32", &found);
    entry->name = strdup("minv_fp32");
    entry->unaryop = GrB_MINV_FP32;

    entry = unaryophash_insert(unaryophash, "identity_fp64", &found);
    entry->name = strdup("identity_fp64");
    entry->unaryop = GrB_IDENTITY_FP64;

    entry = unaryophash_insert(unaryophash, "ainv_fp64", &found);
    entry->name = strdup("ainv_fp64");
    entry->unaryop = GrB_AINV_FP64;

    entry = unaryophash_insert(unaryophash, "minv_fp64", &found);
    entry->name = strdup("minv_fp64");
    entry->unaryop = GrB_MINV_FP64;

    entry = unaryophash_insert(unaryophash, "abs_bool", &found);
    entry->name = strdup("abs_bool");
    entry->unaryop = GrB_ABS_BOOL;

    entry = unaryophash_insert(unaryophash, "abs_int8", &found);
    entry->name = strdup("abs_int8");
    entry->unaryop = GrB_ABS_INT8;

    entry = unaryophash_insert(unaryophash, "bnot_int8", &found);
    entry->name = strdup("bnot_int8");
    entry->unaryop = GrB_BNOT_INT8;

    entry = unaryophash_insert(unaryophash, "abs_int16", &found);
    entry->name = strdup("abs_int16");
    entry->unaryop = GrB_ABS_INT16;

    entry = unaryophash_insert(unaryophash, "bnot_int16", &found);
    entry->name = strdup("bnot_int16");
    entry->unaryop = GrB_BNOT_INT16;

    entry = unaryophash_insert(unaryophash, "abs_int32", &found);
    entry->name = strdup("abs_int32");
    entry->unaryop = GrB_ABS_INT32;

    entry = unaryophash_insert(unaryophash, "bnot_int32", &found);
    entry->name = strdup("bnot_int32");
    entry->unaryop = GrB_BNOT_INT32;

    entry = unaryophash_insert(unaryophash, "abs_int64", &found);
    entry->name = strdup("abs_int64");
    entry->unaryop = GrB_ABS_INT64;

    entry = unaryophash_insert(unaryophash, "bnot_int64", &found);
    entry->name = strdup("bnot_int64");
    entry->unaryop = GrB_BNOT_INT64;

    entry = unaryophash_insert(unaryophash, "abs_uint8", &found);
    entry->name = strdup("abs_uint8");
    entry->unaryop = GrB_ABS_UINT8;

    entry = unaryophash_insert(unaryophash, "bnot_uint8", &found);
    entry->name = strdup("bnot_uint8");
    entry->unaryop = GrB_BNOT_UINT8;

    entry = unaryophash_insert(unaryophash, "abs_uint16", &found);
    entry->name = strdup("abs_uint16");
    entry->unaryop = GrB_ABS_UINT16;

    entry = unaryophash_insert(unaryophash, "bnot_uint16", &found);
    entry->name = strdup("bnot_uint16");
    entry->unaryop = GrB_BNOT_UINT16;

    entry = unaryophash_insert(unaryophash, "abs_uint32", &found);
    entry->name = strdup("abs_uint32");
    entry->unaryop = GrB_ABS_UINT32;

    entry = unaryophash_insert(unaryophash, "bnot_uint32", &found);
    entry->name = strdup("bnot_uint32");
    entry->unaryop = GrB_BNOT_UINT32;

    entry = unaryophash_insert(unaryophash, "abs_uint64", &found);
    entry->name = strdup("abs_uint64");
    entry->unaryop = GrB_ABS_UINT64;

    entry = unaryophash_insert(unaryophash, "bnot_uint64", &found);
    entry->name = strdup("bnot_uint64");
    entry->unaryop = GrB_BNOT_UINT64;

    entry = unaryophash_insert(unaryophash, "abs_fp32", &found);
    entry->name = strdup("abs_fp32");
    entry->unaryop = GrB_ABS_FP32;

    entry = unaryophash_insert(unaryophash, "abs_fp64", &found);
    entry->name = strdup("abs_fp64");
    entry->unaryop = GrB_ABS_FP64;

    entry = unaryophash_insert(unaryophash, "lnot_bool", &found);
    entry->name = strdup("lnot_bool");
    entry->unaryop = GxB_LNOT_BOOL;

    entry = unaryophash_insert(unaryophash, "one_bool", &found);
    entry->name = strdup("one_bool");
    entry->unaryop = GxB_ONE_BOOL;

    entry = unaryophash_insert(unaryophash, "lnot_int8", &found);
    entry->name = strdup("lnot_int8");
    entry->unaryop = GxB_LNOT_INT8;

    entry = unaryophash_insert(unaryophash, "one_int8", &found);
    entry->name = strdup("one_int8");
    entry->unaryop = GxB_ONE_INT8;

    entry = unaryophash_insert(unaryophash, "lnot_int16", &found);
    entry->name = strdup("lnot_int16");
    entry->unaryop = GxB_LNOT_INT16;

    entry = unaryophash_insert(unaryophash, "one_int16", &found);
    entry->name = strdup("one_int16");
    entry->unaryop = GxB_ONE_INT16;

    entry = unaryophash_insert(unaryophash, "lnot_int32", &found);
    entry->name = strdup("lnot_int32");
    entry->unaryop = GxB_LNOT_INT32;

    entry = unaryophash_insert(unaryophash, "one_int32", &found);
    entry->name = strdup("one_int32");
    entry->unaryop = GxB_ONE_INT32;

    entry = unaryophash_insert(unaryophash, "lnot_int64", &found);
    entry->name = strdup("lnot_int64");
    entry->unaryop = GxB_LNOT_INT64;

    entry = unaryophash_insert(unaryophash, "one_int64", &found);
    entry->name = strdup("one_int64");
    entry->unaryop = GxB_ONE_INT64;

    entry = unaryophash_insert(unaryophash, "lnot_uint8", &found);
    entry->name = strdup("lnot_uint8");
    entry->unaryop = GxB_LNOT_UINT8;

    entry = unaryophash_insert(unaryophash, "one_uint8", &found);
    entry->name = strdup("one_uint8");
    entry->unaryop = GxB_ONE_UINT8;

    entry = unaryophash_insert(unaryophash, "lnot_uint16", &found);
    entry->name = strdup("lnot_uint16");
    entry->unaryop = GxB_LNOT_UINT16;

    entry = unaryophash_insert(unaryophash, "one_uint16", &found);
    entry->name = strdup("one_uint16");
    entry->unaryop = GxB_ONE_UINT16;

    entry = unaryophash_insert(unaryophash, "lnot_uint32", &found);
    entry->name = strdup("lnot_uint32");
    entry->unaryop = GxB_LNOT_UINT32;

    entry = unaryophash_insert(unaryophash, "one_uint32", &found);
    entry->name = strdup("one_uint32");
    entry->unaryop = GxB_ONE_UINT32;

    entry = unaryophash_insert(unaryophash, "lnot_uint64", &found);
    entry->name = strdup("lnot_uint64");
    entry->unaryop = GxB_LNOT_UINT64;

    entry = unaryophash_insert(unaryophash, "one_uint64", &found);
    entry->name = strdup("one_uint64");
    entry->unaryop = GxB_ONE_UINT64;

    entry = unaryophash_insert(unaryophash, "lnot_fp32", &found);
    entry->name = strdup("lnot_fp32");
    entry->unaryop = GxB_LNOT_FP32;

    entry = unaryophash_insert(unaryophash, "one_fp32", &found);
    entry->name = strdup("one_fp32");
    entry->unaryop = GxB_ONE_FP32;

    entry = unaryophash_insert(unaryophash, "lnot_fp64", &found);
    entry->name = strdup("lnot_fp64");
    entry->unaryop = GxB_LNOT_FP64;

    entry = unaryophash_insert(unaryophash, "one_fp64", &found);
    entry->name = strdup("one_fp64");
    entry->unaryop = GxB_ONE_FP64;

    entry = unaryophash_insert(unaryophash, "identity_fc32", &found);
    entry->name = strdup("identity_fc32");
    entry->unaryop = GxB_IDENTITY_FC32;

    entry = unaryophash_insert(unaryophash, "ainv_fc32", &found);
    entry->name = strdup("ainv_fc32");
    entry->unaryop = GxB_AINV_FC32;

    entry = unaryophash_insert(unaryophash, "minv_fc32", &found);
    entry->name = strdup("minv_fc32");
    entry->unaryop = GxB_MINV_FC32;

    entry = unaryophash_insert(unaryophash, "identity_fc64", &found);
    entry->name = strdup("identity_fc64");
    entry->unaryop = GxB_IDENTITY_FC64;

    entry = unaryophash_insert(unaryophash, "ainv_fc64", &found);
    entry->name = strdup("ainv_fc64");
    entry->unaryop = GxB_AINV_FC64;

    entry = unaryophash_insert(unaryophash, "minv_fc64", &found);
    entry->name = strdup("minv_fc64");
    entry->unaryop = GxB_MINV_FC64;

    entry = unaryophash_insert(unaryophash, "one_fc32", &found);
    entry->name = strdup("one_fc32");
    entry->unaryop = GxB_ONE_FC32;

    entry = unaryophash_insert(unaryophash, "one_fc64", &found);
    entry->name = strdup("one_fc64");
    entry->unaryop = GxB_ONE_FC64;

    entry = unaryophash_insert(unaryophash, "lnot", &found);
    entry->name = strdup("lnot");
    entry->unaryop = GrB_LNOT;

    entry = unaryophash_insert(unaryophash, "sqrt_fp32", &found);
    entry->name = strdup("sqrt_fp32");
    entry->unaryop = GxB_SQRT_FP32;

    entry = unaryophash_insert(unaryophash, "log_fp32", &found);
    entry->name = strdup("log_fp32");
    entry->unaryop = GxB_LOG_FP32;

    entry = unaryophash_insert(unaryophash, "exp_fp32", &found);
    entry->name = strdup("exp_fp32");
    entry->unaryop = GxB_EXP_FP32;

    entry = unaryophash_insert(unaryophash, "log2_fp32", &found);
    entry->name = strdup("log2_fp32");
    entry->unaryop = GxB_LOG2_FP32;

    entry = unaryophash_insert(unaryophash, "sqrt_fp64", &found);
    entry->name = strdup("sqrt_fp64");
    entry->unaryop = GxB_SQRT_FP64;

    entry = unaryophash_insert(unaryophash, "log_fp64", &found);
    entry->name = strdup("log_fp64");
    entry->unaryop = GxB_LOG_FP64;

    entry = unaryophash_insert(unaryophash, "exp_fp64", &found);
    entry->name = strdup("exp_fp64");
    entry->unaryop = GxB_EXP_FP64;

    entry = unaryophash_insert(unaryophash, "log2_fp64", &found);
    entry->name = strdup("log2_fp64");
    entry->unaryop = GxB_LOG2_FP64;

    entry = unaryophash_insert(unaryophash, "sqrt_fc32", &found);
    entry->name = strdup("sqrt_fc32");
    entry->unaryop = GxB_SQRT_FC32;

    entry = unaryophash_insert(unaryophash, "log_fc32", &found);
    entry->name = strdup("log_fc32");
    entry->unaryop = GxB_LOG_FC32;

    entry = unaryophash_insert(unaryophash, "exp_fc32", &found);
    entry->name = strdup("exp_fc32");
    entry->unaryop = GxB_EXP_FC32;

    entry = unaryophash_insert(unaryophash, "log2_fc32", &found);
    entry->name = strdup("log2_fc32");
    entry->unaryop = GxB_LOG2_FC32;

    entry = unaryophash_insert(unaryophash, "sqrt_fc64", &found);
    entry->name = strdup("sqrt_fc64");
    entry->unaryop = GxB_SQRT_FC64;

    entry = unaryophash_insert(unaryophash, "log_fc64", &found);
    entry->name = strdup("log_fc64");
    entry->unaryop = GxB_LOG_FC64;

    entry = unaryophash_insert(unaryophash, "exp_fc64", &found);
    entry->name = strdup("exp_fc64");
    entry->unaryop = GxB_EXP_FC64;

    entry = unaryophash_insert(unaryophash, "log2_fc64", &found);
    entry->name = strdup("log2_fc64");
    entry->unaryop = GxB_LOG2_FC64;

    entry = unaryophash_insert(unaryophash, "sin_fp32", &found);
    entry->name = strdup("sin_fp32");
    entry->unaryop = GxB_SIN_FP32;

    entry = unaryophash_insert(unaryophash, "cos_fp32", &found);
    entry->name = strdup("cos_fp32");
    entry->unaryop = GxB_COS_FP32;

    entry = unaryophash_insert(unaryophash, "tan_fp32", &found);
    entry->name = strdup("tan_fp32");
    entry->unaryop = GxB_TAN_FP32;

    entry = unaryophash_insert(unaryophash, "sin_fp64", &found);
    entry->name = strdup("sin_fp64");
    entry->unaryop = GxB_SIN_FP64;

    entry = unaryophash_insert(unaryophash, "cos_fp64", &found);
    entry->name = strdup("cos_fp64");
    entry->unaryop = GxB_COS_FP64;

    entry = unaryophash_insert(unaryophash, "tan_fp64", &found);
    entry->name = strdup("tan_fp64");
    entry->unaryop = GxB_TAN_FP64;

    entry = unaryophash_insert(unaryophash, "sin_fc32", &found);
    entry->name = strdup("sin_fc32");
    entry->unaryop = GxB_SIN_FC32;

    entry = unaryophash_insert(unaryophash, "cos_fc32", &found);
    entry->name = strdup("cos_fc32");
    entry->unaryop = GxB_COS_FC32;

    entry = unaryophash_insert(unaryophash, "tan_fc32", &found);
    entry->name = strdup("tan_fc32");
    entry->unaryop = GxB_TAN_FC32;

    entry = unaryophash_insert(unaryophash, "sin_fc64", &found);
    entry->name = strdup("sin_fc64");
    entry->unaryop = GxB_SIN_FC64;

    entry = unaryophash_insert(unaryophash, "cos_fc64", &found);
    entry->name = strdup("cos_fc64");
    entry->unaryop = GxB_COS_FC64;

    entry = unaryophash_insert(unaryophash, "tan_fc64", &found);
    entry->name = strdup("tan_fc64");
    entry->unaryop = GxB_TAN_FC64;

    entry = unaryophash_insert(unaryophash, "acos_fp32", &found);
    entry->name = strdup("acos_fp32");
    entry->unaryop = GxB_ACOS_FP32;

    entry = unaryophash_insert(unaryophash, "asin_fp32", &found);
    entry->name = strdup("asin_fp32");
    entry->unaryop = GxB_ASIN_FP32;

    entry = unaryophash_insert(unaryophash, "atan_fp32", &found);
    entry->name = strdup("atan_fp32");
    entry->unaryop = GxB_ATAN_FP32;

    entry = unaryophash_insert(unaryophash, "acos_fp64", &found);
    entry->name = strdup("acos_fp64");
    entry->unaryop = GxB_ACOS_FP64;

    entry = unaryophash_insert(unaryophash, "asin_fp64", &found);
    entry->name = strdup("asin_fp64");
    entry->unaryop = GxB_ASIN_FP64;

    entry = unaryophash_insert(unaryophash, "atan_fp64", &found);
    entry->name = strdup("atan_fp64");
    entry->unaryop = GxB_ATAN_FP64;

    entry = unaryophash_insert(unaryophash, "acos_fc32", &found);
    entry->name = strdup("acos_fc32");
    entry->unaryop = GxB_ACOS_FC32;

    entry = unaryophash_insert(unaryophash, "asin_fc32", &found);
    entry->name = strdup("asin_fc32");
    entry->unaryop = GxB_ASIN_FC32;

    entry = unaryophash_insert(unaryophash, "atan_fc32", &found);
    entry->name = strdup("atan_fc32");
    entry->unaryop = GxB_ATAN_FC32;

    entry = unaryophash_insert(unaryophash, "acos_fc64", &found);
    entry->name = strdup("acos_fc64");
    entry->unaryop = GxB_ACOS_FC64;

    entry = unaryophash_insert(unaryophash, "asin_fc64", &found);
    entry->name = strdup("asin_fc64");
    entry->unaryop = GxB_ASIN_FC64;

    entry = unaryophash_insert(unaryophash, "atan_fc64", &found);
    entry->name = strdup("atan_fc64");
    entry->unaryop = GxB_ATAN_FC64;

    entry = unaryophash_insert(unaryophash, "sinh_fp32", &found);
    entry->name = strdup("sinh_fp32");
    entry->unaryop = GxB_SINH_FP32;

    entry = unaryophash_insert(unaryophash, "cosh_fp32", &found);
    entry->name = strdup("cosh_fp32");
    entry->unaryop = GxB_COSH_FP32;

    entry = unaryophash_insert(unaryophash, "tanh_fp32", &found);
    entry->name = strdup("tanh_fp32");
    entry->unaryop = GxB_TANH_FP32;

    entry = unaryophash_insert(unaryophash, "sinh_fp64", &found);
    entry->name = strdup("sinh_fp64");
    entry->unaryop = GxB_SINH_FP64;

    entry = unaryophash_insert(unaryophash, "cosh_fp64", &found);
    entry->name = strdup("cosh_fp64");
    entry->unaryop = GxB_COSH_FP64;

    entry = unaryophash_insert(unaryophash, "tanh_fp64", &found);
    entry->name = strdup("tanh_fp64");
    entry->unaryop = GxB_TANH_FP64;

    entry = unaryophash_insert(unaryophash, "sinh_fc32", &found);
    entry->name = strdup("sinh_fc32");
    entry->unaryop = GxB_SINH_FC32;

    entry = unaryophash_insert(unaryophash, "cosh_fc32", &found);
    entry->name = strdup("cosh_fc32");
    entry->unaryop = GxB_COSH_FC32;

    entry = unaryophash_insert(unaryophash, "tanh_fc32", &found);
    entry->name = strdup("tanh_fc32");
    entry->unaryop = GxB_TANH_FC32;

    entry = unaryophash_insert(unaryophash, "sinh_fc64", &found);
    entry->name = strdup("sinh_fc64");
    entry->unaryop = GxB_SINH_FC64;

    entry = unaryophash_insert(unaryophash, "cosh_fc64", &found);
    entry->name = strdup("cosh_fc64");
    entry->unaryop = GxB_COSH_FC64;

    entry = unaryophash_insert(unaryophash, "tanh_fc64", &found);
    entry->name = strdup("tanh_fc64");
    entry->unaryop = GxB_TANH_FC64;

    entry = unaryophash_insert(unaryophash, "acosh_fp32", &found);
    entry->name = strdup("acosh_fp32");
    entry->unaryop = GxB_ACOSH_FP32;

    entry = unaryophash_insert(unaryophash, "asinh_fp32", &found);
    entry->name = strdup("asinh_fp32");
    entry->unaryop = GxB_ASINH_FP32;

    entry = unaryophash_insert(unaryophash, "atanh_fp32", &found);
    entry->name = strdup("atanh_fp32");
    entry->unaryop = GxB_ATANH_FP32;

    entry = unaryophash_insert(unaryophash, "signum_fp32", &found);
    entry->name = strdup("signum_fp32");
    entry->unaryop = GxB_SIGNUM_FP32;

    entry = unaryophash_insert(unaryophash, "acosh_fp64", &found);
    entry->name = strdup("acosh_fp64");
    entry->unaryop = GxB_ACOSH_FP64;

    entry = unaryophash_insert(unaryophash, "asinh_fp64", &found);
    entry->name = strdup("asinh_fp64");
    entry->unaryop = GxB_ASINH_FP64;

    entry = unaryophash_insert(unaryophash, "atanh_fp64", &found);
    entry->name = strdup("atanh_fp64");
    entry->unaryop = GxB_ATANH_FP64;

    entry = unaryophash_insert(unaryophash, "signum_fp64", &found);
    entry->name = strdup("signum_fp64");
    entry->unaryop = GxB_SIGNUM_FP64;

    entry = unaryophash_insert(unaryophash, "acosh_fc32", &found);
    entry->name = strdup("acosh_fc32");
    entry->unaryop = GxB_ACOSH_FC32;

    entry = unaryophash_insert(unaryophash, "asinh_fc32", &found);
    entry->name = strdup("asinh_fc32");
    entry->unaryop = GxB_ASINH_FC32;

    entry = unaryophash_insert(unaryophash, "atanh_fc32", &found);
    entry->name = strdup("atanh_fc32");
    entry->unaryop = GxB_ATANH_FC32;

    entry = unaryophash_insert(unaryophash, "signum_fc32", &found);
    entry->name = strdup("signum_fc32");
    entry->unaryop = GxB_SIGNUM_FC32;

    entry = unaryophash_insert(unaryophash, "acosh_fc64", &found);
    entry->name = strdup("acosh_fc64");
    entry->unaryop = GxB_ACOSH_FC64;

    entry = unaryophash_insert(unaryophash, "asinh_fc64", &found);
    entry->name = strdup("asinh_fc64");
    entry->unaryop = GxB_ASINH_FC64;

    entry = unaryophash_insert(unaryophash, "atanh_fc64", &found);
    entry->name = strdup("atanh_fc64");
    entry->unaryop = GxB_ATANH_FC64;

    entry = unaryophash_insert(unaryophash, "signum_fc64", &found);
    entry->name = strdup("signum_fc64");
    entry->unaryop = GxB_SIGNUM_FC64;

    entry = unaryophash_insert(unaryophash, "ceil_fp32", &found);
    entry->name = strdup("ceil_fp32");
    entry->unaryop = GxB_CEIL_FP32;

    entry = unaryophash_insert(unaryophash, "floor_fp32", &found);
    entry->name = strdup("floor_fp32");
    entry->unaryop = GxB_FLOOR_FP32;

    entry = unaryophash_insert(unaryophash, "round_fp32", &found);
    entry->name = strdup("round_fp32");
    entry->unaryop = GxB_ROUND_FP32;

    entry = unaryophash_insert(unaryophash, "trunc_fp32", &found);
    entry->name = strdup("trunc_fp32");
    entry->unaryop = GxB_TRUNC_FP32;

    entry = unaryophash_insert(unaryophash, "ceil_fp64", &found);
    entry->name = strdup("ceil_fp64");
    entry->unaryop = GxB_CEIL_FP64;

    entry = unaryophash_insert(unaryophash, "floor_fp64", &found);
    entry->name = strdup("floor_fp64");
    entry->unaryop = GxB_FLOOR_FP64;

    entry = unaryophash_insert(unaryophash, "round_fp64", &found);
    entry->name = strdup("round_fp64");
    entry->unaryop = GxB_ROUND_FP64;

    entry = unaryophash_insert(unaryophash, "trunc_fp64", &found);
    entry->name = strdup("trunc_fp64");
    entry->unaryop = GxB_TRUNC_FP64;

    entry = unaryophash_insert(unaryophash, "ceil_fc32", &found);
    entry->name = strdup("ceil_fc32");
    entry->unaryop = GxB_CEIL_FC32;

    entry = unaryophash_insert(unaryophash, "floor_fc32", &found);
    entry->name = strdup("floor_fc32");
    entry->unaryop = GxB_FLOOR_FC32;

    entry = unaryophash_insert(unaryophash, "round_fc32", &found);
    entry->name = strdup("round_fc32");
    entry->unaryop = GxB_ROUND_FC32;

    entry = unaryophash_insert(unaryophash, "trunc_fc32", &found);
    entry->name = strdup("trunc_fc32");
    entry->unaryop = GxB_TRUNC_FC32;

    entry = unaryophash_insert(unaryophash, "ceil_fc64", &found);
    entry->name = strdup("ceil_fc64");
    entry->unaryop = GxB_CEIL_FC64;

    entry = unaryophash_insert(unaryophash, "floor_fc64", &found);
    entry->name = strdup("floor_fc64");
    entry->unaryop = GxB_FLOOR_FC64;

    entry = unaryophash_insert(unaryophash, "round_fc64", &found);
    entry->name = strdup("round_fc64");
    entry->unaryop = GxB_ROUND_FC64;

    entry = unaryophash_insert(unaryophash, "trunc_fc64", &found);
    entry->name = strdup("trunc_fc64");
    entry->unaryop = GxB_TRUNC_FC64;

    entry = unaryophash_insert(unaryophash, "exp2_fp32", &found);
    entry->name = strdup("exp2_fp32");
    entry->unaryop = GxB_EXP2_FP32;

    entry = unaryophash_insert(unaryophash, "expm1_fp32", &found);
    entry->name = strdup("expm1_fp32");
    entry->unaryop = GxB_EXPM1_FP32;

    entry = unaryophash_insert(unaryophash, "log10_fp32", &found);
    entry->name = strdup("log10_fp32");
    entry->unaryop = GxB_LOG10_FP32;

    entry = unaryophash_insert(unaryophash, "log1p_fp32", &found);
    entry->name = strdup("log1p_fp32");
    entry->unaryop = GxB_LOG1P_FP32;

    entry = unaryophash_insert(unaryophash, "exp2_fp64", &found);
    entry->name = strdup("exp2_fp64");
    entry->unaryop = GxB_EXP2_FP64;

    entry = unaryophash_insert(unaryophash, "expm1_fp64", &found);
    entry->name = strdup("expm1_fp64");
    entry->unaryop = GxB_EXPM1_FP64;

    entry = unaryophash_insert(unaryophash, "log10_fp64", &found);
    entry->name = strdup("log10_fp64");
    entry->unaryop = GxB_LOG10_FP64;

    entry = unaryophash_insert(unaryophash, "log1p_fp64", &found);
    entry->name = strdup("log1p_fp64");
    entry->unaryop = GxB_LOG1P_FP64;

    entry = unaryophash_insert(unaryophash, "exp2_fc32", &found);
    entry->name = strdup("exp2_fc32");
    entry->unaryop = GxB_EXP2_FC32;

    entry = unaryophash_insert(unaryophash, "expm1_fc32", &found);
    entry->name = strdup("expm1_fc32");
    entry->unaryop = GxB_EXPM1_FC32;

    entry = unaryophash_insert(unaryophash, "log10_fc32", &found);
    entry->name = strdup("log10_fc32");
    entry->unaryop = GxB_LOG10_FC32;

    entry = unaryophash_insert(unaryophash, "log1p_fc32", &found);
    entry->name = strdup("log1p_fc32");
    entry->unaryop = GxB_LOG1P_FC32;

    entry = unaryophash_insert(unaryophash, "exp2_fc64", &found);
    entry->name = strdup("exp2_fc64");
    entry->unaryop = GxB_EXP2_FC64;

    entry = unaryophash_insert(unaryophash, "expm1_fc64", &found);
    entry->name = strdup("expm1_fc64");
    entry->unaryop = GxB_EXPM1_FC64;

    entry = unaryophash_insert(unaryophash, "log10_fc64", &found);
    entry->name = strdup("log10_fc64");
    entry->unaryop = GxB_LOG10_FC64;

    entry = unaryophash_insert(unaryophash, "log1p_fc64", &found);
    entry->name = strdup("log1p_fc64");
    entry->unaryop = GxB_LOG1P_FC64;

    entry = unaryophash_insert(unaryophash, "lgamma_fp32", &found);
    entry->name = strdup("lgamma_fp32");
    entry->unaryop = GxB_LGAMMA_FP32;

    entry = unaryophash_insert(unaryophash, "tgamma_fp32", &found);
    entry->name = strdup("tgamma_fp32");
    entry->unaryop = GxB_TGAMMA_FP32;

    entry = unaryophash_insert(unaryophash, "erf_fp32", &found);
    entry->name = strdup("erf_fp32");
    entry->unaryop = GxB_ERF_FP32;

    entry = unaryophash_insert(unaryophash, "erfc_fp32", &found);
    entry->name = strdup("erfc_fp32");
    entry->unaryop = GxB_ERFC_FP32;

    entry = unaryophash_insert(unaryophash, "lgamma_fp64", &found);
    entry->name = strdup("lgamma_fp64");
    entry->unaryop = GxB_LGAMMA_FP64;

    entry = unaryophash_insert(unaryophash, "tgamma_fp64", &found);
    entry->name = strdup("tgamma_fp64");
    entry->unaryop = GxB_TGAMMA_FP64;

    entry = unaryophash_insert(unaryophash, "erf_fp64", &found);
    entry->name = strdup("erf_fp64");
    entry->unaryop = GxB_ERF_FP64;

    entry = unaryophash_insert(unaryophash, "erfc_fp64", &found);
    entry->name = strdup("erfc_fp64");
    entry->unaryop = GxB_ERFC_FP64;

    entry = unaryophash_insert(unaryophash, "cbrt_fp32", &found);
    entry->name = strdup("cbrt_fp32");
    entry->unaryop = GxB_CBRT_FP32;

    entry = unaryophash_insert(unaryophash, "cbrt_fp64", &found);
    entry->name = strdup("cbrt_fp64");
    entry->unaryop = GxB_CBRT_FP64;

    entry = unaryophash_insert(unaryophash, "frexpx_fp32", &found);
    entry->name = strdup("frexpx_fp32");
    entry->unaryop = GxB_FREXPX_FP32;

    entry = unaryophash_insert(unaryophash, "frexpe_fp32", &found);
    entry->name = strdup("frexpe_fp32");
    entry->unaryop = GxB_FREXPE_FP32;

    entry = unaryophash_insert(unaryophash, "frexpx_fp64", &found);
    entry->name = strdup("frexpx_fp64");
    entry->unaryop = GxB_FREXPX_FP64;

    entry = unaryophash_insert(unaryophash, "frexpe_fp64", &found);
    entry->name = strdup("frexpe_fp64");
    entry->unaryop = GxB_FREXPE_FP64;

    entry = unaryophash_insert(unaryophash, "conj_fc32", &found);
    entry->name = strdup("conj_fc32");
    entry->unaryop = GxB_CONJ_FC32;

    entry = unaryophash_insert(unaryophash, "conj_fc64", &found);
    entry->name = strdup("conj_fc64");
    entry->unaryop = GxB_CONJ_FC64;

    entry = unaryophash_insert(unaryophash, "creal_fc32", &found);
    entry->name = strdup("creal_fc32");
    entry->unaryop = GxB_CREAL_FC32;

    entry = unaryophash_insert(unaryophash, "cimag_fc32", &found);
    entry->name = strdup("cimag_fc32");
    entry->unaryop = GxB_CIMAG_FC32;

    entry = unaryophash_insert(unaryophash, "carg_fc32", &found);
    entry->name = strdup("carg_fc32");
    entry->unaryop = GxB_CARG_FC32;

    entry = unaryophash_insert(unaryophash, "abs_fc32", &found);
    entry->name = strdup("abs_fc32");
    entry->unaryop = GxB_ABS_FC32;

    entry = unaryophash_insert(unaryophash, "creal_fc64", &found);
    entry->name = strdup("creal_fc64");
    entry->unaryop = GxB_CREAL_FC64;

    entry = unaryophash_insert(unaryophash, "cimag_fc64", &found);
    entry->name = strdup("cimag_fc64");
    entry->unaryop = GxB_CIMAG_FC64;

    entry = unaryophash_insert(unaryophash, "carg_fc64", &found);
    entry->name = strdup("carg_fc64");
    entry->unaryop = GxB_CARG_FC64;

    entry = unaryophash_insert(unaryophash, "abs_fc64", &found);
    entry->name = strdup("abs_fc64");
    entry->unaryop = GxB_ABS_FC64;

    entry = unaryophash_insert(unaryophash, "isinf_fp32", &found);
    entry->name = strdup("isinf_fp32");
    entry->unaryop = GxB_ISINF_FP32;

    entry = unaryophash_insert(unaryophash, "isinf_fp64", &found);
    entry->name = strdup("isinf_fp64");
    entry->unaryop = GxB_ISINF_FP64;

    entry = unaryophash_insert(unaryophash, "isinf_fc32", &found);
    entry->name = strdup("isinf_fc32");
    entry->unaryop = GxB_ISINF_FC32;

    entry = unaryophash_insert(unaryophash, "isinf_fc64", &found);
    entry->name = strdup("isinf_fc64");
    entry->unaryop = GxB_ISINF_FC64;

    entry = unaryophash_insert(unaryophash, "isnan_fp32", &found);
    entry->name = strdup("isnan_fp32");
    entry->unaryop = GxB_ISNAN_FP32;

    entry = unaryophash_insert(unaryophash, "isnan_fp64", &found);
    entry->name = strdup("isnan_fp64");
    entry->unaryop = GxB_ISNAN_FP64;

    entry = unaryophash_insert(unaryophash, "isnan_fc32", &found);
    entry->name = strdup("isnan_fc32");
    entry->unaryop = GxB_ISNAN_FC32;

    entry = unaryophash_insert(unaryophash, "isnan_fc64", &found);
    entry->name = strdup("isnan_fc64");
    entry->unaryop = GxB_ISNAN_FC64;

    entry = unaryophash_insert(unaryophash, "isfinite_fp32", &found);
    entry->name = strdup("isfinite_fp32");
    entry->unaryop = GxB_ISFINITE_FP32;

    entry = unaryophash_insert(unaryophash, "isfinite_fp64", &found);
    entry->name = strdup("isfinite_fp64");
    entry->unaryop = GxB_ISFINITE_FP64;

    entry = unaryophash_insert(unaryophash, "isfinite_fc32", &found);
    entry->name = strdup("isfinite_fc32");
    entry->unaryop = GxB_ISFINITE_FC32;

    entry = unaryophash_insert(unaryophash, "isfinite_fc64", &found);
    entry->name = strdup("isfinite_fc64");
    entry->unaryop = GxB_ISFINITE_FC64;

    entry = unaryophash_insert(unaryophash, "positioni_int32", &found);
    entry->name = strdup("positioni_int32");
    entry->unaryop = GxB_POSITIONI_INT32;

    entry = unaryophash_insert(unaryophash, "positioni_int64", &found);
    entry->name = strdup("positioni_int64");
    entry->unaryop = GxB_POSITIONI_INT64;

    entry = unaryophash_insert(unaryophash, "positioni1_int32", &found);
    entry->name = strdup("positioni1_int32");
    entry->unaryop = GxB_POSITIONI1_INT32;

    entry = unaryophash_insert(unaryophash, "positioni1_int64", &found);
    entry->name = strdup("positioni1_int64");
    entry->unaryop = GxB_POSITIONI1_INT64;

    entry = unaryophash_insert(unaryophash, "positionj_int32", &found);
    entry->name = strdup("positionj_int32");
    entry->unaryop = GxB_POSITIONJ_INT32;

    entry = unaryophash_insert(unaryophash, "positionj_int64", &found);
    entry->name = strdup("positionj_int64");
    entry->unaryop = GxB_POSITIONJ_INT64;

    entry = unaryophash_insert(unaryophash, "positionj1_int32", &found);
    entry->name = strdup("positionj1_int32");
    entry->unaryop = GxB_POSITIONJ1_INT32;

    entry = unaryophash_insert(unaryophash, "positionj1_int64", &found);
    entry->name = strdup("positionj1_int64");
    entry->unaryop = GxB_POSITIONJ1_INT64;

    entry = unaryophash_insert(unaryophash, "abs_bool", &found);
    entry->name = strdup("abs_bool");
    entry->unaryop = GxB_ABS_BOOL;

    entry = unaryophash_insert(unaryophash, "abs_int8", &found);
    entry->name = strdup("abs_int8");
    entry->unaryop = GxB_ABS_INT8;

    entry = unaryophash_insert(unaryophash, "abs_int16", &found);
    entry->name = strdup("abs_int16");
    entry->unaryop = GxB_ABS_INT16;

    entry = unaryophash_insert(unaryophash, "abs_int32", &found);
    entry->name = strdup("abs_int32");
    entry->unaryop = GxB_ABS_INT32;

    entry = unaryophash_insert(unaryophash, "abs_int64", &found);
    entry->name = strdup("abs_int64");
    entry->unaryop = GxB_ABS_INT64;

    entry = unaryophash_insert(unaryophash, "abs_uint8", &found);
    entry->name = strdup("abs_uint8");
    entry->unaryop = GxB_ABS_UINT8;

    entry = unaryophash_insert(unaryophash, "abs_uint16", &found);
    entry->name = strdup("abs_uint16");
    entry->unaryop = GxB_ABS_UINT16;

    entry = unaryophash_insert(unaryophash, "abs_uint32", &found);
    entry->name = strdup("abs_uint32");
    entry->unaryop = GxB_ABS_UINT32;

    entry = unaryophash_insert(unaryophash, "abs_uint64", &found);
    entry->name = strdup("abs_uint64");
    entry->unaryop = GxB_ABS_UINT64;

    entry = unaryophash_insert(unaryophash, "abs_fp32", &found);
    entry->name = strdup("abs_fp32");
    entry->unaryop = GxB_ABS_FP32;

    entry = unaryophash_insert(unaryophash, "abs_fp64", &found);
    entry->name = strdup("abs_fp64");
    entry->unaryop = GxB_ABS_FP64;
    }

GrB_UnaryOp lookup_unaryop(char *name)
{
    unaryop_entry *entry;
	entry = unaryophash_lookup(unaryophash, name);
    if (entry)
        return entry->unaryop;
    return NULL;
    }
