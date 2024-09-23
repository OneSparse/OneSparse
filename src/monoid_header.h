typedef struct monoid_entry
{
	uint32 status;
	char *name;
    GrB_Monoid monoid;
} monoid_entry;

static uint32 hash_string_pointer(char *s);

static uint32
hash_string_pointer(char *s)
{
	unsigned char *ss = (unsigned char *) s;

	return hash_bytes(ss, strlen(s));
    }

#define MONOIDHASH_INITIAL_SIZE 2000

#define SH_PREFIX		monoidhash
#define SH_ELEMENT_TYPE monoid_entry
#define SH_KEY_TYPE		char *
#define	SH_KEY			name
#define SH_HASH_KEY(tb, key)	hash_string_pointer(key)
#define SH_EQUAL(tb, a, b)		(strcmp(a, b) == 0)
#define	SH_SCOPE		static inline
#define SH_DECLARE
#define SH_DEFINE
#include "lib/simplehash.h"

static monoidhash_hash *monoidhash;

void initialize_monoids()
{
    bool found;
    monoid_entry *entry;

    monoidhash = monoidhash_create(TopMemoryContext, MONOIDHASH_INITIAL_SIZE, NULL);
    
    entry = monoidhash_insert(monoidhash, "min_int8_monoid", &found);
    entry->name = strdup("min_int8_monoid");
    entry->monoid = GxB_MIN_INT8_MONOID;

    entry = monoidhash_insert(monoidhash, "min_int16_monoid", &found);
    entry->name = strdup("min_int16_monoid");
    entry->monoid = GxB_MIN_INT16_MONOID;

    entry = monoidhash_insert(monoidhash, "min_int32_monoid", &found);
    entry->name = strdup("min_int32_monoid");
    entry->monoid = GxB_MIN_INT32_MONOID;

    entry = monoidhash_insert(monoidhash, "min_int64_monoid", &found);
    entry->name = strdup("min_int64_monoid");
    entry->monoid = GxB_MIN_INT64_MONOID;

    entry = monoidhash_insert(monoidhash, "min_uint8_monoid", &found);
    entry->name = strdup("min_uint8_monoid");
    entry->monoid = GxB_MIN_UINT8_MONOID;

    entry = monoidhash_insert(monoidhash, "min_uint16_monoid", &found);
    entry->name = strdup("min_uint16_monoid");
    entry->monoid = GxB_MIN_UINT16_MONOID;

    entry = monoidhash_insert(monoidhash, "min_uint32_monoid", &found);
    entry->name = strdup("min_uint32_monoid");
    entry->monoid = GxB_MIN_UINT32_MONOID;

    entry = monoidhash_insert(monoidhash, "min_uint64_monoid", &found);
    entry->name = strdup("min_uint64_monoid");
    entry->monoid = GxB_MIN_UINT64_MONOID;

    entry = monoidhash_insert(monoidhash, "min_fp32_monoid", &found);
    entry->name = strdup("min_fp32_monoid");
    entry->monoid = GxB_MIN_FP32_MONOID;

    entry = monoidhash_insert(monoidhash, "min_fp64_monoid", &found);
    entry->name = strdup("min_fp64_monoid");
    entry->monoid = GxB_MIN_FP64_MONOID;

    entry = monoidhash_insert(monoidhash, "min_monoid_int8", &found);
    entry->name = strdup("min_monoid_int8");
    entry->monoid = GrB_MIN_MONOID_INT8;

    entry = monoidhash_insert(monoidhash, "min_monoid_int16", &found);
    entry->name = strdup("min_monoid_int16");
    entry->monoid = GrB_MIN_MONOID_INT16;

    entry = monoidhash_insert(monoidhash, "min_monoid_int32", &found);
    entry->name = strdup("min_monoid_int32");
    entry->monoid = GrB_MIN_MONOID_INT32;

    entry = monoidhash_insert(monoidhash, "min_monoid_int64", &found);
    entry->name = strdup("min_monoid_int64");
    entry->monoid = GrB_MIN_MONOID_INT64;

    entry = monoidhash_insert(monoidhash, "min_monoid_uint8", &found);
    entry->name = strdup("min_monoid_uint8");
    entry->monoid = GrB_MIN_MONOID_UINT8;

    entry = monoidhash_insert(monoidhash, "min_monoid_uint16", &found);
    entry->name = strdup("min_monoid_uint16");
    entry->monoid = GrB_MIN_MONOID_UINT16;

    entry = monoidhash_insert(monoidhash, "min_monoid_uint32", &found);
    entry->name = strdup("min_monoid_uint32");
    entry->monoid = GrB_MIN_MONOID_UINT32;

    entry = monoidhash_insert(monoidhash, "min_monoid_uint64", &found);
    entry->name = strdup("min_monoid_uint64");
    entry->monoid = GrB_MIN_MONOID_UINT64;

    entry = monoidhash_insert(monoidhash, "min_monoid_fp32", &found);
    entry->name = strdup("min_monoid_fp32");
    entry->monoid = GrB_MIN_MONOID_FP32;

    entry = monoidhash_insert(monoidhash, "min_monoid_fp64", &found);
    entry->name = strdup("min_monoid_fp64");
    entry->monoid = GrB_MIN_MONOID_FP64;

    entry = monoidhash_insert(monoidhash, "max_int8_monoid", &found);
    entry->name = strdup("max_int8_monoid");
    entry->monoid = GxB_MAX_INT8_MONOID;

    entry = monoidhash_insert(monoidhash, "max_int16_monoid", &found);
    entry->name = strdup("max_int16_monoid");
    entry->monoid = GxB_MAX_INT16_MONOID;

    entry = monoidhash_insert(monoidhash, "max_int32_monoid", &found);
    entry->name = strdup("max_int32_monoid");
    entry->monoid = GxB_MAX_INT32_MONOID;

    entry = monoidhash_insert(monoidhash, "max_int64_monoid", &found);
    entry->name = strdup("max_int64_monoid");
    entry->monoid = GxB_MAX_INT64_MONOID;

    entry = monoidhash_insert(monoidhash, "max_uint8_monoid", &found);
    entry->name = strdup("max_uint8_monoid");
    entry->monoid = GxB_MAX_UINT8_MONOID;

    entry = monoidhash_insert(monoidhash, "max_uint16_monoid", &found);
    entry->name = strdup("max_uint16_monoid");
    entry->monoid = GxB_MAX_UINT16_MONOID;

    entry = monoidhash_insert(monoidhash, "max_uint32_monoid", &found);
    entry->name = strdup("max_uint32_monoid");
    entry->monoid = GxB_MAX_UINT32_MONOID;

    entry = monoidhash_insert(monoidhash, "max_uint64_monoid", &found);
    entry->name = strdup("max_uint64_monoid");
    entry->monoid = GxB_MAX_UINT64_MONOID;

    entry = monoidhash_insert(monoidhash, "max_fp32_monoid", &found);
    entry->name = strdup("max_fp32_monoid");
    entry->monoid = GxB_MAX_FP32_MONOID;

    entry = monoidhash_insert(monoidhash, "max_fp64_monoid", &found);
    entry->name = strdup("max_fp64_monoid");
    entry->monoid = GxB_MAX_FP64_MONOID;

    entry = monoidhash_insert(monoidhash, "max_monoid_int8", &found);
    entry->name = strdup("max_monoid_int8");
    entry->monoid = GrB_MAX_MONOID_INT8;

    entry = monoidhash_insert(monoidhash, "max_monoid_int16", &found);
    entry->name = strdup("max_monoid_int16");
    entry->monoid = GrB_MAX_MONOID_INT16;

    entry = monoidhash_insert(monoidhash, "max_monoid_int32", &found);
    entry->name = strdup("max_monoid_int32");
    entry->monoid = GrB_MAX_MONOID_INT32;

    entry = monoidhash_insert(monoidhash, "max_monoid_int64", &found);
    entry->name = strdup("max_monoid_int64");
    entry->monoid = GrB_MAX_MONOID_INT64;

    entry = monoidhash_insert(monoidhash, "max_monoid_uint8", &found);
    entry->name = strdup("max_monoid_uint8");
    entry->monoid = GrB_MAX_MONOID_UINT8;

    entry = monoidhash_insert(monoidhash, "max_monoid_uint16", &found);
    entry->name = strdup("max_monoid_uint16");
    entry->monoid = GrB_MAX_MONOID_UINT16;

    entry = monoidhash_insert(monoidhash, "max_monoid_uint32", &found);
    entry->name = strdup("max_monoid_uint32");
    entry->monoid = GrB_MAX_MONOID_UINT32;

    entry = monoidhash_insert(monoidhash, "max_monoid_uint64", &found);
    entry->name = strdup("max_monoid_uint64");
    entry->monoid = GrB_MAX_MONOID_UINT64;

    entry = monoidhash_insert(monoidhash, "max_monoid_fp32", &found);
    entry->name = strdup("max_monoid_fp32");
    entry->monoid = GrB_MAX_MONOID_FP32;

    entry = monoidhash_insert(monoidhash, "max_monoid_fp64", &found);
    entry->name = strdup("max_monoid_fp64");
    entry->monoid = GrB_MAX_MONOID_FP64;

    entry = monoidhash_insert(monoidhash, "plus_int8_monoid", &found);
    entry->name = strdup("plus_int8_monoid");
    entry->monoid = GxB_PLUS_INT8_MONOID;

    entry = monoidhash_insert(monoidhash, "plus_int16_monoid", &found);
    entry->name = strdup("plus_int16_monoid");
    entry->monoid = GxB_PLUS_INT16_MONOID;

    entry = monoidhash_insert(monoidhash, "plus_int32_monoid", &found);
    entry->name = strdup("plus_int32_monoid");
    entry->monoid = GxB_PLUS_INT32_MONOID;

    entry = monoidhash_insert(monoidhash, "plus_int64_monoid", &found);
    entry->name = strdup("plus_int64_monoid");
    entry->monoid = GxB_PLUS_INT64_MONOID;

    entry = monoidhash_insert(monoidhash, "plus_uint8_monoid", &found);
    entry->name = strdup("plus_uint8_monoid");
    entry->monoid = GxB_PLUS_UINT8_MONOID;

    entry = monoidhash_insert(monoidhash, "plus_uint16_monoid", &found);
    entry->name = strdup("plus_uint16_monoid");
    entry->monoid = GxB_PLUS_UINT16_MONOID;

    entry = monoidhash_insert(monoidhash, "plus_uint32_monoid", &found);
    entry->name = strdup("plus_uint32_monoid");
    entry->monoid = GxB_PLUS_UINT32_MONOID;

    entry = monoidhash_insert(monoidhash, "plus_uint64_monoid", &found);
    entry->name = strdup("plus_uint64_monoid");
    entry->monoid = GxB_PLUS_UINT64_MONOID;

    entry = monoidhash_insert(monoidhash, "plus_fp32_monoid", &found);
    entry->name = strdup("plus_fp32_monoid");
    entry->monoid = GxB_PLUS_FP32_MONOID;

    entry = monoidhash_insert(monoidhash, "plus_fp64_monoid", &found);
    entry->name = strdup("plus_fp64_monoid");
    entry->monoid = GxB_PLUS_FP64_MONOID;

    entry = monoidhash_insert(monoidhash, "plus_monoid_int8", &found);
    entry->name = strdup("plus_monoid_int8");
    entry->monoid = GrB_PLUS_MONOID_INT8;

    entry = monoidhash_insert(monoidhash, "plus_monoid_int16", &found);
    entry->name = strdup("plus_monoid_int16");
    entry->monoid = GrB_PLUS_MONOID_INT16;

    entry = monoidhash_insert(monoidhash, "plus_monoid_int32", &found);
    entry->name = strdup("plus_monoid_int32");
    entry->monoid = GrB_PLUS_MONOID_INT32;

    entry = monoidhash_insert(monoidhash, "plus_monoid_int64", &found);
    entry->name = strdup("plus_monoid_int64");
    entry->monoid = GrB_PLUS_MONOID_INT64;

    entry = monoidhash_insert(monoidhash, "plus_monoid_uint8", &found);
    entry->name = strdup("plus_monoid_uint8");
    entry->monoid = GrB_PLUS_MONOID_UINT8;

    entry = monoidhash_insert(monoidhash, "plus_monoid_uint16", &found);
    entry->name = strdup("plus_monoid_uint16");
    entry->monoid = GrB_PLUS_MONOID_UINT16;

    entry = monoidhash_insert(monoidhash, "plus_monoid_uint32", &found);
    entry->name = strdup("plus_monoid_uint32");
    entry->monoid = GrB_PLUS_MONOID_UINT32;

    entry = monoidhash_insert(monoidhash, "plus_monoid_uint64", &found);
    entry->name = strdup("plus_monoid_uint64");
    entry->monoid = GrB_PLUS_MONOID_UINT64;

    entry = monoidhash_insert(monoidhash, "plus_monoid_fp32", &found);
    entry->name = strdup("plus_monoid_fp32");
    entry->monoid = GrB_PLUS_MONOID_FP32;

    entry = monoidhash_insert(monoidhash, "plus_monoid_fp64", &found);
    entry->name = strdup("plus_monoid_fp64");
    entry->monoid = GrB_PLUS_MONOID_FP64;

    entry = monoidhash_insert(monoidhash, "plus_fc32_monoid", &found);
    entry->name = strdup("plus_fc32_monoid");
    entry->monoid = GxB_PLUS_FC32_MONOID;

    entry = monoidhash_insert(monoidhash, "plus_fc64_monoid", &found);
    entry->name = strdup("plus_fc64_monoid");
    entry->monoid = GxB_PLUS_FC64_MONOID;

    entry = monoidhash_insert(monoidhash, "times_int8_monoid", &found);
    entry->name = strdup("times_int8_monoid");
    entry->monoid = GxB_TIMES_INT8_MONOID;

    entry = monoidhash_insert(monoidhash, "times_int16_monoid", &found);
    entry->name = strdup("times_int16_monoid");
    entry->monoid = GxB_TIMES_INT16_MONOID;

    entry = monoidhash_insert(monoidhash, "times_int32_monoid", &found);
    entry->name = strdup("times_int32_monoid");
    entry->monoid = GxB_TIMES_INT32_MONOID;

    entry = monoidhash_insert(monoidhash, "times_int64_monoid", &found);
    entry->name = strdup("times_int64_monoid");
    entry->monoid = GxB_TIMES_INT64_MONOID;

    entry = monoidhash_insert(monoidhash, "times_uint8_monoid", &found);
    entry->name = strdup("times_uint8_monoid");
    entry->monoid = GxB_TIMES_UINT8_MONOID;

    entry = monoidhash_insert(monoidhash, "times_uint16_monoid", &found);
    entry->name = strdup("times_uint16_monoid");
    entry->monoid = GxB_TIMES_UINT16_MONOID;

    entry = monoidhash_insert(monoidhash, "times_uint32_monoid", &found);
    entry->name = strdup("times_uint32_monoid");
    entry->monoid = GxB_TIMES_UINT32_MONOID;

    entry = monoidhash_insert(monoidhash, "times_uint64_monoid", &found);
    entry->name = strdup("times_uint64_monoid");
    entry->monoid = GxB_TIMES_UINT64_MONOID;

    entry = monoidhash_insert(monoidhash, "times_fp32_monoid", &found);
    entry->name = strdup("times_fp32_monoid");
    entry->monoid = GxB_TIMES_FP32_MONOID;

    entry = monoidhash_insert(monoidhash, "times_fp64_monoid", &found);
    entry->name = strdup("times_fp64_monoid");
    entry->monoid = GxB_TIMES_FP64_MONOID;

    entry = monoidhash_insert(monoidhash, "times_monoid_int8", &found);
    entry->name = strdup("times_monoid_int8");
    entry->monoid = GrB_TIMES_MONOID_INT8;

    entry = monoidhash_insert(monoidhash, "times_monoid_int16", &found);
    entry->name = strdup("times_monoid_int16");
    entry->monoid = GrB_TIMES_MONOID_INT16;

    entry = monoidhash_insert(monoidhash, "times_monoid_int32", &found);
    entry->name = strdup("times_monoid_int32");
    entry->monoid = GrB_TIMES_MONOID_INT32;

    entry = monoidhash_insert(monoidhash, "times_monoid_int64", &found);
    entry->name = strdup("times_monoid_int64");
    entry->monoid = GrB_TIMES_MONOID_INT64;

    entry = monoidhash_insert(monoidhash, "times_monoid_uint8", &found);
    entry->name = strdup("times_monoid_uint8");
    entry->monoid = GrB_TIMES_MONOID_UINT8;

    entry = monoidhash_insert(monoidhash, "times_monoid_uint16", &found);
    entry->name = strdup("times_monoid_uint16");
    entry->monoid = GrB_TIMES_MONOID_UINT16;

    entry = monoidhash_insert(monoidhash, "times_monoid_uint32", &found);
    entry->name = strdup("times_monoid_uint32");
    entry->monoid = GrB_TIMES_MONOID_UINT32;

    entry = monoidhash_insert(monoidhash, "times_monoid_uint64", &found);
    entry->name = strdup("times_monoid_uint64");
    entry->monoid = GrB_TIMES_MONOID_UINT64;

    entry = monoidhash_insert(monoidhash, "times_monoid_fp32", &found);
    entry->name = strdup("times_monoid_fp32");
    entry->monoid = GrB_TIMES_MONOID_FP32;

    entry = monoidhash_insert(monoidhash, "times_monoid_fp64", &found);
    entry->name = strdup("times_monoid_fp64");
    entry->monoid = GrB_TIMES_MONOID_FP64;

    entry = monoidhash_insert(monoidhash, "times_fc32_monoid", &found);
    entry->name = strdup("times_fc32_monoid");
    entry->monoid = GxB_TIMES_FC32_MONOID;

    entry = monoidhash_insert(monoidhash, "times_fc64_monoid", &found);
    entry->name = strdup("times_fc64_monoid");
    entry->monoid = GxB_TIMES_FC64_MONOID;

    entry = monoidhash_insert(monoidhash, "any_bool_monoid", &found);
    entry->name = strdup("any_bool_monoid");
    entry->monoid = GxB_ANY_BOOL_MONOID;

    entry = monoidhash_insert(monoidhash, "any_int8_monoid", &found);
    entry->name = strdup("any_int8_monoid");
    entry->monoid = GxB_ANY_INT8_MONOID;

    entry = monoidhash_insert(monoidhash, "any_int16_monoid", &found);
    entry->name = strdup("any_int16_monoid");
    entry->monoid = GxB_ANY_INT16_MONOID;

    entry = monoidhash_insert(monoidhash, "any_int32_monoid", &found);
    entry->name = strdup("any_int32_monoid");
    entry->monoid = GxB_ANY_INT32_MONOID;

    entry = monoidhash_insert(monoidhash, "any_int64_monoid", &found);
    entry->name = strdup("any_int64_monoid");
    entry->monoid = GxB_ANY_INT64_MONOID;

    entry = monoidhash_insert(monoidhash, "any_uint8_monoid", &found);
    entry->name = strdup("any_uint8_monoid");
    entry->monoid = GxB_ANY_UINT8_MONOID;

    entry = monoidhash_insert(monoidhash, "any_uint16_monoid", &found);
    entry->name = strdup("any_uint16_monoid");
    entry->monoid = GxB_ANY_UINT16_MONOID;

    entry = monoidhash_insert(monoidhash, "any_uint32_monoid", &found);
    entry->name = strdup("any_uint32_monoid");
    entry->monoid = GxB_ANY_UINT32_MONOID;

    entry = monoidhash_insert(monoidhash, "any_uint64_monoid", &found);
    entry->name = strdup("any_uint64_monoid");
    entry->monoid = GxB_ANY_UINT64_MONOID;

    entry = monoidhash_insert(monoidhash, "any_fp32_monoid", &found);
    entry->name = strdup("any_fp32_monoid");
    entry->monoid = GxB_ANY_FP32_MONOID;

    entry = monoidhash_insert(monoidhash, "any_fp64_monoid", &found);
    entry->name = strdup("any_fp64_monoid");
    entry->monoid = GxB_ANY_FP64_MONOID;

    entry = monoidhash_insert(monoidhash, "any_fc32_monoid", &found);
    entry->name = strdup("any_fc32_monoid");
    entry->monoid = GxB_ANY_FC32_MONOID;

    entry = monoidhash_insert(monoidhash, "any_fc64_monoid", &found);
    entry->name = strdup("any_fc64_monoid");
    entry->monoid = GxB_ANY_FC64_MONOID;

    entry = monoidhash_insert(monoidhash, "lor_bool_monoid", &found);
    entry->name = strdup("lor_bool_monoid");
    entry->monoid = GxB_LOR_BOOL_MONOID;

    entry = monoidhash_insert(monoidhash, "land_bool_monoid", &found);
    entry->name = strdup("land_bool_monoid");
    entry->monoid = GxB_LAND_BOOL_MONOID;

    entry = monoidhash_insert(monoidhash, "lxor_bool_monoid", &found);
    entry->name = strdup("lxor_bool_monoid");
    entry->monoid = GxB_LXOR_BOOL_MONOID;

    entry = monoidhash_insert(monoidhash, "lxnor_bool_monoid", &found);
    entry->name = strdup("lxnor_bool_monoid");
    entry->monoid = GxB_LXNOR_BOOL_MONOID;

    entry = monoidhash_insert(monoidhash, "eq_bool_monoid", &found);
    entry->name = strdup("eq_bool_monoid");
    entry->monoid = GxB_EQ_BOOL_MONOID;

    entry = monoidhash_insert(monoidhash, "lor_monoid_bool", &found);
    entry->name = strdup("lor_monoid_bool");
    entry->monoid = GrB_LOR_MONOID_BOOL;

    entry = monoidhash_insert(monoidhash, "land_monoid_bool", &found);
    entry->name = strdup("land_monoid_bool");
    entry->monoid = GrB_LAND_MONOID_BOOL;

    entry = monoidhash_insert(monoidhash, "lxor_monoid_bool", &found);
    entry->name = strdup("lxor_monoid_bool");
    entry->monoid = GrB_LXOR_MONOID_BOOL;

    entry = monoidhash_insert(monoidhash, "lxnor_monoid_bool", &found);
    entry->name = strdup("lxnor_monoid_bool");
    entry->monoid = GrB_LXNOR_MONOID_BOOL;

    entry = monoidhash_insert(monoidhash, "bor_uint8_monoid", &found);
    entry->name = strdup("bor_uint8_monoid");
    entry->monoid = GxB_BOR_UINT8_MONOID;

    entry = monoidhash_insert(monoidhash, "bor_uint16_monoid", &found);
    entry->name = strdup("bor_uint16_monoid");
    entry->monoid = GxB_BOR_UINT16_MONOID;

    entry = monoidhash_insert(monoidhash, "bor_uint32_monoid", &found);
    entry->name = strdup("bor_uint32_monoid");
    entry->monoid = GxB_BOR_UINT32_MONOID;

    entry = monoidhash_insert(monoidhash, "bor_uint64_monoid", &found);
    entry->name = strdup("bor_uint64_monoid");
    entry->monoid = GxB_BOR_UINT64_MONOID;

    entry = monoidhash_insert(monoidhash, "band_uint8_monoid", &found);
    entry->name = strdup("band_uint8_monoid");
    entry->monoid = GxB_BAND_UINT8_MONOID;

    entry = monoidhash_insert(monoidhash, "band_uint16_monoid", &found);
    entry->name = strdup("band_uint16_monoid");
    entry->monoid = GxB_BAND_UINT16_MONOID;

    entry = monoidhash_insert(monoidhash, "band_uint32_monoid", &found);
    entry->name = strdup("band_uint32_monoid");
    entry->monoid = GxB_BAND_UINT32_MONOID;

    entry = monoidhash_insert(monoidhash, "band_uint64_monoid", &found);
    entry->name = strdup("band_uint64_monoid");
    entry->monoid = GxB_BAND_UINT64_MONOID;

    entry = monoidhash_insert(monoidhash, "bxor_uint8_monoid", &found);
    entry->name = strdup("bxor_uint8_monoid");
    entry->monoid = GxB_BXOR_UINT8_MONOID;

    entry = monoidhash_insert(monoidhash, "bxor_uint16_monoid", &found);
    entry->name = strdup("bxor_uint16_monoid");
    entry->monoid = GxB_BXOR_UINT16_MONOID;

    entry = monoidhash_insert(monoidhash, "bxor_uint32_monoid", &found);
    entry->name = strdup("bxor_uint32_monoid");
    entry->monoid = GxB_BXOR_UINT32_MONOID;

    entry = monoidhash_insert(monoidhash, "bxor_uint64_monoid", &found);
    entry->name = strdup("bxor_uint64_monoid");
    entry->monoid = GxB_BXOR_UINT64_MONOID;

    entry = monoidhash_insert(monoidhash, "bxnor_uint8_monoid", &found);
    entry->name = strdup("bxnor_uint8_monoid");
    entry->monoid = GxB_BXNOR_UINT8_MONOID;

    entry = monoidhash_insert(monoidhash, "bxnor_uint16_monoid", &found);
    entry->name = strdup("bxnor_uint16_monoid");
    entry->monoid = GxB_BXNOR_UINT16_MONOID;

    entry = monoidhash_insert(monoidhash, "bxnor_uint32_monoid", &found);
    entry->name = strdup("bxnor_uint32_monoid");
    entry->monoid = GxB_BXNOR_UINT32_MONOID;

    entry = monoidhash_insert(monoidhash, "bxnor_uint64_monoid", &found);
    entry->name = strdup("bxnor_uint64_monoid");
    entry->monoid = GxB_BXNOR_UINT64_MONOID;
    }

GrB_Monoid lookup_monoid(char *name)
{
    monoid_entry *entry;
	entry = monoidhash_lookup(monoidhash, name);
    if (entry)
        return entry->monoid;
    return NULL;
    }
