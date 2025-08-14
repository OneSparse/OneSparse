typedef struct binaryop_entry
{
	uint32 status;
	char *name;
    GrB_BinaryOp binaryop;
} binaryop_entry;

static uint32 hash_string_pointer(char *s);

static uint32
hash_string_pointer(char *s)
{
	unsigned char *ss = (unsigned char *) s;

	return hash_bytes(ss, strlen(s));
    }

#define BINARYOPHASH_INITIAL_SIZE 2000

#define SH_PREFIX		binaryophash
#define SH_ELEMENT_TYPE binaryop_entry
#define SH_KEY_TYPE		char *
#define	SH_KEY			name
#define SH_HASH_KEY(tb, key)	hash_string_pointer(key)
#define SH_EQUAL(tb, a, b)		(strcmp(a, b) == 0)
#define	SH_SCOPE		static inline
#define SH_DECLARE
#define SH_DEFINE
#include "lib/simplehash.h"

static binaryophash_hash *binaryophash;

void initialize_binaryops()
{
    bool found;
    binaryop_entry *entry;

    binaryophash = binaryophash_create(TopMemoryContext, BINARYOPHASH_INITIAL_SIZE, NULL);
    
    entry = binaryophash_insert(binaryophash, "first_bool", &found);
    entry->name = strdup("first_bool");
    entry->binaryop = GrB_FIRST_BOOL;

    entry = binaryophash_insert(binaryophash, "second_bool", &found);
    entry->name = strdup("second_bool");
    entry->binaryop = GrB_SECOND_BOOL;

    entry = binaryophash_insert(binaryophash, "oneb_bool", &found);
    entry->name = strdup("oneb_bool");
    entry->binaryop = GrB_ONEB_BOOL;

    entry = binaryophash_insert(binaryophash, "first_int8", &found);
    entry->name = strdup("first_int8");
    entry->binaryop = GrB_FIRST_INT8;

    entry = binaryophash_insert(binaryophash, "second_int8", &found);
    entry->name = strdup("second_int8");
    entry->binaryop = GrB_SECOND_INT8;

    entry = binaryophash_insert(binaryophash, "oneb_int8", &found);
    entry->name = strdup("oneb_int8");
    entry->binaryop = GrB_ONEB_INT8;

    entry = binaryophash_insert(binaryophash, "first_int16", &found);
    entry->name = strdup("first_int16");
    entry->binaryop = GrB_FIRST_INT16;

    entry = binaryophash_insert(binaryophash, "second_int16", &found);
    entry->name = strdup("second_int16");
    entry->binaryop = GrB_SECOND_INT16;

    entry = binaryophash_insert(binaryophash, "oneb_int16", &found);
    entry->name = strdup("oneb_int16");
    entry->binaryop = GrB_ONEB_INT16;

    entry = binaryophash_insert(binaryophash, "first_int32", &found);
    entry->name = strdup("first_int32");
    entry->binaryop = GrB_FIRST_INT32;

    entry = binaryophash_insert(binaryophash, "second_int32", &found);
    entry->name = strdup("second_int32");
    entry->binaryop = GrB_SECOND_INT32;

    entry = binaryophash_insert(binaryophash, "oneb_int32", &found);
    entry->name = strdup("oneb_int32");
    entry->binaryop = GrB_ONEB_INT32;

    entry = binaryophash_insert(binaryophash, "first_int64", &found);
    entry->name = strdup("first_int64");
    entry->binaryop = GrB_FIRST_INT64;

    entry = binaryophash_insert(binaryophash, "second_int64", &found);
    entry->name = strdup("second_int64");
    entry->binaryop = GrB_SECOND_INT64;

    entry = binaryophash_insert(binaryophash, "oneb_int64", &found);
    entry->name = strdup("oneb_int64");
    entry->binaryop = GrB_ONEB_INT64;

    entry = binaryophash_insert(binaryophash, "first_uint8", &found);
    entry->name = strdup("first_uint8");
    entry->binaryop = GrB_FIRST_UINT8;

    entry = binaryophash_insert(binaryophash, "second_uint8", &found);
    entry->name = strdup("second_uint8");
    entry->binaryop = GrB_SECOND_UINT8;

    entry = binaryophash_insert(binaryophash, "oneb_uint8", &found);
    entry->name = strdup("oneb_uint8");
    entry->binaryop = GrB_ONEB_UINT8;

    entry = binaryophash_insert(binaryophash, "first_uint16", &found);
    entry->name = strdup("first_uint16");
    entry->binaryop = GrB_FIRST_UINT16;

    entry = binaryophash_insert(binaryophash, "second_uint16", &found);
    entry->name = strdup("second_uint16");
    entry->binaryop = GrB_SECOND_UINT16;

    entry = binaryophash_insert(binaryophash, "oneb_uint16", &found);
    entry->name = strdup("oneb_uint16");
    entry->binaryop = GrB_ONEB_UINT16;

    entry = binaryophash_insert(binaryophash, "first_uint32", &found);
    entry->name = strdup("first_uint32");
    entry->binaryop = GrB_FIRST_UINT32;

    entry = binaryophash_insert(binaryophash, "second_uint32", &found);
    entry->name = strdup("second_uint32");
    entry->binaryop = GrB_SECOND_UINT32;

    entry = binaryophash_insert(binaryophash, "oneb_uint32", &found);
    entry->name = strdup("oneb_uint32");
    entry->binaryop = GrB_ONEB_UINT32;

    entry = binaryophash_insert(binaryophash, "first_uint64", &found);
    entry->name = strdup("first_uint64");
    entry->binaryop = GrB_FIRST_UINT64;

    entry = binaryophash_insert(binaryophash, "second_uint64", &found);
    entry->name = strdup("second_uint64");
    entry->binaryop = GrB_SECOND_UINT64;

    entry = binaryophash_insert(binaryophash, "oneb_uint64", &found);
    entry->name = strdup("oneb_uint64");
    entry->binaryop = GrB_ONEB_UINT64;

    entry = binaryophash_insert(binaryophash, "first_fp32", &found);
    entry->name = strdup("first_fp32");
    entry->binaryop = GrB_FIRST_FP32;

    entry = binaryophash_insert(binaryophash, "second_fp32", &found);
    entry->name = strdup("second_fp32");
    entry->binaryop = GrB_SECOND_FP32;

    entry = binaryophash_insert(binaryophash, "oneb_fp32", &found);
    entry->name = strdup("oneb_fp32");
    entry->binaryop = GrB_ONEB_FP32;

    entry = binaryophash_insert(binaryophash, "first_fp64", &found);
    entry->name = strdup("first_fp64");
    entry->binaryop = GrB_FIRST_FP64;

    entry = binaryophash_insert(binaryophash, "second_fp64", &found);
    entry->name = strdup("second_fp64");
    entry->binaryop = GrB_SECOND_FP64;

    entry = binaryophash_insert(binaryophash, "oneb_fp64", &found);
    entry->name = strdup("oneb_fp64");
    entry->binaryop = GrB_ONEB_FP64;

    entry = binaryophash_insert(binaryophash, "first_fc32", &found);
    entry->name = strdup("first_fc32");
    entry->binaryop = GxB_FIRST_FC32;

    entry = binaryophash_insert(binaryophash, "second_fc32", &found);
    entry->name = strdup("second_fc32");
    entry->binaryop = GxB_SECOND_FC32;

    entry = binaryophash_insert(binaryophash, "oneb_fc32", &found);
    entry->name = strdup("oneb_fc32");
    entry->binaryop = GxB_ONEB_FC32;

    entry = binaryophash_insert(binaryophash, "first_fc64", &found);
    entry->name = strdup("first_fc64");
    entry->binaryop = GxB_FIRST_FC64;

    entry = binaryophash_insert(binaryophash, "second_fc64", &found);
    entry->name = strdup("second_fc64");
    entry->binaryop = GxB_SECOND_FC64;

    entry = binaryophash_insert(binaryophash, "oneb_fc64", &found);
    entry->name = strdup("oneb_fc64");
    entry->binaryop = GxB_ONEB_FC64;

    entry = binaryophash_insert(binaryophash, "pow_bool", &found);
    entry->name = strdup("pow_bool");
    entry->binaryop = GxB_POW_BOOL;

    entry = binaryophash_insert(binaryophash, "pow_int8", &found);
    entry->name = strdup("pow_int8");
    entry->binaryop = GxB_POW_INT8;

    entry = binaryophash_insert(binaryophash, "pow_int16", &found);
    entry->name = strdup("pow_int16");
    entry->binaryop = GxB_POW_INT16;

    entry = binaryophash_insert(binaryophash, "pow_int32", &found);
    entry->name = strdup("pow_int32");
    entry->binaryop = GxB_POW_INT32;

    entry = binaryophash_insert(binaryophash, "pow_int64", &found);
    entry->name = strdup("pow_int64");
    entry->binaryop = GxB_POW_INT64;

    entry = binaryophash_insert(binaryophash, "pow_uint8", &found);
    entry->name = strdup("pow_uint8");
    entry->binaryop = GxB_POW_UINT8;

    entry = binaryophash_insert(binaryophash, "pow_uint16", &found);
    entry->name = strdup("pow_uint16");
    entry->binaryop = GxB_POW_UINT16;

    entry = binaryophash_insert(binaryophash, "pow_uint32", &found);
    entry->name = strdup("pow_uint32");
    entry->binaryop = GxB_POW_UINT32;

    entry = binaryophash_insert(binaryophash, "pow_uint64", &found);
    entry->name = strdup("pow_uint64");
    entry->binaryop = GxB_POW_UINT64;

    entry = binaryophash_insert(binaryophash, "pow_fp32", &found);
    entry->name = strdup("pow_fp32");
    entry->binaryop = GxB_POW_FP32;

    entry = binaryophash_insert(binaryophash, "pow_fp64", &found);
    entry->name = strdup("pow_fp64");
    entry->binaryop = GxB_POW_FP64;

    entry = binaryophash_insert(binaryophash, "pow_fc32", &found);
    entry->name = strdup("pow_fc32");
    entry->binaryop = GxB_POW_FC32;

    entry = binaryophash_insert(binaryophash, "pow_fc64", &found);
    entry->name = strdup("pow_fc64");
    entry->binaryop = GxB_POW_FC64;

    entry = binaryophash_insert(binaryophash, "plus_bool", &found);
    entry->name = strdup("plus_bool");
    entry->binaryop = GrB_PLUS_BOOL;

    entry = binaryophash_insert(binaryophash, "minus_bool", &found);
    entry->name = strdup("minus_bool");
    entry->binaryop = GrB_MINUS_BOOL;

    entry = binaryophash_insert(binaryophash, "times_bool", &found);
    entry->name = strdup("times_bool");
    entry->binaryop = GrB_TIMES_BOOL;

    entry = binaryophash_insert(binaryophash, "div_bool", &found);
    entry->name = strdup("div_bool");
    entry->binaryop = GrB_DIV_BOOL;

    entry = binaryophash_insert(binaryophash, "plus_int8", &found);
    entry->name = strdup("plus_int8");
    entry->binaryop = GrB_PLUS_INT8;

    entry = binaryophash_insert(binaryophash, "minus_int8", &found);
    entry->name = strdup("minus_int8");
    entry->binaryop = GrB_MINUS_INT8;

    entry = binaryophash_insert(binaryophash, "times_int8", &found);
    entry->name = strdup("times_int8");
    entry->binaryop = GrB_TIMES_INT8;

    entry = binaryophash_insert(binaryophash, "div_int8", &found);
    entry->name = strdup("div_int8");
    entry->binaryop = GrB_DIV_INT8;

    entry = binaryophash_insert(binaryophash, "plus_int16", &found);
    entry->name = strdup("plus_int16");
    entry->binaryop = GrB_PLUS_INT16;

    entry = binaryophash_insert(binaryophash, "minus_int16", &found);
    entry->name = strdup("minus_int16");
    entry->binaryop = GrB_MINUS_INT16;

    entry = binaryophash_insert(binaryophash, "times_int16", &found);
    entry->name = strdup("times_int16");
    entry->binaryop = GrB_TIMES_INT16;

    entry = binaryophash_insert(binaryophash, "div_int16", &found);
    entry->name = strdup("div_int16");
    entry->binaryop = GrB_DIV_INT16;

    entry = binaryophash_insert(binaryophash, "plus_int32", &found);
    entry->name = strdup("plus_int32");
    entry->binaryop = GrB_PLUS_INT32;

    entry = binaryophash_insert(binaryophash, "minus_int32", &found);
    entry->name = strdup("minus_int32");
    entry->binaryop = GrB_MINUS_INT32;

    entry = binaryophash_insert(binaryophash, "times_int32", &found);
    entry->name = strdup("times_int32");
    entry->binaryop = GrB_TIMES_INT32;

    entry = binaryophash_insert(binaryophash, "div_int32", &found);
    entry->name = strdup("div_int32");
    entry->binaryop = GrB_DIV_INT32;

    entry = binaryophash_insert(binaryophash, "plus_int64", &found);
    entry->name = strdup("plus_int64");
    entry->binaryop = GrB_PLUS_INT64;

    entry = binaryophash_insert(binaryophash, "minus_int64", &found);
    entry->name = strdup("minus_int64");
    entry->binaryop = GrB_MINUS_INT64;

    entry = binaryophash_insert(binaryophash, "times_int64", &found);
    entry->name = strdup("times_int64");
    entry->binaryop = GrB_TIMES_INT64;

    entry = binaryophash_insert(binaryophash, "div_int64", &found);
    entry->name = strdup("div_int64");
    entry->binaryop = GrB_DIV_INT64;

    entry = binaryophash_insert(binaryophash, "plus_uint8", &found);
    entry->name = strdup("plus_uint8");
    entry->binaryop = GrB_PLUS_UINT8;

    entry = binaryophash_insert(binaryophash, "minus_uint8", &found);
    entry->name = strdup("minus_uint8");
    entry->binaryop = GrB_MINUS_UINT8;

    entry = binaryophash_insert(binaryophash, "times_uint8", &found);
    entry->name = strdup("times_uint8");
    entry->binaryop = GrB_TIMES_UINT8;

    entry = binaryophash_insert(binaryophash, "div_uint8", &found);
    entry->name = strdup("div_uint8");
    entry->binaryop = GrB_DIV_UINT8;

    entry = binaryophash_insert(binaryophash, "plus_uint16", &found);
    entry->name = strdup("plus_uint16");
    entry->binaryop = GrB_PLUS_UINT16;

    entry = binaryophash_insert(binaryophash, "minus_uint16", &found);
    entry->name = strdup("minus_uint16");
    entry->binaryop = GrB_MINUS_UINT16;

    entry = binaryophash_insert(binaryophash, "times_uint16", &found);
    entry->name = strdup("times_uint16");
    entry->binaryop = GrB_TIMES_UINT16;

    entry = binaryophash_insert(binaryophash, "div_uint16", &found);
    entry->name = strdup("div_uint16");
    entry->binaryop = GrB_DIV_UINT16;

    entry = binaryophash_insert(binaryophash, "plus_uint32", &found);
    entry->name = strdup("plus_uint32");
    entry->binaryop = GrB_PLUS_UINT32;

    entry = binaryophash_insert(binaryophash, "minus_uint32", &found);
    entry->name = strdup("minus_uint32");
    entry->binaryop = GrB_MINUS_UINT32;

    entry = binaryophash_insert(binaryophash, "times_uint32", &found);
    entry->name = strdup("times_uint32");
    entry->binaryop = GrB_TIMES_UINT32;

    entry = binaryophash_insert(binaryophash, "div_uint32", &found);
    entry->name = strdup("div_uint32");
    entry->binaryop = GrB_DIV_UINT32;

    entry = binaryophash_insert(binaryophash, "plus_uint64", &found);
    entry->name = strdup("plus_uint64");
    entry->binaryop = GrB_PLUS_UINT64;

    entry = binaryophash_insert(binaryophash, "minus_uint64", &found);
    entry->name = strdup("minus_uint64");
    entry->binaryop = GrB_MINUS_UINT64;

    entry = binaryophash_insert(binaryophash, "times_uint64", &found);
    entry->name = strdup("times_uint64");
    entry->binaryop = GrB_TIMES_UINT64;

    entry = binaryophash_insert(binaryophash, "div_uint64", &found);
    entry->name = strdup("div_uint64");
    entry->binaryop = GrB_DIV_UINT64;

    entry = binaryophash_insert(binaryophash, "plus_fp32", &found);
    entry->name = strdup("plus_fp32");
    entry->binaryop = GrB_PLUS_FP32;

    entry = binaryophash_insert(binaryophash, "minus_fp32", &found);
    entry->name = strdup("minus_fp32");
    entry->binaryop = GrB_MINUS_FP32;

    entry = binaryophash_insert(binaryophash, "times_fp32", &found);
    entry->name = strdup("times_fp32");
    entry->binaryop = GrB_TIMES_FP32;

    entry = binaryophash_insert(binaryophash, "div_fp32", &found);
    entry->name = strdup("div_fp32");
    entry->binaryop = GrB_DIV_FP32;

    entry = binaryophash_insert(binaryophash, "plus_fp64", &found);
    entry->name = strdup("plus_fp64");
    entry->binaryop = GrB_PLUS_FP64;

    entry = binaryophash_insert(binaryophash, "minus_fp64", &found);
    entry->name = strdup("minus_fp64");
    entry->binaryop = GrB_MINUS_FP64;

    entry = binaryophash_insert(binaryophash, "times_fp64", &found);
    entry->name = strdup("times_fp64");
    entry->binaryop = GrB_TIMES_FP64;

    entry = binaryophash_insert(binaryophash, "div_fp64", &found);
    entry->name = strdup("div_fp64");
    entry->binaryop = GrB_DIV_FP64;

    entry = binaryophash_insert(binaryophash, "plus_fc32", &found);
    entry->name = strdup("plus_fc32");
    entry->binaryop = GxB_PLUS_FC32;

    entry = binaryophash_insert(binaryophash, "minus_fc32", &found);
    entry->name = strdup("minus_fc32");
    entry->binaryop = GxB_MINUS_FC32;

    entry = binaryophash_insert(binaryophash, "times_fc32", &found);
    entry->name = strdup("times_fc32");
    entry->binaryop = GxB_TIMES_FC32;

    entry = binaryophash_insert(binaryophash, "div_fc32", &found);
    entry->name = strdup("div_fc32");
    entry->binaryop = GxB_DIV_FC32;

    entry = binaryophash_insert(binaryophash, "plus_fc64", &found);
    entry->name = strdup("plus_fc64");
    entry->binaryop = GxB_PLUS_FC64;

    entry = binaryophash_insert(binaryophash, "minus_fc64", &found);
    entry->name = strdup("minus_fc64");
    entry->binaryop = GxB_MINUS_FC64;

    entry = binaryophash_insert(binaryophash, "times_fc64", &found);
    entry->name = strdup("times_fc64");
    entry->binaryop = GxB_TIMES_FC64;

    entry = binaryophash_insert(binaryophash, "div_fc64", &found);
    entry->name = strdup("div_fc64");
    entry->binaryop = GxB_DIV_FC64;

    entry = binaryophash_insert(binaryophash, "rminus_bool", &found);
    entry->name = strdup("rminus_bool");
    entry->binaryop = GxB_RMINUS_BOOL;

    entry = binaryophash_insert(binaryophash, "rdiv_bool", &found);
    entry->name = strdup("rdiv_bool");
    entry->binaryop = GxB_RDIV_BOOL;

    entry = binaryophash_insert(binaryophash, "pair_bool", &found);
    entry->name = strdup("pair_bool");
    entry->binaryop = GxB_PAIR_BOOL;

    entry = binaryophash_insert(binaryophash, "any_bool", &found);
    entry->name = strdup("any_bool");
    entry->binaryop = GxB_ANY_BOOL;

    entry = binaryophash_insert(binaryophash, "rminus_int8", &found);
    entry->name = strdup("rminus_int8");
    entry->binaryop = GxB_RMINUS_INT8;

    entry = binaryophash_insert(binaryophash, "rdiv_int8", &found);
    entry->name = strdup("rdiv_int8");
    entry->binaryop = GxB_RDIV_INT8;

    entry = binaryophash_insert(binaryophash, "pair_int8", &found);
    entry->name = strdup("pair_int8");
    entry->binaryop = GxB_PAIR_INT8;

    entry = binaryophash_insert(binaryophash, "any_int8", &found);
    entry->name = strdup("any_int8");
    entry->binaryop = GxB_ANY_INT8;

    entry = binaryophash_insert(binaryophash, "rminus_int16", &found);
    entry->name = strdup("rminus_int16");
    entry->binaryop = GxB_RMINUS_INT16;

    entry = binaryophash_insert(binaryophash, "rdiv_int16", &found);
    entry->name = strdup("rdiv_int16");
    entry->binaryop = GxB_RDIV_INT16;

    entry = binaryophash_insert(binaryophash, "pair_int16", &found);
    entry->name = strdup("pair_int16");
    entry->binaryop = GxB_PAIR_INT16;

    entry = binaryophash_insert(binaryophash, "any_int16", &found);
    entry->name = strdup("any_int16");
    entry->binaryop = GxB_ANY_INT16;

    entry = binaryophash_insert(binaryophash, "rminus_int32", &found);
    entry->name = strdup("rminus_int32");
    entry->binaryop = GxB_RMINUS_INT32;

    entry = binaryophash_insert(binaryophash, "rdiv_int32", &found);
    entry->name = strdup("rdiv_int32");
    entry->binaryop = GxB_RDIV_INT32;

    entry = binaryophash_insert(binaryophash, "pair_int32", &found);
    entry->name = strdup("pair_int32");
    entry->binaryop = GxB_PAIR_INT32;

    entry = binaryophash_insert(binaryophash, "any_int32", &found);
    entry->name = strdup("any_int32");
    entry->binaryop = GxB_ANY_INT32;

    entry = binaryophash_insert(binaryophash, "rminus_int64", &found);
    entry->name = strdup("rminus_int64");
    entry->binaryop = GxB_RMINUS_INT64;

    entry = binaryophash_insert(binaryophash, "rdiv_int64", &found);
    entry->name = strdup("rdiv_int64");
    entry->binaryop = GxB_RDIV_INT64;

    entry = binaryophash_insert(binaryophash, "pair_int64", &found);
    entry->name = strdup("pair_int64");
    entry->binaryop = GxB_PAIR_INT64;

    entry = binaryophash_insert(binaryophash, "any_int64", &found);
    entry->name = strdup("any_int64");
    entry->binaryop = GxB_ANY_INT64;

    entry = binaryophash_insert(binaryophash, "rminus_uint8", &found);
    entry->name = strdup("rminus_uint8");
    entry->binaryop = GxB_RMINUS_UINT8;

    entry = binaryophash_insert(binaryophash, "rdiv_uint8", &found);
    entry->name = strdup("rdiv_uint8");
    entry->binaryop = GxB_RDIV_UINT8;

    entry = binaryophash_insert(binaryophash, "pair_uint8", &found);
    entry->name = strdup("pair_uint8");
    entry->binaryop = GxB_PAIR_UINT8;

    entry = binaryophash_insert(binaryophash, "any_uint8", &found);
    entry->name = strdup("any_uint8");
    entry->binaryop = GxB_ANY_UINT8;

    entry = binaryophash_insert(binaryophash, "rminus_uint16", &found);
    entry->name = strdup("rminus_uint16");
    entry->binaryop = GxB_RMINUS_UINT16;

    entry = binaryophash_insert(binaryophash, "rdiv_uint16", &found);
    entry->name = strdup("rdiv_uint16");
    entry->binaryop = GxB_RDIV_UINT16;

    entry = binaryophash_insert(binaryophash, "pair_uint16", &found);
    entry->name = strdup("pair_uint16");
    entry->binaryop = GxB_PAIR_UINT16;

    entry = binaryophash_insert(binaryophash, "any_uint16", &found);
    entry->name = strdup("any_uint16");
    entry->binaryop = GxB_ANY_UINT16;

    entry = binaryophash_insert(binaryophash, "rminus_uint32", &found);
    entry->name = strdup("rminus_uint32");
    entry->binaryop = GxB_RMINUS_UINT32;

    entry = binaryophash_insert(binaryophash, "rdiv_uint32", &found);
    entry->name = strdup("rdiv_uint32");
    entry->binaryop = GxB_RDIV_UINT32;

    entry = binaryophash_insert(binaryophash, "pair_uint32", &found);
    entry->name = strdup("pair_uint32");
    entry->binaryop = GxB_PAIR_UINT32;

    entry = binaryophash_insert(binaryophash, "any_uint32", &found);
    entry->name = strdup("any_uint32");
    entry->binaryop = GxB_ANY_UINT32;

    entry = binaryophash_insert(binaryophash, "rminus_uint64", &found);
    entry->name = strdup("rminus_uint64");
    entry->binaryop = GxB_RMINUS_UINT64;

    entry = binaryophash_insert(binaryophash, "rdiv_uint64", &found);
    entry->name = strdup("rdiv_uint64");
    entry->binaryop = GxB_RDIV_UINT64;

    entry = binaryophash_insert(binaryophash, "pair_uint64", &found);
    entry->name = strdup("pair_uint64");
    entry->binaryop = GxB_PAIR_UINT64;

    entry = binaryophash_insert(binaryophash, "any_uint64", &found);
    entry->name = strdup("any_uint64");
    entry->binaryop = GxB_ANY_UINT64;

    entry = binaryophash_insert(binaryophash, "rminus_fp32", &found);
    entry->name = strdup("rminus_fp32");
    entry->binaryop = GxB_RMINUS_FP32;

    entry = binaryophash_insert(binaryophash, "rdiv_fp32", &found);
    entry->name = strdup("rdiv_fp32");
    entry->binaryop = GxB_RDIV_FP32;

    entry = binaryophash_insert(binaryophash, "pair_fp32", &found);
    entry->name = strdup("pair_fp32");
    entry->binaryop = GxB_PAIR_FP32;

    entry = binaryophash_insert(binaryophash, "any_fp32", &found);
    entry->name = strdup("any_fp32");
    entry->binaryop = GxB_ANY_FP32;

    entry = binaryophash_insert(binaryophash, "rminus_fp64", &found);
    entry->name = strdup("rminus_fp64");
    entry->binaryop = GxB_RMINUS_FP64;

    entry = binaryophash_insert(binaryophash, "rdiv_fp64", &found);
    entry->name = strdup("rdiv_fp64");
    entry->binaryop = GxB_RDIV_FP64;

    entry = binaryophash_insert(binaryophash, "pair_fp64", &found);
    entry->name = strdup("pair_fp64");
    entry->binaryop = GxB_PAIR_FP64;

    entry = binaryophash_insert(binaryophash, "any_fp64", &found);
    entry->name = strdup("any_fp64");
    entry->binaryop = GxB_ANY_FP64;

    entry = binaryophash_insert(binaryophash, "rminus_fc32", &found);
    entry->name = strdup("rminus_fc32");
    entry->binaryop = GxB_RMINUS_FC32;

    entry = binaryophash_insert(binaryophash, "rdiv_fc32", &found);
    entry->name = strdup("rdiv_fc32");
    entry->binaryop = GxB_RDIV_FC32;

    entry = binaryophash_insert(binaryophash, "pair_fc32", &found);
    entry->name = strdup("pair_fc32");
    entry->binaryop = GxB_PAIR_FC32;

    entry = binaryophash_insert(binaryophash, "any_fc32", &found);
    entry->name = strdup("any_fc32");
    entry->binaryop = GxB_ANY_FC32;

    entry = binaryophash_insert(binaryophash, "rminus_fc64", &found);
    entry->name = strdup("rminus_fc64");
    entry->binaryop = GxB_RMINUS_FC64;

    entry = binaryophash_insert(binaryophash, "rdiv_fc64", &found);
    entry->name = strdup("rdiv_fc64");
    entry->binaryop = GxB_RDIV_FC64;

    entry = binaryophash_insert(binaryophash, "pair_fc64", &found);
    entry->name = strdup("pair_fc64");
    entry->binaryop = GxB_PAIR_FC64;

    entry = binaryophash_insert(binaryophash, "any_fc64", &found);
    entry->name = strdup("any_fc64");
    entry->binaryop = GxB_ANY_FC64;

    entry = binaryophash_insert(binaryophash, "iseq_bool", &found);
    entry->name = strdup("iseq_bool");
    entry->binaryop = GxB_ISEQ_BOOL;

    entry = binaryophash_insert(binaryophash, "isne_bool", &found);
    entry->name = strdup("isne_bool");
    entry->binaryop = GxB_ISNE_BOOL;

    entry = binaryophash_insert(binaryophash, "iseq_int8", &found);
    entry->name = strdup("iseq_int8");
    entry->binaryop = GxB_ISEQ_INT8;

    entry = binaryophash_insert(binaryophash, "isne_int8", &found);
    entry->name = strdup("isne_int8");
    entry->binaryop = GxB_ISNE_INT8;

    entry = binaryophash_insert(binaryophash, "iseq_int16", &found);
    entry->name = strdup("iseq_int16");
    entry->binaryop = GxB_ISEQ_INT16;

    entry = binaryophash_insert(binaryophash, "isne_int16", &found);
    entry->name = strdup("isne_int16");
    entry->binaryop = GxB_ISNE_INT16;

    entry = binaryophash_insert(binaryophash, "iseq_int32", &found);
    entry->name = strdup("iseq_int32");
    entry->binaryop = GxB_ISEQ_INT32;

    entry = binaryophash_insert(binaryophash, "isne_int32", &found);
    entry->name = strdup("isne_int32");
    entry->binaryop = GxB_ISNE_INT32;

    entry = binaryophash_insert(binaryophash, "iseq_int64", &found);
    entry->name = strdup("iseq_int64");
    entry->binaryop = GxB_ISEQ_INT64;

    entry = binaryophash_insert(binaryophash, "isne_int64", &found);
    entry->name = strdup("isne_int64");
    entry->binaryop = GxB_ISNE_INT64;

    entry = binaryophash_insert(binaryophash, "iseq_uint8", &found);
    entry->name = strdup("iseq_uint8");
    entry->binaryop = GxB_ISEQ_UINT8;

    entry = binaryophash_insert(binaryophash, "isne_uint8", &found);
    entry->name = strdup("isne_uint8");
    entry->binaryop = GxB_ISNE_UINT8;

    entry = binaryophash_insert(binaryophash, "iseq_uint16", &found);
    entry->name = strdup("iseq_uint16");
    entry->binaryop = GxB_ISEQ_UINT16;

    entry = binaryophash_insert(binaryophash, "isne_uint16", &found);
    entry->name = strdup("isne_uint16");
    entry->binaryop = GxB_ISNE_UINT16;

    entry = binaryophash_insert(binaryophash, "iseq_uint32", &found);
    entry->name = strdup("iseq_uint32");
    entry->binaryop = GxB_ISEQ_UINT32;

    entry = binaryophash_insert(binaryophash, "isne_uint32", &found);
    entry->name = strdup("isne_uint32");
    entry->binaryop = GxB_ISNE_UINT32;

    entry = binaryophash_insert(binaryophash, "iseq_uint64", &found);
    entry->name = strdup("iseq_uint64");
    entry->binaryop = GxB_ISEQ_UINT64;

    entry = binaryophash_insert(binaryophash, "isne_uint64", &found);
    entry->name = strdup("isne_uint64");
    entry->binaryop = GxB_ISNE_UINT64;

    entry = binaryophash_insert(binaryophash, "iseq_fp32", &found);
    entry->name = strdup("iseq_fp32");
    entry->binaryop = GxB_ISEQ_FP32;

    entry = binaryophash_insert(binaryophash, "isne_fp32", &found);
    entry->name = strdup("isne_fp32");
    entry->binaryop = GxB_ISNE_FP32;

    entry = binaryophash_insert(binaryophash, "iseq_fp64", &found);
    entry->name = strdup("iseq_fp64");
    entry->binaryop = GxB_ISEQ_FP64;

    entry = binaryophash_insert(binaryophash, "isne_fp64", &found);
    entry->name = strdup("isne_fp64");
    entry->binaryop = GxB_ISNE_FP64;

    entry = binaryophash_insert(binaryophash, "iseq_fc32", &found);
    entry->name = strdup("iseq_fc32");
    entry->binaryop = GxB_ISEQ_FC32;

    entry = binaryophash_insert(binaryophash, "isne_fc32", &found);
    entry->name = strdup("isne_fc32");
    entry->binaryop = GxB_ISNE_FC32;

    entry = binaryophash_insert(binaryophash, "iseq_fc64", &found);
    entry->name = strdup("iseq_fc64");
    entry->binaryop = GxB_ISEQ_FC64;

    entry = binaryophash_insert(binaryophash, "isne_fc64", &found);
    entry->name = strdup("isne_fc64");
    entry->binaryop = GxB_ISNE_FC64;

    entry = binaryophash_insert(binaryophash, "isgt_bool", &found);
    entry->name = strdup("isgt_bool");
    entry->binaryop = GxB_ISGT_BOOL;

    entry = binaryophash_insert(binaryophash, "islt_bool", &found);
    entry->name = strdup("islt_bool");
    entry->binaryop = GxB_ISLT_BOOL;

    entry = binaryophash_insert(binaryophash, "isge_bool", &found);
    entry->name = strdup("isge_bool");
    entry->binaryop = GxB_ISGE_BOOL;

    entry = binaryophash_insert(binaryophash, "isle_bool", &found);
    entry->name = strdup("isle_bool");
    entry->binaryop = GxB_ISLE_BOOL;

    entry = binaryophash_insert(binaryophash, "isgt_int8", &found);
    entry->name = strdup("isgt_int8");
    entry->binaryop = GxB_ISGT_INT8;

    entry = binaryophash_insert(binaryophash, "islt_int8", &found);
    entry->name = strdup("islt_int8");
    entry->binaryop = GxB_ISLT_INT8;

    entry = binaryophash_insert(binaryophash, "isge_int8", &found);
    entry->name = strdup("isge_int8");
    entry->binaryop = GxB_ISGE_INT8;

    entry = binaryophash_insert(binaryophash, "isle_int8", &found);
    entry->name = strdup("isle_int8");
    entry->binaryop = GxB_ISLE_INT8;

    entry = binaryophash_insert(binaryophash, "isgt_int16", &found);
    entry->name = strdup("isgt_int16");
    entry->binaryop = GxB_ISGT_INT16;

    entry = binaryophash_insert(binaryophash, "islt_int16", &found);
    entry->name = strdup("islt_int16");
    entry->binaryop = GxB_ISLT_INT16;

    entry = binaryophash_insert(binaryophash, "isge_int16", &found);
    entry->name = strdup("isge_int16");
    entry->binaryop = GxB_ISGE_INT16;

    entry = binaryophash_insert(binaryophash, "isle_int16", &found);
    entry->name = strdup("isle_int16");
    entry->binaryop = GxB_ISLE_INT16;

    entry = binaryophash_insert(binaryophash, "isgt_int32", &found);
    entry->name = strdup("isgt_int32");
    entry->binaryop = GxB_ISGT_INT32;

    entry = binaryophash_insert(binaryophash, "islt_int32", &found);
    entry->name = strdup("islt_int32");
    entry->binaryop = GxB_ISLT_INT32;

    entry = binaryophash_insert(binaryophash, "isge_int32", &found);
    entry->name = strdup("isge_int32");
    entry->binaryop = GxB_ISGE_INT32;

    entry = binaryophash_insert(binaryophash, "isle_int32", &found);
    entry->name = strdup("isle_int32");
    entry->binaryop = GxB_ISLE_INT32;

    entry = binaryophash_insert(binaryophash, "isgt_int64", &found);
    entry->name = strdup("isgt_int64");
    entry->binaryop = GxB_ISGT_INT64;

    entry = binaryophash_insert(binaryophash, "islt_int64", &found);
    entry->name = strdup("islt_int64");
    entry->binaryop = GxB_ISLT_INT64;

    entry = binaryophash_insert(binaryophash, "isge_int64", &found);
    entry->name = strdup("isge_int64");
    entry->binaryop = GxB_ISGE_INT64;

    entry = binaryophash_insert(binaryophash, "isle_int64", &found);
    entry->name = strdup("isle_int64");
    entry->binaryop = GxB_ISLE_INT64;

    entry = binaryophash_insert(binaryophash, "isgt_uint8", &found);
    entry->name = strdup("isgt_uint8");
    entry->binaryop = GxB_ISGT_UINT8;

    entry = binaryophash_insert(binaryophash, "islt_uint8", &found);
    entry->name = strdup("islt_uint8");
    entry->binaryop = GxB_ISLT_UINT8;

    entry = binaryophash_insert(binaryophash, "isge_uint8", &found);
    entry->name = strdup("isge_uint8");
    entry->binaryop = GxB_ISGE_UINT8;

    entry = binaryophash_insert(binaryophash, "isle_uint8", &found);
    entry->name = strdup("isle_uint8");
    entry->binaryop = GxB_ISLE_UINT8;

    entry = binaryophash_insert(binaryophash, "isgt_uint16", &found);
    entry->name = strdup("isgt_uint16");
    entry->binaryop = GxB_ISGT_UINT16;

    entry = binaryophash_insert(binaryophash, "islt_uint16", &found);
    entry->name = strdup("islt_uint16");
    entry->binaryop = GxB_ISLT_UINT16;

    entry = binaryophash_insert(binaryophash, "isge_uint16", &found);
    entry->name = strdup("isge_uint16");
    entry->binaryop = GxB_ISGE_UINT16;

    entry = binaryophash_insert(binaryophash, "isle_uint16", &found);
    entry->name = strdup("isle_uint16");
    entry->binaryop = GxB_ISLE_UINT16;

    entry = binaryophash_insert(binaryophash, "isgt_uint32", &found);
    entry->name = strdup("isgt_uint32");
    entry->binaryop = GxB_ISGT_UINT32;

    entry = binaryophash_insert(binaryophash, "islt_uint32", &found);
    entry->name = strdup("islt_uint32");
    entry->binaryop = GxB_ISLT_UINT32;

    entry = binaryophash_insert(binaryophash, "isge_uint32", &found);
    entry->name = strdup("isge_uint32");
    entry->binaryop = GxB_ISGE_UINT32;

    entry = binaryophash_insert(binaryophash, "isle_uint32", &found);
    entry->name = strdup("isle_uint32");
    entry->binaryop = GxB_ISLE_UINT32;

    entry = binaryophash_insert(binaryophash, "isgt_uint64", &found);
    entry->name = strdup("isgt_uint64");
    entry->binaryop = GxB_ISGT_UINT64;

    entry = binaryophash_insert(binaryophash, "islt_uint64", &found);
    entry->name = strdup("islt_uint64");
    entry->binaryop = GxB_ISLT_UINT64;

    entry = binaryophash_insert(binaryophash, "isge_uint64", &found);
    entry->name = strdup("isge_uint64");
    entry->binaryop = GxB_ISGE_UINT64;

    entry = binaryophash_insert(binaryophash, "isle_uint64", &found);
    entry->name = strdup("isle_uint64");
    entry->binaryop = GxB_ISLE_UINT64;

    entry = binaryophash_insert(binaryophash, "isgt_fp32", &found);
    entry->name = strdup("isgt_fp32");
    entry->binaryop = GxB_ISGT_FP32;

    entry = binaryophash_insert(binaryophash, "islt_fp32", &found);
    entry->name = strdup("islt_fp32");
    entry->binaryop = GxB_ISLT_FP32;

    entry = binaryophash_insert(binaryophash, "isge_fp32", &found);
    entry->name = strdup("isge_fp32");
    entry->binaryop = GxB_ISGE_FP32;

    entry = binaryophash_insert(binaryophash, "isle_fp32", &found);
    entry->name = strdup("isle_fp32");
    entry->binaryop = GxB_ISLE_FP32;

    entry = binaryophash_insert(binaryophash, "isgt_fp64", &found);
    entry->name = strdup("isgt_fp64");
    entry->binaryop = GxB_ISGT_FP64;

    entry = binaryophash_insert(binaryophash, "islt_fp64", &found);
    entry->name = strdup("islt_fp64");
    entry->binaryop = GxB_ISLT_FP64;

    entry = binaryophash_insert(binaryophash, "isge_fp64", &found);
    entry->name = strdup("isge_fp64");
    entry->binaryop = GxB_ISGE_FP64;

    entry = binaryophash_insert(binaryophash, "isle_fp64", &found);
    entry->name = strdup("isle_fp64");
    entry->binaryop = GxB_ISLE_FP64;

    entry = binaryophash_insert(binaryophash, "min_bool", &found);
    entry->name = strdup("min_bool");
    entry->binaryop = GrB_MIN_BOOL;

    entry = binaryophash_insert(binaryophash, "max_bool", &found);
    entry->name = strdup("max_bool");
    entry->binaryop = GrB_MAX_BOOL;

    entry = binaryophash_insert(binaryophash, "min_int8", &found);
    entry->name = strdup("min_int8");
    entry->binaryop = GrB_MIN_INT8;

    entry = binaryophash_insert(binaryophash, "max_int8", &found);
    entry->name = strdup("max_int8");
    entry->binaryop = GrB_MAX_INT8;

    entry = binaryophash_insert(binaryophash, "min_int16", &found);
    entry->name = strdup("min_int16");
    entry->binaryop = GrB_MIN_INT16;

    entry = binaryophash_insert(binaryophash, "max_int16", &found);
    entry->name = strdup("max_int16");
    entry->binaryop = GrB_MAX_INT16;

    entry = binaryophash_insert(binaryophash, "min_int32", &found);
    entry->name = strdup("min_int32");
    entry->binaryop = GrB_MIN_INT32;

    entry = binaryophash_insert(binaryophash, "max_int32", &found);
    entry->name = strdup("max_int32");
    entry->binaryop = GrB_MAX_INT32;

    entry = binaryophash_insert(binaryophash, "min_int64", &found);
    entry->name = strdup("min_int64");
    entry->binaryop = GrB_MIN_INT64;

    entry = binaryophash_insert(binaryophash, "max_int64", &found);
    entry->name = strdup("max_int64");
    entry->binaryop = GrB_MAX_INT64;

    entry = binaryophash_insert(binaryophash, "min_uint8", &found);
    entry->name = strdup("min_uint8");
    entry->binaryop = GrB_MIN_UINT8;

    entry = binaryophash_insert(binaryophash, "max_uint8", &found);
    entry->name = strdup("max_uint8");
    entry->binaryop = GrB_MAX_UINT8;

    entry = binaryophash_insert(binaryophash, "min_uint16", &found);
    entry->name = strdup("min_uint16");
    entry->binaryop = GrB_MIN_UINT16;

    entry = binaryophash_insert(binaryophash, "max_uint16", &found);
    entry->name = strdup("max_uint16");
    entry->binaryop = GrB_MAX_UINT16;

    entry = binaryophash_insert(binaryophash, "min_uint32", &found);
    entry->name = strdup("min_uint32");
    entry->binaryop = GrB_MIN_UINT32;

    entry = binaryophash_insert(binaryophash, "max_uint32", &found);
    entry->name = strdup("max_uint32");
    entry->binaryop = GrB_MAX_UINT32;

    entry = binaryophash_insert(binaryophash, "min_uint64", &found);
    entry->name = strdup("min_uint64");
    entry->binaryop = GrB_MIN_UINT64;

    entry = binaryophash_insert(binaryophash, "max_uint64", &found);
    entry->name = strdup("max_uint64");
    entry->binaryop = GrB_MAX_UINT64;

    entry = binaryophash_insert(binaryophash, "min_fp32", &found);
    entry->name = strdup("min_fp32");
    entry->binaryop = GrB_MIN_FP32;

    entry = binaryophash_insert(binaryophash, "max_fp32", &found);
    entry->name = strdup("max_fp32");
    entry->binaryop = GrB_MAX_FP32;

    entry = binaryophash_insert(binaryophash, "min_fp64", &found);
    entry->name = strdup("min_fp64");
    entry->binaryop = GrB_MIN_FP64;

    entry = binaryophash_insert(binaryophash, "max_fp64", &found);
    entry->name = strdup("max_fp64");
    entry->binaryop = GrB_MAX_FP64;

    entry = binaryophash_insert(binaryophash, "lor_bool", &found);
    entry->name = strdup("lor_bool");
    entry->binaryop = GxB_LOR_BOOL;

    entry = binaryophash_insert(binaryophash, "land_bool", &found);
    entry->name = strdup("land_bool");
    entry->binaryop = GxB_LAND_BOOL;

    entry = binaryophash_insert(binaryophash, "lxor_bool", &found);
    entry->name = strdup("lxor_bool");
    entry->binaryop = GxB_LXOR_BOOL;

    entry = binaryophash_insert(binaryophash, "lor_int8", &found);
    entry->name = strdup("lor_int8");
    entry->binaryop = GxB_LOR_INT8;

    entry = binaryophash_insert(binaryophash, "land_int8", &found);
    entry->name = strdup("land_int8");
    entry->binaryop = GxB_LAND_INT8;

    entry = binaryophash_insert(binaryophash, "lxor_int8", &found);
    entry->name = strdup("lxor_int8");
    entry->binaryop = GxB_LXOR_INT8;

    entry = binaryophash_insert(binaryophash, "lor_int16", &found);
    entry->name = strdup("lor_int16");
    entry->binaryop = GxB_LOR_INT16;

    entry = binaryophash_insert(binaryophash, "land_int16", &found);
    entry->name = strdup("land_int16");
    entry->binaryop = GxB_LAND_INT16;

    entry = binaryophash_insert(binaryophash, "lxor_int16", &found);
    entry->name = strdup("lxor_int16");
    entry->binaryop = GxB_LXOR_INT16;

    entry = binaryophash_insert(binaryophash, "lor_int32", &found);
    entry->name = strdup("lor_int32");
    entry->binaryop = GxB_LOR_INT32;

    entry = binaryophash_insert(binaryophash, "land_int32", &found);
    entry->name = strdup("land_int32");
    entry->binaryop = GxB_LAND_INT32;

    entry = binaryophash_insert(binaryophash, "lxor_int32", &found);
    entry->name = strdup("lxor_int32");
    entry->binaryop = GxB_LXOR_INT32;

    entry = binaryophash_insert(binaryophash, "lor_int64", &found);
    entry->name = strdup("lor_int64");
    entry->binaryop = GxB_LOR_INT64;

    entry = binaryophash_insert(binaryophash, "land_int64", &found);
    entry->name = strdup("land_int64");
    entry->binaryop = GxB_LAND_INT64;

    entry = binaryophash_insert(binaryophash, "lxor_int64", &found);
    entry->name = strdup("lxor_int64");
    entry->binaryop = GxB_LXOR_INT64;

    entry = binaryophash_insert(binaryophash, "lor_uint8", &found);
    entry->name = strdup("lor_uint8");
    entry->binaryop = GxB_LOR_UINT8;

    entry = binaryophash_insert(binaryophash, "land_uint8", &found);
    entry->name = strdup("land_uint8");
    entry->binaryop = GxB_LAND_UINT8;

    entry = binaryophash_insert(binaryophash, "lxor_uint8", &found);
    entry->name = strdup("lxor_uint8");
    entry->binaryop = GxB_LXOR_UINT8;

    entry = binaryophash_insert(binaryophash, "lor_uint16", &found);
    entry->name = strdup("lor_uint16");
    entry->binaryop = GxB_LOR_UINT16;

    entry = binaryophash_insert(binaryophash, "land_uint16", &found);
    entry->name = strdup("land_uint16");
    entry->binaryop = GxB_LAND_UINT16;

    entry = binaryophash_insert(binaryophash, "lxor_uint16", &found);
    entry->name = strdup("lxor_uint16");
    entry->binaryop = GxB_LXOR_UINT16;

    entry = binaryophash_insert(binaryophash, "lor_uint32", &found);
    entry->name = strdup("lor_uint32");
    entry->binaryop = GxB_LOR_UINT32;

    entry = binaryophash_insert(binaryophash, "land_uint32", &found);
    entry->name = strdup("land_uint32");
    entry->binaryop = GxB_LAND_UINT32;

    entry = binaryophash_insert(binaryophash, "lxor_uint32", &found);
    entry->name = strdup("lxor_uint32");
    entry->binaryop = GxB_LXOR_UINT32;

    entry = binaryophash_insert(binaryophash, "lor_uint64", &found);
    entry->name = strdup("lor_uint64");
    entry->binaryop = GxB_LOR_UINT64;

    entry = binaryophash_insert(binaryophash, "land_uint64", &found);
    entry->name = strdup("land_uint64");
    entry->binaryop = GxB_LAND_UINT64;

    entry = binaryophash_insert(binaryophash, "lxor_uint64", &found);
    entry->name = strdup("lxor_uint64");
    entry->binaryop = GxB_LXOR_UINT64;

    entry = binaryophash_insert(binaryophash, "lor_fp32", &found);
    entry->name = strdup("lor_fp32");
    entry->binaryop = GxB_LOR_FP32;

    entry = binaryophash_insert(binaryophash, "land_fp32", &found);
    entry->name = strdup("land_fp32");
    entry->binaryop = GxB_LAND_FP32;

    entry = binaryophash_insert(binaryophash, "lxor_fp32", &found);
    entry->name = strdup("lxor_fp32");
    entry->binaryop = GxB_LXOR_FP32;

    entry = binaryophash_insert(binaryophash, "lor_fp64", &found);
    entry->name = strdup("lor_fp64");
    entry->binaryop = GxB_LOR_FP64;

    entry = binaryophash_insert(binaryophash, "land_fp64", &found);
    entry->name = strdup("land_fp64");
    entry->binaryop = GxB_LAND_FP64;

    entry = binaryophash_insert(binaryophash, "lxor_fp64", &found);
    entry->name = strdup("lxor_fp64");
    entry->binaryop = GxB_LXOR_FP64;

    entry = binaryophash_insert(binaryophash, "lor", &found);
    entry->name = strdup("lor");
    entry->binaryop = GrB_LOR;

    entry = binaryophash_insert(binaryophash, "land", &found);
    entry->name = strdup("land");
    entry->binaryop = GrB_LAND;

    entry = binaryophash_insert(binaryophash, "lxor", &found);
    entry->name = strdup("lxor");
    entry->binaryop = GrB_LXOR;

    entry = binaryophash_insert(binaryophash, "lxnor", &found);
    entry->name = strdup("lxnor");
    entry->binaryop = GrB_LXNOR;

    entry = binaryophash_insert(binaryophash, "atan2_fp32", &found);
    entry->name = strdup("atan2_fp32");
    entry->binaryop = GxB_ATAN2_FP32;

    entry = binaryophash_insert(binaryophash, "hypot_fp32", &found);
    entry->name = strdup("hypot_fp32");
    entry->binaryop = GxB_HYPOT_FP32;

    entry = binaryophash_insert(binaryophash, "fmod_fp32", &found);
    entry->name = strdup("fmod_fp32");
    entry->binaryop = GxB_FMOD_FP32;

    entry = binaryophash_insert(binaryophash, "remainder_fp32", &found);
    entry->name = strdup("remainder_fp32");
    entry->binaryop = GxB_REMAINDER_FP32;

    entry = binaryophash_insert(binaryophash, "atan2_fp64", &found);
    entry->name = strdup("atan2_fp64");
    entry->binaryop = GxB_ATAN2_FP64;

    entry = binaryophash_insert(binaryophash, "hypot_fp64", &found);
    entry->name = strdup("hypot_fp64");
    entry->binaryop = GxB_HYPOT_FP64;

    entry = binaryophash_insert(binaryophash, "fmod_fp64", &found);
    entry->name = strdup("fmod_fp64");
    entry->binaryop = GxB_FMOD_FP64;

    entry = binaryophash_insert(binaryophash, "remainder_fp64", &found);
    entry->name = strdup("remainder_fp64");
    entry->binaryop = GxB_REMAINDER_FP64;

    entry = binaryophash_insert(binaryophash, "ldexp_fp32", &found);
    entry->name = strdup("ldexp_fp32");
    entry->binaryop = GxB_LDEXP_FP32;

    entry = binaryophash_insert(binaryophash, "copysign_fp32", &found);
    entry->name = strdup("copysign_fp32");
    entry->binaryop = GxB_COPYSIGN_FP32;

    entry = binaryophash_insert(binaryophash, "ldexp_fp64", &found);
    entry->name = strdup("ldexp_fp64");
    entry->binaryop = GxB_LDEXP_FP64;

    entry = binaryophash_insert(binaryophash, "copysign_fp64", &found);
    entry->name = strdup("copysign_fp64");
    entry->binaryop = GxB_COPYSIGN_FP64;

    entry = binaryophash_insert(binaryophash, "bget_int8", &found);
    entry->name = strdup("bget_int8");
    entry->binaryop = GxB_BGET_INT8;

    entry = binaryophash_insert(binaryophash, "bset_int8", &found);
    entry->name = strdup("bset_int8");
    entry->binaryop = GxB_BSET_INT8;

    entry = binaryophash_insert(binaryophash, "bclr_int8", &found);
    entry->name = strdup("bclr_int8");
    entry->binaryop = GxB_BCLR_INT8;

    entry = binaryophash_insert(binaryophash, "bget_int16", &found);
    entry->name = strdup("bget_int16");
    entry->binaryop = GxB_BGET_INT16;

    entry = binaryophash_insert(binaryophash, "bset_int16", &found);
    entry->name = strdup("bset_int16");
    entry->binaryop = GxB_BSET_INT16;

    entry = binaryophash_insert(binaryophash, "bclr_int16", &found);
    entry->name = strdup("bclr_int16");
    entry->binaryop = GxB_BCLR_INT16;

    entry = binaryophash_insert(binaryophash, "bget_int32", &found);
    entry->name = strdup("bget_int32");
    entry->binaryop = GxB_BGET_INT32;

    entry = binaryophash_insert(binaryophash, "bset_int32", &found);
    entry->name = strdup("bset_int32");
    entry->binaryop = GxB_BSET_INT32;

    entry = binaryophash_insert(binaryophash, "bclr_int32", &found);
    entry->name = strdup("bclr_int32");
    entry->binaryop = GxB_BCLR_INT32;

    entry = binaryophash_insert(binaryophash, "bget_int64", &found);
    entry->name = strdup("bget_int64");
    entry->binaryop = GxB_BGET_INT64;

    entry = binaryophash_insert(binaryophash, "bset_int64", &found);
    entry->name = strdup("bset_int64");
    entry->binaryop = GxB_BSET_INT64;

    entry = binaryophash_insert(binaryophash, "bclr_int64", &found);
    entry->name = strdup("bclr_int64");
    entry->binaryop = GxB_BCLR_INT64;

    entry = binaryophash_insert(binaryophash, "bget_uint8", &found);
    entry->name = strdup("bget_uint8");
    entry->binaryop = GxB_BGET_UINT8;

    entry = binaryophash_insert(binaryophash, "bset_uint8", &found);
    entry->name = strdup("bset_uint8");
    entry->binaryop = GxB_BSET_UINT8;

    entry = binaryophash_insert(binaryophash, "bclr_uint8", &found);
    entry->name = strdup("bclr_uint8");
    entry->binaryop = GxB_BCLR_UINT8;

    entry = binaryophash_insert(binaryophash, "bget_uint16", &found);
    entry->name = strdup("bget_uint16");
    entry->binaryop = GxB_BGET_UINT16;

    entry = binaryophash_insert(binaryophash, "bset_uint16", &found);
    entry->name = strdup("bset_uint16");
    entry->binaryop = GxB_BSET_UINT16;

    entry = binaryophash_insert(binaryophash, "bclr_uint16", &found);
    entry->name = strdup("bclr_uint16");
    entry->binaryop = GxB_BCLR_UINT16;

    entry = binaryophash_insert(binaryophash, "bget_uint32", &found);
    entry->name = strdup("bget_uint32");
    entry->binaryop = GxB_BGET_UINT32;

    entry = binaryophash_insert(binaryophash, "bset_uint32", &found);
    entry->name = strdup("bset_uint32");
    entry->binaryop = GxB_BSET_UINT32;

    entry = binaryophash_insert(binaryophash, "bclr_uint32", &found);
    entry->name = strdup("bclr_uint32");
    entry->binaryop = GxB_BCLR_UINT32;

    entry = binaryophash_insert(binaryophash, "bget_uint64", &found);
    entry->name = strdup("bget_uint64");
    entry->binaryop = GxB_BGET_UINT64;

    entry = binaryophash_insert(binaryophash, "bset_uint64", &found);
    entry->name = strdup("bset_uint64");
    entry->binaryop = GxB_BSET_UINT64;

    entry = binaryophash_insert(binaryophash, "bclr_uint64", &found);
    entry->name = strdup("bclr_uint64");
    entry->binaryop = GxB_BCLR_UINT64;

    entry = binaryophash_insert(binaryophash, "bor_int8", &found);
    entry->name = strdup("bor_int8");
    entry->binaryop = GrB_BOR_INT8;

    entry = binaryophash_insert(binaryophash, "band_int8", &found);
    entry->name = strdup("band_int8");
    entry->binaryop = GrB_BAND_INT8;

    entry = binaryophash_insert(binaryophash, "bxor_int8", &found);
    entry->name = strdup("bxor_int8");
    entry->binaryop = GrB_BXOR_INT8;

    entry = binaryophash_insert(binaryophash, "bxnor_int8", &found);
    entry->name = strdup("bxnor_int8");
    entry->binaryop = GrB_BXNOR_INT8;

    entry = binaryophash_insert(binaryophash, "bor_int16", &found);
    entry->name = strdup("bor_int16");
    entry->binaryop = GrB_BOR_INT16;

    entry = binaryophash_insert(binaryophash, "band_int16", &found);
    entry->name = strdup("band_int16");
    entry->binaryop = GrB_BAND_INT16;

    entry = binaryophash_insert(binaryophash, "bxor_int16", &found);
    entry->name = strdup("bxor_int16");
    entry->binaryop = GrB_BXOR_INT16;

    entry = binaryophash_insert(binaryophash, "bxnor_int16", &found);
    entry->name = strdup("bxnor_int16");
    entry->binaryop = GrB_BXNOR_INT16;

    entry = binaryophash_insert(binaryophash, "bor_int32", &found);
    entry->name = strdup("bor_int32");
    entry->binaryop = GrB_BOR_INT32;

    entry = binaryophash_insert(binaryophash, "band_int32", &found);
    entry->name = strdup("band_int32");
    entry->binaryop = GrB_BAND_INT32;

    entry = binaryophash_insert(binaryophash, "bxor_int32", &found);
    entry->name = strdup("bxor_int32");
    entry->binaryop = GrB_BXOR_INT32;

    entry = binaryophash_insert(binaryophash, "bxnor_int32", &found);
    entry->name = strdup("bxnor_int32");
    entry->binaryop = GrB_BXNOR_INT32;

    entry = binaryophash_insert(binaryophash, "bor_int64", &found);
    entry->name = strdup("bor_int64");
    entry->binaryop = GrB_BOR_INT64;

    entry = binaryophash_insert(binaryophash, "band_int64", &found);
    entry->name = strdup("band_int64");
    entry->binaryop = GrB_BAND_INT64;

    entry = binaryophash_insert(binaryophash, "bxor_int64", &found);
    entry->name = strdup("bxor_int64");
    entry->binaryop = GrB_BXOR_INT64;

    entry = binaryophash_insert(binaryophash, "bxnor_int64", &found);
    entry->name = strdup("bxnor_int64");
    entry->binaryop = GrB_BXNOR_INT64;

    entry = binaryophash_insert(binaryophash, "bor_uint8", &found);
    entry->name = strdup("bor_uint8");
    entry->binaryop = GrB_BOR_UINT8;

    entry = binaryophash_insert(binaryophash, "band_uint8", &found);
    entry->name = strdup("band_uint8");
    entry->binaryop = GrB_BAND_UINT8;

    entry = binaryophash_insert(binaryophash, "bxor_uint8", &found);
    entry->name = strdup("bxor_uint8");
    entry->binaryop = GrB_BXOR_UINT8;

    entry = binaryophash_insert(binaryophash, "bxnor_uint8", &found);
    entry->name = strdup("bxnor_uint8");
    entry->binaryop = GrB_BXNOR_UINT8;

    entry = binaryophash_insert(binaryophash, "bor_uint16", &found);
    entry->name = strdup("bor_uint16");
    entry->binaryop = GrB_BOR_UINT16;

    entry = binaryophash_insert(binaryophash, "band_uint16", &found);
    entry->name = strdup("band_uint16");
    entry->binaryop = GrB_BAND_UINT16;

    entry = binaryophash_insert(binaryophash, "bxor_uint16", &found);
    entry->name = strdup("bxor_uint16");
    entry->binaryop = GrB_BXOR_UINT16;

    entry = binaryophash_insert(binaryophash, "bxnor_uint16", &found);
    entry->name = strdup("bxnor_uint16");
    entry->binaryop = GrB_BXNOR_UINT16;

    entry = binaryophash_insert(binaryophash, "bor_uint32", &found);
    entry->name = strdup("bor_uint32");
    entry->binaryop = GrB_BOR_UINT32;

    entry = binaryophash_insert(binaryophash, "band_uint32", &found);
    entry->name = strdup("band_uint32");
    entry->binaryop = GrB_BAND_UINT32;

    entry = binaryophash_insert(binaryophash, "bxor_uint32", &found);
    entry->name = strdup("bxor_uint32");
    entry->binaryop = GrB_BXOR_UINT32;

    entry = binaryophash_insert(binaryophash, "bxnor_uint32", &found);
    entry->name = strdup("bxnor_uint32");
    entry->binaryop = GrB_BXNOR_UINT32;

    entry = binaryophash_insert(binaryophash, "bor_uint64", &found);
    entry->name = strdup("bor_uint64");
    entry->binaryop = GrB_BOR_UINT64;

    entry = binaryophash_insert(binaryophash, "band_uint64", &found);
    entry->name = strdup("band_uint64");
    entry->binaryop = GrB_BAND_UINT64;

    entry = binaryophash_insert(binaryophash, "bxor_uint64", &found);
    entry->name = strdup("bxor_uint64");
    entry->binaryop = GrB_BXOR_UINT64;

    entry = binaryophash_insert(binaryophash, "bxnor_uint64", &found);
    entry->name = strdup("bxnor_uint64");
    entry->binaryop = GrB_BXNOR_UINT64;

    entry = binaryophash_insert(binaryophash, "bshift_int8", &found);
    entry->name = strdup("bshift_int8");
    entry->binaryop = GxB_BSHIFT_INT8;

    entry = binaryophash_insert(binaryophash, "bshift_int16", &found);
    entry->name = strdup("bshift_int16");
    entry->binaryop = GxB_BSHIFT_INT16;

    entry = binaryophash_insert(binaryophash, "bshift_int32", &found);
    entry->name = strdup("bshift_int32");
    entry->binaryop = GxB_BSHIFT_INT32;

    entry = binaryophash_insert(binaryophash, "bshift_int64", &found);
    entry->name = strdup("bshift_int64");
    entry->binaryop = GxB_BSHIFT_INT64;

    entry = binaryophash_insert(binaryophash, "bshift_uint8", &found);
    entry->name = strdup("bshift_uint8");
    entry->binaryop = GxB_BSHIFT_UINT8;

    entry = binaryophash_insert(binaryophash, "bshift_uint16", &found);
    entry->name = strdup("bshift_uint16");
    entry->binaryop = GxB_BSHIFT_UINT16;

    entry = binaryophash_insert(binaryophash, "bshift_uint32", &found);
    entry->name = strdup("bshift_uint32");
    entry->binaryop = GxB_BSHIFT_UINT32;

    entry = binaryophash_insert(binaryophash, "bshift_uint64", &found);
    entry->name = strdup("bshift_uint64");
    entry->binaryop = GxB_BSHIFT_UINT64;

    entry = binaryophash_insert(binaryophash, "eq_bool", &found);
    entry->name = strdup("eq_bool");
    entry->binaryop = GrB_EQ_BOOL;

    entry = binaryophash_insert(binaryophash, "ne_bool", &found);
    entry->name = strdup("ne_bool");
    entry->binaryop = GrB_NE_BOOL;

    entry = binaryophash_insert(binaryophash, "gt_bool", &found);
    entry->name = strdup("gt_bool");
    entry->binaryop = GrB_GT_BOOL;

    entry = binaryophash_insert(binaryophash, "lt_bool", &found);
    entry->name = strdup("lt_bool");
    entry->binaryop = GrB_LT_BOOL;

    entry = binaryophash_insert(binaryophash, "eq_int8", &found);
    entry->name = strdup("eq_int8");
    entry->binaryop = GrB_EQ_INT8;

    entry = binaryophash_insert(binaryophash, "ne_int8", &found);
    entry->name = strdup("ne_int8");
    entry->binaryop = GrB_NE_INT8;

    entry = binaryophash_insert(binaryophash, "gt_int8", &found);
    entry->name = strdup("gt_int8");
    entry->binaryop = GrB_GT_INT8;

    entry = binaryophash_insert(binaryophash, "lt_int8", &found);
    entry->name = strdup("lt_int8");
    entry->binaryop = GrB_LT_INT8;

    entry = binaryophash_insert(binaryophash, "eq_int16", &found);
    entry->name = strdup("eq_int16");
    entry->binaryop = GrB_EQ_INT16;

    entry = binaryophash_insert(binaryophash, "ne_int16", &found);
    entry->name = strdup("ne_int16");
    entry->binaryop = GrB_NE_INT16;

    entry = binaryophash_insert(binaryophash, "gt_int16", &found);
    entry->name = strdup("gt_int16");
    entry->binaryop = GrB_GT_INT16;

    entry = binaryophash_insert(binaryophash, "lt_int16", &found);
    entry->name = strdup("lt_int16");
    entry->binaryop = GrB_LT_INT16;

    entry = binaryophash_insert(binaryophash, "eq_int32", &found);
    entry->name = strdup("eq_int32");
    entry->binaryop = GrB_EQ_INT32;

    entry = binaryophash_insert(binaryophash, "ne_int32", &found);
    entry->name = strdup("ne_int32");
    entry->binaryop = GrB_NE_INT32;

    entry = binaryophash_insert(binaryophash, "gt_int32", &found);
    entry->name = strdup("gt_int32");
    entry->binaryop = GrB_GT_INT32;

    entry = binaryophash_insert(binaryophash, "lt_int32", &found);
    entry->name = strdup("lt_int32");
    entry->binaryop = GrB_LT_INT32;

    entry = binaryophash_insert(binaryophash, "eq_int64", &found);
    entry->name = strdup("eq_int64");
    entry->binaryop = GrB_EQ_INT64;

    entry = binaryophash_insert(binaryophash, "ne_int64", &found);
    entry->name = strdup("ne_int64");
    entry->binaryop = GrB_NE_INT64;

    entry = binaryophash_insert(binaryophash, "gt_int64", &found);
    entry->name = strdup("gt_int64");
    entry->binaryop = GrB_GT_INT64;

    entry = binaryophash_insert(binaryophash, "lt_int64", &found);
    entry->name = strdup("lt_int64");
    entry->binaryop = GrB_LT_INT64;

    entry = binaryophash_insert(binaryophash, "eq_uint8", &found);
    entry->name = strdup("eq_uint8");
    entry->binaryop = GrB_EQ_UINT8;

    entry = binaryophash_insert(binaryophash, "ne_uint8", &found);
    entry->name = strdup("ne_uint8");
    entry->binaryop = GrB_NE_UINT8;

    entry = binaryophash_insert(binaryophash, "gt_uint8", &found);
    entry->name = strdup("gt_uint8");
    entry->binaryop = GrB_GT_UINT8;

    entry = binaryophash_insert(binaryophash, "lt_uint8", &found);
    entry->name = strdup("lt_uint8");
    entry->binaryop = GrB_LT_UINT8;

    entry = binaryophash_insert(binaryophash, "eq_uint16", &found);
    entry->name = strdup("eq_uint16");
    entry->binaryop = GrB_EQ_UINT16;

    entry = binaryophash_insert(binaryophash, "ne_uint16", &found);
    entry->name = strdup("ne_uint16");
    entry->binaryop = GrB_NE_UINT16;

    entry = binaryophash_insert(binaryophash, "gt_uint16", &found);
    entry->name = strdup("gt_uint16");
    entry->binaryop = GrB_GT_UINT16;

    entry = binaryophash_insert(binaryophash, "lt_uint16", &found);
    entry->name = strdup("lt_uint16");
    entry->binaryop = GrB_LT_UINT16;

    entry = binaryophash_insert(binaryophash, "eq_uint32", &found);
    entry->name = strdup("eq_uint32");
    entry->binaryop = GrB_EQ_UINT32;

    entry = binaryophash_insert(binaryophash, "ne_uint32", &found);
    entry->name = strdup("ne_uint32");
    entry->binaryop = GrB_NE_UINT32;

    entry = binaryophash_insert(binaryophash, "gt_uint32", &found);
    entry->name = strdup("gt_uint32");
    entry->binaryop = GrB_GT_UINT32;

    entry = binaryophash_insert(binaryophash, "lt_uint32", &found);
    entry->name = strdup("lt_uint32");
    entry->binaryop = GrB_LT_UINT32;

    entry = binaryophash_insert(binaryophash, "eq_uint64", &found);
    entry->name = strdup("eq_uint64");
    entry->binaryop = GrB_EQ_UINT64;

    entry = binaryophash_insert(binaryophash, "ne_uint64", &found);
    entry->name = strdup("ne_uint64");
    entry->binaryop = GrB_NE_UINT64;

    entry = binaryophash_insert(binaryophash, "gt_uint64", &found);
    entry->name = strdup("gt_uint64");
    entry->binaryop = GrB_GT_UINT64;

    entry = binaryophash_insert(binaryophash, "lt_uint64", &found);
    entry->name = strdup("lt_uint64");
    entry->binaryop = GrB_LT_UINT64;

    entry = binaryophash_insert(binaryophash, "eq_fp32", &found);
    entry->name = strdup("eq_fp32");
    entry->binaryop = GrB_EQ_FP32;

    entry = binaryophash_insert(binaryophash, "ne_fp32", &found);
    entry->name = strdup("ne_fp32");
    entry->binaryop = GrB_NE_FP32;

    entry = binaryophash_insert(binaryophash, "gt_fp32", &found);
    entry->name = strdup("gt_fp32");
    entry->binaryop = GrB_GT_FP32;

    entry = binaryophash_insert(binaryophash, "lt_fp32", &found);
    entry->name = strdup("lt_fp32");
    entry->binaryop = GrB_LT_FP32;

    entry = binaryophash_insert(binaryophash, "eq_fp64", &found);
    entry->name = strdup("eq_fp64");
    entry->binaryop = GrB_EQ_FP64;

    entry = binaryophash_insert(binaryophash, "ne_fp64", &found);
    entry->name = strdup("ne_fp64");
    entry->binaryop = GrB_NE_FP64;

    entry = binaryophash_insert(binaryophash, "gt_fp64", &found);
    entry->name = strdup("gt_fp64");
    entry->binaryop = GrB_GT_FP64;

    entry = binaryophash_insert(binaryophash, "lt_fp64", &found);
    entry->name = strdup("lt_fp64");
    entry->binaryop = GrB_LT_FP64;

    entry = binaryophash_insert(binaryophash, "eq_fc32", &found);
    entry->name = strdup("eq_fc32");
    entry->binaryop = GxB_EQ_FC32;

    entry = binaryophash_insert(binaryophash, "ne_fc32", &found);
    entry->name = strdup("ne_fc32");
    entry->binaryop = GxB_NE_FC32;

    entry = binaryophash_insert(binaryophash, "eq_fc64", &found);
    entry->name = strdup("eq_fc64");
    entry->binaryop = GxB_EQ_FC64;

    entry = binaryophash_insert(binaryophash, "ne_fc64", &found);
    entry->name = strdup("ne_fc64");
    entry->binaryop = GxB_NE_FC64;

    entry = binaryophash_insert(binaryophash, "ge_bool", &found);
    entry->name = strdup("ge_bool");
    entry->binaryop = GrB_GE_BOOL;

    entry = binaryophash_insert(binaryophash, "le_bool", &found);
    entry->name = strdup("le_bool");
    entry->binaryop = GrB_LE_BOOL;

    entry = binaryophash_insert(binaryophash, "ge_int8", &found);
    entry->name = strdup("ge_int8");
    entry->binaryop = GrB_GE_INT8;

    entry = binaryophash_insert(binaryophash, "le_int8", &found);
    entry->name = strdup("le_int8");
    entry->binaryop = GrB_LE_INT8;

    entry = binaryophash_insert(binaryophash, "ge_int16", &found);
    entry->name = strdup("ge_int16");
    entry->binaryop = GrB_GE_INT16;

    entry = binaryophash_insert(binaryophash, "le_int16", &found);
    entry->name = strdup("le_int16");
    entry->binaryop = GrB_LE_INT16;

    entry = binaryophash_insert(binaryophash, "ge_int32", &found);
    entry->name = strdup("ge_int32");
    entry->binaryop = GrB_GE_INT32;

    entry = binaryophash_insert(binaryophash, "le_int32", &found);
    entry->name = strdup("le_int32");
    entry->binaryop = GrB_LE_INT32;

    entry = binaryophash_insert(binaryophash, "ge_int64", &found);
    entry->name = strdup("ge_int64");
    entry->binaryop = GrB_GE_INT64;

    entry = binaryophash_insert(binaryophash, "le_int64", &found);
    entry->name = strdup("le_int64");
    entry->binaryop = GrB_LE_INT64;

    entry = binaryophash_insert(binaryophash, "ge_uint8", &found);
    entry->name = strdup("ge_uint8");
    entry->binaryop = GrB_GE_UINT8;

    entry = binaryophash_insert(binaryophash, "le_uint8", &found);
    entry->name = strdup("le_uint8");
    entry->binaryop = GrB_LE_UINT8;

    entry = binaryophash_insert(binaryophash, "ge_uint16", &found);
    entry->name = strdup("ge_uint16");
    entry->binaryop = GrB_GE_UINT16;

    entry = binaryophash_insert(binaryophash, "le_uint16", &found);
    entry->name = strdup("le_uint16");
    entry->binaryop = GrB_LE_UINT16;

    entry = binaryophash_insert(binaryophash, "ge_uint32", &found);
    entry->name = strdup("ge_uint32");
    entry->binaryop = GrB_GE_UINT32;

    entry = binaryophash_insert(binaryophash, "le_uint32", &found);
    entry->name = strdup("le_uint32");
    entry->binaryop = GrB_LE_UINT32;

    entry = binaryophash_insert(binaryophash, "ge_uint64", &found);
    entry->name = strdup("ge_uint64");
    entry->binaryop = GrB_GE_UINT64;

    entry = binaryophash_insert(binaryophash, "le_uint64", &found);
    entry->name = strdup("le_uint64");
    entry->binaryop = GrB_LE_UINT64;

    entry = binaryophash_insert(binaryophash, "ge_fp32", &found);
    entry->name = strdup("ge_fp32");
    entry->binaryop = GrB_GE_FP32;

    entry = binaryophash_insert(binaryophash, "le_fp32", &found);
    entry->name = strdup("le_fp32");
    entry->binaryop = GrB_LE_FP32;

    entry = binaryophash_insert(binaryophash, "ge_fp64", &found);
    entry->name = strdup("ge_fp64");
    entry->binaryop = GrB_GE_FP64;

    entry = binaryophash_insert(binaryophash, "le_fp64", &found);
    entry->name = strdup("le_fp64");
    entry->binaryop = GrB_LE_FP64;

    entry = binaryophash_insert(binaryophash, "cmplx_fp32", &found);
    entry->name = strdup("cmplx_fp32");
    entry->binaryop = GxB_CMPLX_FP32;

    entry = binaryophash_insert(binaryophash, "cmplx_fp64", &found);
    entry->name = strdup("cmplx_fp64");
    entry->binaryop = GxB_CMPLX_FP64;

    entry = binaryophash_insert(binaryophash, "firsti_int32", &found);
    entry->name = strdup("firsti_int32");
    entry->binaryop = GxB_FIRSTI_INT32;

    entry = binaryophash_insert(binaryophash, "firsti_int64", &found);
    entry->name = strdup("firsti_int64");
    entry->binaryop = GxB_FIRSTI_INT64;

    entry = binaryophash_insert(binaryophash, "firsti1_int32", &found);
    entry->name = strdup("firsti1_int32");
    entry->binaryop = GxB_FIRSTI1_INT32;

    entry = binaryophash_insert(binaryophash, "firsti1_int64", &found);
    entry->name = strdup("firsti1_int64");
    entry->binaryop = GxB_FIRSTI1_INT64;

    entry = binaryophash_insert(binaryophash, "firstj_int32", &found);
    entry->name = strdup("firstj_int32");
    entry->binaryop = GxB_FIRSTJ_INT32;

    entry = binaryophash_insert(binaryophash, "firstj_int64", &found);
    entry->name = strdup("firstj_int64");
    entry->binaryop = GxB_FIRSTJ_INT64;

    entry = binaryophash_insert(binaryophash, "firstj1_int32", &found);
    entry->name = strdup("firstj1_int32");
    entry->binaryop = GxB_FIRSTJ1_INT32;

    entry = binaryophash_insert(binaryophash, "firstj1_int64", &found);
    entry->name = strdup("firstj1_int64");
    entry->binaryop = GxB_FIRSTJ1_INT64;

    entry = binaryophash_insert(binaryophash, "secondi_int32", &found);
    entry->name = strdup("secondi_int32");
    entry->binaryop = GxB_SECONDI_INT32;

    entry = binaryophash_insert(binaryophash, "secondi_int64", &found);
    entry->name = strdup("secondi_int64");
    entry->binaryop = GxB_SECONDI_INT64;

    entry = binaryophash_insert(binaryophash, "secondi1_int32", &found);
    entry->name = strdup("secondi1_int32");
    entry->binaryop = GxB_SECONDI1_INT32;

    entry = binaryophash_insert(binaryophash, "secondi1_int64", &found);
    entry->name = strdup("secondi1_int64");
    entry->binaryop = GxB_SECONDI1_INT64;

    entry = binaryophash_insert(binaryophash, "secondj_int32", &found);
    entry->name = strdup("secondj_int32");
    entry->binaryop = GxB_SECONDJ_INT32;

    entry = binaryophash_insert(binaryophash, "secondj_int64", &found);
    entry->name = strdup("secondj_int64");
    entry->binaryop = GxB_SECONDJ_INT64;

    entry = binaryophash_insert(binaryophash, "secondj1_int32", &found);
    entry->name = strdup("secondj1_int32");
    entry->binaryop = GxB_SECONDJ1_INT32;

    entry = binaryophash_insert(binaryophash, "secondj1_int64", &found);
    entry->name = strdup("secondj1_int64");
    entry->binaryop = GxB_SECONDJ1_INT64;

    entry = binaryophash_insert(binaryophash, "ignore_dup", &found);
    entry->name = strdup("ignore_dup");
    entry->binaryop = GxB_IGNORE_DUP;
    }

GrB_BinaryOp lookup_binaryop(char *name)
{
    binaryop_entry *entry;
	entry = binaryophash_lookup(binaryophash, name);
    if (entry)
        return entry->binaryop;
    return NULL;
    }
