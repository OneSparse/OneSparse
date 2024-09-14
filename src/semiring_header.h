typedef struct semiring_entry
{
	uint32 status;
	char *name;
    GrB_Semiring semiring;
} semiring_entry;

static uint32 hash_string_pointer(char *s);

static uint32
hash_string_pointer(char *s)
{
	unsigned char *ss = (unsigned char *) s;

	return hash_bytes(ss, strlen(s));
    }

#define SEMIRINGHASH_INITIAL_SIZE 2000

#define SH_PREFIX		semiringhash
#define SH_ELEMENT_TYPE semiring_entry
#define SH_KEY_TYPE		char *
#define	SH_KEY			name
#define SH_HASH_KEY(tb, key)	hash_string_pointer(key)
#define SH_EQUAL(tb, a, b)		(strcmp(a, b) == 0)
#define	SH_SCOPE		static inline
#define SH_DECLARE
#define SH_DEFINE
#include "lib/simplehash.h"

static semiringhash_hash *semiringhash;

void initialize_semirings()
{
    bool found;
    semiring_entry *entry;

    semiringhash = semiringhash_create(TopMemoryContext, SEMIRINGHASH_INITIAL_SIZE, NULL);
    
    entry = semiringhash_insert(semiringhash, "min_first_int8", &found);
    entry->name = pstrdup("min_first_int8");
    entry->semiring = GxB_MIN_FIRST_INT8;

    entry = semiringhash_insert(semiringhash, "max_first_int8", &found);
    entry->name = pstrdup("max_first_int8");
    entry->semiring = GxB_MAX_FIRST_INT8;

    entry = semiringhash_insert(semiringhash, "plus_first_int8", &found);
    entry->name = pstrdup("plus_first_int8");
    entry->semiring = GxB_PLUS_FIRST_INT8;

    entry = semiringhash_insert(semiringhash, "times_first_int8", &found);
    entry->name = pstrdup("times_first_int8");
    entry->semiring = GxB_TIMES_FIRST_INT8;

    entry = semiringhash_insert(semiringhash, "any_first_int8", &found);
    entry->name = pstrdup("any_first_int8");
    entry->semiring = GxB_ANY_FIRST_INT8;

    entry = semiringhash_insert(semiringhash, "min_first_int16", &found);
    entry->name = pstrdup("min_first_int16");
    entry->semiring = GxB_MIN_FIRST_INT16;

    entry = semiringhash_insert(semiringhash, "max_first_int16", &found);
    entry->name = pstrdup("max_first_int16");
    entry->semiring = GxB_MAX_FIRST_INT16;

    entry = semiringhash_insert(semiringhash, "plus_first_int16", &found);
    entry->name = pstrdup("plus_first_int16");
    entry->semiring = GxB_PLUS_FIRST_INT16;

    entry = semiringhash_insert(semiringhash, "times_first_int16", &found);
    entry->name = pstrdup("times_first_int16");
    entry->semiring = GxB_TIMES_FIRST_INT16;

    entry = semiringhash_insert(semiringhash, "any_first_int16", &found);
    entry->name = pstrdup("any_first_int16");
    entry->semiring = GxB_ANY_FIRST_INT16;

    entry = semiringhash_insert(semiringhash, "min_first_int32", &found);
    entry->name = pstrdup("min_first_int32");
    entry->semiring = GxB_MIN_FIRST_INT32;

    entry = semiringhash_insert(semiringhash, "max_first_int32", &found);
    entry->name = pstrdup("max_first_int32");
    entry->semiring = GxB_MAX_FIRST_INT32;

    entry = semiringhash_insert(semiringhash, "plus_first_int32", &found);
    entry->name = pstrdup("plus_first_int32");
    entry->semiring = GxB_PLUS_FIRST_INT32;

    entry = semiringhash_insert(semiringhash, "times_first_int32", &found);
    entry->name = pstrdup("times_first_int32");
    entry->semiring = GxB_TIMES_FIRST_INT32;

    entry = semiringhash_insert(semiringhash, "any_first_int32", &found);
    entry->name = pstrdup("any_first_int32");
    entry->semiring = GxB_ANY_FIRST_INT32;

    entry = semiringhash_insert(semiringhash, "min_first_int64", &found);
    entry->name = pstrdup("min_first_int64");
    entry->semiring = GxB_MIN_FIRST_INT64;

    entry = semiringhash_insert(semiringhash, "max_first_int64", &found);
    entry->name = pstrdup("max_first_int64");
    entry->semiring = GxB_MAX_FIRST_INT64;

    entry = semiringhash_insert(semiringhash, "plus_first_int64", &found);
    entry->name = pstrdup("plus_first_int64");
    entry->semiring = GxB_PLUS_FIRST_INT64;

    entry = semiringhash_insert(semiringhash, "times_first_int64", &found);
    entry->name = pstrdup("times_first_int64");
    entry->semiring = GxB_TIMES_FIRST_INT64;

    entry = semiringhash_insert(semiringhash, "any_first_int64", &found);
    entry->name = pstrdup("any_first_int64");
    entry->semiring = GxB_ANY_FIRST_INT64;

    entry = semiringhash_insert(semiringhash, "min_first_uint8", &found);
    entry->name = pstrdup("min_first_uint8");
    entry->semiring = GxB_MIN_FIRST_UINT8;

    entry = semiringhash_insert(semiringhash, "max_first_uint8", &found);
    entry->name = pstrdup("max_first_uint8");
    entry->semiring = GxB_MAX_FIRST_UINT8;

    entry = semiringhash_insert(semiringhash, "plus_first_uint8", &found);
    entry->name = pstrdup("plus_first_uint8");
    entry->semiring = GxB_PLUS_FIRST_UINT8;

    entry = semiringhash_insert(semiringhash, "times_first_uint8", &found);
    entry->name = pstrdup("times_first_uint8");
    entry->semiring = GxB_TIMES_FIRST_UINT8;

    entry = semiringhash_insert(semiringhash, "any_first_uint8", &found);
    entry->name = pstrdup("any_first_uint8");
    entry->semiring = GxB_ANY_FIRST_UINT8;

    entry = semiringhash_insert(semiringhash, "min_first_uint16", &found);
    entry->name = pstrdup("min_first_uint16");
    entry->semiring = GxB_MIN_FIRST_UINT16;

    entry = semiringhash_insert(semiringhash, "max_first_uint16", &found);
    entry->name = pstrdup("max_first_uint16");
    entry->semiring = GxB_MAX_FIRST_UINT16;

    entry = semiringhash_insert(semiringhash, "plus_first_uint16", &found);
    entry->name = pstrdup("plus_first_uint16");
    entry->semiring = GxB_PLUS_FIRST_UINT16;

    entry = semiringhash_insert(semiringhash, "times_first_uint16", &found);
    entry->name = pstrdup("times_first_uint16");
    entry->semiring = GxB_TIMES_FIRST_UINT16;

    entry = semiringhash_insert(semiringhash, "any_first_uint16", &found);
    entry->name = pstrdup("any_first_uint16");
    entry->semiring = GxB_ANY_FIRST_UINT16;

    entry = semiringhash_insert(semiringhash, "min_first_uint32", &found);
    entry->name = pstrdup("min_first_uint32");
    entry->semiring = GxB_MIN_FIRST_UINT32;

    entry = semiringhash_insert(semiringhash, "max_first_uint32", &found);
    entry->name = pstrdup("max_first_uint32");
    entry->semiring = GxB_MAX_FIRST_UINT32;

    entry = semiringhash_insert(semiringhash, "plus_first_uint32", &found);
    entry->name = pstrdup("plus_first_uint32");
    entry->semiring = GxB_PLUS_FIRST_UINT32;

    entry = semiringhash_insert(semiringhash, "times_first_uint32", &found);
    entry->name = pstrdup("times_first_uint32");
    entry->semiring = GxB_TIMES_FIRST_UINT32;

    entry = semiringhash_insert(semiringhash, "any_first_uint32", &found);
    entry->name = pstrdup("any_first_uint32");
    entry->semiring = GxB_ANY_FIRST_UINT32;

    entry = semiringhash_insert(semiringhash, "min_first_uint64", &found);
    entry->name = pstrdup("min_first_uint64");
    entry->semiring = GxB_MIN_FIRST_UINT64;

    entry = semiringhash_insert(semiringhash, "max_first_uint64", &found);
    entry->name = pstrdup("max_first_uint64");
    entry->semiring = GxB_MAX_FIRST_UINT64;

    entry = semiringhash_insert(semiringhash, "plus_first_uint64", &found);
    entry->name = pstrdup("plus_first_uint64");
    entry->semiring = GxB_PLUS_FIRST_UINT64;

    entry = semiringhash_insert(semiringhash, "times_first_uint64", &found);
    entry->name = pstrdup("times_first_uint64");
    entry->semiring = GxB_TIMES_FIRST_UINT64;

    entry = semiringhash_insert(semiringhash, "any_first_uint64", &found);
    entry->name = pstrdup("any_first_uint64");
    entry->semiring = GxB_ANY_FIRST_UINT64;

    entry = semiringhash_insert(semiringhash, "min_first_fp32", &found);
    entry->name = pstrdup("min_first_fp32");
    entry->semiring = GxB_MIN_FIRST_FP32;

    entry = semiringhash_insert(semiringhash, "max_first_fp32", &found);
    entry->name = pstrdup("max_first_fp32");
    entry->semiring = GxB_MAX_FIRST_FP32;

    entry = semiringhash_insert(semiringhash, "plus_first_fp32", &found);
    entry->name = pstrdup("plus_first_fp32");
    entry->semiring = GxB_PLUS_FIRST_FP32;

    entry = semiringhash_insert(semiringhash, "times_first_fp32", &found);
    entry->name = pstrdup("times_first_fp32");
    entry->semiring = GxB_TIMES_FIRST_FP32;

    entry = semiringhash_insert(semiringhash, "any_first_fp32", &found);
    entry->name = pstrdup("any_first_fp32");
    entry->semiring = GxB_ANY_FIRST_FP32;

    entry = semiringhash_insert(semiringhash, "min_first_fp64", &found);
    entry->name = pstrdup("min_first_fp64");
    entry->semiring = GxB_MIN_FIRST_FP64;

    entry = semiringhash_insert(semiringhash, "max_first_fp64", &found);
    entry->name = pstrdup("max_first_fp64");
    entry->semiring = GxB_MAX_FIRST_FP64;

    entry = semiringhash_insert(semiringhash, "plus_first_fp64", &found);
    entry->name = pstrdup("plus_first_fp64");
    entry->semiring = GxB_PLUS_FIRST_FP64;

    entry = semiringhash_insert(semiringhash, "times_first_fp64", &found);
    entry->name = pstrdup("times_first_fp64");
    entry->semiring = GxB_TIMES_FIRST_FP64;

    entry = semiringhash_insert(semiringhash, "any_first_fp64", &found);
    entry->name = pstrdup("any_first_fp64");
    entry->semiring = GxB_ANY_FIRST_FP64;

    entry = semiringhash_insert(semiringhash, "min_second_int8", &found);
    entry->name = pstrdup("min_second_int8");
    entry->semiring = GxB_MIN_SECOND_INT8;

    entry = semiringhash_insert(semiringhash, "max_second_int8", &found);
    entry->name = pstrdup("max_second_int8");
    entry->semiring = GxB_MAX_SECOND_INT8;

    entry = semiringhash_insert(semiringhash, "plus_second_int8", &found);
    entry->name = pstrdup("plus_second_int8");
    entry->semiring = GxB_PLUS_SECOND_INT8;

    entry = semiringhash_insert(semiringhash, "times_second_int8", &found);
    entry->name = pstrdup("times_second_int8");
    entry->semiring = GxB_TIMES_SECOND_INT8;

    entry = semiringhash_insert(semiringhash, "any_second_int8", &found);
    entry->name = pstrdup("any_second_int8");
    entry->semiring = GxB_ANY_SECOND_INT8;

    entry = semiringhash_insert(semiringhash, "min_second_int16", &found);
    entry->name = pstrdup("min_second_int16");
    entry->semiring = GxB_MIN_SECOND_INT16;

    entry = semiringhash_insert(semiringhash, "max_second_int16", &found);
    entry->name = pstrdup("max_second_int16");
    entry->semiring = GxB_MAX_SECOND_INT16;

    entry = semiringhash_insert(semiringhash, "plus_second_int16", &found);
    entry->name = pstrdup("plus_second_int16");
    entry->semiring = GxB_PLUS_SECOND_INT16;

    entry = semiringhash_insert(semiringhash, "times_second_int16", &found);
    entry->name = pstrdup("times_second_int16");
    entry->semiring = GxB_TIMES_SECOND_INT16;

    entry = semiringhash_insert(semiringhash, "any_second_int16", &found);
    entry->name = pstrdup("any_second_int16");
    entry->semiring = GxB_ANY_SECOND_INT16;

    entry = semiringhash_insert(semiringhash, "min_second_int32", &found);
    entry->name = pstrdup("min_second_int32");
    entry->semiring = GxB_MIN_SECOND_INT32;

    entry = semiringhash_insert(semiringhash, "max_second_int32", &found);
    entry->name = pstrdup("max_second_int32");
    entry->semiring = GxB_MAX_SECOND_INT32;

    entry = semiringhash_insert(semiringhash, "plus_second_int32", &found);
    entry->name = pstrdup("plus_second_int32");
    entry->semiring = GxB_PLUS_SECOND_INT32;

    entry = semiringhash_insert(semiringhash, "times_second_int32", &found);
    entry->name = pstrdup("times_second_int32");
    entry->semiring = GxB_TIMES_SECOND_INT32;

    entry = semiringhash_insert(semiringhash, "any_second_int32", &found);
    entry->name = pstrdup("any_second_int32");
    entry->semiring = GxB_ANY_SECOND_INT32;

    entry = semiringhash_insert(semiringhash, "min_second_int64", &found);
    entry->name = pstrdup("min_second_int64");
    entry->semiring = GxB_MIN_SECOND_INT64;

    entry = semiringhash_insert(semiringhash, "max_second_int64", &found);
    entry->name = pstrdup("max_second_int64");
    entry->semiring = GxB_MAX_SECOND_INT64;

    entry = semiringhash_insert(semiringhash, "plus_second_int64", &found);
    entry->name = pstrdup("plus_second_int64");
    entry->semiring = GxB_PLUS_SECOND_INT64;

    entry = semiringhash_insert(semiringhash, "times_second_int64", &found);
    entry->name = pstrdup("times_second_int64");
    entry->semiring = GxB_TIMES_SECOND_INT64;

    entry = semiringhash_insert(semiringhash, "any_second_int64", &found);
    entry->name = pstrdup("any_second_int64");
    entry->semiring = GxB_ANY_SECOND_INT64;

    entry = semiringhash_insert(semiringhash, "min_second_uint8", &found);
    entry->name = pstrdup("min_second_uint8");
    entry->semiring = GxB_MIN_SECOND_UINT8;

    entry = semiringhash_insert(semiringhash, "max_second_uint8", &found);
    entry->name = pstrdup("max_second_uint8");
    entry->semiring = GxB_MAX_SECOND_UINT8;

    entry = semiringhash_insert(semiringhash, "plus_second_uint8", &found);
    entry->name = pstrdup("plus_second_uint8");
    entry->semiring = GxB_PLUS_SECOND_UINT8;

    entry = semiringhash_insert(semiringhash, "times_second_uint8", &found);
    entry->name = pstrdup("times_second_uint8");
    entry->semiring = GxB_TIMES_SECOND_UINT8;

    entry = semiringhash_insert(semiringhash, "any_second_uint8", &found);
    entry->name = pstrdup("any_second_uint8");
    entry->semiring = GxB_ANY_SECOND_UINT8;

    entry = semiringhash_insert(semiringhash, "min_second_uint16", &found);
    entry->name = pstrdup("min_second_uint16");
    entry->semiring = GxB_MIN_SECOND_UINT16;

    entry = semiringhash_insert(semiringhash, "max_second_uint16", &found);
    entry->name = pstrdup("max_second_uint16");
    entry->semiring = GxB_MAX_SECOND_UINT16;

    entry = semiringhash_insert(semiringhash, "plus_second_uint16", &found);
    entry->name = pstrdup("plus_second_uint16");
    entry->semiring = GxB_PLUS_SECOND_UINT16;

    entry = semiringhash_insert(semiringhash, "times_second_uint16", &found);
    entry->name = pstrdup("times_second_uint16");
    entry->semiring = GxB_TIMES_SECOND_UINT16;

    entry = semiringhash_insert(semiringhash, "any_second_uint16", &found);
    entry->name = pstrdup("any_second_uint16");
    entry->semiring = GxB_ANY_SECOND_UINT16;

    entry = semiringhash_insert(semiringhash, "min_second_uint32", &found);
    entry->name = pstrdup("min_second_uint32");
    entry->semiring = GxB_MIN_SECOND_UINT32;

    entry = semiringhash_insert(semiringhash, "max_second_uint32", &found);
    entry->name = pstrdup("max_second_uint32");
    entry->semiring = GxB_MAX_SECOND_UINT32;

    entry = semiringhash_insert(semiringhash, "plus_second_uint32", &found);
    entry->name = pstrdup("plus_second_uint32");
    entry->semiring = GxB_PLUS_SECOND_UINT32;

    entry = semiringhash_insert(semiringhash, "times_second_uint32", &found);
    entry->name = pstrdup("times_second_uint32");
    entry->semiring = GxB_TIMES_SECOND_UINT32;

    entry = semiringhash_insert(semiringhash, "any_second_uint32", &found);
    entry->name = pstrdup("any_second_uint32");
    entry->semiring = GxB_ANY_SECOND_UINT32;

    entry = semiringhash_insert(semiringhash, "min_second_uint64", &found);
    entry->name = pstrdup("min_second_uint64");
    entry->semiring = GxB_MIN_SECOND_UINT64;

    entry = semiringhash_insert(semiringhash, "max_second_uint64", &found);
    entry->name = pstrdup("max_second_uint64");
    entry->semiring = GxB_MAX_SECOND_UINT64;

    entry = semiringhash_insert(semiringhash, "plus_second_uint64", &found);
    entry->name = pstrdup("plus_second_uint64");
    entry->semiring = GxB_PLUS_SECOND_UINT64;

    entry = semiringhash_insert(semiringhash, "times_second_uint64", &found);
    entry->name = pstrdup("times_second_uint64");
    entry->semiring = GxB_TIMES_SECOND_UINT64;

    entry = semiringhash_insert(semiringhash, "any_second_uint64", &found);
    entry->name = pstrdup("any_second_uint64");
    entry->semiring = GxB_ANY_SECOND_UINT64;

    entry = semiringhash_insert(semiringhash, "min_second_fp32", &found);
    entry->name = pstrdup("min_second_fp32");
    entry->semiring = GxB_MIN_SECOND_FP32;

    entry = semiringhash_insert(semiringhash, "max_second_fp32", &found);
    entry->name = pstrdup("max_second_fp32");
    entry->semiring = GxB_MAX_SECOND_FP32;

    entry = semiringhash_insert(semiringhash, "plus_second_fp32", &found);
    entry->name = pstrdup("plus_second_fp32");
    entry->semiring = GxB_PLUS_SECOND_FP32;

    entry = semiringhash_insert(semiringhash, "times_second_fp32", &found);
    entry->name = pstrdup("times_second_fp32");
    entry->semiring = GxB_TIMES_SECOND_FP32;

    entry = semiringhash_insert(semiringhash, "any_second_fp32", &found);
    entry->name = pstrdup("any_second_fp32");
    entry->semiring = GxB_ANY_SECOND_FP32;

    entry = semiringhash_insert(semiringhash, "min_second_fp64", &found);
    entry->name = pstrdup("min_second_fp64");
    entry->semiring = GxB_MIN_SECOND_FP64;

    entry = semiringhash_insert(semiringhash, "max_second_fp64", &found);
    entry->name = pstrdup("max_second_fp64");
    entry->semiring = GxB_MAX_SECOND_FP64;

    entry = semiringhash_insert(semiringhash, "plus_second_fp64", &found);
    entry->name = pstrdup("plus_second_fp64");
    entry->semiring = GxB_PLUS_SECOND_FP64;

    entry = semiringhash_insert(semiringhash, "times_second_fp64", &found);
    entry->name = pstrdup("times_second_fp64");
    entry->semiring = GxB_TIMES_SECOND_FP64;

    entry = semiringhash_insert(semiringhash, "any_second_fp64", &found);
    entry->name = pstrdup("any_second_fp64");
    entry->semiring = GxB_ANY_SECOND_FP64;

    entry = semiringhash_insert(semiringhash, "min_pair_int8", &found);
    entry->name = pstrdup("min_pair_int8");
    entry->semiring = GxB_MIN_PAIR_INT8;

    entry = semiringhash_insert(semiringhash, "max_pair_int8", &found);
    entry->name = pstrdup("max_pair_int8");
    entry->semiring = GxB_MAX_PAIR_INT8;

    entry = semiringhash_insert(semiringhash, "plus_pair_int8", &found);
    entry->name = pstrdup("plus_pair_int8");
    entry->semiring = GxB_PLUS_PAIR_INT8;

    entry = semiringhash_insert(semiringhash, "times_pair_int8", &found);
    entry->name = pstrdup("times_pair_int8");
    entry->semiring = GxB_TIMES_PAIR_INT8;

    entry = semiringhash_insert(semiringhash, "any_pair_int8", &found);
    entry->name = pstrdup("any_pair_int8");
    entry->semiring = GxB_ANY_PAIR_INT8;

    entry = semiringhash_insert(semiringhash, "min_pair_int16", &found);
    entry->name = pstrdup("min_pair_int16");
    entry->semiring = GxB_MIN_PAIR_INT16;

    entry = semiringhash_insert(semiringhash, "max_pair_int16", &found);
    entry->name = pstrdup("max_pair_int16");
    entry->semiring = GxB_MAX_PAIR_INT16;

    entry = semiringhash_insert(semiringhash, "plus_pair_int16", &found);
    entry->name = pstrdup("plus_pair_int16");
    entry->semiring = GxB_PLUS_PAIR_INT16;

    entry = semiringhash_insert(semiringhash, "times_pair_int16", &found);
    entry->name = pstrdup("times_pair_int16");
    entry->semiring = GxB_TIMES_PAIR_INT16;

    entry = semiringhash_insert(semiringhash, "any_pair_int16", &found);
    entry->name = pstrdup("any_pair_int16");
    entry->semiring = GxB_ANY_PAIR_INT16;

    entry = semiringhash_insert(semiringhash, "min_pair_int32", &found);
    entry->name = pstrdup("min_pair_int32");
    entry->semiring = GxB_MIN_PAIR_INT32;

    entry = semiringhash_insert(semiringhash, "max_pair_int32", &found);
    entry->name = pstrdup("max_pair_int32");
    entry->semiring = GxB_MAX_PAIR_INT32;

    entry = semiringhash_insert(semiringhash, "plus_pair_int32", &found);
    entry->name = pstrdup("plus_pair_int32");
    entry->semiring = GxB_PLUS_PAIR_INT32;

    entry = semiringhash_insert(semiringhash, "times_pair_int32", &found);
    entry->name = pstrdup("times_pair_int32");
    entry->semiring = GxB_TIMES_PAIR_INT32;

    entry = semiringhash_insert(semiringhash, "any_pair_int32", &found);
    entry->name = pstrdup("any_pair_int32");
    entry->semiring = GxB_ANY_PAIR_INT32;

    entry = semiringhash_insert(semiringhash, "min_pair_int64", &found);
    entry->name = pstrdup("min_pair_int64");
    entry->semiring = GxB_MIN_PAIR_INT64;

    entry = semiringhash_insert(semiringhash, "max_pair_int64", &found);
    entry->name = pstrdup("max_pair_int64");
    entry->semiring = GxB_MAX_PAIR_INT64;

    entry = semiringhash_insert(semiringhash, "plus_pair_int64", &found);
    entry->name = pstrdup("plus_pair_int64");
    entry->semiring = GxB_PLUS_PAIR_INT64;

    entry = semiringhash_insert(semiringhash, "times_pair_int64", &found);
    entry->name = pstrdup("times_pair_int64");
    entry->semiring = GxB_TIMES_PAIR_INT64;

    entry = semiringhash_insert(semiringhash, "any_pair_int64", &found);
    entry->name = pstrdup("any_pair_int64");
    entry->semiring = GxB_ANY_PAIR_INT64;

    entry = semiringhash_insert(semiringhash, "min_pair_uint8", &found);
    entry->name = pstrdup("min_pair_uint8");
    entry->semiring = GxB_MIN_PAIR_UINT8;

    entry = semiringhash_insert(semiringhash, "max_pair_uint8", &found);
    entry->name = pstrdup("max_pair_uint8");
    entry->semiring = GxB_MAX_PAIR_UINT8;

    entry = semiringhash_insert(semiringhash, "plus_pair_uint8", &found);
    entry->name = pstrdup("plus_pair_uint8");
    entry->semiring = GxB_PLUS_PAIR_UINT8;

    entry = semiringhash_insert(semiringhash, "times_pair_uint8", &found);
    entry->name = pstrdup("times_pair_uint8");
    entry->semiring = GxB_TIMES_PAIR_UINT8;

    entry = semiringhash_insert(semiringhash, "any_pair_uint8", &found);
    entry->name = pstrdup("any_pair_uint8");
    entry->semiring = GxB_ANY_PAIR_UINT8;

    entry = semiringhash_insert(semiringhash, "min_pair_uint16", &found);
    entry->name = pstrdup("min_pair_uint16");
    entry->semiring = GxB_MIN_PAIR_UINT16;

    entry = semiringhash_insert(semiringhash, "max_pair_uint16", &found);
    entry->name = pstrdup("max_pair_uint16");
    entry->semiring = GxB_MAX_PAIR_UINT16;

    entry = semiringhash_insert(semiringhash, "plus_pair_uint16", &found);
    entry->name = pstrdup("plus_pair_uint16");
    entry->semiring = GxB_PLUS_PAIR_UINT16;

    entry = semiringhash_insert(semiringhash, "times_pair_uint16", &found);
    entry->name = pstrdup("times_pair_uint16");
    entry->semiring = GxB_TIMES_PAIR_UINT16;

    entry = semiringhash_insert(semiringhash, "any_pair_uint16", &found);
    entry->name = pstrdup("any_pair_uint16");
    entry->semiring = GxB_ANY_PAIR_UINT16;

    entry = semiringhash_insert(semiringhash, "min_pair_uint32", &found);
    entry->name = pstrdup("min_pair_uint32");
    entry->semiring = GxB_MIN_PAIR_UINT32;

    entry = semiringhash_insert(semiringhash, "max_pair_uint32", &found);
    entry->name = pstrdup("max_pair_uint32");
    entry->semiring = GxB_MAX_PAIR_UINT32;

    entry = semiringhash_insert(semiringhash, "plus_pair_uint32", &found);
    entry->name = pstrdup("plus_pair_uint32");
    entry->semiring = GxB_PLUS_PAIR_UINT32;

    entry = semiringhash_insert(semiringhash, "times_pair_uint32", &found);
    entry->name = pstrdup("times_pair_uint32");
    entry->semiring = GxB_TIMES_PAIR_UINT32;

    entry = semiringhash_insert(semiringhash, "any_pair_uint32", &found);
    entry->name = pstrdup("any_pair_uint32");
    entry->semiring = GxB_ANY_PAIR_UINT32;

    entry = semiringhash_insert(semiringhash, "min_pair_uint64", &found);
    entry->name = pstrdup("min_pair_uint64");
    entry->semiring = GxB_MIN_PAIR_UINT64;

    entry = semiringhash_insert(semiringhash, "max_pair_uint64", &found);
    entry->name = pstrdup("max_pair_uint64");
    entry->semiring = GxB_MAX_PAIR_UINT64;

    entry = semiringhash_insert(semiringhash, "plus_pair_uint64", &found);
    entry->name = pstrdup("plus_pair_uint64");
    entry->semiring = GxB_PLUS_PAIR_UINT64;

    entry = semiringhash_insert(semiringhash, "times_pair_uint64", &found);
    entry->name = pstrdup("times_pair_uint64");
    entry->semiring = GxB_TIMES_PAIR_UINT64;

    entry = semiringhash_insert(semiringhash, "any_pair_uint64", &found);
    entry->name = pstrdup("any_pair_uint64");
    entry->semiring = GxB_ANY_PAIR_UINT64;

    entry = semiringhash_insert(semiringhash, "min_pair_fp32", &found);
    entry->name = pstrdup("min_pair_fp32");
    entry->semiring = GxB_MIN_PAIR_FP32;

    entry = semiringhash_insert(semiringhash, "max_pair_fp32", &found);
    entry->name = pstrdup("max_pair_fp32");
    entry->semiring = GxB_MAX_PAIR_FP32;

    entry = semiringhash_insert(semiringhash, "plus_pair_fp32", &found);
    entry->name = pstrdup("plus_pair_fp32");
    entry->semiring = GxB_PLUS_PAIR_FP32;

    entry = semiringhash_insert(semiringhash, "times_pair_fp32", &found);
    entry->name = pstrdup("times_pair_fp32");
    entry->semiring = GxB_TIMES_PAIR_FP32;

    entry = semiringhash_insert(semiringhash, "any_pair_fp32", &found);
    entry->name = pstrdup("any_pair_fp32");
    entry->semiring = GxB_ANY_PAIR_FP32;

    entry = semiringhash_insert(semiringhash, "min_pair_fp64", &found);
    entry->name = pstrdup("min_pair_fp64");
    entry->semiring = GxB_MIN_PAIR_FP64;

    entry = semiringhash_insert(semiringhash, "max_pair_fp64", &found);
    entry->name = pstrdup("max_pair_fp64");
    entry->semiring = GxB_MAX_PAIR_FP64;

    entry = semiringhash_insert(semiringhash, "plus_pair_fp64", &found);
    entry->name = pstrdup("plus_pair_fp64");
    entry->semiring = GxB_PLUS_PAIR_FP64;

    entry = semiringhash_insert(semiringhash, "times_pair_fp64", &found);
    entry->name = pstrdup("times_pair_fp64");
    entry->semiring = GxB_TIMES_PAIR_FP64;

    entry = semiringhash_insert(semiringhash, "any_pair_fp64", &found);
    entry->name = pstrdup("any_pair_fp64");
    entry->semiring = GxB_ANY_PAIR_FP64;

    entry = semiringhash_insert(semiringhash, "min_min_int8", &found);
    entry->name = pstrdup("min_min_int8");
    entry->semiring = GxB_MIN_MIN_INT8;

    entry = semiringhash_insert(semiringhash, "max_min_int8", &found);
    entry->name = pstrdup("max_min_int8");
    entry->semiring = GxB_MAX_MIN_INT8;

    entry = semiringhash_insert(semiringhash, "plus_min_int8", &found);
    entry->name = pstrdup("plus_min_int8");
    entry->semiring = GxB_PLUS_MIN_INT8;

    entry = semiringhash_insert(semiringhash, "times_min_int8", &found);
    entry->name = pstrdup("times_min_int8");
    entry->semiring = GxB_TIMES_MIN_INT8;

    entry = semiringhash_insert(semiringhash, "any_min_int8", &found);
    entry->name = pstrdup("any_min_int8");
    entry->semiring = GxB_ANY_MIN_INT8;

    entry = semiringhash_insert(semiringhash, "min_min_int16", &found);
    entry->name = pstrdup("min_min_int16");
    entry->semiring = GxB_MIN_MIN_INT16;

    entry = semiringhash_insert(semiringhash, "max_min_int16", &found);
    entry->name = pstrdup("max_min_int16");
    entry->semiring = GxB_MAX_MIN_INT16;

    entry = semiringhash_insert(semiringhash, "plus_min_int16", &found);
    entry->name = pstrdup("plus_min_int16");
    entry->semiring = GxB_PLUS_MIN_INT16;

    entry = semiringhash_insert(semiringhash, "times_min_int16", &found);
    entry->name = pstrdup("times_min_int16");
    entry->semiring = GxB_TIMES_MIN_INT16;

    entry = semiringhash_insert(semiringhash, "any_min_int16", &found);
    entry->name = pstrdup("any_min_int16");
    entry->semiring = GxB_ANY_MIN_INT16;

    entry = semiringhash_insert(semiringhash, "min_min_int32", &found);
    entry->name = pstrdup("min_min_int32");
    entry->semiring = GxB_MIN_MIN_INT32;

    entry = semiringhash_insert(semiringhash, "max_min_int32", &found);
    entry->name = pstrdup("max_min_int32");
    entry->semiring = GxB_MAX_MIN_INT32;

    entry = semiringhash_insert(semiringhash, "plus_min_int32", &found);
    entry->name = pstrdup("plus_min_int32");
    entry->semiring = GxB_PLUS_MIN_INT32;

    entry = semiringhash_insert(semiringhash, "times_min_int32", &found);
    entry->name = pstrdup("times_min_int32");
    entry->semiring = GxB_TIMES_MIN_INT32;

    entry = semiringhash_insert(semiringhash, "any_min_int32", &found);
    entry->name = pstrdup("any_min_int32");
    entry->semiring = GxB_ANY_MIN_INT32;

    entry = semiringhash_insert(semiringhash, "min_min_int64", &found);
    entry->name = pstrdup("min_min_int64");
    entry->semiring = GxB_MIN_MIN_INT64;

    entry = semiringhash_insert(semiringhash, "max_min_int64", &found);
    entry->name = pstrdup("max_min_int64");
    entry->semiring = GxB_MAX_MIN_INT64;

    entry = semiringhash_insert(semiringhash, "plus_min_int64", &found);
    entry->name = pstrdup("plus_min_int64");
    entry->semiring = GxB_PLUS_MIN_INT64;

    entry = semiringhash_insert(semiringhash, "times_min_int64", &found);
    entry->name = pstrdup("times_min_int64");
    entry->semiring = GxB_TIMES_MIN_INT64;

    entry = semiringhash_insert(semiringhash, "any_min_int64", &found);
    entry->name = pstrdup("any_min_int64");
    entry->semiring = GxB_ANY_MIN_INT64;

    entry = semiringhash_insert(semiringhash, "min_min_uint8", &found);
    entry->name = pstrdup("min_min_uint8");
    entry->semiring = GxB_MIN_MIN_UINT8;

    entry = semiringhash_insert(semiringhash, "max_min_uint8", &found);
    entry->name = pstrdup("max_min_uint8");
    entry->semiring = GxB_MAX_MIN_UINT8;

    entry = semiringhash_insert(semiringhash, "plus_min_uint8", &found);
    entry->name = pstrdup("plus_min_uint8");
    entry->semiring = GxB_PLUS_MIN_UINT8;

    entry = semiringhash_insert(semiringhash, "times_min_uint8", &found);
    entry->name = pstrdup("times_min_uint8");
    entry->semiring = GxB_TIMES_MIN_UINT8;

    entry = semiringhash_insert(semiringhash, "any_min_uint8", &found);
    entry->name = pstrdup("any_min_uint8");
    entry->semiring = GxB_ANY_MIN_UINT8;

    entry = semiringhash_insert(semiringhash, "min_min_uint16", &found);
    entry->name = pstrdup("min_min_uint16");
    entry->semiring = GxB_MIN_MIN_UINT16;

    entry = semiringhash_insert(semiringhash, "max_min_uint16", &found);
    entry->name = pstrdup("max_min_uint16");
    entry->semiring = GxB_MAX_MIN_UINT16;

    entry = semiringhash_insert(semiringhash, "plus_min_uint16", &found);
    entry->name = pstrdup("plus_min_uint16");
    entry->semiring = GxB_PLUS_MIN_UINT16;

    entry = semiringhash_insert(semiringhash, "times_min_uint16", &found);
    entry->name = pstrdup("times_min_uint16");
    entry->semiring = GxB_TIMES_MIN_UINT16;

    entry = semiringhash_insert(semiringhash, "any_min_uint16", &found);
    entry->name = pstrdup("any_min_uint16");
    entry->semiring = GxB_ANY_MIN_UINT16;

    entry = semiringhash_insert(semiringhash, "min_min_uint32", &found);
    entry->name = pstrdup("min_min_uint32");
    entry->semiring = GxB_MIN_MIN_UINT32;

    entry = semiringhash_insert(semiringhash, "max_min_uint32", &found);
    entry->name = pstrdup("max_min_uint32");
    entry->semiring = GxB_MAX_MIN_UINT32;

    entry = semiringhash_insert(semiringhash, "plus_min_uint32", &found);
    entry->name = pstrdup("plus_min_uint32");
    entry->semiring = GxB_PLUS_MIN_UINT32;

    entry = semiringhash_insert(semiringhash, "times_min_uint32", &found);
    entry->name = pstrdup("times_min_uint32");
    entry->semiring = GxB_TIMES_MIN_UINT32;

    entry = semiringhash_insert(semiringhash, "any_min_uint32", &found);
    entry->name = pstrdup("any_min_uint32");
    entry->semiring = GxB_ANY_MIN_UINT32;

    entry = semiringhash_insert(semiringhash, "min_min_uint64", &found);
    entry->name = pstrdup("min_min_uint64");
    entry->semiring = GxB_MIN_MIN_UINT64;

    entry = semiringhash_insert(semiringhash, "max_min_uint64", &found);
    entry->name = pstrdup("max_min_uint64");
    entry->semiring = GxB_MAX_MIN_UINT64;

    entry = semiringhash_insert(semiringhash, "plus_min_uint64", &found);
    entry->name = pstrdup("plus_min_uint64");
    entry->semiring = GxB_PLUS_MIN_UINT64;

    entry = semiringhash_insert(semiringhash, "times_min_uint64", &found);
    entry->name = pstrdup("times_min_uint64");
    entry->semiring = GxB_TIMES_MIN_UINT64;

    entry = semiringhash_insert(semiringhash, "any_min_uint64", &found);
    entry->name = pstrdup("any_min_uint64");
    entry->semiring = GxB_ANY_MIN_UINT64;

    entry = semiringhash_insert(semiringhash, "min_min_fp32", &found);
    entry->name = pstrdup("min_min_fp32");
    entry->semiring = GxB_MIN_MIN_FP32;

    entry = semiringhash_insert(semiringhash, "max_min_fp32", &found);
    entry->name = pstrdup("max_min_fp32");
    entry->semiring = GxB_MAX_MIN_FP32;

    entry = semiringhash_insert(semiringhash, "plus_min_fp32", &found);
    entry->name = pstrdup("plus_min_fp32");
    entry->semiring = GxB_PLUS_MIN_FP32;

    entry = semiringhash_insert(semiringhash, "times_min_fp32", &found);
    entry->name = pstrdup("times_min_fp32");
    entry->semiring = GxB_TIMES_MIN_FP32;

    entry = semiringhash_insert(semiringhash, "any_min_fp32", &found);
    entry->name = pstrdup("any_min_fp32");
    entry->semiring = GxB_ANY_MIN_FP32;

    entry = semiringhash_insert(semiringhash, "min_min_fp64", &found);
    entry->name = pstrdup("min_min_fp64");
    entry->semiring = GxB_MIN_MIN_FP64;

    entry = semiringhash_insert(semiringhash, "max_min_fp64", &found);
    entry->name = pstrdup("max_min_fp64");
    entry->semiring = GxB_MAX_MIN_FP64;

    entry = semiringhash_insert(semiringhash, "plus_min_fp64", &found);
    entry->name = pstrdup("plus_min_fp64");
    entry->semiring = GxB_PLUS_MIN_FP64;

    entry = semiringhash_insert(semiringhash, "times_min_fp64", &found);
    entry->name = pstrdup("times_min_fp64");
    entry->semiring = GxB_TIMES_MIN_FP64;

    entry = semiringhash_insert(semiringhash, "any_min_fp64", &found);
    entry->name = pstrdup("any_min_fp64");
    entry->semiring = GxB_ANY_MIN_FP64;

    entry = semiringhash_insert(semiringhash, "min_max_int8", &found);
    entry->name = pstrdup("min_max_int8");
    entry->semiring = GxB_MIN_MAX_INT8;

    entry = semiringhash_insert(semiringhash, "max_max_int8", &found);
    entry->name = pstrdup("max_max_int8");
    entry->semiring = GxB_MAX_MAX_INT8;

    entry = semiringhash_insert(semiringhash, "plus_max_int8", &found);
    entry->name = pstrdup("plus_max_int8");
    entry->semiring = GxB_PLUS_MAX_INT8;

    entry = semiringhash_insert(semiringhash, "times_max_int8", &found);
    entry->name = pstrdup("times_max_int8");
    entry->semiring = GxB_TIMES_MAX_INT8;

    entry = semiringhash_insert(semiringhash, "any_max_int8", &found);
    entry->name = pstrdup("any_max_int8");
    entry->semiring = GxB_ANY_MAX_INT8;

    entry = semiringhash_insert(semiringhash, "min_max_int16", &found);
    entry->name = pstrdup("min_max_int16");
    entry->semiring = GxB_MIN_MAX_INT16;

    entry = semiringhash_insert(semiringhash, "max_max_int16", &found);
    entry->name = pstrdup("max_max_int16");
    entry->semiring = GxB_MAX_MAX_INT16;

    entry = semiringhash_insert(semiringhash, "plus_max_int16", &found);
    entry->name = pstrdup("plus_max_int16");
    entry->semiring = GxB_PLUS_MAX_INT16;

    entry = semiringhash_insert(semiringhash, "times_max_int16", &found);
    entry->name = pstrdup("times_max_int16");
    entry->semiring = GxB_TIMES_MAX_INT16;

    entry = semiringhash_insert(semiringhash, "any_max_int16", &found);
    entry->name = pstrdup("any_max_int16");
    entry->semiring = GxB_ANY_MAX_INT16;

    entry = semiringhash_insert(semiringhash, "min_max_int32", &found);
    entry->name = pstrdup("min_max_int32");
    entry->semiring = GxB_MIN_MAX_INT32;

    entry = semiringhash_insert(semiringhash, "max_max_int32", &found);
    entry->name = pstrdup("max_max_int32");
    entry->semiring = GxB_MAX_MAX_INT32;

    entry = semiringhash_insert(semiringhash, "plus_max_int32", &found);
    entry->name = pstrdup("plus_max_int32");
    entry->semiring = GxB_PLUS_MAX_INT32;

    entry = semiringhash_insert(semiringhash, "times_max_int32", &found);
    entry->name = pstrdup("times_max_int32");
    entry->semiring = GxB_TIMES_MAX_INT32;

    entry = semiringhash_insert(semiringhash, "any_max_int32", &found);
    entry->name = pstrdup("any_max_int32");
    entry->semiring = GxB_ANY_MAX_INT32;

    entry = semiringhash_insert(semiringhash, "min_max_int64", &found);
    entry->name = pstrdup("min_max_int64");
    entry->semiring = GxB_MIN_MAX_INT64;

    entry = semiringhash_insert(semiringhash, "max_max_int64", &found);
    entry->name = pstrdup("max_max_int64");
    entry->semiring = GxB_MAX_MAX_INT64;

    entry = semiringhash_insert(semiringhash, "plus_max_int64", &found);
    entry->name = pstrdup("plus_max_int64");
    entry->semiring = GxB_PLUS_MAX_INT64;

    entry = semiringhash_insert(semiringhash, "times_max_int64", &found);
    entry->name = pstrdup("times_max_int64");
    entry->semiring = GxB_TIMES_MAX_INT64;

    entry = semiringhash_insert(semiringhash, "any_max_int64", &found);
    entry->name = pstrdup("any_max_int64");
    entry->semiring = GxB_ANY_MAX_INT64;

    entry = semiringhash_insert(semiringhash, "min_max_uint8", &found);
    entry->name = pstrdup("min_max_uint8");
    entry->semiring = GxB_MIN_MAX_UINT8;

    entry = semiringhash_insert(semiringhash, "max_max_uint8", &found);
    entry->name = pstrdup("max_max_uint8");
    entry->semiring = GxB_MAX_MAX_UINT8;

    entry = semiringhash_insert(semiringhash, "plus_max_uint8", &found);
    entry->name = pstrdup("plus_max_uint8");
    entry->semiring = GxB_PLUS_MAX_UINT8;

    entry = semiringhash_insert(semiringhash, "times_max_uint8", &found);
    entry->name = pstrdup("times_max_uint8");
    entry->semiring = GxB_TIMES_MAX_UINT8;

    entry = semiringhash_insert(semiringhash, "any_max_uint8", &found);
    entry->name = pstrdup("any_max_uint8");
    entry->semiring = GxB_ANY_MAX_UINT8;

    entry = semiringhash_insert(semiringhash, "min_max_uint16", &found);
    entry->name = pstrdup("min_max_uint16");
    entry->semiring = GxB_MIN_MAX_UINT16;

    entry = semiringhash_insert(semiringhash, "max_max_uint16", &found);
    entry->name = pstrdup("max_max_uint16");
    entry->semiring = GxB_MAX_MAX_UINT16;

    entry = semiringhash_insert(semiringhash, "plus_max_uint16", &found);
    entry->name = pstrdup("plus_max_uint16");
    entry->semiring = GxB_PLUS_MAX_UINT16;

    entry = semiringhash_insert(semiringhash, "times_max_uint16", &found);
    entry->name = pstrdup("times_max_uint16");
    entry->semiring = GxB_TIMES_MAX_UINT16;

    entry = semiringhash_insert(semiringhash, "any_max_uint16", &found);
    entry->name = pstrdup("any_max_uint16");
    entry->semiring = GxB_ANY_MAX_UINT16;

    entry = semiringhash_insert(semiringhash, "min_max_uint32", &found);
    entry->name = pstrdup("min_max_uint32");
    entry->semiring = GxB_MIN_MAX_UINT32;

    entry = semiringhash_insert(semiringhash, "max_max_uint32", &found);
    entry->name = pstrdup("max_max_uint32");
    entry->semiring = GxB_MAX_MAX_UINT32;

    entry = semiringhash_insert(semiringhash, "plus_max_uint32", &found);
    entry->name = pstrdup("plus_max_uint32");
    entry->semiring = GxB_PLUS_MAX_UINT32;

    entry = semiringhash_insert(semiringhash, "times_max_uint32", &found);
    entry->name = pstrdup("times_max_uint32");
    entry->semiring = GxB_TIMES_MAX_UINT32;

    entry = semiringhash_insert(semiringhash, "any_max_uint32", &found);
    entry->name = pstrdup("any_max_uint32");
    entry->semiring = GxB_ANY_MAX_UINT32;

    entry = semiringhash_insert(semiringhash, "min_max_uint64", &found);
    entry->name = pstrdup("min_max_uint64");
    entry->semiring = GxB_MIN_MAX_UINT64;

    entry = semiringhash_insert(semiringhash, "max_max_uint64", &found);
    entry->name = pstrdup("max_max_uint64");
    entry->semiring = GxB_MAX_MAX_UINT64;

    entry = semiringhash_insert(semiringhash, "plus_max_uint64", &found);
    entry->name = pstrdup("plus_max_uint64");
    entry->semiring = GxB_PLUS_MAX_UINT64;

    entry = semiringhash_insert(semiringhash, "times_max_uint64", &found);
    entry->name = pstrdup("times_max_uint64");
    entry->semiring = GxB_TIMES_MAX_UINT64;

    entry = semiringhash_insert(semiringhash, "any_max_uint64", &found);
    entry->name = pstrdup("any_max_uint64");
    entry->semiring = GxB_ANY_MAX_UINT64;

    entry = semiringhash_insert(semiringhash, "min_max_fp32", &found);
    entry->name = pstrdup("min_max_fp32");
    entry->semiring = GxB_MIN_MAX_FP32;

    entry = semiringhash_insert(semiringhash, "max_max_fp32", &found);
    entry->name = pstrdup("max_max_fp32");
    entry->semiring = GxB_MAX_MAX_FP32;

    entry = semiringhash_insert(semiringhash, "plus_max_fp32", &found);
    entry->name = pstrdup("plus_max_fp32");
    entry->semiring = GxB_PLUS_MAX_FP32;

    entry = semiringhash_insert(semiringhash, "times_max_fp32", &found);
    entry->name = pstrdup("times_max_fp32");
    entry->semiring = GxB_TIMES_MAX_FP32;

    entry = semiringhash_insert(semiringhash, "any_max_fp32", &found);
    entry->name = pstrdup("any_max_fp32");
    entry->semiring = GxB_ANY_MAX_FP32;

    entry = semiringhash_insert(semiringhash, "min_max_fp64", &found);
    entry->name = pstrdup("min_max_fp64");
    entry->semiring = GxB_MIN_MAX_FP64;

    entry = semiringhash_insert(semiringhash, "max_max_fp64", &found);
    entry->name = pstrdup("max_max_fp64");
    entry->semiring = GxB_MAX_MAX_FP64;

    entry = semiringhash_insert(semiringhash, "plus_max_fp64", &found);
    entry->name = pstrdup("plus_max_fp64");
    entry->semiring = GxB_PLUS_MAX_FP64;

    entry = semiringhash_insert(semiringhash, "times_max_fp64", &found);
    entry->name = pstrdup("times_max_fp64");
    entry->semiring = GxB_TIMES_MAX_FP64;

    entry = semiringhash_insert(semiringhash, "any_max_fp64", &found);
    entry->name = pstrdup("any_max_fp64");
    entry->semiring = GxB_ANY_MAX_FP64;

    entry = semiringhash_insert(semiringhash, "min_plus_int8", &found);
    entry->name = pstrdup("min_plus_int8");
    entry->semiring = GxB_MIN_PLUS_INT8;

    entry = semiringhash_insert(semiringhash, "max_plus_int8", &found);
    entry->name = pstrdup("max_plus_int8");
    entry->semiring = GxB_MAX_PLUS_INT8;

    entry = semiringhash_insert(semiringhash, "plus_plus_int8", &found);
    entry->name = pstrdup("plus_plus_int8");
    entry->semiring = GxB_PLUS_PLUS_INT8;

    entry = semiringhash_insert(semiringhash, "times_plus_int8", &found);
    entry->name = pstrdup("times_plus_int8");
    entry->semiring = GxB_TIMES_PLUS_INT8;

    entry = semiringhash_insert(semiringhash, "any_plus_int8", &found);
    entry->name = pstrdup("any_plus_int8");
    entry->semiring = GxB_ANY_PLUS_INT8;

    entry = semiringhash_insert(semiringhash, "min_plus_int16", &found);
    entry->name = pstrdup("min_plus_int16");
    entry->semiring = GxB_MIN_PLUS_INT16;

    entry = semiringhash_insert(semiringhash, "max_plus_int16", &found);
    entry->name = pstrdup("max_plus_int16");
    entry->semiring = GxB_MAX_PLUS_INT16;

    entry = semiringhash_insert(semiringhash, "plus_plus_int16", &found);
    entry->name = pstrdup("plus_plus_int16");
    entry->semiring = GxB_PLUS_PLUS_INT16;

    entry = semiringhash_insert(semiringhash, "times_plus_int16", &found);
    entry->name = pstrdup("times_plus_int16");
    entry->semiring = GxB_TIMES_PLUS_INT16;

    entry = semiringhash_insert(semiringhash, "any_plus_int16", &found);
    entry->name = pstrdup("any_plus_int16");
    entry->semiring = GxB_ANY_PLUS_INT16;

    entry = semiringhash_insert(semiringhash, "min_plus_int32", &found);
    entry->name = pstrdup("min_plus_int32");
    entry->semiring = GxB_MIN_PLUS_INT32;

    entry = semiringhash_insert(semiringhash, "max_plus_int32", &found);
    entry->name = pstrdup("max_plus_int32");
    entry->semiring = GxB_MAX_PLUS_INT32;

    entry = semiringhash_insert(semiringhash, "plus_plus_int32", &found);
    entry->name = pstrdup("plus_plus_int32");
    entry->semiring = GxB_PLUS_PLUS_INT32;

    entry = semiringhash_insert(semiringhash, "times_plus_int32", &found);
    entry->name = pstrdup("times_plus_int32");
    entry->semiring = GxB_TIMES_PLUS_INT32;

    entry = semiringhash_insert(semiringhash, "any_plus_int32", &found);
    entry->name = pstrdup("any_plus_int32");
    entry->semiring = GxB_ANY_PLUS_INT32;

    entry = semiringhash_insert(semiringhash, "min_plus_int64", &found);
    entry->name = pstrdup("min_plus_int64");
    entry->semiring = GxB_MIN_PLUS_INT64;

    entry = semiringhash_insert(semiringhash, "max_plus_int64", &found);
    entry->name = pstrdup("max_plus_int64");
    entry->semiring = GxB_MAX_PLUS_INT64;

    entry = semiringhash_insert(semiringhash, "plus_plus_int64", &found);
    entry->name = pstrdup("plus_plus_int64");
    entry->semiring = GxB_PLUS_PLUS_INT64;

    entry = semiringhash_insert(semiringhash, "times_plus_int64", &found);
    entry->name = pstrdup("times_plus_int64");
    entry->semiring = GxB_TIMES_PLUS_INT64;

    entry = semiringhash_insert(semiringhash, "any_plus_int64", &found);
    entry->name = pstrdup("any_plus_int64");
    entry->semiring = GxB_ANY_PLUS_INT64;

    entry = semiringhash_insert(semiringhash, "min_plus_uint8", &found);
    entry->name = pstrdup("min_plus_uint8");
    entry->semiring = GxB_MIN_PLUS_UINT8;

    entry = semiringhash_insert(semiringhash, "max_plus_uint8", &found);
    entry->name = pstrdup("max_plus_uint8");
    entry->semiring = GxB_MAX_PLUS_UINT8;

    entry = semiringhash_insert(semiringhash, "plus_plus_uint8", &found);
    entry->name = pstrdup("plus_plus_uint8");
    entry->semiring = GxB_PLUS_PLUS_UINT8;

    entry = semiringhash_insert(semiringhash, "times_plus_uint8", &found);
    entry->name = pstrdup("times_plus_uint8");
    entry->semiring = GxB_TIMES_PLUS_UINT8;

    entry = semiringhash_insert(semiringhash, "any_plus_uint8", &found);
    entry->name = pstrdup("any_plus_uint8");
    entry->semiring = GxB_ANY_PLUS_UINT8;

    entry = semiringhash_insert(semiringhash, "min_plus_uint16", &found);
    entry->name = pstrdup("min_plus_uint16");
    entry->semiring = GxB_MIN_PLUS_UINT16;

    entry = semiringhash_insert(semiringhash, "max_plus_uint16", &found);
    entry->name = pstrdup("max_plus_uint16");
    entry->semiring = GxB_MAX_PLUS_UINT16;

    entry = semiringhash_insert(semiringhash, "plus_plus_uint16", &found);
    entry->name = pstrdup("plus_plus_uint16");
    entry->semiring = GxB_PLUS_PLUS_UINT16;

    entry = semiringhash_insert(semiringhash, "times_plus_uint16", &found);
    entry->name = pstrdup("times_plus_uint16");
    entry->semiring = GxB_TIMES_PLUS_UINT16;

    entry = semiringhash_insert(semiringhash, "any_plus_uint16", &found);
    entry->name = pstrdup("any_plus_uint16");
    entry->semiring = GxB_ANY_PLUS_UINT16;

    entry = semiringhash_insert(semiringhash, "min_plus_uint32", &found);
    entry->name = pstrdup("min_plus_uint32");
    entry->semiring = GxB_MIN_PLUS_UINT32;

    entry = semiringhash_insert(semiringhash, "max_plus_uint32", &found);
    entry->name = pstrdup("max_plus_uint32");
    entry->semiring = GxB_MAX_PLUS_UINT32;

    entry = semiringhash_insert(semiringhash, "plus_plus_uint32", &found);
    entry->name = pstrdup("plus_plus_uint32");
    entry->semiring = GxB_PLUS_PLUS_UINT32;

    entry = semiringhash_insert(semiringhash, "times_plus_uint32", &found);
    entry->name = pstrdup("times_plus_uint32");
    entry->semiring = GxB_TIMES_PLUS_UINT32;

    entry = semiringhash_insert(semiringhash, "any_plus_uint32", &found);
    entry->name = pstrdup("any_plus_uint32");
    entry->semiring = GxB_ANY_PLUS_UINT32;

    entry = semiringhash_insert(semiringhash, "min_plus_uint64", &found);
    entry->name = pstrdup("min_plus_uint64");
    entry->semiring = GxB_MIN_PLUS_UINT64;

    entry = semiringhash_insert(semiringhash, "max_plus_uint64", &found);
    entry->name = pstrdup("max_plus_uint64");
    entry->semiring = GxB_MAX_PLUS_UINT64;

    entry = semiringhash_insert(semiringhash, "plus_plus_uint64", &found);
    entry->name = pstrdup("plus_plus_uint64");
    entry->semiring = GxB_PLUS_PLUS_UINT64;

    entry = semiringhash_insert(semiringhash, "times_plus_uint64", &found);
    entry->name = pstrdup("times_plus_uint64");
    entry->semiring = GxB_TIMES_PLUS_UINT64;

    entry = semiringhash_insert(semiringhash, "any_plus_uint64", &found);
    entry->name = pstrdup("any_plus_uint64");
    entry->semiring = GxB_ANY_PLUS_UINT64;

    entry = semiringhash_insert(semiringhash, "min_plus_fp32", &found);
    entry->name = pstrdup("min_plus_fp32");
    entry->semiring = GxB_MIN_PLUS_FP32;

    entry = semiringhash_insert(semiringhash, "max_plus_fp32", &found);
    entry->name = pstrdup("max_plus_fp32");
    entry->semiring = GxB_MAX_PLUS_FP32;

    entry = semiringhash_insert(semiringhash, "plus_plus_fp32", &found);
    entry->name = pstrdup("plus_plus_fp32");
    entry->semiring = GxB_PLUS_PLUS_FP32;

    entry = semiringhash_insert(semiringhash, "times_plus_fp32", &found);
    entry->name = pstrdup("times_plus_fp32");
    entry->semiring = GxB_TIMES_PLUS_FP32;

    entry = semiringhash_insert(semiringhash, "any_plus_fp32", &found);
    entry->name = pstrdup("any_plus_fp32");
    entry->semiring = GxB_ANY_PLUS_FP32;

    entry = semiringhash_insert(semiringhash, "min_plus_fp64", &found);
    entry->name = pstrdup("min_plus_fp64");
    entry->semiring = GxB_MIN_PLUS_FP64;

    entry = semiringhash_insert(semiringhash, "max_plus_fp64", &found);
    entry->name = pstrdup("max_plus_fp64");
    entry->semiring = GxB_MAX_PLUS_FP64;

    entry = semiringhash_insert(semiringhash, "plus_plus_fp64", &found);
    entry->name = pstrdup("plus_plus_fp64");
    entry->semiring = GxB_PLUS_PLUS_FP64;

    entry = semiringhash_insert(semiringhash, "times_plus_fp64", &found);
    entry->name = pstrdup("times_plus_fp64");
    entry->semiring = GxB_TIMES_PLUS_FP64;

    entry = semiringhash_insert(semiringhash, "any_plus_fp64", &found);
    entry->name = pstrdup("any_plus_fp64");
    entry->semiring = GxB_ANY_PLUS_FP64;

    entry = semiringhash_insert(semiringhash, "min_minus_int8", &found);
    entry->name = pstrdup("min_minus_int8");
    entry->semiring = GxB_MIN_MINUS_INT8;

    entry = semiringhash_insert(semiringhash, "max_minus_int8", &found);
    entry->name = pstrdup("max_minus_int8");
    entry->semiring = GxB_MAX_MINUS_INT8;

    entry = semiringhash_insert(semiringhash, "plus_minus_int8", &found);
    entry->name = pstrdup("plus_minus_int8");
    entry->semiring = GxB_PLUS_MINUS_INT8;

    entry = semiringhash_insert(semiringhash, "times_minus_int8", &found);
    entry->name = pstrdup("times_minus_int8");
    entry->semiring = GxB_TIMES_MINUS_INT8;

    entry = semiringhash_insert(semiringhash, "any_minus_int8", &found);
    entry->name = pstrdup("any_minus_int8");
    entry->semiring = GxB_ANY_MINUS_INT8;

    entry = semiringhash_insert(semiringhash, "min_minus_int16", &found);
    entry->name = pstrdup("min_minus_int16");
    entry->semiring = GxB_MIN_MINUS_INT16;

    entry = semiringhash_insert(semiringhash, "max_minus_int16", &found);
    entry->name = pstrdup("max_minus_int16");
    entry->semiring = GxB_MAX_MINUS_INT16;

    entry = semiringhash_insert(semiringhash, "plus_minus_int16", &found);
    entry->name = pstrdup("plus_minus_int16");
    entry->semiring = GxB_PLUS_MINUS_INT16;

    entry = semiringhash_insert(semiringhash, "times_minus_int16", &found);
    entry->name = pstrdup("times_minus_int16");
    entry->semiring = GxB_TIMES_MINUS_INT16;

    entry = semiringhash_insert(semiringhash, "any_minus_int16", &found);
    entry->name = pstrdup("any_minus_int16");
    entry->semiring = GxB_ANY_MINUS_INT16;

    entry = semiringhash_insert(semiringhash, "min_minus_int32", &found);
    entry->name = pstrdup("min_minus_int32");
    entry->semiring = GxB_MIN_MINUS_INT32;

    entry = semiringhash_insert(semiringhash, "max_minus_int32", &found);
    entry->name = pstrdup("max_minus_int32");
    entry->semiring = GxB_MAX_MINUS_INT32;

    entry = semiringhash_insert(semiringhash, "plus_minus_int32", &found);
    entry->name = pstrdup("plus_minus_int32");
    entry->semiring = GxB_PLUS_MINUS_INT32;

    entry = semiringhash_insert(semiringhash, "times_minus_int32", &found);
    entry->name = pstrdup("times_minus_int32");
    entry->semiring = GxB_TIMES_MINUS_INT32;

    entry = semiringhash_insert(semiringhash, "any_minus_int32", &found);
    entry->name = pstrdup("any_minus_int32");
    entry->semiring = GxB_ANY_MINUS_INT32;

    entry = semiringhash_insert(semiringhash, "min_minus_int64", &found);
    entry->name = pstrdup("min_minus_int64");
    entry->semiring = GxB_MIN_MINUS_INT64;

    entry = semiringhash_insert(semiringhash, "max_minus_int64", &found);
    entry->name = pstrdup("max_minus_int64");
    entry->semiring = GxB_MAX_MINUS_INT64;

    entry = semiringhash_insert(semiringhash, "plus_minus_int64", &found);
    entry->name = pstrdup("plus_minus_int64");
    entry->semiring = GxB_PLUS_MINUS_INT64;

    entry = semiringhash_insert(semiringhash, "times_minus_int64", &found);
    entry->name = pstrdup("times_minus_int64");
    entry->semiring = GxB_TIMES_MINUS_INT64;

    entry = semiringhash_insert(semiringhash, "any_minus_int64", &found);
    entry->name = pstrdup("any_minus_int64");
    entry->semiring = GxB_ANY_MINUS_INT64;

    entry = semiringhash_insert(semiringhash, "min_minus_uint8", &found);
    entry->name = pstrdup("min_minus_uint8");
    entry->semiring = GxB_MIN_MINUS_UINT8;

    entry = semiringhash_insert(semiringhash, "max_minus_uint8", &found);
    entry->name = pstrdup("max_minus_uint8");
    entry->semiring = GxB_MAX_MINUS_UINT8;

    entry = semiringhash_insert(semiringhash, "plus_minus_uint8", &found);
    entry->name = pstrdup("plus_minus_uint8");
    entry->semiring = GxB_PLUS_MINUS_UINT8;

    entry = semiringhash_insert(semiringhash, "times_minus_uint8", &found);
    entry->name = pstrdup("times_minus_uint8");
    entry->semiring = GxB_TIMES_MINUS_UINT8;

    entry = semiringhash_insert(semiringhash, "any_minus_uint8", &found);
    entry->name = pstrdup("any_minus_uint8");
    entry->semiring = GxB_ANY_MINUS_UINT8;

    entry = semiringhash_insert(semiringhash, "min_minus_uint16", &found);
    entry->name = pstrdup("min_minus_uint16");
    entry->semiring = GxB_MIN_MINUS_UINT16;

    entry = semiringhash_insert(semiringhash, "max_minus_uint16", &found);
    entry->name = pstrdup("max_minus_uint16");
    entry->semiring = GxB_MAX_MINUS_UINT16;

    entry = semiringhash_insert(semiringhash, "plus_minus_uint16", &found);
    entry->name = pstrdup("plus_minus_uint16");
    entry->semiring = GxB_PLUS_MINUS_UINT16;

    entry = semiringhash_insert(semiringhash, "times_minus_uint16", &found);
    entry->name = pstrdup("times_minus_uint16");
    entry->semiring = GxB_TIMES_MINUS_UINT16;

    entry = semiringhash_insert(semiringhash, "any_minus_uint16", &found);
    entry->name = pstrdup("any_minus_uint16");
    entry->semiring = GxB_ANY_MINUS_UINT16;

    entry = semiringhash_insert(semiringhash, "min_minus_uint32", &found);
    entry->name = pstrdup("min_minus_uint32");
    entry->semiring = GxB_MIN_MINUS_UINT32;

    entry = semiringhash_insert(semiringhash, "max_minus_uint32", &found);
    entry->name = pstrdup("max_minus_uint32");
    entry->semiring = GxB_MAX_MINUS_UINT32;

    entry = semiringhash_insert(semiringhash, "plus_minus_uint32", &found);
    entry->name = pstrdup("plus_minus_uint32");
    entry->semiring = GxB_PLUS_MINUS_UINT32;

    entry = semiringhash_insert(semiringhash, "times_minus_uint32", &found);
    entry->name = pstrdup("times_minus_uint32");
    entry->semiring = GxB_TIMES_MINUS_UINT32;

    entry = semiringhash_insert(semiringhash, "any_minus_uint32", &found);
    entry->name = pstrdup("any_minus_uint32");
    entry->semiring = GxB_ANY_MINUS_UINT32;

    entry = semiringhash_insert(semiringhash, "min_minus_uint64", &found);
    entry->name = pstrdup("min_minus_uint64");
    entry->semiring = GxB_MIN_MINUS_UINT64;

    entry = semiringhash_insert(semiringhash, "max_minus_uint64", &found);
    entry->name = pstrdup("max_minus_uint64");
    entry->semiring = GxB_MAX_MINUS_UINT64;

    entry = semiringhash_insert(semiringhash, "plus_minus_uint64", &found);
    entry->name = pstrdup("plus_minus_uint64");
    entry->semiring = GxB_PLUS_MINUS_UINT64;

    entry = semiringhash_insert(semiringhash, "times_minus_uint64", &found);
    entry->name = pstrdup("times_minus_uint64");
    entry->semiring = GxB_TIMES_MINUS_UINT64;

    entry = semiringhash_insert(semiringhash, "any_minus_uint64", &found);
    entry->name = pstrdup("any_minus_uint64");
    entry->semiring = GxB_ANY_MINUS_UINT64;

    entry = semiringhash_insert(semiringhash, "min_minus_fp32", &found);
    entry->name = pstrdup("min_minus_fp32");
    entry->semiring = GxB_MIN_MINUS_FP32;

    entry = semiringhash_insert(semiringhash, "max_minus_fp32", &found);
    entry->name = pstrdup("max_minus_fp32");
    entry->semiring = GxB_MAX_MINUS_FP32;

    entry = semiringhash_insert(semiringhash, "plus_minus_fp32", &found);
    entry->name = pstrdup("plus_minus_fp32");
    entry->semiring = GxB_PLUS_MINUS_FP32;

    entry = semiringhash_insert(semiringhash, "times_minus_fp32", &found);
    entry->name = pstrdup("times_minus_fp32");
    entry->semiring = GxB_TIMES_MINUS_FP32;

    entry = semiringhash_insert(semiringhash, "any_minus_fp32", &found);
    entry->name = pstrdup("any_minus_fp32");
    entry->semiring = GxB_ANY_MINUS_FP32;

    entry = semiringhash_insert(semiringhash, "min_minus_fp64", &found);
    entry->name = pstrdup("min_minus_fp64");
    entry->semiring = GxB_MIN_MINUS_FP64;

    entry = semiringhash_insert(semiringhash, "max_minus_fp64", &found);
    entry->name = pstrdup("max_minus_fp64");
    entry->semiring = GxB_MAX_MINUS_FP64;

    entry = semiringhash_insert(semiringhash, "plus_minus_fp64", &found);
    entry->name = pstrdup("plus_minus_fp64");
    entry->semiring = GxB_PLUS_MINUS_FP64;

    entry = semiringhash_insert(semiringhash, "times_minus_fp64", &found);
    entry->name = pstrdup("times_minus_fp64");
    entry->semiring = GxB_TIMES_MINUS_FP64;

    entry = semiringhash_insert(semiringhash, "any_minus_fp64", &found);
    entry->name = pstrdup("any_minus_fp64");
    entry->semiring = GxB_ANY_MINUS_FP64;

    entry = semiringhash_insert(semiringhash, "min_times_int8", &found);
    entry->name = pstrdup("min_times_int8");
    entry->semiring = GxB_MIN_TIMES_INT8;

    entry = semiringhash_insert(semiringhash, "max_times_int8", &found);
    entry->name = pstrdup("max_times_int8");
    entry->semiring = GxB_MAX_TIMES_INT8;

    entry = semiringhash_insert(semiringhash, "plus_times_int8", &found);
    entry->name = pstrdup("plus_times_int8");
    entry->semiring = GxB_PLUS_TIMES_INT8;

    entry = semiringhash_insert(semiringhash, "times_times_int8", &found);
    entry->name = pstrdup("times_times_int8");
    entry->semiring = GxB_TIMES_TIMES_INT8;

    entry = semiringhash_insert(semiringhash, "any_times_int8", &found);
    entry->name = pstrdup("any_times_int8");
    entry->semiring = GxB_ANY_TIMES_INT8;

    entry = semiringhash_insert(semiringhash, "min_times_int16", &found);
    entry->name = pstrdup("min_times_int16");
    entry->semiring = GxB_MIN_TIMES_INT16;

    entry = semiringhash_insert(semiringhash, "max_times_int16", &found);
    entry->name = pstrdup("max_times_int16");
    entry->semiring = GxB_MAX_TIMES_INT16;

    entry = semiringhash_insert(semiringhash, "plus_times_int16", &found);
    entry->name = pstrdup("plus_times_int16");
    entry->semiring = GxB_PLUS_TIMES_INT16;

    entry = semiringhash_insert(semiringhash, "times_times_int16", &found);
    entry->name = pstrdup("times_times_int16");
    entry->semiring = GxB_TIMES_TIMES_INT16;

    entry = semiringhash_insert(semiringhash, "any_times_int16", &found);
    entry->name = pstrdup("any_times_int16");
    entry->semiring = GxB_ANY_TIMES_INT16;

    entry = semiringhash_insert(semiringhash, "min_times_int32", &found);
    entry->name = pstrdup("min_times_int32");
    entry->semiring = GxB_MIN_TIMES_INT32;

    entry = semiringhash_insert(semiringhash, "max_times_int32", &found);
    entry->name = pstrdup("max_times_int32");
    entry->semiring = GxB_MAX_TIMES_INT32;

    entry = semiringhash_insert(semiringhash, "plus_times_int32", &found);
    entry->name = pstrdup("plus_times_int32");
    entry->semiring = GxB_PLUS_TIMES_INT32;

    entry = semiringhash_insert(semiringhash, "times_times_int32", &found);
    entry->name = pstrdup("times_times_int32");
    entry->semiring = GxB_TIMES_TIMES_INT32;

    entry = semiringhash_insert(semiringhash, "any_times_int32", &found);
    entry->name = pstrdup("any_times_int32");
    entry->semiring = GxB_ANY_TIMES_INT32;

    entry = semiringhash_insert(semiringhash, "min_times_int64", &found);
    entry->name = pstrdup("min_times_int64");
    entry->semiring = GxB_MIN_TIMES_INT64;

    entry = semiringhash_insert(semiringhash, "max_times_int64", &found);
    entry->name = pstrdup("max_times_int64");
    entry->semiring = GxB_MAX_TIMES_INT64;

    entry = semiringhash_insert(semiringhash, "plus_times_int64", &found);
    entry->name = pstrdup("plus_times_int64");
    entry->semiring = GxB_PLUS_TIMES_INT64;

    entry = semiringhash_insert(semiringhash, "times_times_int64", &found);
    entry->name = pstrdup("times_times_int64");
    entry->semiring = GxB_TIMES_TIMES_INT64;

    entry = semiringhash_insert(semiringhash, "any_times_int64", &found);
    entry->name = pstrdup("any_times_int64");
    entry->semiring = GxB_ANY_TIMES_INT64;

    entry = semiringhash_insert(semiringhash, "min_times_uint8", &found);
    entry->name = pstrdup("min_times_uint8");
    entry->semiring = GxB_MIN_TIMES_UINT8;

    entry = semiringhash_insert(semiringhash, "max_times_uint8", &found);
    entry->name = pstrdup("max_times_uint8");
    entry->semiring = GxB_MAX_TIMES_UINT8;

    entry = semiringhash_insert(semiringhash, "plus_times_uint8", &found);
    entry->name = pstrdup("plus_times_uint8");
    entry->semiring = GxB_PLUS_TIMES_UINT8;

    entry = semiringhash_insert(semiringhash, "times_times_uint8", &found);
    entry->name = pstrdup("times_times_uint8");
    entry->semiring = GxB_TIMES_TIMES_UINT8;

    entry = semiringhash_insert(semiringhash, "any_times_uint8", &found);
    entry->name = pstrdup("any_times_uint8");
    entry->semiring = GxB_ANY_TIMES_UINT8;

    entry = semiringhash_insert(semiringhash, "min_times_uint16", &found);
    entry->name = pstrdup("min_times_uint16");
    entry->semiring = GxB_MIN_TIMES_UINT16;

    entry = semiringhash_insert(semiringhash, "max_times_uint16", &found);
    entry->name = pstrdup("max_times_uint16");
    entry->semiring = GxB_MAX_TIMES_UINT16;

    entry = semiringhash_insert(semiringhash, "plus_times_uint16", &found);
    entry->name = pstrdup("plus_times_uint16");
    entry->semiring = GxB_PLUS_TIMES_UINT16;

    entry = semiringhash_insert(semiringhash, "times_times_uint16", &found);
    entry->name = pstrdup("times_times_uint16");
    entry->semiring = GxB_TIMES_TIMES_UINT16;

    entry = semiringhash_insert(semiringhash, "any_times_uint16", &found);
    entry->name = pstrdup("any_times_uint16");
    entry->semiring = GxB_ANY_TIMES_UINT16;

    entry = semiringhash_insert(semiringhash, "min_times_uint32", &found);
    entry->name = pstrdup("min_times_uint32");
    entry->semiring = GxB_MIN_TIMES_UINT32;

    entry = semiringhash_insert(semiringhash, "max_times_uint32", &found);
    entry->name = pstrdup("max_times_uint32");
    entry->semiring = GxB_MAX_TIMES_UINT32;

    entry = semiringhash_insert(semiringhash, "plus_times_uint32", &found);
    entry->name = pstrdup("plus_times_uint32");
    entry->semiring = GxB_PLUS_TIMES_UINT32;

    entry = semiringhash_insert(semiringhash, "times_times_uint32", &found);
    entry->name = pstrdup("times_times_uint32");
    entry->semiring = GxB_TIMES_TIMES_UINT32;

    entry = semiringhash_insert(semiringhash, "any_times_uint32", &found);
    entry->name = pstrdup("any_times_uint32");
    entry->semiring = GxB_ANY_TIMES_UINT32;

    entry = semiringhash_insert(semiringhash, "min_times_uint64", &found);
    entry->name = pstrdup("min_times_uint64");
    entry->semiring = GxB_MIN_TIMES_UINT64;

    entry = semiringhash_insert(semiringhash, "max_times_uint64", &found);
    entry->name = pstrdup("max_times_uint64");
    entry->semiring = GxB_MAX_TIMES_UINT64;

    entry = semiringhash_insert(semiringhash, "plus_times_uint64", &found);
    entry->name = pstrdup("plus_times_uint64");
    entry->semiring = GxB_PLUS_TIMES_UINT64;

    entry = semiringhash_insert(semiringhash, "times_times_uint64", &found);
    entry->name = pstrdup("times_times_uint64");
    entry->semiring = GxB_TIMES_TIMES_UINT64;

    entry = semiringhash_insert(semiringhash, "any_times_uint64", &found);
    entry->name = pstrdup("any_times_uint64");
    entry->semiring = GxB_ANY_TIMES_UINT64;

    entry = semiringhash_insert(semiringhash, "min_times_fp32", &found);
    entry->name = pstrdup("min_times_fp32");
    entry->semiring = GxB_MIN_TIMES_FP32;

    entry = semiringhash_insert(semiringhash, "max_times_fp32", &found);
    entry->name = pstrdup("max_times_fp32");
    entry->semiring = GxB_MAX_TIMES_FP32;

    entry = semiringhash_insert(semiringhash, "plus_times_fp32", &found);
    entry->name = pstrdup("plus_times_fp32");
    entry->semiring = GxB_PLUS_TIMES_FP32;

    entry = semiringhash_insert(semiringhash, "times_times_fp32", &found);
    entry->name = pstrdup("times_times_fp32");
    entry->semiring = GxB_TIMES_TIMES_FP32;

    entry = semiringhash_insert(semiringhash, "any_times_fp32", &found);
    entry->name = pstrdup("any_times_fp32");
    entry->semiring = GxB_ANY_TIMES_FP32;

    entry = semiringhash_insert(semiringhash, "min_times_fp64", &found);
    entry->name = pstrdup("min_times_fp64");
    entry->semiring = GxB_MIN_TIMES_FP64;

    entry = semiringhash_insert(semiringhash, "max_times_fp64", &found);
    entry->name = pstrdup("max_times_fp64");
    entry->semiring = GxB_MAX_TIMES_FP64;

    entry = semiringhash_insert(semiringhash, "plus_times_fp64", &found);
    entry->name = pstrdup("plus_times_fp64");
    entry->semiring = GxB_PLUS_TIMES_FP64;

    entry = semiringhash_insert(semiringhash, "times_times_fp64", &found);
    entry->name = pstrdup("times_times_fp64");
    entry->semiring = GxB_TIMES_TIMES_FP64;

    entry = semiringhash_insert(semiringhash, "any_times_fp64", &found);
    entry->name = pstrdup("any_times_fp64");
    entry->semiring = GxB_ANY_TIMES_FP64;

    entry = semiringhash_insert(semiringhash, "min_div_int8", &found);
    entry->name = pstrdup("min_div_int8");
    entry->semiring = GxB_MIN_DIV_INT8;

    entry = semiringhash_insert(semiringhash, "max_div_int8", &found);
    entry->name = pstrdup("max_div_int8");
    entry->semiring = GxB_MAX_DIV_INT8;

    entry = semiringhash_insert(semiringhash, "plus_div_int8", &found);
    entry->name = pstrdup("plus_div_int8");
    entry->semiring = GxB_PLUS_DIV_INT8;

    entry = semiringhash_insert(semiringhash, "times_div_int8", &found);
    entry->name = pstrdup("times_div_int8");
    entry->semiring = GxB_TIMES_DIV_INT8;

    entry = semiringhash_insert(semiringhash, "any_div_int8", &found);
    entry->name = pstrdup("any_div_int8");
    entry->semiring = GxB_ANY_DIV_INT8;

    entry = semiringhash_insert(semiringhash, "min_div_int16", &found);
    entry->name = pstrdup("min_div_int16");
    entry->semiring = GxB_MIN_DIV_INT16;

    entry = semiringhash_insert(semiringhash, "max_div_int16", &found);
    entry->name = pstrdup("max_div_int16");
    entry->semiring = GxB_MAX_DIV_INT16;

    entry = semiringhash_insert(semiringhash, "plus_div_int16", &found);
    entry->name = pstrdup("plus_div_int16");
    entry->semiring = GxB_PLUS_DIV_INT16;

    entry = semiringhash_insert(semiringhash, "times_div_int16", &found);
    entry->name = pstrdup("times_div_int16");
    entry->semiring = GxB_TIMES_DIV_INT16;

    entry = semiringhash_insert(semiringhash, "any_div_int16", &found);
    entry->name = pstrdup("any_div_int16");
    entry->semiring = GxB_ANY_DIV_INT16;

    entry = semiringhash_insert(semiringhash, "min_div_int32", &found);
    entry->name = pstrdup("min_div_int32");
    entry->semiring = GxB_MIN_DIV_INT32;

    entry = semiringhash_insert(semiringhash, "max_div_int32", &found);
    entry->name = pstrdup("max_div_int32");
    entry->semiring = GxB_MAX_DIV_INT32;

    entry = semiringhash_insert(semiringhash, "plus_div_int32", &found);
    entry->name = pstrdup("plus_div_int32");
    entry->semiring = GxB_PLUS_DIV_INT32;

    entry = semiringhash_insert(semiringhash, "times_div_int32", &found);
    entry->name = pstrdup("times_div_int32");
    entry->semiring = GxB_TIMES_DIV_INT32;

    entry = semiringhash_insert(semiringhash, "any_div_int32", &found);
    entry->name = pstrdup("any_div_int32");
    entry->semiring = GxB_ANY_DIV_INT32;

    entry = semiringhash_insert(semiringhash, "min_div_int64", &found);
    entry->name = pstrdup("min_div_int64");
    entry->semiring = GxB_MIN_DIV_INT64;

    entry = semiringhash_insert(semiringhash, "max_div_int64", &found);
    entry->name = pstrdup("max_div_int64");
    entry->semiring = GxB_MAX_DIV_INT64;

    entry = semiringhash_insert(semiringhash, "plus_div_int64", &found);
    entry->name = pstrdup("plus_div_int64");
    entry->semiring = GxB_PLUS_DIV_INT64;

    entry = semiringhash_insert(semiringhash, "times_div_int64", &found);
    entry->name = pstrdup("times_div_int64");
    entry->semiring = GxB_TIMES_DIV_INT64;

    entry = semiringhash_insert(semiringhash, "any_div_int64", &found);
    entry->name = pstrdup("any_div_int64");
    entry->semiring = GxB_ANY_DIV_INT64;

    entry = semiringhash_insert(semiringhash, "min_div_uint8", &found);
    entry->name = pstrdup("min_div_uint8");
    entry->semiring = GxB_MIN_DIV_UINT8;

    entry = semiringhash_insert(semiringhash, "max_div_uint8", &found);
    entry->name = pstrdup("max_div_uint8");
    entry->semiring = GxB_MAX_DIV_UINT8;

    entry = semiringhash_insert(semiringhash, "plus_div_uint8", &found);
    entry->name = pstrdup("plus_div_uint8");
    entry->semiring = GxB_PLUS_DIV_UINT8;

    entry = semiringhash_insert(semiringhash, "times_div_uint8", &found);
    entry->name = pstrdup("times_div_uint8");
    entry->semiring = GxB_TIMES_DIV_UINT8;

    entry = semiringhash_insert(semiringhash, "any_div_uint8", &found);
    entry->name = pstrdup("any_div_uint8");
    entry->semiring = GxB_ANY_DIV_UINT8;

    entry = semiringhash_insert(semiringhash, "min_div_uint16", &found);
    entry->name = pstrdup("min_div_uint16");
    entry->semiring = GxB_MIN_DIV_UINT16;

    entry = semiringhash_insert(semiringhash, "max_div_uint16", &found);
    entry->name = pstrdup("max_div_uint16");
    entry->semiring = GxB_MAX_DIV_UINT16;

    entry = semiringhash_insert(semiringhash, "plus_div_uint16", &found);
    entry->name = pstrdup("plus_div_uint16");
    entry->semiring = GxB_PLUS_DIV_UINT16;

    entry = semiringhash_insert(semiringhash, "times_div_uint16", &found);
    entry->name = pstrdup("times_div_uint16");
    entry->semiring = GxB_TIMES_DIV_UINT16;

    entry = semiringhash_insert(semiringhash, "any_div_uint16", &found);
    entry->name = pstrdup("any_div_uint16");
    entry->semiring = GxB_ANY_DIV_UINT16;

    entry = semiringhash_insert(semiringhash, "min_div_uint32", &found);
    entry->name = pstrdup("min_div_uint32");
    entry->semiring = GxB_MIN_DIV_UINT32;

    entry = semiringhash_insert(semiringhash, "max_div_uint32", &found);
    entry->name = pstrdup("max_div_uint32");
    entry->semiring = GxB_MAX_DIV_UINT32;

    entry = semiringhash_insert(semiringhash, "plus_div_uint32", &found);
    entry->name = pstrdup("plus_div_uint32");
    entry->semiring = GxB_PLUS_DIV_UINT32;

    entry = semiringhash_insert(semiringhash, "times_div_uint32", &found);
    entry->name = pstrdup("times_div_uint32");
    entry->semiring = GxB_TIMES_DIV_UINT32;

    entry = semiringhash_insert(semiringhash, "any_div_uint32", &found);
    entry->name = pstrdup("any_div_uint32");
    entry->semiring = GxB_ANY_DIV_UINT32;

    entry = semiringhash_insert(semiringhash, "min_div_uint64", &found);
    entry->name = pstrdup("min_div_uint64");
    entry->semiring = GxB_MIN_DIV_UINT64;

    entry = semiringhash_insert(semiringhash, "max_div_uint64", &found);
    entry->name = pstrdup("max_div_uint64");
    entry->semiring = GxB_MAX_DIV_UINT64;

    entry = semiringhash_insert(semiringhash, "plus_div_uint64", &found);
    entry->name = pstrdup("plus_div_uint64");
    entry->semiring = GxB_PLUS_DIV_UINT64;

    entry = semiringhash_insert(semiringhash, "times_div_uint64", &found);
    entry->name = pstrdup("times_div_uint64");
    entry->semiring = GxB_TIMES_DIV_UINT64;

    entry = semiringhash_insert(semiringhash, "any_div_uint64", &found);
    entry->name = pstrdup("any_div_uint64");
    entry->semiring = GxB_ANY_DIV_UINT64;

    entry = semiringhash_insert(semiringhash, "min_div_fp32", &found);
    entry->name = pstrdup("min_div_fp32");
    entry->semiring = GxB_MIN_DIV_FP32;

    entry = semiringhash_insert(semiringhash, "max_div_fp32", &found);
    entry->name = pstrdup("max_div_fp32");
    entry->semiring = GxB_MAX_DIV_FP32;

    entry = semiringhash_insert(semiringhash, "plus_div_fp32", &found);
    entry->name = pstrdup("plus_div_fp32");
    entry->semiring = GxB_PLUS_DIV_FP32;

    entry = semiringhash_insert(semiringhash, "times_div_fp32", &found);
    entry->name = pstrdup("times_div_fp32");
    entry->semiring = GxB_TIMES_DIV_FP32;

    entry = semiringhash_insert(semiringhash, "any_div_fp32", &found);
    entry->name = pstrdup("any_div_fp32");
    entry->semiring = GxB_ANY_DIV_FP32;

    entry = semiringhash_insert(semiringhash, "min_div_fp64", &found);
    entry->name = pstrdup("min_div_fp64");
    entry->semiring = GxB_MIN_DIV_FP64;

    entry = semiringhash_insert(semiringhash, "max_div_fp64", &found);
    entry->name = pstrdup("max_div_fp64");
    entry->semiring = GxB_MAX_DIV_FP64;

    entry = semiringhash_insert(semiringhash, "plus_div_fp64", &found);
    entry->name = pstrdup("plus_div_fp64");
    entry->semiring = GxB_PLUS_DIV_FP64;

    entry = semiringhash_insert(semiringhash, "times_div_fp64", &found);
    entry->name = pstrdup("times_div_fp64");
    entry->semiring = GxB_TIMES_DIV_FP64;

    entry = semiringhash_insert(semiringhash, "any_div_fp64", &found);
    entry->name = pstrdup("any_div_fp64");
    entry->semiring = GxB_ANY_DIV_FP64;

    entry = semiringhash_insert(semiringhash, "min_rdiv_int8", &found);
    entry->name = pstrdup("min_rdiv_int8");
    entry->semiring = GxB_MIN_RDIV_INT8;

    entry = semiringhash_insert(semiringhash, "max_rdiv_int8", &found);
    entry->name = pstrdup("max_rdiv_int8");
    entry->semiring = GxB_MAX_RDIV_INT8;

    entry = semiringhash_insert(semiringhash, "plus_rdiv_int8", &found);
    entry->name = pstrdup("plus_rdiv_int8");
    entry->semiring = GxB_PLUS_RDIV_INT8;

    entry = semiringhash_insert(semiringhash, "times_rdiv_int8", &found);
    entry->name = pstrdup("times_rdiv_int8");
    entry->semiring = GxB_TIMES_RDIV_INT8;

    entry = semiringhash_insert(semiringhash, "any_rdiv_int8", &found);
    entry->name = pstrdup("any_rdiv_int8");
    entry->semiring = GxB_ANY_RDIV_INT8;

    entry = semiringhash_insert(semiringhash, "min_rdiv_int16", &found);
    entry->name = pstrdup("min_rdiv_int16");
    entry->semiring = GxB_MIN_RDIV_INT16;

    entry = semiringhash_insert(semiringhash, "max_rdiv_int16", &found);
    entry->name = pstrdup("max_rdiv_int16");
    entry->semiring = GxB_MAX_RDIV_INT16;

    entry = semiringhash_insert(semiringhash, "plus_rdiv_int16", &found);
    entry->name = pstrdup("plus_rdiv_int16");
    entry->semiring = GxB_PLUS_RDIV_INT16;

    entry = semiringhash_insert(semiringhash, "times_rdiv_int16", &found);
    entry->name = pstrdup("times_rdiv_int16");
    entry->semiring = GxB_TIMES_RDIV_INT16;

    entry = semiringhash_insert(semiringhash, "any_rdiv_int16", &found);
    entry->name = pstrdup("any_rdiv_int16");
    entry->semiring = GxB_ANY_RDIV_INT16;

    entry = semiringhash_insert(semiringhash, "min_rdiv_int32", &found);
    entry->name = pstrdup("min_rdiv_int32");
    entry->semiring = GxB_MIN_RDIV_INT32;

    entry = semiringhash_insert(semiringhash, "max_rdiv_int32", &found);
    entry->name = pstrdup("max_rdiv_int32");
    entry->semiring = GxB_MAX_RDIV_INT32;

    entry = semiringhash_insert(semiringhash, "plus_rdiv_int32", &found);
    entry->name = pstrdup("plus_rdiv_int32");
    entry->semiring = GxB_PLUS_RDIV_INT32;

    entry = semiringhash_insert(semiringhash, "times_rdiv_int32", &found);
    entry->name = pstrdup("times_rdiv_int32");
    entry->semiring = GxB_TIMES_RDIV_INT32;

    entry = semiringhash_insert(semiringhash, "any_rdiv_int32", &found);
    entry->name = pstrdup("any_rdiv_int32");
    entry->semiring = GxB_ANY_RDIV_INT32;

    entry = semiringhash_insert(semiringhash, "min_rdiv_int64", &found);
    entry->name = pstrdup("min_rdiv_int64");
    entry->semiring = GxB_MIN_RDIV_INT64;

    entry = semiringhash_insert(semiringhash, "max_rdiv_int64", &found);
    entry->name = pstrdup("max_rdiv_int64");
    entry->semiring = GxB_MAX_RDIV_INT64;

    entry = semiringhash_insert(semiringhash, "plus_rdiv_int64", &found);
    entry->name = pstrdup("plus_rdiv_int64");
    entry->semiring = GxB_PLUS_RDIV_INT64;

    entry = semiringhash_insert(semiringhash, "times_rdiv_int64", &found);
    entry->name = pstrdup("times_rdiv_int64");
    entry->semiring = GxB_TIMES_RDIV_INT64;

    entry = semiringhash_insert(semiringhash, "any_rdiv_int64", &found);
    entry->name = pstrdup("any_rdiv_int64");
    entry->semiring = GxB_ANY_RDIV_INT64;

    entry = semiringhash_insert(semiringhash, "min_rdiv_uint8", &found);
    entry->name = pstrdup("min_rdiv_uint8");
    entry->semiring = GxB_MIN_RDIV_UINT8;

    entry = semiringhash_insert(semiringhash, "max_rdiv_uint8", &found);
    entry->name = pstrdup("max_rdiv_uint8");
    entry->semiring = GxB_MAX_RDIV_UINT8;

    entry = semiringhash_insert(semiringhash, "plus_rdiv_uint8", &found);
    entry->name = pstrdup("plus_rdiv_uint8");
    entry->semiring = GxB_PLUS_RDIV_UINT8;

    entry = semiringhash_insert(semiringhash, "times_rdiv_uint8", &found);
    entry->name = pstrdup("times_rdiv_uint8");
    entry->semiring = GxB_TIMES_RDIV_UINT8;

    entry = semiringhash_insert(semiringhash, "any_rdiv_uint8", &found);
    entry->name = pstrdup("any_rdiv_uint8");
    entry->semiring = GxB_ANY_RDIV_UINT8;

    entry = semiringhash_insert(semiringhash, "min_rdiv_uint16", &found);
    entry->name = pstrdup("min_rdiv_uint16");
    entry->semiring = GxB_MIN_RDIV_UINT16;

    entry = semiringhash_insert(semiringhash, "max_rdiv_uint16", &found);
    entry->name = pstrdup("max_rdiv_uint16");
    entry->semiring = GxB_MAX_RDIV_UINT16;

    entry = semiringhash_insert(semiringhash, "plus_rdiv_uint16", &found);
    entry->name = pstrdup("plus_rdiv_uint16");
    entry->semiring = GxB_PLUS_RDIV_UINT16;

    entry = semiringhash_insert(semiringhash, "times_rdiv_uint16", &found);
    entry->name = pstrdup("times_rdiv_uint16");
    entry->semiring = GxB_TIMES_RDIV_UINT16;

    entry = semiringhash_insert(semiringhash, "any_rdiv_uint16", &found);
    entry->name = pstrdup("any_rdiv_uint16");
    entry->semiring = GxB_ANY_RDIV_UINT16;

    entry = semiringhash_insert(semiringhash, "min_rdiv_uint32", &found);
    entry->name = pstrdup("min_rdiv_uint32");
    entry->semiring = GxB_MIN_RDIV_UINT32;

    entry = semiringhash_insert(semiringhash, "max_rdiv_uint32", &found);
    entry->name = pstrdup("max_rdiv_uint32");
    entry->semiring = GxB_MAX_RDIV_UINT32;

    entry = semiringhash_insert(semiringhash, "plus_rdiv_uint32", &found);
    entry->name = pstrdup("plus_rdiv_uint32");
    entry->semiring = GxB_PLUS_RDIV_UINT32;

    entry = semiringhash_insert(semiringhash, "times_rdiv_uint32", &found);
    entry->name = pstrdup("times_rdiv_uint32");
    entry->semiring = GxB_TIMES_RDIV_UINT32;

    entry = semiringhash_insert(semiringhash, "any_rdiv_uint32", &found);
    entry->name = pstrdup("any_rdiv_uint32");
    entry->semiring = GxB_ANY_RDIV_UINT32;

    entry = semiringhash_insert(semiringhash, "min_rdiv_uint64", &found);
    entry->name = pstrdup("min_rdiv_uint64");
    entry->semiring = GxB_MIN_RDIV_UINT64;

    entry = semiringhash_insert(semiringhash, "max_rdiv_uint64", &found);
    entry->name = pstrdup("max_rdiv_uint64");
    entry->semiring = GxB_MAX_RDIV_UINT64;

    entry = semiringhash_insert(semiringhash, "plus_rdiv_uint64", &found);
    entry->name = pstrdup("plus_rdiv_uint64");
    entry->semiring = GxB_PLUS_RDIV_UINT64;

    entry = semiringhash_insert(semiringhash, "times_rdiv_uint64", &found);
    entry->name = pstrdup("times_rdiv_uint64");
    entry->semiring = GxB_TIMES_RDIV_UINT64;

    entry = semiringhash_insert(semiringhash, "any_rdiv_uint64", &found);
    entry->name = pstrdup("any_rdiv_uint64");
    entry->semiring = GxB_ANY_RDIV_UINT64;

    entry = semiringhash_insert(semiringhash, "min_rdiv_fp32", &found);
    entry->name = pstrdup("min_rdiv_fp32");
    entry->semiring = GxB_MIN_RDIV_FP32;

    entry = semiringhash_insert(semiringhash, "max_rdiv_fp32", &found);
    entry->name = pstrdup("max_rdiv_fp32");
    entry->semiring = GxB_MAX_RDIV_FP32;

    entry = semiringhash_insert(semiringhash, "plus_rdiv_fp32", &found);
    entry->name = pstrdup("plus_rdiv_fp32");
    entry->semiring = GxB_PLUS_RDIV_FP32;

    entry = semiringhash_insert(semiringhash, "times_rdiv_fp32", &found);
    entry->name = pstrdup("times_rdiv_fp32");
    entry->semiring = GxB_TIMES_RDIV_FP32;

    entry = semiringhash_insert(semiringhash, "any_rdiv_fp32", &found);
    entry->name = pstrdup("any_rdiv_fp32");
    entry->semiring = GxB_ANY_RDIV_FP32;

    entry = semiringhash_insert(semiringhash, "min_rdiv_fp64", &found);
    entry->name = pstrdup("min_rdiv_fp64");
    entry->semiring = GxB_MIN_RDIV_FP64;

    entry = semiringhash_insert(semiringhash, "max_rdiv_fp64", &found);
    entry->name = pstrdup("max_rdiv_fp64");
    entry->semiring = GxB_MAX_RDIV_FP64;

    entry = semiringhash_insert(semiringhash, "plus_rdiv_fp64", &found);
    entry->name = pstrdup("plus_rdiv_fp64");
    entry->semiring = GxB_PLUS_RDIV_FP64;

    entry = semiringhash_insert(semiringhash, "times_rdiv_fp64", &found);
    entry->name = pstrdup("times_rdiv_fp64");
    entry->semiring = GxB_TIMES_RDIV_FP64;

    entry = semiringhash_insert(semiringhash, "any_rdiv_fp64", &found);
    entry->name = pstrdup("any_rdiv_fp64");
    entry->semiring = GxB_ANY_RDIV_FP64;

    entry = semiringhash_insert(semiringhash, "min_rminus_int8", &found);
    entry->name = pstrdup("min_rminus_int8");
    entry->semiring = GxB_MIN_RMINUS_INT8;

    entry = semiringhash_insert(semiringhash, "max_rminus_int8", &found);
    entry->name = pstrdup("max_rminus_int8");
    entry->semiring = GxB_MAX_RMINUS_INT8;

    entry = semiringhash_insert(semiringhash, "plus_rminus_int8", &found);
    entry->name = pstrdup("plus_rminus_int8");
    entry->semiring = GxB_PLUS_RMINUS_INT8;

    entry = semiringhash_insert(semiringhash, "times_rminus_int8", &found);
    entry->name = pstrdup("times_rminus_int8");
    entry->semiring = GxB_TIMES_RMINUS_INT8;

    entry = semiringhash_insert(semiringhash, "any_rminus_int8", &found);
    entry->name = pstrdup("any_rminus_int8");
    entry->semiring = GxB_ANY_RMINUS_INT8;

    entry = semiringhash_insert(semiringhash, "min_rminus_int16", &found);
    entry->name = pstrdup("min_rminus_int16");
    entry->semiring = GxB_MIN_RMINUS_INT16;

    entry = semiringhash_insert(semiringhash, "max_rminus_int16", &found);
    entry->name = pstrdup("max_rminus_int16");
    entry->semiring = GxB_MAX_RMINUS_INT16;

    entry = semiringhash_insert(semiringhash, "plus_rminus_int16", &found);
    entry->name = pstrdup("plus_rminus_int16");
    entry->semiring = GxB_PLUS_RMINUS_INT16;

    entry = semiringhash_insert(semiringhash, "times_rminus_int16", &found);
    entry->name = pstrdup("times_rminus_int16");
    entry->semiring = GxB_TIMES_RMINUS_INT16;

    entry = semiringhash_insert(semiringhash, "any_rminus_int16", &found);
    entry->name = pstrdup("any_rminus_int16");
    entry->semiring = GxB_ANY_RMINUS_INT16;

    entry = semiringhash_insert(semiringhash, "min_rminus_int32", &found);
    entry->name = pstrdup("min_rminus_int32");
    entry->semiring = GxB_MIN_RMINUS_INT32;

    entry = semiringhash_insert(semiringhash, "max_rminus_int32", &found);
    entry->name = pstrdup("max_rminus_int32");
    entry->semiring = GxB_MAX_RMINUS_INT32;

    entry = semiringhash_insert(semiringhash, "plus_rminus_int32", &found);
    entry->name = pstrdup("plus_rminus_int32");
    entry->semiring = GxB_PLUS_RMINUS_INT32;

    entry = semiringhash_insert(semiringhash, "times_rminus_int32", &found);
    entry->name = pstrdup("times_rminus_int32");
    entry->semiring = GxB_TIMES_RMINUS_INT32;

    entry = semiringhash_insert(semiringhash, "any_rminus_int32", &found);
    entry->name = pstrdup("any_rminus_int32");
    entry->semiring = GxB_ANY_RMINUS_INT32;

    entry = semiringhash_insert(semiringhash, "min_rminus_int64", &found);
    entry->name = pstrdup("min_rminus_int64");
    entry->semiring = GxB_MIN_RMINUS_INT64;

    entry = semiringhash_insert(semiringhash, "max_rminus_int64", &found);
    entry->name = pstrdup("max_rminus_int64");
    entry->semiring = GxB_MAX_RMINUS_INT64;

    entry = semiringhash_insert(semiringhash, "plus_rminus_int64", &found);
    entry->name = pstrdup("plus_rminus_int64");
    entry->semiring = GxB_PLUS_RMINUS_INT64;

    entry = semiringhash_insert(semiringhash, "times_rminus_int64", &found);
    entry->name = pstrdup("times_rminus_int64");
    entry->semiring = GxB_TIMES_RMINUS_INT64;

    entry = semiringhash_insert(semiringhash, "any_rminus_int64", &found);
    entry->name = pstrdup("any_rminus_int64");
    entry->semiring = GxB_ANY_RMINUS_INT64;

    entry = semiringhash_insert(semiringhash, "min_rminus_uint8", &found);
    entry->name = pstrdup("min_rminus_uint8");
    entry->semiring = GxB_MIN_RMINUS_UINT8;

    entry = semiringhash_insert(semiringhash, "max_rminus_uint8", &found);
    entry->name = pstrdup("max_rminus_uint8");
    entry->semiring = GxB_MAX_RMINUS_UINT8;

    entry = semiringhash_insert(semiringhash, "plus_rminus_uint8", &found);
    entry->name = pstrdup("plus_rminus_uint8");
    entry->semiring = GxB_PLUS_RMINUS_UINT8;

    entry = semiringhash_insert(semiringhash, "times_rminus_uint8", &found);
    entry->name = pstrdup("times_rminus_uint8");
    entry->semiring = GxB_TIMES_RMINUS_UINT8;

    entry = semiringhash_insert(semiringhash, "any_rminus_uint8", &found);
    entry->name = pstrdup("any_rminus_uint8");
    entry->semiring = GxB_ANY_RMINUS_UINT8;

    entry = semiringhash_insert(semiringhash, "min_rminus_uint16", &found);
    entry->name = pstrdup("min_rminus_uint16");
    entry->semiring = GxB_MIN_RMINUS_UINT16;

    entry = semiringhash_insert(semiringhash, "max_rminus_uint16", &found);
    entry->name = pstrdup("max_rminus_uint16");
    entry->semiring = GxB_MAX_RMINUS_UINT16;

    entry = semiringhash_insert(semiringhash, "plus_rminus_uint16", &found);
    entry->name = pstrdup("plus_rminus_uint16");
    entry->semiring = GxB_PLUS_RMINUS_UINT16;

    entry = semiringhash_insert(semiringhash, "times_rminus_uint16", &found);
    entry->name = pstrdup("times_rminus_uint16");
    entry->semiring = GxB_TIMES_RMINUS_UINT16;

    entry = semiringhash_insert(semiringhash, "any_rminus_uint16", &found);
    entry->name = pstrdup("any_rminus_uint16");
    entry->semiring = GxB_ANY_RMINUS_UINT16;

    entry = semiringhash_insert(semiringhash, "min_rminus_uint32", &found);
    entry->name = pstrdup("min_rminus_uint32");
    entry->semiring = GxB_MIN_RMINUS_UINT32;

    entry = semiringhash_insert(semiringhash, "max_rminus_uint32", &found);
    entry->name = pstrdup("max_rminus_uint32");
    entry->semiring = GxB_MAX_RMINUS_UINT32;

    entry = semiringhash_insert(semiringhash, "plus_rminus_uint32", &found);
    entry->name = pstrdup("plus_rminus_uint32");
    entry->semiring = GxB_PLUS_RMINUS_UINT32;

    entry = semiringhash_insert(semiringhash, "times_rminus_uint32", &found);
    entry->name = pstrdup("times_rminus_uint32");
    entry->semiring = GxB_TIMES_RMINUS_UINT32;

    entry = semiringhash_insert(semiringhash, "any_rminus_uint32", &found);
    entry->name = pstrdup("any_rminus_uint32");
    entry->semiring = GxB_ANY_RMINUS_UINT32;

    entry = semiringhash_insert(semiringhash, "min_rminus_uint64", &found);
    entry->name = pstrdup("min_rminus_uint64");
    entry->semiring = GxB_MIN_RMINUS_UINT64;

    entry = semiringhash_insert(semiringhash, "max_rminus_uint64", &found);
    entry->name = pstrdup("max_rminus_uint64");
    entry->semiring = GxB_MAX_RMINUS_UINT64;

    entry = semiringhash_insert(semiringhash, "plus_rminus_uint64", &found);
    entry->name = pstrdup("plus_rminus_uint64");
    entry->semiring = GxB_PLUS_RMINUS_UINT64;

    entry = semiringhash_insert(semiringhash, "times_rminus_uint64", &found);
    entry->name = pstrdup("times_rminus_uint64");
    entry->semiring = GxB_TIMES_RMINUS_UINT64;

    entry = semiringhash_insert(semiringhash, "any_rminus_uint64", &found);
    entry->name = pstrdup("any_rminus_uint64");
    entry->semiring = GxB_ANY_RMINUS_UINT64;

    entry = semiringhash_insert(semiringhash, "min_rminus_fp32", &found);
    entry->name = pstrdup("min_rminus_fp32");
    entry->semiring = GxB_MIN_RMINUS_FP32;

    entry = semiringhash_insert(semiringhash, "max_rminus_fp32", &found);
    entry->name = pstrdup("max_rminus_fp32");
    entry->semiring = GxB_MAX_RMINUS_FP32;

    entry = semiringhash_insert(semiringhash, "plus_rminus_fp32", &found);
    entry->name = pstrdup("plus_rminus_fp32");
    entry->semiring = GxB_PLUS_RMINUS_FP32;

    entry = semiringhash_insert(semiringhash, "times_rminus_fp32", &found);
    entry->name = pstrdup("times_rminus_fp32");
    entry->semiring = GxB_TIMES_RMINUS_FP32;

    entry = semiringhash_insert(semiringhash, "any_rminus_fp32", &found);
    entry->name = pstrdup("any_rminus_fp32");
    entry->semiring = GxB_ANY_RMINUS_FP32;

    entry = semiringhash_insert(semiringhash, "min_rminus_fp64", &found);
    entry->name = pstrdup("min_rminus_fp64");
    entry->semiring = GxB_MIN_RMINUS_FP64;

    entry = semiringhash_insert(semiringhash, "max_rminus_fp64", &found);
    entry->name = pstrdup("max_rminus_fp64");
    entry->semiring = GxB_MAX_RMINUS_FP64;

    entry = semiringhash_insert(semiringhash, "plus_rminus_fp64", &found);
    entry->name = pstrdup("plus_rminus_fp64");
    entry->semiring = GxB_PLUS_RMINUS_FP64;

    entry = semiringhash_insert(semiringhash, "times_rminus_fp64", &found);
    entry->name = pstrdup("times_rminus_fp64");
    entry->semiring = GxB_TIMES_RMINUS_FP64;

    entry = semiringhash_insert(semiringhash, "any_rminus_fp64", &found);
    entry->name = pstrdup("any_rminus_fp64");
    entry->semiring = GxB_ANY_RMINUS_FP64;

    entry = semiringhash_insert(semiringhash, "min_iseq_int8", &found);
    entry->name = pstrdup("min_iseq_int8");
    entry->semiring = GxB_MIN_ISEQ_INT8;

    entry = semiringhash_insert(semiringhash, "max_iseq_int8", &found);
    entry->name = pstrdup("max_iseq_int8");
    entry->semiring = GxB_MAX_ISEQ_INT8;

    entry = semiringhash_insert(semiringhash, "plus_iseq_int8", &found);
    entry->name = pstrdup("plus_iseq_int8");
    entry->semiring = GxB_PLUS_ISEQ_INT8;

    entry = semiringhash_insert(semiringhash, "times_iseq_int8", &found);
    entry->name = pstrdup("times_iseq_int8");
    entry->semiring = GxB_TIMES_ISEQ_INT8;

    entry = semiringhash_insert(semiringhash, "any_iseq_int8", &found);
    entry->name = pstrdup("any_iseq_int8");
    entry->semiring = GxB_ANY_ISEQ_INT8;

    entry = semiringhash_insert(semiringhash, "min_iseq_int16", &found);
    entry->name = pstrdup("min_iseq_int16");
    entry->semiring = GxB_MIN_ISEQ_INT16;

    entry = semiringhash_insert(semiringhash, "max_iseq_int16", &found);
    entry->name = pstrdup("max_iseq_int16");
    entry->semiring = GxB_MAX_ISEQ_INT16;

    entry = semiringhash_insert(semiringhash, "plus_iseq_int16", &found);
    entry->name = pstrdup("plus_iseq_int16");
    entry->semiring = GxB_PLUS_ISEQ_INT16;

    entry = semiringhash_insert(semiringhash, "times_iseq_int16", &found);
    entry->name = pstrdup("times_iseq_int16");
    entry->semiring = GxB_TIMES_ISEQ_INT16;

    entry = semiringhash_insert(semiringhash, "any_iseq_int16", &found);
    entry->name = pstrdup("any_iseq_int16");
    entry->semiring = GxB_ANY_ISEQ_INT16;

    entry = semiringhash_insert(semiringhash, "min_iseq_int32", &found);
    entry->name = pstrdup("min_iseq_int32");
    entry->semiring = GxB_MIN_ISEQ_INT32;

    entry = semiringhash_insert(semiringhash, "max_iseq_int32", &found);
    entry->name = pstrdup("max_iseq_int32");
    entry->semiring = GxB_MAX_ISEQ_INT32;

    entry = semiringhash_insert(semiringhash, "plus_iseq_int32", &found);
    entry->name = pstrdup("plus_iseq_int32");
    entry->semiring = GxB_PLUS_ISEQ_INT32;

    entry = semiringhash_insert(semiringhash, "times_iseq_int32", &found);
    entry->name = pstrdup("times_iseq_int32");
    entry->semiring = GxB_TIMES_ISEQ_INT32;

    entry = semiringhash_insert(semiringhash, "any_iseq_int32", &found);
    entry->name = pstrdup("any_iseq_int32");
    entry->semiring = GxB_ANY_ISEQ_INT32;

    entry = semiringhash_insert(semiringhash, "min_iseq_int64", &found);
    entry->name = pstrdup("min_iseq_int64");
    entry->semiring = GxB_MIN_ISEQ_INT64;

    entry = semiringhash_insert(semiringhash, "max_iseq_int64", &found);
    entry->name = pstrdup("max_iseq_int64");
    entry->semiring = GxB_MAX_ISEQ_INT64;

    entry = semiringhash_insert(semiringhash, "plus_iseq_int64", &found);
    entry->name = pstrdup("plus_iseq_int64");
    entry->semiring = GxB_PLUS_ISEQ_INT64;

    entry = semiringhash_insert(semiringhash, "times_iseq_int64", &found);
    entry->name = pstrdup("times_iseq_int64");
    entry->semiring = GxB_TIMES_ISEQ_INT64;

    entry = semiringhash_insert(semiringhash, "any_iseq_int64", &found);
    entry->name = pstrdup("any_iseq_int64");
    entry->semiring = GxB_ANY_ISEQ_INT64;

    entry = semiringhash_insert(semiringhash, "min_iseq_uint8", &found);
    entry->name = pstrdup("min_iseq_uint8");
    entry->semiring = GxB_MIN_ISEQ_UINT8;

    entry = semiringhash_insert(semiringhash, "max_iseq_uint8", &found);
    entry->name = pstrdup("max_iseq_uint8");
    entry->semiring = GxB_MAX_ISEQ_UINT8;

    entry = semiringhash_insert(semiringhash, "plus_iseq_uint8", &found);
    entry->name = pstrdup("plus_iseq_uint8");
    entry->semiring = GxB_PLUS_ISEQ_UINT8;

    entry = semiringhash_insert(semiringhash, "times_iseq_uint8", &found);
    entry->name = pstrdup("times_iseq_uint8");
    entry->semiring = GxB_TIMES_ISEQ_UINT8;

    entry = semiringhash_insert(semiringhash, "any_iseq_uint8", &found);
    entry->name = pstrdup("any_iseq_uint8");
    entry->semiring = GxB_ANY_ISEQ_UINT8;

    entry = semiringhash_insert(semiringhash, "min_iseq_uint16", &found);
    entry->name = pstrdup("min_iseq_uint16");
    entry->semiring = GxB_MIN_ISEQ_UINT16;

    entry = semiringhash_insert(semiringhash, "max_iseq_uint16", &found);
    entry->name = pstrdup("max_iseq_uint16");
    entry->semiring = GxB_MAX_ISEQ_UINT16;

    entry = semiringhash_insert(semiringhash, "plus_iseq_uint16", &found);
    entry->name = pstrdup("plus_iseq_uint16");
    entry->semiring = GxB_PLUS_ISEQ_UINT16;

    entry = semiringhash_insert(semiringhash, "times_iseq_uint16", &found);
    entry->name = pstrdup("times_iseq_uint16");
    entry->semiring = GxB_TIMES_ISEQ_UINT16;

    entry = semiringhash_insert(semiringhash, "any_iseq_uint16", &found);
    entry->name = pstrdup("any_iseq_uint16");
    entry->semiring = GxB_ANY_ISEQ_UINT16;

    entry = semiringhash_insert(semiringhash, "min_iseq_uint32", &found);
    entry->name = pstrdup("min_iseq_uint32");
    entry->semiring = GxB_MIN_ISEQ_UINT32;

    entry = semiringhash_insert(semiringhash, "max_iseq_uint32", &found);
    entry->name = pstrdup("max_iseq_uint32");
    entry->semiring = GxB_MAX_ISEQ_UINT32;

    entry = semiringhash_insert(semiringhash, "plus_iseq_uint32", &found);
    entry->name = pstrdup("plus_iseq_uint32");
    entry->semiring = GxB_PLUS_ISEQ_UINT32;

    entry = semiringhash_insert(semiringhash, "times_iseq_uint32", &found);
    entry->name = pstrdup("times_iseq_uint32");
    entry->semiring = GxB_TIMES_ISEQ_UINT32;

    entry = semiringhash_insert(semiringhash, "any_iseq_uint32", &found);
    entry->name = pstrdup("any_iseq_uint32");
    entry->semiring = GxB_ANY_ISEQ_UINT32;

    entry = semiringhash_insert(semiringhash, "min_iseq_uint64", &found);
    entry->name = pstrdup("min_iseq_uint64");
    entry->semiring = GxB_MIN_ISEQ_UINT64;

    entry = semiringhash_insert(semiringhash, "max_iseq_uint64", &found);
    entry->name = pstrdup("max_iseq_uint64");
    entry->semiring = GxB_MAX_ISEQ_UINT64;

    entry = semiringhash_insert(semiringhash, "plus_iseq_uint64", &found);
    entry->name = pstrdup("plus_iseq_uint64");
    entry->semiring = GxB_PLUS_ISEQ_UINT64;

    entry = semiringhash_insert(semiringhash, "times_iseq_uint64", &found);
    entry->name = pstrdup("times_iseq_uint64");
    entry->semiring = GxB_TIMES_ISEQ_UINT64;

    entry = semiringhash_insert(semiringhash, "any_iseq_uint64", &found);
    entry->name = pstrdup("any_iseq_uint64");
    entry->semiring = GxB_ANY_ISEQ_UINT64;

    entry = semiringhash_insert(semiringhash, "min_iseq_fp32", &found);
    entry->name = pstrdup("min_iseq_fp32");
    entry->semiring = GxB_MIN_ISEQ_FP32;

    entry = semiringhash_insert(semiringhash, "max_iseq_fp32", &found);
    entry->name = pstrdup("max_iseq_fp32");
    entry->semiring = GxB_MAX_ISEQ_FP32;

    entry = semiringhash_insert(semiringhash, "plus_iseq_fp32", &found);
    entry->name = pstrdup("plus_iseq_fp32");
    entry->semiring = GxB_PLUS_ISEQ_FP32;

    entry = semiringhash_insert(semiringhash, "times_iseq_fp32", &found);
    entry->name = pstrdup("times_iseq_fp32");
    entry->semiring = GxB_TIMES_ISEQ_FP32;

    entry = semiringhash_insert(semiringhash, "any_iseq_fp32", &found);
    entry->name = pstrdup("any_iseq_fp32");
    entry->semiring = GxB_ANY_ISEQ_FP32;

    entry = semiringhash_insert(semiringhash, "min_iseq_fp64", &found);
    entry->name = pstrdup("min_iseq_fp64");
    entry->semiring = GxB_MIN_ISEQ_FP64;

    entry = semiringhash_insert(semiringhash, "max_iseq_fp64", &found);
    entry->name = pstrdup("max_iseq_fp64");
    entry->semiring = GxB_MAX_ISEQ_FP64;

    entry = semiringhash_insert(semiringhash, "plus_iseq_fp64", &found);
    entry->name = pstrdup("plus_iseq_fp64");
    entry->semiring = GxB_PLUS_ISEQ_FP64;

    entry = semiringhash_insert(semiringhash, "times_iseq_fp64", &found);
    entry->name = pstrdup("times_iseq_fp64");
    entry->semiring = GxB_TIMES_ISEQ_FP64;

    entry = semiringhash_insert(semiringhash, "any_iseq_fp64", &found);
    entry->name = pstrdup("any_iseq_fp64");
    entry->semiring = GxB_ANY_ISEQ_FP64;

    entry = semiringhash_insert(semiringhash, "min_isne_int8", &found);
    entry->name = pstrdup("min_isne_int8");
    entry->semiring = GxB_MIN_ISNE_INT8;

    entry = semiringhash_insert(semiringhash, "max_isne_int8", &found);
    entry->name = pstrdup("max_isne_int8");
    entry->semiring = GxB_MAX_ISNE_INT8;

    entry = semiringhash_insert(semiringhash, "plus_isne_int8", &found);
    entry->name = pstrdup("plus_isne_int8");
    entry->semiring = GxB_PLUS_ISNE_INT8;

    entry = semiringhash_insert(semiringhash, "times_isne_int8", &found);
    entry->name = pstrdup("times_isne_int8");
    entry->semiring = GxB_TIMES_ISNE_INT8;

    entry = semiringhash_insert(semiringhash, "any_isne_int8", &found);
    entry->name = pstrdup("any_isne_int8");
    entry->semiring = GxB_ANY_ISNE_INT8;

    entry = semiringhash_insert(semiringhash, "min_isne_int16", &found);
    entry->name = pstrdup("min_isne_int16");
    entry->semiring = GxB_MIN_ISNE_INT16;

    entry = semiringhash_insert(semiringhash, "max_isne_int16", &found);
    entry->name = pstrdup("max_isne_int16");
    entry->semiring = GxB_MAX_ISNE_INT16;

    entry = semiringhash_insert(semiringhash, "plus_isne_int16", &found);
    entry->name = pstrdup("plus_isne_int16");
    entry->semiring = GxB_PLUS_ISNE_INT16;

    entry = semiringhash_insert(semiringhash, "times_isne_int16", &found);
    entry->name = pstrdup("times_isne_int16");
    entry->semiring = GxB_TIMES_ISNE_INT16;

    entry = semiringhash_insert(semiringhash, "any_isne_int16", &found);
    entry->name = pstrdup("any_isne_int16");
    entry->semiring = GxB_ANY_ISNE_INT16;

    entry = semiringhash_insert(semiringhash, "min_isne_int32", &found);
    entry->name = pstrdup("min_isne_int32");
    entry->semiring = GxB_MIN_ISNE_INT32;

    entry = semiringhash_insert(semiringhash, "max_isne_int32", &found);
    entry->name = pstrdup("max_isne_int32");
    entry->semiring = GxB_MAX_ISNE_INT32;

    entry = semiringhash_insert(semiringhash, "plus_isne_int32", &found);
    entry->name = pstrdup("plus_isne_int32");
    entry->semiring = GxB_PLUS_ISNE_INT32;

    entry = semiringhash_insert(semiringhash, "times_isne_int32", &found);
    entry->name = pstrdup("times_isne_int32");
    entry->semiring = GxB_TIMES_ISNE_INT32;

    entry = semiringhash_insert(semiringhash, "any_isne_int32", &found);
    entry->name = pstrdup("any_isne_int32");
    entry->semiring = GxB_ANY_ISNE_INT32;

    entry = semiringhash_insert(semiringhash, "min_isne_int64", &found);
    entry->name = pstrdup("min_isne_int64");
    entry->semiring = GxB_MIN_ISNE_INT64;

    entry = semiringhash_insert(semiringhash, "max_isne_int64", &found);
    entry->name = pstrdup("max_isne_int64");
    entry->semiring = GxB_MAX_ISNE_INT64;

    entry = semiringhash_insert(semiringhash, "plus_isne_int64", &found);
    entry->name = pstrdup("plus_isne_int64");
    entry->semiring = GxB_PLUS_ISNE_INT64;

    entry = semiringhash_insert(semiringhash, "times_isne_int64", &found);
    entry->name = pstrdup("times_isne_int64");
    entry->semiring = GxB_TIMES_ISNE_INT64;

    entry = semiringhash_insert(semiringhash, "any_isne_int64", &found);
    entry->name = pstrdup("any_isne_int64");
    entry->semiring = GxB_ANY_ISNE_INT64;

    entry = semiringhash_insert(semiringhash, "min_isne_uint8", &found);
    entry->name = pstrdup("min_isne_uint8");
    entry->semiring = GxB_MIN_ISNE_UINT8;

    entry = semiringhash_insert(semiringhash, "max_isne_uint8", &found);
    entry->name = pstrdup("max_isne_uint8");
    entry->semiring = GxB_MAX_ISNE_UINT8;

    entry = semiringhash_insert(semiringhash, "plus_isne_uint8", &found);
    entry->name = pstrdup("plus_isne_uint8");
    entry->semiring = GxB_PLUS_ISNE_UINT8;

    entry = semiringhash_insert(semiringhash, "times_isne_uint8", &found);
    entry->name = pstrdup("times_isne_uint8");
    entry->semiring = GxB_TIMES_ISNE_UINT8;

    entry = semiringhash_insert(semiringhash, "any_isne_uint8", &found);
    entry->name = pstrdup("any_isne_uint8");
    entry->semiring = GxB_ANY_ISNE_UINT8;

    entry = semiringhash_insert(semiringhash, "min_isne_uint16", &found);
    entry->name = pstrdup("min_isne_uint16");
    entry->semiring = GxB_MIN_ISNE_UINT16;

    entry = semiringhash_insert(semiringhash, "max_isne_uint16", &found);
    entry->name = pstrdup("max_isne_uint16");
    entry->semiring = GxB_MAX_ISNE_UINT16;

    entry = semiringhash_insert(semiringhash, "plus_isne_uint16", &found);
    entry->name = pstrdup("plus_isne_uint16");
    entry->semiring = GxB_PLUS_ISNE_UINT16;

    entry = semiringhash_insert(semiringhash, "times_isne_uint16", &found);
    entry->name = pstrdup("times_isne_uint16");
    entry->semiring = GxB_TIMES_ISNE_UINT16;

    entry = semiringhash_insert(semiringhash, "any_isne_uint16", &found);
    entry->name = pstrdup("any_isne_uint16");
    entry->semiring = GxB_ANY_ISNE_UINT16;

    entry = semiringhash_insert(semiringhash, "min_isne_uint32", &found);
    entry->name = pstrdup("min_isne_uint32");
    entry->semiring = GxB_MIN_ISNE_UINT32;

    entry = semiringhash_insert(semiringhash, "max_isne_uint32", &found);
    entry->name = pstrdup("max_isne_uint32");
    entry->semiring = GxB_MAX_ISNE_UINT32;

    entry = semiringhash_insert(semiringhash, "plus_isne_uint32", &found);
    entry->name = pstrdup("plus_isne_uint32");
    entry->semiring = GxB_PLUS_ISNE_UINT32;

    entry = semiringhash_insert(semiringhash, "times_isne_uint32", &found);
    entry->name = pstrdup("times_isne_uint32");
    entry->semiring = GxB_TIMES_ISNE_UINT32;

    entry = semiringhash_insert(semiringhash, "any_isne_uint32", &found);
    entry->name = pstrdup("any_isne_uint32");
    entry->semiring = GxB_ANY_ISNE_UINT32;

    entry = semiringhash_insert(semiringhash, "min_isne_uint64", &found);
    entry->name = pstrdup("min_isne_uint64");
    entry->semiring = GxB_MIN_ISNE_UINT64;

    entry = semiringhash_insert(semiringhash, "max_isne_uint64", &found);
    entry->name = pstrdup("max_isne_uint64");
    entry->semiring = GxB_MAX_ISNE_UINT64;

    entry = semiringhash_insert(semiringhash, "plus_isne_uint64", &found);
    entry->name = pstrdup("plus_isne_uint64");
    entry->semiring = GxB_PLUS_ISNE_UINT64;

    entry = semiringhash_insert(semiringhash, "times_isne_uint64", &found);
    entry->name = pstrdup("times_isne_uint64");
    entry->semiring = GxB_TIMES_ISNE_UINT64;

    entry = semiringhash_insert(semiringhash, "any_isne_uint64", &found);
    entry->name = pstrdup("any_isne_uint64");
    entry->semiring = GxB_ANY_ISNE_UINT64;

    entry = semiringhash_insert(semiringhash, "min_isne_fp32", &found);
    entry->name = pstrdup("min_isne_fp32");
    entry->semiring = GxB_MIN_ISNE_FP32;

    entry = semiringhash_insert(semiringhash, "max_isne_fp32", &found);
    entry->name = pstrdup("max_isne_fp32");
    entry->semiring = GxB_MAX_ISNE_FP32;

    entry = semiringhash_insert(semiringhash, "plus_isne_fp32", &found);
    entry->name = pstrdup("plus_isne_fp32");
    entry->semiring = GxB_PLUS_ISNE_FP32;

    entry = semiringhash_insert(semiringhash, "times_isne_fp32", &found);
    entry->name = pstrdup("times_isne_fp32");
    entry->semiring = GxB_TIMES_ISNE_FP32;

    entry = semiringhash_insert(semiringhash, "any_isne_fp32", &found);
    entry->name = pstrdup("any_isne_fp32");
    entry->semiring = GxB_ANY_ISNE_FP32;

    entry = semiringhash_insert(semiringhash, "min_isne_fp64", &found);
    entry->name = pstrdup("min_isne_fp64");
    entry->semiring = GxB_MIN_ISNE_FP64;

    entry = semiringhash_insert(semiringhash, "max_isne_fp64", &found);
    entry->name = pstrdup("max_isne_fp64");
    entry->semiring = GxB_MAX_ISNE_FP64;

    entry = semiringhash_insert(semiringhash, "plus_isne_fp64", &found);
    entry->name = pstrdup("plus_isne_fp64");
    entry->semiring = GxB_PLUS_ISNE_FP64;

    entry = semiringhash_insert(semiringhash, "times_isne_fp64", &found);
    entry->name = pstrdup("times_isne_fp64");
    entry->semiring = GxB_TIMES_ISNE_FP64;

    entry = semiringhash_insert(semiringhash, "any_isne_fp64", &found);
    entry->name = pstrdup("any_isne_fp64");
    entry->semiring = GxB_ANY_ISNE_FP64;

    entry = semiringhash_insert(semiringhash, "min_isgt_int8", &found);
    entry->name = pstrdup("min_isgt_int8");
    entry->semiring = GxB_MIN_ISGT_INT8;

    entry = semiringhash_insert(semiringhash, "max_isgt_int8", &found);
    entry->name = pstrdup("max_isgt_int8");
    entry->semiring = GxB_MAX_ISGT_INT8;

    entry = semiringhash_insert(semiringhash, "plus_isgt_int8", &found);
    entry->name = pstrdup("plus_isgt_int8");
    entry->semiring = GxB_PLUS_ISGT_INT8;

    entry = semiringhash_insert(semiringhash, "times_isgt_int8", &found);
    entry->name = pstrdup("times_isgt_int8");
    entry->semiring = GxB_TIMES_ISGT_INT8;

    entry = semiringhash_insert(semiringhash, "any_isgt_int8", &found);
    entry->name = pstrdup("any_isgt_int8");
    entry->semiring = GxB_ANY_ISGT_INT8;

    entry = semiringhash_insert(semiringhash, "min_isgt_int16", &found);
    entry->name = pstrdup("min_isgt_int16");
    entry->semiring = GxB_MIN_ISGT_INT16;

    entry = semiringhash_insert(semiringhash, "max_isgt_int16", &found);
    entry->name = pstrdup("max_isgt_int16");
    entry->semiring = GxB_MAX_ISGT_INT16;

    entry = semiringhash_insert(semiringhash, "plus_isgt_int16", &found);
    entry->name = pstrdup("plus_isgt_int16");
    entry->semiring = GxB_PLUS_ISGT_INT16;

    entry = semiringhash_insert(semiringhash, "times_isgt_int16", &found);
    entry->name = pstrdup("times_isgt_int16");
    entry->semiring = GxB_TIMES_ISGT_INT16;

    entry = semiringhash_insert(semiringhash, "any_isgt_int16", &found);
    entry->name = pstrdup("any_isgt_int16");
    entry->semiring = GxB_ANY_ISGT_INT16;

    entry = semiringhash_insert(semiringhash, "min_isgt_int32", &found);
    entry->name = pstrdup("min_isgt_int32");
    entry->semiring = GxB_MIN_ISGT_INT32;

    entry = semiringhash_insert(semiringhash, "max_isgt_int32", &found);
    entry->name = pstrdup("max_isgt_int32");
    entry->semiring = GxB_MAX_ISGT_INT32;

    entry = semiringhash_insert(semiringhash, "plus_isgt_int32", &found);
    entry->name = pstrdup("plus_isgt_int32");
    entry->semiring = GxB_PLUS_ISGT_INT32;

    entry = semiringhash_insert(semiringhash, "times_isgt_int32", &found);
    entry->name = pstrdup("times_isgt_int32");
    entry->semiring = GxB_TIMES_ISGT_INT32;

    entry = semiringhash_insert(semiringhash, "any_isgt_int32", &found);
    entry->name = pstrdup("any_isgt_int32");
    entry->semiring = GxB_ANY_ISGT_INT32;

    entry = semiringhash_insert(semiringhash, "min_isgt_int64", &found);
    entry->name = pstrdup("min_isgt_int64");
    entry->semiring = GxB_MIN_ISGT_INT64;

    entry = semiringhash_insert(semiringhash, "max_isgt_int64", &found);
    entry->name = pstrdup("max_isgt_int64");
    entry->semiring = GxB_MAX_ISGT_INT64;

    entry = semiringhash_insert(semiringhash, "plus_isgt_int64", &found);
    entry->name = pstrdup("plus_isgt_int64");
    entry->semiring = GxB_PLUS_ISGT_INT64;

    entry = semiringhash_insert(semiringhash, "times_isgt_int64", &found);
    entry->name = pstrdup("times_isgt_int64");
    entry->semiring = GxB_TIMES_ISGT_INT64;

    entry = semiringhash_insert(semiringhash, "any_isgt_int64", &found);
    entry->name = pstrdup("any_isgt_int64");
    entry->semiring = GxB_ANY_ISGT_INT64;

    entry = semiringhash_insert(semiringhash, "min_isgt_uint8", &found);
    entry->name = pstrdup("min_isgt_uint8");
    entry->semiring = GxB_MIN_ISGT_UINT8;

    entry = semiringhash_insert(semiringhash, "max_isgt_uint8", &found);
    entry->name = pstrdup("max_isgt_uint8");
    entry->semiring = GxB_MAX_ISGT_UINT8;

    entry = semiringhash_insert(semiringhash, "plus_isgt_uint8", &found);
    entry->name = pstrdup("plus_isgt_uint8");
    entry->semiring = GxB_PLUS_ISGT_UINT8;

    entry = semiringhash_insert(semiringhash, "times_isgt_uint8", &found);
    entry->name = pstrdup("times_isgt_uint8");
    entry->semiring = GxB_TIMES_ISGT_UINT8;

    entry = semiringhash_insert(semiringhash, "any_isgt_uint8", &found);
    entry->name = pstrdup("any_isgt_uint8");
    entry->semiring = GxB_ANY_ISGT_UINT8;

    entry = semiringhash_insert(semiringhash, "min_isgt_uint16", &found);
    entry->name = pstrdup("min_isgt_uint16");
    entry->semiring = GxB_MIN_ISGT_UINT16;

    entry = semiringhash_insert(semiringhash, "max_isgt_uint16", &found);
    entry->name = pstrdup("max_isgt_uint16");
    entry->semiring = GxB_MAX_ISGT_UINT16;

    entry = semiringhash_insert(semiringhash, "plus_isgt_uint16", &found);
    entry->name = pstrdup("plus_isgt_uint16");
    entry->semiring = GxB_PLUS_ISGT_UINT16;

    entry = semiringhash_insert(semiringhash, "times_isgt_uint16", &found);
    entry->name = pstrdup("times_isgt_uint16");
    entry->semiring = GxB_TIMES_ISGT_UINT16;

    entry = semiringhash_insert(semiringhash, "any_isgt_uint16", &found);
    entry->name = pstrdup("any_isgt_uint16");
    entry->semiring = GxB_ANY_ISGT_UINT16;

    entry = semiringhash_insert(semiringhash, "min_isgt_uint32", &found);
    entry->name = pstrdup("min_isgt_uint32");
    entry->semiring = GxB_MIN_ISGT_UINT32;

    entry = semiringhash_insert(semiringhash, "max_isgt_uint32", &found);
    entry->name = pstrdup("max_isgt_uint32");
    entry->semiring = GxB_MAX_ISGT_UINT32;

    entry = semiringhash_insert(semiringhash, "plus_isgt_uint32", &found);
    entry->name = pstrdup("plus_isgt_uint32");
    entry->semiring = GxB_PLUS_ISGT_UINT32;

    entry = semiringhash_insert(semiringhash, "times_isgt_uint32", &found);
    entry->name = pstrdup("times_isgt_uint32");
    entry->semiring = GxB_TIMES_ISGT_UINT32;

    entry = semiringhash_insert(semiringhash, "any_isgt_uint32", &found);
    entry->name = pstrdup("any_isgt_uint32");
    entry->semiring = GxB_ANY_ISGT_UINT32;

    entry = semiringhash_insert(semiringhash, "min_isgt_uint64", &found);
    entry->name = pstrdup("min_isgt_uint64");
    entry->semiring = GxB_MIN_ISGT_UINT64;

    entry = semiringhash_insert(semiringhash, "max_isgt_uint64", &found);
    entry->name = pstrdup("max_isgt_uint64");
    entry->semiring = GxB_MAX_ISGT_UINT64;

    entry = semiringhash_insert(semiringhash, "plus_isgt_uint64", &found);
    entry->name = pstrdup("plus_isgt_uint64");
    entry->semiring = GxB_PLUS_ISGT_UINT64;

    entry = semiringhash_insert(semiringhash, "times_isgt_uint64", &found);
    entry->name = pstrdup("times_isgt_uint64");
    entry->semiring = GxB_TIMES_ISGT_UINT64;

    entry = semiringhash_insert(semiringhash, "any_isgt_uint64", &found);
    entry->name = pstrdup("any_isgt_uint64");
    entry->semiring = GxB_ANY_ISGT_UINT64;

    entry = semiringhash_insert(semiringhash, "min_isgt_fp32", &found);
    entry->name = pstrdup("min_isgt_fp32");
    entry->semiring = GxB_MIN_ISGT_FP32;

    entry = semiringhash_insert(semiringhash, "max_isgt_fp32", &found);
    entry->name = pstrdup("max_isgt_fp32");
    entry->semiring = GxB_MAX_ISGT_FP32;

    entry = semiringhash_insert(semiringhash, "plus_isgt_fp32", &found);
    entry->name = pstrdup("plus_isgt_fp32");
    entry->semiring = GxB_PLUS_ISGT_FP32;

    entry = semiringhash_insert(semiringhash, "times_isgt_fp32", &found);
    entry->name = pstrdup("times_isgt_fp32");
    entry->semiring = GxB_TIMES_ISGT_FP32;

    entry = semiringhash_insert(semiringhash, "any_isgt_fp32", &found);
    entry->name = pstrdup("any_isgt_fp32");
    entry->semiring = GxB_ANY_ISGT_FP32;

    entry = semiringhash_insert(semiringhash, "min_isgt_fp64", &found);
    entry->name = pstrdup("min_isgt_fp64");
    entry->semiring = GxB_MIN_ISGT_FP64;

    entry = semiringhash_insert(semiringhash, "max_isgt_fp64", &found);
    entry->name = pstrdup("max_isgt_fp64");
    entry->semiring = GxB_MAX_ISGT_FP64;

    entry = semiringhash_insert(semiringhash, "plus_isgt_fp64", &found);
    entry->name = pstrdup("plus_isgt_fp64");
    entry->semiring = GxB_PLUS_ISGT_FP64;

    entry = semiringhash_insert(semiringhash, "times_isgt_fp64", &found);
    entry->name = pstrdup("times_isgt_fp64");
    entry->semiring = GxB_TIMES_ISGT_FP64;

    entry = semiringhash_insert(semiringhash, "any_isgt_fp64", &found);
    entry->name = pstrdup("any_isgt_fp64");
    entry->semiring = GxB_ANY_ISGT_FP64;

    entry = semiringhash_insert(semiringhash, "min_islt_int8", &found);
    entry->name = pstrdup("min_islt_int8");
    entry->semiring = GxB_MIN_ISLT_INT8;

    entry = semiringhash_insert(semiringhash, "max_islt_int8", &found);
    entry->name = pstrdup("max_islt_int8");
    entry->semiring = GxB_MAX_ISLT_INT8;

    entry = semiringhash_insert(semiringhash, "plus_islt_int8", &found);
    entry->name = pstrdup("plus_islt_int8");
    entry->semiring = GxB_PLUS_ISLT_INT8;

    entry = semiringhash_insert(semiringhash, "times_islt_int8", &found);
    entry->name = pstrdup("times_islt_int8");
    entry->semiring = GxB_TIMES_ISLT_INT8;

    entry = semiringhash_insert(semiringhash, "any_islt_int8", &found);
    entry->name = pstrdup("any_islt_int8");
    entry->semiring = GxB_ANY_ISLT_INT8;

    entry = semiringhash_insert(semiringhash, "min_islt_int16", &found);
    entry->name = pstrdup("min_islt_int16");
    entry->semiring = GxB_MIN_ISLT_INT16;

    entry = semiringhash_insert(semiringhash, "max_islt_int16", &found);
    entry->name = pstrdup("max_islt_int16");
    entry->semiring = GxB_MAX_ISLT_INT16;

    entry = semiringhash_insert(semiringhash, "plus_islt_int16", &found);
    entry->name = pstrdup("plus_islt_int16");
    entry->semiring = GxB_PLUS_ISLT_INT16;

    entry = semiringhash_insert(semiringhash, "times_islt_int16", &found);
    entry->name = pstrdup("times_islt_int16");
    entry->semiring = GxB_TIMES_ISLT_INT16;

    entry = semiringhash_insert(semiringhash, "any_islt_int16", &found);
    entry->name = pstrdup("any_islt_int16");
    entry->semiring = GxB_ANY_ISLT_INT16;

    entry = semiringhash_insert(semiringhash, "min_islt_int32", &found);
    entry->name = pstrdup("min_islt_int32");
    entry->semiring = GxB_MIN_ISLT_INT32;

    entry = semiringhash_insert(semiringhash, "max_islt_int32", &found);
    entry->name = pstrdup("max_islt_int32");
    entry->semiring = GxB_MAX_ISLT_INT32;

    entry = semiringhash_insert(semiringhash, "plus_islt_int32", &found);
    entry->name = pstrdup("plus_islt_int32");
    entry->semiring = GxB_PLUS_ISLT_INT32;

    entry = semiringhash_insert(semiringhash, "times_islt_int32", &found);
    entry->name = pstrdup("times_islt_int32");
    entry->semiring = GxB_TIMES_ISLT_INT32;

    entry = semiringhash_insert(semiringhash, "any_islt_int32", &found);
    entry->name = pstrdup("any_islt_int32");
    entry->semiring = GxB_ANY_ISLT_INT32;

    entry = semiringhash_insert(semiringhash, "min_islt_int64", &found);
    entry->name = pstrdup("min_islt_int64");
    entry->semiring = GxB_MIN_ISLT_INT64;

    entry = semiringhash_insert(semiringhash, "max_islt_int64", &found);
    entry->name = pstrdup("max_islt_int64");
    entry->semiring = GxB_MAX_ISLT_INT64;

    entry = semiringhash_insert(semiringhash, "plus_islt_int64", &found);
    entry->name = pstrdup("plus_islt_int64");
    entry->semiring = GxB_PLUS_ISLT_INT64;

    entry = semiringhash_insert(semiringhash, "times_islt_int64", &found);
    entry->name = pstrdup("times_islt_int64");
    entry->semiring = GxB_TIMES_ISLT_INT64;

    entry = semiringhash_insert(semiringhash, "any_islt_int64", &found);
    entry->name = pstrdup("any_islt_int64");
    entry->semiring = GxB_ANY_ISLT_INT64;

    entry = semiringhash_insert(semiringhash, "min_islt_uint8", &found);
    entry->name = pstrdup("min_islt_uint8");
    entry->semiring = GxB_MIN_ISLT_UINT8;

    entry = semiringhash_insert(semiringhash, "max_islt_uint8", &found);
    entry->name = pstrdup("max_islt_uint8");
    entry->semiring = GxB_MAX_ISLT_UINT8;

    entry = semiringhash_insert(semiringhash, "plus_islt_uint8", &found);
    entry->name = pstrdup("plus_islt_uint8");
    entry->semiring = GxB_PLUS_ISLT_UINT8;

    entry = semiringhash_insert(semiringhash, "times_islt_uint8", &found);
    entry->name = pstrdup("times_islt_uint8");
    entry->semiring = GxB_TIMES_ISLT_UINT8;

    entry = semiringhash_insert(semiringhash, "any_islt_uint8", &found);
    entry->name = pstrdup("any_islt_uint8");
    entry->semiring = GxB_ANY_ISLT_UINT8;

    entry = semiringhash_insert(semiringhash, "min_islt_uint16", &found);
    entry->name = pstrdup("min_islt_uint16");
    entry->semiring = GxB_MIN_ISLT_UINT16;

    entry = semiringhash_insert(semiringhash, "max_islt_uint16", &found);
    entry->name = pstrdup("max_islt_uint16");
    entry->semiring = GxB_MAX_ISLT_UINT16;

    entry = semiringhash_insert(semiringhash, "plus_islt_uint16", &found);
    entry->name = pstrdup("plus_islt_uint16");
    entry->semiring = GxB_PLUS_ISLT_UINT16;

    entry = semiringhash_insert(semiringhash, "times_islt_uint16", &found);
    entry->name = pstrdup("times_islt_uint16");
    entry->semiring = GxB_TIMES_ISLT_UINT16;

    entry = semiringhash_insert(semiringhash, "any_islt_uint16", &found);
    entry->name = pstrdup("any_islt_uint16");
    entry->semiring = GxB_ANY_ISLT_UINT16;

    entry = semiringhash_insert(semiringhash, "min_islt_uint32", &found);
    entry->name = pstrdup("min_islt_uint32");
    entry->semiring = GxB_MIN_ISLT_UINT32;

    entry = semiringhash_insert(semiringhash, "max_islt_uint32", &found);
    entry->name = pstrdup("max_islt_uint32");
    entry->semiring = GxB_MAX_ISLT_UINT32;

    entry = semiringhash_insert(semiringhash, "plus_islt_uint32", &found);
    entry->name = pstrdup("plus_islt_uint32");
    entry->semiring = GxB_PLUS_ISLT_UINT32;

    entry = semiringhash_insert(semiringhash, "times_islt_uint32", &found);
    entry->name = pstrdup("times_islt_uint32");
    entry->semiring = GxB_TIMES_ISLT_UINT32;

    entry = semiringhash_insert(semiringhash, "any_islt_uint32", &found);
    entry->name = pstrdup("any_islt_uint32");
    entry->semiring = GxB_ANY_ISLT_UINT32;

    entry = semiringhash_insert(semiringhash, "min_islt_uint64", &found);
    entry->name = pstrdup("min_islt_uint64");
    entry->semiring = GxB_MIN_ISLT_UINT64;

    entry = semiringhash_insert(semiringhash, "max_islt_uint64", &found);
    entry->name = pstrdup("max_islt_uint64");
    entry->semiring = GxB_MAX_ISLT_UINT64;

    entry = semiringhash_insert(semiringhash, "plus_islt_uint64", &found);
    entry->name = pstrdup("plus_islt_uint64");
    entry->semiring = GxB_PLUS_ISLT_UINT64;

    entry = semiringhash_insert(semiringhash, "times_islt_uint64", &found);
    entry->name = pstrdup("times_islt_uint64");
    entry->semiring = GxB_TIMES_ISLT_UINT64;

    entry = semiringhash_insert(semiringhash, "any_islt_uint64", &found);
    entry->name = pstrdup("any_islt_uint64");
    entry->semiring = GxB_ANY_ISLT_UINT64;

    entry = semiringhash_insert(semiringhash, "min_islt_fp32", &found);
    entry->name = pstrdup("min_islt_fp32");
    entry->semiring = GxB_MIN_ISLT_FP32;

    entry = semiringhash_insert(semiringhash, "max_islt_fp32", &found);
    entry->name = pstrdup("max_islt_fp32");
    entry->semiring = GxB_MAX_ISLT_FP32;

    entry = semiringhash_insert(semiringhash, "plus_islt_fp32", &found);
    entry->name = pstrdup("plus_islt_fp32");
    entry->semiring = GxB_PLUS_ISLT_FP32;

    entry = semiringhash_insert(semiringhash, "times_islt_fp32", &found);
    entry->name = pstrdup("times_islt_fp32");
    entry->semiring = GxB_TIMES_ISLT_FP32;

    entry = semiringhash_insert(semiringhash, "any_islt_fp32", &found);
    entry->name = pstrdup("any_islt_fp32");
    entry->semiring = GxB_ANY_ISLT_FP32;

    entry = semiringhash_insert(semiringhash, "min_islt_fp64", &found);
    entry->name = pstrdup("min_islt_fp64");
    entry->semiring = GxB_MIN_ISLT_FP64;

    entry = semiringhash_insert(semiringhash, "max_islt_fp64", &found);
    entry->name = pstrdup("max_islt_fp64");
    entry->semiring = GxB_MAX_ISLT_FP64;

    entry = semiringhash_insert(semiringhash, "plus_islt_fp64", &found);
    entry->name = pstrdup("plus_islt_fp64");
    entry->semiring = GxB_PLUS_ISLT_FP64;

    entry = semiringhash_insert(semiringhash, "times_islt_fp64", &found);
    entry->name = pstrdup("times_islt_fp64");
    entry->semiring = GxB_TIMES_ISLT_FP64;

    entry = semiringhash_insert(semiringhash, "any_islt_fp64", &found);
    entry->name = pstrdup("any_islt_fp64");
    entry->semiring = GxB_ANY_ISLT_FP64;

    entry = semiringhash_insert(semiringhash, "min_isge_int8", &found);
    entry->name = pstrdup("min_isge_int8");
    entry->semiring = GxB_MIN_ISGE_INT8;

    entry = semiringhash_insert(semiringhash, "max_isge_int8", &found);
    entry->name = pstrdup("max_isge_int8");
    entry->semiring = GxB_MAX_ISGE_INT8;

    entry = semiringhash_insert(semiringhash, "plus_isge_int8", &found);
    entry->name = pstrdup("plus_isge_int8");
    entry->semiring = GxB_PLUS_ISGE_INT8;

    entry = semiringhash_insert(semiringhash, "times_isge_int8", &found);
    entry->name = pstrdup("times_isge_int8");
    entry->semiring = GxB_TIMES_ISGE_INT8;

    entry = semiringhash_insert(semiringhash, "any_isge_int8", &found);
    entry->name = pstrdup("any_isge_int8");
    entry->semiring = GxB_ANY_ISGE_INT8;

    entry = semiringhash_insert(semiringhash, "min_isge_int16", &found);
    entry->name = pstrdup("min_isge_int16");
    entry->semiring = GxB_MIN_ISGE_INT16;

    entry = semiringhash_insert(semiringhash, "max_isge_int16", &found);
    entry->name = pstrdup("max_isge_int16");
    entry->semiring = GxB_MAX_ISGE_INT16;

    entry = semiringhash_insert(semiringhash, "plus_isge_int16", &found);
    entry->name = pstrdup("plus_isge_int16");
    entry->semiring = GxB_PLUS_ISGE_INT16;

    entry = semiringhash_insert(semiringhash, "times_isge_int16", &found);
    entry->name = pstrdup("times_isge_int16");
    entry->semiring = GxB_TIMES_ISGE_INT16;

    entry = semiringhash_insert(semiringhash, "any_isge_int16", &found);
    entry->name = pstrdup("any_isge_int16");
    entry->semiring = GxB_ANY_ISGE_INT16;

    entry = semiringhash_insert(semiringhash, "min_isge_int32", &found);
    entry->name = pstrdup("min_isge_int32");
    entry->semiring = GxB_MIN_ISGE_INT32;

    entry = semiringhash_insert(semiringhash, "max_isge_int32", &found);
    entry->name = pstrdup("max_isge_int32");
    entry->semiring = GxB_MAX_ISGE_INT32;

    entry = semiringhash_insert(semiringhash, "plus_isge_int32", &found);
    entry->name = pstrdup("plus_isge_int32");
    entry->semiring = GxB_PLUS_ISGE_INT32;

    entry = semiringhash_insert(semiringhash, "times_isge_int32", &found);
    entry->name = pstrdup("times_isge_int32");
    entry->semiring = GxB_TIMES_ISGE_INT32;

    entry = semiringhash_insert(semiringhash, "any_isge_int32", &found);
    entry->name = pstrdup("any_isge_int32");
    entry->semiring = GxB_ANY_ISGE_INT32;

    entry = semiringhash_insert(semiringhash, "min_isge_int64", &found);
    entry->name = pstrdup("min_isge_int64");
    entry->semiring = GxB_MIN_ISGE_INT64;

    entry = semiringhash_insert(semiringhash, "max_isge_int64", &found);
    entry->name = pstrdup("max_isge_int64");
    entry->semiring = GxB_MAX_ISGE_INT64;

    entry = semiringhash_insert(semiringhash, "plus_isge_int64", &found);
    entry->name = pstrdup("plus_isge_int64");
    entry->semiring = GxB_PLUS_ISGE_INT64;

    entry = semiringhash_insert(semiringhash, "times_isge_int64", &found);
    entry->name = pstrdup("times_isge_int64");
    entry->semiring = GxB_TIMES_ISGE_INT64;

    entry = semiringhash_insert(semiringhash, "any_isge_int64", &found);
    entry->name = pstrdup("any_isge_int64");
    entry->semiring = GxB_ANY_ISGE_INT64;

    entry = semiringhash_insert(semiringhash, "min_isge_uint8", &found);
    entry->name = pstrdup("min_isge_uint8");
    entry->semiring = GxB_MIN_ISGE_UINT8;

    entry = semiringhash_insert(semiringhash, "max_isge_uint8", &found);
    entry->name = pstrdup("max_isge_uint8");
    entry->semiring = GxB_MAX_ISGE_UINT8;

    entry = semiringhash_insert(semiringhash, "plus_isge_uint8", &found);
    entry->name = pstrdup("plus_isge_uint8");
    entry->semiring = GxB_PLUS_ISGE_UINT8;

    entry = semiringhash_insert(semiringhash, "times_isge_uint8", &found);
    entry->name = pstrdup("times_isge_uint8");
    entry->semiring = GxB_TIMES_ISGE_UINT8;

    entry = semiringhash_insert(semiringhash, "any_isge_uint8", &found);
    entry->name = pstrdup("any_isge_uint8");
    entry->semiring = GxB_ANY_ISGE_UINT8;

    entry = semiringhash_insert(semiringhash, "min_isge_uint16", &found);
    entry->name = pstrdup("min_isge_uint16");
    entry->semiring = GxB_MIN_ISGE_UINT16;

    entry = semiringhash_insert(semiringhash, "max_isge_uint16", &found);
    entry->name = pstrdup("max_isge_uint16");
    entry->semiring = GxB_MAX_ISGE_UINT16;

    entry = semiringhash_insert(semiringhash, "plus_isge_uint16", &found);
    entry->name = pstrdup("plus_isge_uint16");
    entry->semiring = GxB_PLUS_ISGE_UINT16;

    entry = semiringhash_insert(semiringhash, "times_isge_uint16", &found);
    entry->name = pstrdup("times_isge_uint16");
    entry->semiring = GxB_TIMES_ISGE_UINT16;

    entry = semiringhash_insert(semiringhash, "any_isge_uint16", &found);
    entry->name = pstrdup("any_isge_uint16");
    entry->semiring = GxB_ANY_ISGE_UINT16;

    entry = semiringhash_insert(semiringhash, "min_isge_uint32", &found);
    entry->name = pstrdup("min_isge_uint32");
    entry->semiring = GxB_MIN_ISGE_UINT32;

    entry = semiringhash_insert(semiringhash, "max_isge_uint32", &found);
    entry->name = pstrdup("max_isge_uint32");
    entry->semiring = GxB_MAX_ISGE_UINT32;

    entry = semiringhash_insert(semiringhash, "plus_isge_uint32", &found);
    entry->name = pstrdup("plus_isge_uint32");
    entry->semiring = GxB_PLUS_ISGE_UINT32;

    entry = semiringhash_insert(semiringhash, "times_isge_uint32", &found);
    entry->name = pstrdup("times_isge_uint32");
    entry->semiring = GxB_TIMES_ISGE_UINT32;

    entry = semiringhash_insert(semiringhash, "any_isge_uint32", &found);
    entry->name = pstrdup("any_isge_uint32");
    entry->semiring = GxB_ANY_ISGE_UINT32;

    entry = semiringhash_insert(semiringhash, "min_isge_uint64", &found);
    entry->name = pstrdup("min_isge_uint64");
    entry->semiring = GxB_MIN_ISGE_UINT64;

    entry = semiringhash_insert(semiringhash, "max_isge_uint64", &found);
    entry->name = pstrdup("max_isge_uint64");
    entry->semiring = GxB_MAX_ISGE_UINT64;

    entry = semiringhash_insert(semiringhash, "plus_isge_uint64", &found);
    entry->name = pstrdup("plus_isge_uint64");
    entry->semiring = GxB_PLUS_ISGE_UINT64;

    entry = semiringhash_insert(semiringhash, "times_isge_uint64", &found);
    entry->name = pstrdup("times_isge_uint64");
    entry->semiring = GxB_TIMES_ISGE_UINT64;

    entry = semiringhash_insert(semiringhash, "any_isge_uint64", &found);
    entry->name = pstrdup("any_isge_uint64");
    entry->semiring = GxB_ANY_ISGE_UINT64;

    entry = semiringhash_insert(semiringhash, "min_isge_fp32", &found);
    entry->name = pstrdup("min_isge_fp32");
    entry->semiring = GxB_MIN_ISGE_FP32;

    entry = semiringhash_insert(semiringhash, "max_isge_fp32", &found);
    entry->name = pstrdup("max_isge_fp32");
    entry->semiring = GxB_MAX_ISGE_FP32;

    entry = semiringhash_insert(semiringhash, "plus_isge_fp32", &found);
    entry->name = pstrdup("plus_isge_fp32");
    entry->semiring = GxB_PLUS_ISGE_FP32;

    entry = semiringhash_insert(semiringhash, "times_isge_fp32", &found);
    entry->name = pstrdup("times_isge_fp32");
    entry->semiring = GxB_TIMES_ISGE_FP32;

    entry = semiringhash_insert(semiringhash, "any_isge_fp32", &found);
    entry->name = pstrdup("any_isge_fp32");
    entry->semiring = GxB_ANY_ISGE_FP32;

    entry = semiringhash_insert(semiringhash, "min_isge_fp64", &found);
    entry->name = pstrdup("min_isge_fp64");
    entry->semiring = GxB_MIN_ISGE_FP64;

    entry = semiringhash_insert(semiringhash, "max_isge_fp64", &found);
    entry->name = pstrdup("max_isge_fp64");
    entry->semiring = GxB_MAX_ISGE_FP64;

    entry = semiringhash_insert(semiringhash, "plus_isge_fp64", &found);
    entry->name = pstrdup("plus_isge_fp64");
    entry->semiring = GxB_PLUS_ISGE_FP64;

    entry = semiringhash_insert(semiringhash, "times_isge_fp64", &found);
    entry->name = pstrdup("times_isge_fp64");
    entry->semiring = GxB_TIMES_ISGE_FP64;

    entry = semiringhash_insert(semiringhash, "any_isge_fp64", &found);
    entry->name = pstrdup("any_isge_fp64");
    entry->semiring = GxB_ANY_ISGE_FP64;

    entry = semiringhash_insert(semiringhash, "min_isle_int8", &found);
    entry->name = pstrdup("min_isle_int8");
    entry->semiring = GxB_MIN_ISLE_INT8;

    entry = semiringhash_insert(semiringhash, "max_isle_int8", &found);
    entry->name = pstrdup("max_isle_int8");
    entry->semiring = GxB_MAX_ISLE_INT8;

    entry = semiringhash_insert(semiringhash, "plus_isle_int8", &found);
    entry->name = pstrdup("plus_isle_int8");
    entry->semiring = GxB_PLUS_ISLE_INT8;

    entry = semiringhash_insert(semiringhash, "times_isle_int8", &found);
    entry->name = pstrdup("times_isle_int8");
    entry->semiring = GxB_TIMES_ISLE_INT8;

    entry = semiringhash_insert(semiringhash, "any_isle_int8", &found);
    entry->name = pstrdup("any_isle_int8");
    entry->semiring = GxB_ANY_ISLE_INT8;

    entry = semiringhash_insert(semiringhash, "min_isle_int16", &found);
    entry->name = pstrdup("min_isle_int16");
    entry->semiring = GxB_MIN_ISLE_INT16;

    entry = semiringhash_insert(semiringhash, "max_isle_int16", &found);
    entry->name = pstrdup("max_isle_int16");
    entry->semiring = GxB_MAX_ISLE_INT16;

    entry = semiringhash_insert(semiringhash, "plus_isle_int16", &found);
    entry->name = pstrdup("plus_isle_int16");
    entry->semiring = GxB_PLUS_ISLE_INT16;

    entry = semiringhash_insert(semiringhash, "times_isle_int16", &found);
    entry->name = pstrdup("times_isle_int16");
    entry->semiring = GxB_TIMES_ISLE_INT16;

    entry = semiringhash_insert(semiringhash, "any_isle_int16", &found);
    entry->name = pstrdup("any_isle_int16");
    entry->semiring = GxB_ANY_ISLE_INT16;

    entry = semiringhash_insert(semiringhash, "min_isle_int32", &found);
    entry->name = pstrdup("min_isle_int32");
    entry->semiring = GxB_MIN_ISLE_INT32;

    entry = semiringhash_insert(semiringhash, "max_isle_int32", &found);
    entry->name = pstrdup("max_isle_int32");
    entry->semiring = GxB_MAX_ISLE_INT32;

    entry = semiringhash_insert(semiringhash, "plus_isle_int32", &found);
    entry->name = pstrdup("plus_isle_int32");
    entry->semiring = GxB_PLUS_ISLE_INT32;

    entry = semiringhash_insert(semiringhash, "times_isle_int32", &found);
    entry->name = pstrdup("times_isle_int32");
    entry->semiring = GxB_TIMES_ISLE_INT32;

    entry = semiringhash_insert(semiringhash, "any_isle_int32", &found);
    entry->name = pstrdup("any_isle_int32");
    entry->semiring = GxB_ANY_ISLE_INT32;

    entry = semiringhash_insert(semiringhash, "min_isle_int64", &found);
    entry->name = pstrdup("min_isle_int64");
    entry->semiring = GxB_MIN_ISLE_INT64;

    entry = semiringhash_insert(semiringhash, "max_isle_int64", &found);
    entry->name = pstrdup("max_isle_int64");
    entry->semiring = GxB_MAX_ISLE_INT64;

    entry = semiringhash_insert(semiringhash, "plus_isle_int64", &found);
    entry->name = pstrdup("plus_isle_int64");
    entry->semiring = GxB_PLUS_ISLE_INT64;

    entry = semiringhash_insert(semiringhash, "times_isle_int64", &found);
    entry->name = pstrdup("times_isle_int64");
    entry->semiring = GxB_TIMES_ISLE_INT64;

    entry = semiringhash_insert(semiringhash, "any_isle_int64", &found);
    entry->name = pstrdup("any_isle_int64");
    entry->semiring = GxB_ANY_ISLE_INT64;

    entry = semiringhash_insert(semiringhash, "min_isle_uint8", &found);
    entry->name = pstrdup("min_isle_uint8");
    entry->semiring = GxB_MIN_ISLE_UINT8;

    entry = semiringhash_insert(semiringhash, "max_isle_uint8", &found);
    entry->name = pstrdup("max_isle_uint8");
    entry->semiring = GxB_MAX_ISLE_UINT8;

    entry = semiringhash_insert(semiringhash, "plus_isle_uint8", &found);
    entry->name = pstrdup("plus_isle_uint8");
    entry->semiring = GxB_PLUS_ISLE_UINT8;

    entry = semiringhash_insert(semiringhash, "times_isle_uint8", &found);
    entry->name = pstrdup("times_isle_uint8");
    entry->semiring = GxB_TIMES_ISLE_UINT8;

    entry = semiringhash_insert(semiringhash, "any_isle_uint8", &found);
    entry->name = pstrdup("any_isle_uint8");
    entry->semiring = GxB_ANY_ISLE_UINT8;

    entry = semiringhash_insert(semiringhash, "min_isle_uint16", &found);
    entry->name = pstrdup("min_isle_uint16");
    entry->semiring = GxB_MIN_ISLE_UINT16;

    entry = semiringhash_insert(semiringhash, "max_isle_uint16", &found);
    entry->name = pstrdup("max_isle_uint16");
    entry->semiring = GxB_MAX_ISLE_UINT16;

    entry = semiringhash_insert(semiringhash, "plus_isle_uint16", &found);
    entry->name = pstrdup("plus_isle_uint16");
    entry->semiring = GxB_PLUS_ISLE_UINT16;

    entry = semiringhash_insert(semiringhash, "times_isle_uint16", &found);
    entry->name = pstrdup("times_isle_uint16");
    entry->semiring = GxB_TIMES_ISLE_UINT16;

    entry = semiringhash_insert(semiringhash, "any_isle_uint16", &found);
    entry->name = pstrdup("any_isle_uint16");
    entry->semiring = GxB_ANY_ISLE_UINT16;

    entry = semiringhash_insert(semiringhash, "min_isle_uint32", &found);
    entry->name = pstrdup("min_isle_uint32");
    entry->semiring = GxB_MIN_ISLE_UINT32;

    entry = semiringhash_insert(semiringhash, "max_isle_uint32", &found);
    entry->name = pstrdup("max_isle_uint32");
    entry->semiring = GxB_MAX_ISLE_UINT32;

    entry = semiringhash_insert(semiringhash, "plus_isle_uint32", &found);
    entry->name = pstrdup("plus_isle_uint32");
    entry->semiring = GxB_PLUS_ISLE_UINT32;

    entry = semiringhash_insert(semiringhash, "times_isle_uint32", &found);
    entry->name = pstrdup("times_isle_uint32");
    entry->semiring = GxB_TIMES_ISLE_UINT32;

    entry = semiringhash_insert(semiringhash, "any_isle_uint32", &found);
    entry->name = pstrdup("any_isle_uint32");
    entry->semiring = GxB_ANY_ISLE_UINT32;

    entry = semiringhash_insert(semiringhash, "min_isle_uint64", &found);
    entry->name = pstrdup("min_isle_uint64");
    entry->semiring = GxB_MIN_ISLE_UINT64;

    entry = semiringhash_insert(semiringhash, "max_isle_uint64", &found);
    entry->name = pstrdup("max_isle_uint64");
    entry->semiring = GxB_MAX_ISLE_UINT64;

    entry = semiringhash_insert(semiringhash, "plus_isle_uint64", &found);
    entry->name = pstrdup("plus_isle_uint64");
    entry->semiring = GxB_PLUS_ISLE_UINT64;

    entry = semiringhash_insert(semiringhash, "times_isle_uint64", &found);
    entry->name = pstrdup("times_isle_uint64");
    entry->semiring = GxB_TIMES_ISLE_UINT64;

    entry = semiringhash_insert(semiringhash, "any_isle_uint64", &found);
    entry->name = pstrdup("any_isle_uint64");
    entry->semiring = GxB_ANY_ISLE_UINT64;

    entry = semiringhash_insert(semiringhash, "min_isle_fp32", &found);
    entry->name = pstrdup("min_isle_fp32");
    entry->semiring = GxB_MIN_ISLE_FP32;

    entry = semiringhash_insert(semiringhash, "max_isle_fp32", &found);
    entry->name = pstrdup("max_isle_fp32");
    entry->semiring = GxB_MAX_ISLE_FP32;

    entry = semiringhash_insert(semiringhash, "plus_isle_fp32", &found);
    entry->name = pstrdup("plus_isle_fp32");
    entry->semiring = GxB_PLUS_ISLE_FP32;

    entry = semiringhash_insert(semiringhash, "times_isle_fp32", &found);
    entry->name = pstrdup("times_isle_fp32");
    entry->semiring = GxB_TIMES_ISLE_FP32;

    entry = semiringhash_insert(semiringhash, "any_isle_fp32", &found);
    entry->name = pstrdup("any_isle_fp32");
    entry->semiring = GxB_ANY_ISLE_FP32;

    entry = semiringhash_insert(semiringhash, "min_isle_fp64", &found);
    entry->name = pstrdup("min_isle_fp64");
    entry->semiring = GxB_MIN_ISLE_FP64;

    entry = semiringhash_insert(semiringhash, "max_isle_fp64", &found);
    entry->name = pstrdup("max_isle_fp64");
    entry->semiring = GxB_MAX_ISLE_FP64;

    entry = semiringhash_insert(semiringhash, "plus_isle_fp64", &found);
    entry->name = pstrdup("plus_isle_fp64");
    entry->semiring = GxB_PLUS_ISLE_FP64;

    entry = semiringhash_insert(semiringhash, "times_isle_fp64", &found);
    entry->name = pstrdup("times_isle_fp64");
    entry->semiring = GxB_TIMES_ISLE_FP64;

    entry = semiringhash_insert(semiringhash, "any_isle_fp64", &found);
    entry->name = pstrdup("any_isle_fp64");
    entry->semiring = GxB_ANY_ISLE_FP64;

    entry = semiringhash_insert(semiringhash, "min_lor_int8", &found);
    entry->name = pstrdup("min_lor_int8");
    entry->semiring = GxB_MIN_LOR_INT8;

    entry = semiringhash_insert(semiringhash, "max_lor_int8", &found);
    entry->name = pstrdup("max_lor_int8");
    entry->semiring = GxB_MAX_LOR_INT8;

    entry = semiringhash_insert(semiringhash, "plus_lor_int8", &found);
    entry->name = pstrdup("plus_lor_int8");
    entry->semiring = GxB_PLUS_LOR_INT8;

    entry = semiringhash_insert(semiringhash, "times_lor_int8", &found);
    entry->name = pstrdup("times_lor_int8");
    entry->semiring = GxB_TIMES_LOR_INT8;

    entry = semiringhash_insert(semiringhash, "any_lor_int8", &found);
    entry->name = pstrdup("any_lor_int8");
    entry->semiring = GxB_ANY_LOR_INT8;

    entry = semiringhash_insert(semiringhash, "min_lor_int16", &found);
    entry->name = pstrdup("min_lor_int16");
    entry->semiring = GxB_MIN_LOR_INT16;

    entry = semiringhash_insert(semiringhash, "max_lor_int16", &found);
    entry->name = pstrdup("max_lor_int16");
    entry->semiring = GxB_MAX_LOR_INT16;

    entry = semiringhash_insert(semiringhash, "plus_lor_int16", &found);
    entry->name = pstrdup("plus_lor_int16");
    entry->semiring = GxB_PLUS_LOR_INT16;

    entry = semiringhash_insert(semiringhash, "times_lor_int16", &found);
    entry->name = pstrdup("times_lor_int16");
    entry->semiring = GxB_TIMES_LOR_INT16;

    entry = semiringhash_insert(semiringhash, "any_lor_int16", &found);
    entry->name = pstrdup("any_lor_int16");
    entry->semiring = GxB_ANY_LOR_INT16;

    entry = semiringhash_insert(semiringhash, "min_lor_int32", &found);
    entry->name = pstrdup("min_lor_int32");
    entry->semiring = GxB_MIN_LOR_INT32;

    entry = semiringhash_insert(semiringhash, "max_lor_int32", &found);
    entry->name = pstrdup("max_lor_int32");
    entry->semiring = GxB_MAX_LOR_INT32;

    entry = semiringhash_insert(semiringhash, "plus_lor_int32", &found);
    entry->name = pstrdup("plus_lor_int32");
    entry->semiring = GxB_PLUS_LOR_INT32;

    entry = semiringhash_insert(semiringhash, "times_lor_int32", &found);
    entry->name = pstrdup("times_lor_int32");
    entry->semiring = GxB_TIMES_LOR_INT32;

    entry = semiringhash_insert(semiringhash, "any_lor_int32", &found);
    entry->name = pstrdup("any_lor_int32");
    entry->semiring = GxB_ANY_LOR_INT32;

    entry = semiringhash_insert(semiringhash, "min_lor_int64", &found);
    entry->name = pstrdup("min_lor_int64");
    entry->semiring = GxB_MIN_LOR_INT64;

    entry = semiringhash_insert(semiringhash, "max_lor_int64", &found);
    entry->name = pstrdup("max_lor_int64");
    entry->semiring = GxB_MAX_LOR_INT64;

    entry = semiringhash_insert(semiringhash, "plus_lor_int64", &found);
    entry->name = pstrdup("plus_lor_int64");
    entry->semiring = GxB_PLUS_LOR_INT64;

    entry = semiringhash_insert(semiringhash, "times_lor_int64", &found);
    entry->name = pstrdup("times_lor_int64");
    entry->semiring = GxB_TIMES_LOR_INT64;

    entry = semiringhash_insert(semiringhash, "any_lor_int64", &found);
    entry->name = pstrdup("any_lor_int64");
    entry->semiring = GxB_ANY_LOR_INT64;

    entry = semiringhash_insert(semiringhash, "min_lor_uint8", &found);
    entry->name = pstrdup("min_lor_uint8");
    entry->semiring = GxB_MIN_LOR_UINT8;

    entry = semiringhash_insert(semiringhash, "max_lor_uint8", &found);
    entry->name = pstrdup("max_lor_uint8");
    entry->semiring = GxB_MAX_LOR_UINT8;

    entry = semiringhash_insert(semiringhash, "plus_lor_uint8", &found);
    entry->name = pstrdup("plus_lor_uint8");
    entry->semiring = GxB_PLUS_LOR_UINT8;

    entry = semiringhash_insert(semiringhash, "times_lor_uint8", &found);
    entry->name = pstrdup("times_lor_uint8");
    entry->semiring = GxB_TIMES_LOR_UINT8;

    entry = semiringhash_insert(semiringhash, "any_lor_uint8", &found);
    entry->name = pstrdup("any_lor_uint8");
    entry->semiring = GxB_ANY_LOR_UINT8;

    entry = semiringhash_insert(semiringhash, "min_lor_uint16", &found);
    entry->name = pstrdup("min_lor_uint16");
    entry->semiring = GxB_MIN_LOR_UINT16;

    entry = semiringhash_insert(semiringhash, "max_lor_uint16", &found);
    entry->name = pstrdup("max_lor_uint16");
    entry->semiring = GxB_MAX_LOR_UINT16;

    entry = semiringhash_insert(semiringhash, "plus_lor_uint16", &found);
    entry->name = pstrdup("plus_lor_uint16");
    entry->semiring = GxB_PLUS_LOR_UINT16;

    entry = semiringhash_insert(semiringhash, "times_lor_uint16", &found);
    entry->name = pstrdup("times_lor_uint16");
    entry->semiring = GxB_TIMES_LOR_UINT16;

    entry = semiringhash_insert(semiringhash, "any_lor_uint16", &found);
    entry->name = pstrdup("any_lor_uint16");
    entry->semiring = GxB_ANY_LOR_UINT16;

    entry = semiringhash_insert(semiringhash, "min_lor_uint32", &found);
    entry->name = pstrdup("min_lor_uint32");
    entry->semiring = GxB_MIN_LOR_UINT32;

    entry = semiringhash_insert(semiringhash, "max_lor_uint32", &found);
    entry->name = pstrdup("max_lor_uint32");
    entry->semiring = GxB_MAX_LOR_UINT32;

    entry = semiringhash_insert(semiringhash, "plus_lor_uint32", &found);
    entry->name = pstrdup("plus_lor_uint32");
    entry->semiring = GxB_PLUS_LOR_UINT32;

    entry = semiringhash_insert(semiringhash, "times_lor_uint32", &found);
    entry->name = pstrdup("times_lor_uint32");
    entry->semiring = GxB_TIMES_LOR_UINT32;

    entry = semiringhash_insert(semiringhash, "any_lor_uint32", &found);
    entry->name = pstrdup("any_lor_uint32");
    entry->semiring = GxB_ANY_LOR_UINT32;

    entry = semiringhash_insert(semiringhash, "min_lor_uint64", &found);
    entry->name = pstrdup("min_lor_uint64");
    entry->semiring = GxB_MIN_LOR_UINT64;

    entry = semiringhash_insert(semiringhash, "max_lor_uint64", &found);
    entry->name = pstrdup("max_lor_uint64");
    entry->semiring = GxB_MAX_LOR_UINT64;

    entry = semiringhash_insert(semiringhash, "plus_lor_uint64", &found);
    entry->name = pstrdup("plus_lor_uint64");
    entry->semiring = GxB_PLUS_LOR_UINT64;

    entry = semiringhash_insert(semiringhash, "times_lor_uint64", &found);
    entry->name = pstrdup("times_lor_uint64");
    entry->semiring = GxB_TIMES_LOR_UINT64;

    entry = semiringhash_insert(semiringhash, "any_lor_uint64", &found);
    entry->name = pstrdup("any_lor_uint64");
    entry->semiring = GxB_ANY_LOR_UINT64;

    entry = semiringhash_insert(semiringhash, "min_lor_fp32", &found);
    entry->name = pstrdup("min_lor_fp32");
    entry->semiring = GxB_MIN_LOR_FP32;

    entry = semiringhash_insert(semiringhash, "max_lor_fp32", &found);
    entry->name = pstrdup("max_lor_fp32");
    entry->semiring = GxB_MAX_LOR_FP32;

    entry = semiringhash_insert(semiringhash, "plus_lor_fp32", &found);
    entry->name = pstrdup("plus_lor_fp32");
    entry->semiring = GxB_PLUS_LOR_FP32;

    entry = semiringhash_insert(semiringhash, "times_lor_fp32", &found);
    entry->name = pstrdup("times_lor_fp32");
    entry->semiring = GxB_TIMES_LOR_FP32;

    entry = semiringhash_insert(semiringhash, "any_lor_fp32", &found);
    entry->name = pstrdup("any_lor_fp32");
    entry->semiring = GxB_ANY_LOR_FP32;

    entry = semiringhash_insert(semiringhash, "min_lor_fp64", &found);
    entry->name = pstrdup("min_lor_fp64");
    entry->semiring = GxB_MIN_LOR_FP64;

    entry = semiringhash_insert(semiringhash, "max_lor_fp64", &found);
    entry->name = pstrdup("max_lor_fp64");
    entry->semiring = GxB_MAX_LOR_FP64;

    entry = semiringhash_insert(semiringhash, "plus_lor_fp64", &found);
    entry->name = pstrdup("plus_lor_fp64");
    entry->semiring = GxB_PLUS_LOR_FP64;

    entry = semiringhash_insert(semiringhash, "times_lor_fp64", &found);
    entry->name = pstrdup("times_lor_fp64");
    entry->semiring = GxB_TIMES_LOR_FP64;

    entry = semiringhash_insert(semiringhash, "any_lor_fp64", &found);
    entry->name = pstrdup("any_lor_fp64");
    entry->semiring = GxB_ANY_LOR_FP64;

    entry = semiringhash_insert(semiringhash, "min_land_int8", &found);
    entry->name = pstrdup("min_land_int8");
    entry->semiring = GxB_MIN_LAND_INT8;

    entry = semiringhash_insert(semiringhash, "max_land_int8", &found);
    entry->name = pstrdup("max_land_int8");
    entry->semiring = GxB_MAX_LAND_INT8;

    entry = semiringhash_insert(semiringhash, "plus_land_int8", &found);
    entry->name = pstrdup("plus_land_int8");
    entry->semiring = GxB_PLUS_LAND_INT8;

    entry = semiringhash_insert(semiringhash, "times_land_int8", &found);
    entry->name = pstrdup("times_land_int8");
    entry->semiring = GxB_TIMES_LAND_INT8;

    entry = semiringhash_insert(semiringhash, "any_land_int8", &found);
    entry->name = pstrdup("any_land_int8");
    entry->semiring = GxB_ANY_LAND_INT8;

    entry = semiringhash_insert(semiringhash, "min_land_int16", &found);
    entry->name = pstrdup("min_land_int16");
    entry->semiring = GxB_MIN_LAND_INT16;

    entry = semiringhash_insert(semiringhash, "max_land_int16", &found);
    entry->name = pstrdup("max_land_int16");
    entry->semiring = GxB_MAX_LAND_INT16;

    entry = semiringhash_insert(semiringhash, "plus_land_int16", &found);
    entry->name = pstrdup("plus_land_int16");
    entry->semiring = GxB_PLUS_LAND_INT16;

    entry = semiringhash_insert(semiringhash, "times_land_int16", &found);
    entry->name = pstrdup("times_land_int16");
    entry->semiring = GxB_TIMES_LAND_INT16;

    entry = semiringhash_insert(semiringhash, "any_land_int16", &found);
    entry->name = pstrdup("any_land_int16");
    entry->semiring = GxB_ANY_LAND_INT16;

    entry = semiringhash_insert(semiringhash, "min_land_int32", &found);
    entry->name = pstrdup("min_land_int32");
    entry->semiring = GxB_MIN_LAND_INT32;

    entry = semiringhash_insert(semiringhash, "max_land_int32", &found);
    entry->name = pstrdup("max_land_int32");
    entry->semiring = GxB_MAX_LAND_INT32;

    entry = semiringhash_insert(semiringhash, "plus_land_int32", &found);
    entry->name = pstrdup("plus_land_int32");
    entry->semiring = GxB_PLUS_LAND_INT32;

    entry = semiringhash_insert(semiringhash, "times_land_int32", &found);
    entry->name = pstrdup("times_land_int32");
    entry->semiring = GxB_TIMES_LAND_INT32;

    entry = semiringhash_insert(semiringhash, "any_land_int32", &found);
    entry->name = pstrdup("any_land_int32");
    entry->semiring = GxB_ANY_LAND_INT32;

    entry = semiringhash_insert(semiringhash, "min_land_int64", &found);
    entry->name = pstrdup("min_land_int64");
    entry->semiring = GxB_MIN_LAND_INT64;

    entry = semiringhash_insert(semiringhash, "max_land_int64", &found);
    entry->name = pstrdup("max_land_int64");
    entry->semiring = GxB_MAX_LAND_INT64;

    entry = semiringhash_insert(semiringhash, "plus_land_int64", &found);
    entry->name = pstrdup("plus_land_int64");
    entry->semiring = GxB_PLUS_LAND_INT64;

    entry = semiringhash_insert(semiringhash, "times_land_int64", &found);
    entry->name = pstrdup("times_land_int64");
    entry->semiring = GxB_TIMES_LAND_INT64;

    entry = semiringhash_insert(semiringhash, "any_land_int64", &found);
    entry->name = pstrdup("any_land_int64");
    entry->semiring = GxB_ANY_LAND_INT64;

    entry = semiringhash_insert(semiringhash, "min_land_uint8", &found);
    entry->name = pstrdup("min_land_uint8");
    entry->semiring = GxB_MIN_LAND_UINT8;

    entry = semiringhash_insert(semiringhash, "max_land_uint8", &found);
    entry->name = pstrdup("max_land_uint8");
    entry->semiring = GxB_MAX_LAND_UINT8;

    entry = semiringhash_insert(semiringhash, "plus_land_uint8", &found);
    entry->name = pstrdup("plus_land_uint8");
    entry->semiring = GxB_PLUS_LAND_UINT8;

    entry = semiringhash_insert(semiringhash, "times_land_uint8", &found);
    entry->name = pstrdup("times_land_uint8");
    entry->semiring = GxB_TIMES_LAND_UINT8;

    entry = semiringhash_insert(semiringhash, "any_land_uint8", &found);
    entry->name = pstrdup("any_land_uint8");
    entry->semiring = GxB_ANY_LAND_UINT8;

    entry = semiringhash_insert(semiringhash, "min_land_uint16", &found);
    entry->name = pstrdup("min_land_uint16");
    entry->semiring = GxB_MIN_LAND_UINT16;

    entry = semiringhash_insert(semiringhash, "max_land_uint16", &found);
    entry->name = pstrdup("max_land_uint16");
    entry->semiring = GxB_MAX_LAND_UINT16;

    entry = semiringhash_insert(semiringhash, "plus_land_uint16", &found);
    entry->name = pstrdup("plus_land_uint16");
    entry->semiring = GxB_PLUS_LAND_UINT16;

    entry = semiringhash_insert(semiringhash, "times_land_uint16", &found);
    entry->name = pstrdup("times_land_uint16");
    entry->semiring = GxB_TIMES_LAND_UINT16;

    entry = semiringhash_insert(semiringhash, "any_land_uint16", &found);
    entry->name = pstrdup("any_land_uint16");
    entry->semiring = GxB_ANY_LAND_UINT16;

    entry = semiringhash_insert(semiringhash, "min_land_uint32", &found);
    entry->name = pstrdup("min_land_uint32");
    entry->semiring = GxB_MIN_LAND_UINT32;

    entry = semiringhash_insert(semiringhash, "max_land_uint32", &found);
    entry->name = pstrdup("max_land_uint32");
    entry->semiring = GxB_MAX_LAND_UINT32;

    entry = semiringhash_insert(semiringhash, "plus_land_uint32", &found);
    entry->name = pstrdup("plus_land_uint32");
    entry->semiring = GxB_PLUS_LAND_UINT32;

    entry = semiringhash_insert(semiringhash, "times_land_uint32", &found);
    entry->name = pstrdup("times_land_uint32");
    entry->semiring = GxB_TIMES_LAND_UINT32;

    entry = semiringhash_insert(semiringhash, "any_land_uint32", &found);
    entry->name = pstrdup("any_land_uint32");
    entry->semiring = GxB_ANY_LAND_UINT32;

    entry = semiringhash_insert(semiringhash, "min_land_uint64", &found);
    entry->name = pstrdup("min_land_uint64");
    entry->semiring = GxB_MIN_LAND_UINT64;

    entry = semiringhash_insert(semiringhash, "max_land_uint64", &found);
    entry->name = pstrdup("max_land_uint64");
    entry->semiring = GxB_MAX_LAND_UINT64;

    entry = semiringhash_insert(semiringhash, "plus_land_uint64", &found);
    entry->name = pstrdup("plus_land_uint64");
    entry->semiring = GxB_PLUS_LAND_UINT64;

    entry = semiringhash_insert(semiringhash, "times_land_uint64", &found);
    entry->name = pstrdup("times_land_uint64");
    entry->semiring = GxB_TIMES_LAND_UINT64;

    entry = semiringhash_insert(semiringhash, "any_land_uint64", &found);
    entry->name = pstrdup("any_land_uint64");
    entry->semiring = GxB_ANY_LAND_UINT64;

    entry = semiringhash_insert(semiringhash, "min_land_fp32", &found);
    entry->name = pstrdup("min_land_fp32");
    entry->semiring = GxB_MIN_LAND_FP32;

    entry = semiringhash_insert(semiringhash, "max_land_fp32", &found);
    entry->name = pstrdup("max_land_fp32");
    entry->semiring = GxB_MAX_LAND_FP32;

    entry = semiringhash_insert(semiringhash, "plus_land_fp32", &found);
    entry->name = pstrdup("plus_land_fp32");
    entry->semiring = GxB_PLUS_LAND_FP32;

    entry = semiringhash_insert(semiringhash, "times_land_fp32", &found);
    entry->name = pstrdup("times_land_fp32");
    entry->semiring = GxB_TIMES_LAND_FP32;

    entry = semiringhash_insert(semiringhash, "any_land_fp32", &found);
    entry->name = pstrdup("any_land_fp32");
    entry->semiring = GxB_ANY_LAND_FP32;

    entry = semiringhash_insert(semiringhash, "min_land_fp64", &found);
    entry->name = pstrdup("min_land_fp64");
    entry->semiring = GxB_MIN_LAND_FP64;

    entry = semiringhash_insert(semiringhash, "max_land_fp64", &found);
    entry->name = pstrdup("max_land_fp64");
    entry->semiring = GxB_MAX_LAND_FP64;

    entry = semiringhash_insert(semiringhash, "plus_land_fp64", &found);
    entry->name = pstrdup("plus_land_fp64");
    entry->semiring = GxB_PLUS_LAND_FP64;

    entry = semiringhash_insert(semiringhash, "times_land_fp64", &found);
    entry->name = pstrdup("times_land_fp64");
    entry->semiring = GxB_TIMES_LAND_FP64;

    entry = semiringhash_insert(semiringhash, "any_land_fp64", &found);
    entry->name = pstrdup("any_land_fp64");
    entry->semiring = GxB_ANY_LAND_FP64;

    entry = semiringhash_insert(semiringhash, "min_lxor_int8", &found);
    entry->name = pstrdup("min_lxor_int8");
    entry->semiring = GxB_MIN_LXOR_INT8;

    entry = semiringhash_insert(semiringhash, "max_lxor_int8", &found);
    entry->name = pstrdup("max_lxor_int8");
    entry->semiring = GxB_MAX_LXOR_INT8;

    entry = semiringhash_insert(semiringhash, "plus_lxor_int8", &found);
    entry->name = pstrdup("plus_lxor_int8");
    entry->semiring = GxB_PLUS_LXOR_INT8;

    entry = semiringhash_insert(semiringhash, "times_lxor_int8", &found);
    entry->name = pstrdup("times_lxor_int8");
    entry->semiring = GxB_TIMES_LXOR_INT8;

    entry = semiringhash_insert(semiringhash, "any_lxor_int8", &found);
    entry->name = pstrdup("any_lxor_int8");
    entry->semiring = GxB_ANY_LXOR_INT8;

    entry = semiringhash_insert(semiringhash, "min_lxor_int16", &found);
    entry->name = pstrdup("min_lxor_int16");
    entry->semiring = GxB_MIN_LXOR_INT16;

    entry = semiringhash_insert(semiringhash, "max_lxor_int16", &found);
    entry->name = pstrdup("max_lxor_int16");
    entry->semiring = GxB_MAX_LXOR_INT16;

    entry = semiringhash_insert(semiringhash, "plus_lxor_int16", &found);
    entry->name = pstrdup("plus_lxor_int16");
    entry->semiring = GxB_PLUS_LXOR_INT16;

    entry = semiringhash_insert(semiringhash, "times_lxor_int16", &found);
    entry->name = pstrdup("times_lxor_int16");
    entry->semiring = GxB_TIMES_LXOR_INT16;

    entry = semiringhash_insert(semiringhash, "any_lxor_int16", &found);
    entry->name = pstrdup("any_lxor_int16");
    entry->semiring = GxB_ANY_LXOR_INT16;

    entry = semiringhash_insert(semiringhash, "min_lxor_int32", &found);
    entry->name = pstrdup("min_lxor_int32");
    entry->semiring = GxB_MIN_LXOR_INT32;

    entry = semiringhash_insert(semiringhash, "max_lxor_int32", &found);
    entry->name = pstrdup("max_lxor_int32");
    entry->semiring = GxB_MAX_LXOR_INT32;

    entry = semiringhash_insert(semiringhash, "plus_lxor_int32", &found);
    entry->name = pstrdup("plus_lxor_int32");
    entry->semiring = GxB_PLUS_LXOR_INT32;

    entry = semiringhash_insert(semiringhash, "times_lxor_int32", &found);
    entry->name = pstrdup("times_lxor_int32");
    entry->semiring = GxB_TIMES_LXOR_INT32;

    entry = semiringhash_insert(semiringhash, "any_lxor_int32", &found);
    entry->name = pstrdup("any_lxor_int32");
    entry->semiring = GxB_ANY_LXOR_INT32;

    entry = semiringhash_insert(semiringhash, "min_lxor_int64", &found);
    entry->name = pstrdup("min_lxor_int64");
    entry->semiring = GxB_MIN_LXOR_INT64;

    entry = semiringhash_insert(semiringhash, "max_lxor_int64", &found);
    entry->name = pstrdup("max_lxor_int64");
    entry->semiring = GxB_MAX_LXOR_INT64;

    entry = semiringhash_insert(semiringhash, "plus_lxor_int64", &found);
    entry->name = pstrdup("plus_lxor_int64");
    entry->semiring = GxB_PLUS_LXOR_INT64;

    entry = semiringhash_insert(semiringhash, "times_lxor_int64", &found);
    entry->name = pstrdup("times_lxor_int64");
    entry->semiring = GxB_TIMES_LXOR_INT64;

    entry = semiringhash_insert(semiringhash, "any_lxor_int64", &found);
    entry->name = pstrdup("any_lxor_int64");
    entry->semiring = GxB_ANY_LXOR_INT64;

    entry = semiringhash_insert(semiringhash, "min_lxor_uint8", &found);
    entry->name = pstrdup("min_lxor_uint8");
    entry->semiring = GxB_MIN_LXOR_UINT8;

    entry = semiringhash_insert(semiringhash, "max_lxor_uint8", &found);
    entry->name = pstrdup("max_lxor_uint8");
    entry->semiring = GxB_MAX_LXOR_UINT8;

    entry = semiringhash_insert(semiringhash, "plus_lxor_uint8", &found);
    entry->name = pstrdup("plus_lxor_uint8");
    entry->semiring = GxB_PLUS_LXOR_UINT8;

    entry = semiringhash_insert(semiringhash, "times_lxor_uint8", &found);
    entry->name = pstrdup("times_lxor_uint8");
    entry->semiring = GxB_TIMES_LXOR_UINT8;

    entry = semiringhash_insert(semiringhash, "any_lxor_uint8", &found);
    entry->name = pstrdup("any_lxor_uint8");
    entry->semiring = GxB_ANY_LXOR_UINT8;

    entry = semiringhash_insert(semiringhash, "min_lxor_uint16", &found);
    entry->name = pstrdup("min_lxor_uint16");
    entry->semiring = GxB_MIN_LXOR_UINT16;

    entry = semiringhash_insert(semiringhash, "max_lxor_uint16", &found);
    entry->name = pstrdup("max_lxor_uint16");
    entry->semiring = GxB_MAX_LXOR_UINT16;

    entry = semiringhash_insert(semiringhash, "plus_lxor_uint16", &found);
    entry->name = pstrdup("plus_lxor_uint16");
    entry->semiring = GxB_PLUS_LXOR_UINT16;

    entry = semiringhash_insert(semiringhash, "times_lxor_uint16", &found);
    entry->name = pstrdup("times_lxor_uint16");
    entry->semiring = GxB_TIMES_LXOR_UINT16;

    entry = semiringhash_insert(semiringhash, "any_lxor_uint16", &found);
    entry->name = pstrdup("any_lxor_uint16");
    entry->semiring = GxB_ANY_LXOR_UINT16;

    entry = semiringhash_insert(semiringhash, "min_lxor_uint32", &found);
    entry->name = pstrdup("min_lxor_uint32");
    entry->semiring = GxB_MIN_LXOR_UINT32;

    entry = semiringhash_insert(semiringhash, "max_lxor_uint32", &found);
    entry->name = pstrdup("max_lxor_uint32");
    entry->semiring = GxB_MAX_LXOR_UINT32;

    entry = semiringhash_insert(semiringhash, "plus_lxor_uint32", &found);
    entry->name = pstrdup("plus_lxor_uint32");
    entry->semiring = GxB_PLUS_LXOR_UINT32;

    entry = semiringhash_insert(semiringhash, "times_lxor_uint32", &found);
    entry->name = pstrdup("times_lxor_uint32");
    entry->semiring = GxB_TIMES_LXOR_UINT32;

    entry = semiringhash_insert(semiringhash, "any_lxor_uint32", &found);
    entry->name = pstrdup("any_lxor_uint32");
    entry->semiring = GxB_ANY_LXOR_UINT32;

    entry = semiringhash_insert(semiringhash, "min_lxor_uint64", &found);
    entry->name = pstrdup("min_lxor_uint64");
    entry->semiring = GxB_MIN_LXOR_UINT64;

    entry = semiringhash_insert(semiringhash, "max_lxor_uint64", &found);
    entry->name = pstrdup("max_lxor_uint64");
    entry->semiring = GxB_MAX_LXOR_UINT64;

    entry = semiringhash_insert(semiringhash, "plus_lxor_uint64", &found);
    entry->name = pstrdup("plus_lxor_uint64");
    entry->semiring = GxB_PLUS_LXOR_UINT64;

    entry = semiringhash_insert(semiringhash, "times_lxor_uint64", &found);
    entry->name = pstrdup("times_lxor_uint64");
    entry->semiring = GxB_TIMES_LXOR_UINT64;

    entry = semiringhash_insert(semiringhash, "any_lxor_uint64", &found);
    entry->name = pstrdup("any_lxor_uint64");
    entry->semiring = GxB_ANY_LXOR_UINT64;

    entry = semiringhash_insert(semiringhash, "min_lxor_fp32", &found);
    entry->name = pstrdup("min_lxor_fp32");
    entry->semiring = GxB_MIN_LXOR_FP32;

    entry = semiringhash_insert(semiringhash, "max_lxor_fp32", &found);
    entry->name = pstrdup("max_lxor_fp32");
    entry->semiring = GxB_MAX_LXOR_FP32;

    entry = semiringhash_insert(semiringhash, "plus_lxor_fp32", &found);
    entry->name = pstrdup("plus_lxor_fp32");
    entry->semiring = GxB_PLUS_LXOR_FP32;

    entry = semiringhash_insert(semiringhash, "times_lxor_fp32", &found);
    entry->name = pstrdup("times_lxor_fp32");
    entry->semiring = GxB_TIMES_LXOR_FP32;

    entry = semiringhash_insert(semiringhash, "any_lxor_fp32", &found);
    entry->name = pstrdup("any_lxor_fp32");
    entry->semiring = GxB_ANY_LXOR_FP32;

    entry = semiringhash_insert(semiringhash, "min_lxor_fp64", &found);
    entry->name = pstrdup("min_lxor_fp64");
    entry->semiring = GxB_MIN_LXOR_FP64;

    entry = semiringhash_insert(semiringhash, "max_lxor_fp64", &found);
    entry->name = pstrdup("max_lxor_fp64");
    entry->semiring = GxB_MAX_LXOR_FP64;

    entry = semiringhash_insert(semiringhash, "plus_lxor_fp64", &found);
    entry->name = pstrdup("plus_lxor_fp64");
    entry->semiring = GxB_PLUS_LXOR_FP64;

    entry = semiringhash_insert(semiringhash, "times_lxor_fp64", &found);
    entry->name = pstrdup("times_lxor_fp64");
    entry->semiring = GxB_TIMES_LXOR_FP64;

    entry = semiringhash_insert(semiringhash, "any_lxor_fp64", &found);
    entry->name = pstrdup("any_lxor_fp64");
    entry->semiring = GxB_ANY_LXOR_FP64;

    entry = semiringhash_insert(semiringhash, "lor_eq_int8", &found);
    entry->name = pstrdup("lor_eq_int8");
    entry->semiring = GxB_LOR_EQ_INT8;

    entry = semiringhash_insert(semiringhash, "land_eq_int8", &found);
    entry->name = pstrdup("land_eq_int8");
    entry->semiring = GxB_LAND_EQ_INT8;

    entry = semiringhash_insert(semiringhash, "lxor_eq_int8", &found);
    entry->name = pstrdup("lxor_eq_int8");
    entry->semiring = GxB_LXOR_EQ_INT8;

    entry = semiringhash_insert(semiringhash, "eq_eq_int8", &found);
    entry->name = pstrdup("eq_eq_int8");
    entry->semiring = GxB_EQ_EQ_INT8;

    entry = semiringhash_insert(semiringhash, "any_eq_int8", &found);
    entry->name = pstrdup("any_eq_int8");
    entry->semiring = GxB_ANY_EQ_INT8;

    entry = semiringhash_insert(semiringhash, "lor_eq_int16", &found);
    entry->name = pstrdup("lor_eq_int16");
    entry->semiring = GxB_LOR_EQ_INT16;

    entry = semiringhash_insert(semiringhash, "land_eq_int16", &found);
    entry->name = pstrdup("land_eq_int16");
    entry->semiring = GxB_LAND_EQ_INT16;

    entry = semiringhash_insert(semiringhash, "lxor_eq_int16", &found);
    entry->name = pstrdup("lxor_eq_int16");
    entry->semiring = GxB_LXOR_EQ_INT16;

    entry = semiringhash_insert(semiringhash, "eq_eq_int16", &found);
    entry->name = pstrdup("eq_eq_int16");
    entry->semiring = GxB_EQ_EQ_INT16;

    entry = semiringhash_insert(semiringhash, "any_eq_int16", &found);
    entry->name = pstrdup("any_eq_int16");
    entry->semiring = GxB_ANY_EQ_INT16;

    entry = semiringhash_insert(semiringhash, "lor_eq_int32", &found);
    entry->name = pstrdup("lor_eq_int32");
    entry->semiring = GxB_LOR_EQ_INT32;

    entry = semiringhash_insert(semiringhash, "land_eq_int32", &found);
    entry->name = pstrdup("land_eq_int32");
    entry->semiring = GxB_LAND_EQ_INT32;

    entry = semiringhash_insert(semiringhash, "lxor_eq_int32", &found);
    entry->name = pstrdup("lxor_eq_int32");
    entry->semiring = GxB_LXOR_EQ_INT32;

    entry = semiringhash_insert(semiringhash, "eq_eq_int32", &found);
    entry->name = pstrdup("eq_eq_int32");
    entry->semiring = GxB_EQ_EQ_INT32;

    entry = semiringhash_insert(semiringhash, "any_eq_int32", &found);
    entry->name = pstrdup("any_eq_int32");
    entry->semiring = GxB_ANY_EQ_INT32;

    entry = semiringhash_insert(semiringhash, "lor_eq_int64", &found);
    entry->name = pstrdup("lor_eq_int64");
    entry->semiring = GxB_LOR_EQ_INT64;

    entry = semiringhash_insert(semiringhash, "land_eq_int64", &found);
    entry->name = pstrdup("land_eq_int64");
    entry->semiring = GxB_LAND_EQ_INT64;

    entry = semiringhash_insert(semiringhash, "lxor_eq_int64", &found);
    entry->name = pstrdup("lxor_eq_int64");
    entry->semiring = GxB_LXOR_EQ_INT64;

    entry = semiringhash_insert(semiringhash, "eq_eq_int64", &found);
    entry->name = pstrdup("eq_eq_int64");
    entry->semiring = GxB_EQ_EQ_INT64;

    entry = semiringhash_insert(semiringhash, "any_eq_int64", &found);
    entry->name = pstrdup("any_eq_int64");
    entry->semiring = GxB_ANY_EQ_INT64;

    entry = semiringhash_insert(semiringhash, "lor_eq_uint8", &found);
    entry->name = pstrdup("lor_eq_uint8");
    entry->semiring = GxB_LOR_EQ_UINT8;

    entry = semiringhash_insert(semiringhash, "land_eq_uint8", &found);
    entry->name = pstrdup("land_eq_uint8");
    entry->semiring = GxB_LAND_EQ_UINT8;

    entry = semiringhash_insert(semiringhash, "lxor_eq_uint8", &found);
    entry->name = pstrdup("lxor_eq_uint8");
    entry->semiring = GxB_LXOR_EQ_UINT8;

    entry = semiringhash_insert(semiringhash, "eq_eq_uint8", &found);
    entry->name = pstrdup("eq_eq_uint8");
    entry->semiring = GxB_EQ_EQ_UINT8;

    entry = semiringhash_insert(semiringhash, "any_eq_uint8", &found);
    entry->name = pstrdup("any_eq_uint8");
    entry->semiring = GxB_ANY_EQ_UINT8;

    entry = semiringhash_insert(semiringhash, "lor_eq_uint16", &found);
    entry->name = pstrdup("lor_eq_uint16");
    entry->semiring = GxB_LOR_EQ_UINT16;

    entry = semiringhash_insert(semiringhash, "land_eq_uint16", &found);
    entry->name = pstrdup("land_eq_uint16");
    entry->semiring = GxB_LAND_EQ_UINT16;

    entry = semiringhash_insert(semiringhash, "lxor_eq_uint16", &found);
    entry->name = pstrdup("lxor_eq_uint16");
    entry->semiring = GxB_LXOR_EQ_UINT16;

    entry = semiringhash_insert(semiringhash, "eq_eq_uint16", &found);
    entry->name = pstrdup("eq_eq_uint16");
    entry->semiring = GxB_EQ_EQ_UINT16;

    entry = semiringhash_insert(semiringhash, "any_eq_uint16", &found);
    entry->name = pstrdup("any_eq_uint16");
    entry->semiring = GxB_ANY_EQ_UINT16;

    entry = semiringhash_insert(semiringhash, "lor_eq_uint32", &found);
    entry->name = pstrdup("lor_eq_uint32");
    entry->semiring = GxB_LOR_EQ_UINT32;

    entry = semiringhash_insert(semiringhash, "land_eq_uint32", &found);
    entry->name = pstrdup("land_eq_uint32");
    entry->semiring = GxB_LAND_EQ_UINT32;

    entry = semiringhash_insert(semiringhash, "lxor_eq_uint32", &found);
    entry->name = pstrdup("lxor_eq_uint32");
    entry->semiring = GxB_LXOR_EQ_UINT32;

    entry = semiringhash_insert(semiringhash, "eq_eq_uint32", &found);
    entry->name = pstrdup("eq_eq_uint32");
    entry->semiring = GxB_EQ_EQ_UINT32;

    entry = semiringhash_insert(semiringhash, "any_eq_uint32", &found);
    entry->name = pstrdup("any_eq_uint32");
    entry->semiring = GxB_ANY_EQ_UINT32;

    entry = semiringhash_insert(semiringhash, "lor_eq_uint64", &found);
    entry->name = pstrdup("lor_eq_uint64");
    entry->semiring = GxB_LOR_EQ_UINT64;

    entry = semiringhash_insert(semiringhash, "land_eq_uint64", &found);
    entry->name = pstrdup("land_eq_uint64");
    entry->semiring = GxB_LAND_EQ_UINT64;

    entry = semiringhash_insert(semiringhash, "lxor_eq_uint64", &found);
    entry->name = pstrdup("lxor_eq_uint64");
    entry->semiring = GxB_LXOR_EQ_UINT64;

    entry = semiringhash_insert(semiringhash, "eq_eq_uint64", &found);
    entry->name = pstrdup("eq_eq_uint64");
    entry->semiring = GxB_EQ_EQ_UINT64;

    entry = semiringhash_insert(semiringhash, "any_eq_uint64", &found);
    entry->name = pstrdup("any_eq_uint64");
    entry->semiring = GxB_ANY_EQ_UINT64;

    entry = semiringhash_insert(semiringhash, "lor_eq_fp32", &found);
    entry->name = pstrdup("lor_eq_fp32");
    entry->semiring = GxB_LOR_EQ_FP32;

    entry = semiringhash_insert(semiringhash, "land_eq_fp32", &found);
    entry->name = pstrdup("land_eq_fp32");
    entry->semiring = GxB_LAND_EQ_FP32;

    entry = semiringhash_insert(semiringhash, "lxor_eq_fp32", &found);
    entry->name = pstrdup("lxor_eq_fp32");
    entry->semiring = GxB_LXOR_EQ_FP32;

    entry = semiringhash_insert(semiringhash, "eq_eq_fp32", &found);
    entry->name = pstrdup("eq_eq_fp32");
    entry->semiring = GxB_EQ_EQ_FP32;

    entry = semiringhash_insert(semiringhash, "any_eq_fp32", &found);
    entry->name = pstrdup("any_eq_fp32");
    entry->semiring = GxB_ANY_EQ_FP32;

    entry = semiringhash_insert(semiringhash, "lor_eq_fp64", &found);
    entry->name = pstrdup("lor_eq_fp64");
    entry->semiring = GxB_LOR_EQ_FP64;

    entry = semiringhash_insert(semiringhash, "land_eq_fp64", &found);
    entry->name = pstrdup("land_eq_fp64");
    entry->semiring = GxB_LAND_EQ_FP64;

    entry = semiringhash_insert(semiringhash, "lxor_eq_fp64", &found);
    entry->name = pstrdup("lxor_eq_fp64");
    entry->semiring = GxB_LXOR_EQ_FP64;

    entry = semiringhash_insert(semiringhash, "eq_eq_fp64", &found);
    entry->name = pstrdup("eq_eq_fp64");
    entry->semiring = GxB_EQ_EQ_FP64;

    entry = semiringhash_insert(semiringhash, "any_eq_fp64", &found);
    entry->name = pstrdup("any_eq_fp64");
    entry->semiring = GxB_ANY_EQ_FP64;

    entry = semiringhash_insert(semiringhash, "lor_ne_int8", &found);
    entry->name = pstrdup("lor_ne_int8");
    entry->semiring = GxB_LOR_NE_INT8;

    entry = semiringhash_insert(semiringhash, "land_ne_int8", &found);
    entry->name = pstrdup("land_ne_int8");
    entry->semiring = GxB_LAND_NE_INT8;

    entry = semiringhash_insert(semiringhash, "lxor_ne_int8", &found);
    entry->name = pstrdup("lxor_ne_int8");
    entry->semiring = GxB_LXOR_NE_INT8;

    entry = semiringhash_insert(semiringhash, "eq_ne_int8", &found);
    entry->name = pstrdup("eq_ne_int8");
    entry->semiring = GxB_EQ_NE_INT8;

    entry = semiringhash_insert(semiringhash, "any_ne_int8", &found);
    entry->name = pstrdup("any_ne_int8");
    entry->semiring = GxB_ANY_NE_INT8;

    entry = semiringhash_insert(semiringhash, "lor_ne_int16", &found);
    entry->name = pstrdup("lor_ne_int16");
    entry->semiring = GxB_LOR_NE_INT16;

    entry = semiringhash_insert(semiringhash, "land_ne_int16", &found);
    entry->name = pstrdup("land_ne_int16");
    entry->semiring = GxB_LAND_NE_INT16;

    entry = semiringhash_insert(semiringhash, "lxor_ne_int16", &found);
    entry->name = pstrdup("lxor_ne_int16");
    entry->semiring = GxB_LXOR_NE_INT16;

    entry = semiringhash_insert(semiringhash, "eq_ne_int16", &found);
    entry->name = pstrdup("eq_ne_int16");
    entry->semiring = GxB_EQ_NE_INT16;

    entry = semiringhash_insert(semiringhash, "any_ne_int16", &found);
    entry->name = pstrdup("any_ne_int16");
    entry->semiring = GxB_ANY_NE_INT16;

    entry = semiringhash_insert(semiringhash, "lor_ne_int32", &found);
    entry->name = pstrdup("lor_ne_int32");
    entry->semiring = GxB_LOR_NE_INT32;

    entry = semiringhash_insert(semiringhash, "land_ne_int32", &found);
    entry->name = pstrdup("land_ne_int32");
    entry->semiring = GxB_LAND_NE_INT32;

    entry = semiringhash_insert(semiringhash, "lxor_ne_int32", &found);
    entry->name = pstrdup("lxor_ne_int32");
    entry->semiring = GxB_LXOR_NE_INT32;

    entry = semiringhash_insert(semiringhash, "eq_ne_int32", &found);
    entry->name = pstrdup("eq_ne_int32");
    entry->semiring = GxB_EQ_NE_INT32;

    entry = semiringhash_insert(semiringhash, "any_ne_int32", &found);
    entry->name = pstrdup("any_ne_int32");
    entry->semiring = GxB_ANY_NE_INT32;

    entry = semiringhash_insert(semiringhash, "lor_ne_int64", &found);
    entry->name = pstrdup("lor_ne_int64");
    entry->semiring = GxB_LOR_NE_INT64;

    entry = semiringhash_insert(semiringhash, "land_ne_int64", &found);
    entry->name = pstrdup("land_ne_int64");
    entry->semiring = GxB_LAND_NE_INT64;

    entry = semiringhash_insert(semiringhash, "lxor_ne_int64", &found);
    entry->name = pstrdup("lxor_ne_int64");
    entry->semiring = GxB_LXOR_NE_INT64;

    entry = semiringhash_insert(semiringhash, "eq_ne_int64", &found);
    entry->name = pstrdup("eq_ne_int64");
    entry->semiring = GxB_EQ_NE_INT64;

    entry = semiringhash_insert(semiringhash, "any_ne_int64", &found);
    entry->name = pstrdup("any_ne_int64");
    entry->semiring = GxB_ANY_NE_INT64;

    entry = semiringhash_insert(semiringhash, "lor_ne_uint8", &found);
    entry->name = pstrdup("lor_ne_uint8");
    entry->semiring = GxB_LOR_NE_UINT8;

    entry = semiringhash_insert(semiringhash, "land_ne_uint8", &found);
    entry->name = pstrdup("land_ne_uint8");
    entry->semiring = GxB_LAND_NE_UINT8;

    entry = semiringhash_insert(semiringhash, "lxor_ne_uint8", &found);
    entry->name = pstrdup("lxor_ne_uint8");
    entry->semiring = GxB_LXOR_NE_UINT8;

    entry = semiringhash_insert(semiringhash, "eq_ne_uint8", &found);
    entry->name = pstrdup("eq_ne_uint8");
    entry->semiring = GxB_EQ_NE_UINT8;

    entry = semiringhash_insert(semiringhash, "any_ne_uint8", &found);
    entry->name = pstrdup("any_ne_uint8");
    entry->semiring = GxB_ANY_NE_UINT8;

    entry = semiringhash_insert(semiringhash, "lor_ne_uint16", &found);
    entry->name = pstrdup("lor_ne_uint16");
    entry->semiring = GxB_LOR_NE_UINT16;

    entry = semiringhash_insert(semiringhash, "land_ne_uint16", &found);
    entry->name = pstrdup("land_ne_uint16");
    entry->semiring = GxB_LAND_NE_UINT16;

    entry = semiringhash_insert(semiringhash, "lxor_ne_uint16", &found);
    entry->name = pstrdup("lxor_ne_uint16");
    entry->semiring = GxB_LXOR_NE_UINT16;

    entry = semiringhash_insert(semiringhash, "eq_ne_uint16", &found);
    entry->name = pstrdup("eq_ne_uint16");
    entry->semiring = GxB_EQ_NE_UINT16;

    entry = semiringhash_insert(semiringhash, "any_ne_uint16", &found);
    entry->name = pstrdup("any_ne_uint16");
    entry->semiring = GxB_ANY_NE_UINT16;

    entry = semiringhash_insert(semiringhash, "lor_ne_uint32", &found);
    entry->name = pstrdup("lor_ne_uint32");
    entry->semiring = GxB_LOR_NE_UINT32;

    entry = semiringhash_insert(semiringhash, "land_ne_uint32", &found);
    entry->name = pstrdup("land_ne_uint32");
    entry->semiring = GxB_LAND_NE_UINT32;

    entry = semiringhash_insert(semiringhash, "lxor_ne_uint32", &found);
    entry->name = pstrdup("lxor_ne_uint32");
    entry->semiring = GxB_LXOR_NE_UINT32;

    entry = semiringhash_insert(semiringhash, "eq_ne_uint32", &found);
    entry->name = pstrdup("eq_ne_uint32");
    entry->semiring = GxB_EQ_NE_UINT32;

    entry = semiringhash_insert(semiringhash, "any_ne_uint32", &found);
    entry->name = pstrdup("any_ne_uint32");
    entry->semiring = GxB_ANY_NE_UINT32;

    entry = semiringhash_insert(semiringhash, "lor_ne_uint64", &found);
    entry->name = pstrdup("lor_ne_uint64");
    entry->semiring = GxB_LOR_NE_UINT64;

    entry = semiringhash_insert(semiringhash, "land_ne_uint64", &found);
    entry->name = pstrdup("land_ne_uint64");
    entry->semiring = GxB_LAND_NE_UINT64;

    entry = semiringhash_insert(semiringhash, "lxor_ne_uint64", &found);
    entry->name = pstrdup("lxor_ne_uint64");
    entry->semiring = GxB_LXOR_NE_UINT64;

    entry = semiringhash_insert(semiringhash, "eq_ne_uint64", &found);
    entry->name = pstrdup("eq_ne_uint64");
    entry->semiring = GxB_EQ_NE_UINT64;

    entry = semiringhash_insert(semiringhash, "any_ne_uint64", &found);
    entry->name = pstrdup("any_ne_uint64");
    entry->semiring = GxB_ANY_NE_UINT64;

    entry = semiringhash_insert(semiringhash, "lor_ne_fp32", &found);
    entry->name = pstrdup("lor_ne_fp32");
    entry->semiring = GxB_LOR_NE_FP32;

    entry = semiringhash_insert(semiringhash, "land_ne_fp32", &found);
    entry->name = pstrdup("land_ne_fp32");
    entry->semiring = GxB_LAND_NE_FP32;

    entry = semiringhash_insert(semiringhash, "lxor_ne_fp32", &found);
    entry->name = pstrdup("lxor_ne_fp32");
    entry->semiring = GxB_LXOR_NE_FP32;

    entry = semiringhash_insert(semiringhash, "eq_ne_fp32", &found);
    entry->name = pstrdup("eq_ne_fp32");
    entry->semiring = GxB_EQ_NE_FP32;

    entry = semiringhash_insert(semiringhash, "any_ne_fp32", &found);
    entry->name = pstrdup("any_ne_fp32");
    entry->semiring = GxB_ANY_NE_FP32;

    entry = semiringhash_insert(semiringhash, "lor_ne_fp64", &found);
    entry->name = pstrdup("lor_ne_fp64");
    entry->semiring = GxB_LOR_NE_FP64;

    entry = semiringhash_insert(semiringhash, "land_ne_fp64", &found);
    entry->name = pstrdup("land_ne_fp64");
    entry->semiring = GxB_LAND_NE_FP64;

    entry = semiringhash_insert(semiringhash, "lxor_ne_fp64", &found);
    entry->name = pstrdup("lxor_ne_fp64");
    entry->semiring = GxB_LXOR_NE_FP64;

    entry = semiringhash_insert(semiringhash, "eq_ne_fp64", &found);
    entry->name = pstrdup("eq_ne_fp64");
    entry->semiring = GxB_EQ_NE_FP64;

    entry = semiringhash_insert(semiringhash, "any_ne_fp64", &found);
    entry->name = pstrdup("any_ne_fp64");
    entry->semiring = GxB_ANY_NE_FP64;

    entry = semiringhash_insert(semiringhash, "lor_gt_int8", &found);
    entry->name = pstrdup("lor_gt_int8");
    entry->semiring = GxB_LOR_GT_INT8;

    entry = semiringhash_insert(semiringhash, "land_gt_int8", &found);
    entry->name = pstrdup("land_gt_int8");
    entry->semiring = GxB_LAND_GT_INT8;

    entry = semiringhash_insert(semiringhash, "lxor_gt_int8", &found);
    entry->name = pstrdup("lxor_gt_int8");
    entry->semiring = GxB_LXOR_GT_INT8;

    entry = semiringhash_insert(semiringhash, "eq_gt_int8", &found);
    entry->name = pstrdup("eq_gt_int8");
    entry->semiring = GxB_EQ_GT_INT8;

    entry = semiringhash_insert(semiringhash, "any_gt_int8", &found);
    entry->name = pstrdup("any_gt_int8");
    entry->semiring = GxB_ANY_GT_INT8;

    entry = semiringhash_insert(semiringhash, "lor_gt_int16", &found);
    entry->name = pstrdup("lor_gt_int16");
    entry->semiring = GxB_LOR_GT_INT16;

    entry = semiringhash_insert(semiringhash, "land_gt_int16", &found);
    entry->name = pstrdup("land_gt_int16");
    entry->semiring = GxB_LAND_GT_INT16;

    entry = semiringhash_insert(semiringhash, "lxor_gt_int16", &found);
    entry->name = pstrdup("lxor_gt_int16");
    entry->semiring = GxB_LXOR_GT_INT16;

    entry = semiringhash_insert(semiringhash, "eq_gt_int16", &found);
    entry->name = pstrdup("eq_gt_int16");
    entry->semiring = GxB_EQ_GT_INT16;

    entry = semiringhash_insert(semiringhash, "any_gt_int16", &found);
    entry->name = pstrdup("any_gt_int16");
    entry->semiring = GxB_ANY_GT_INT16;

    entry = semiringhash_insert(semiringhash, "lor_gt_int32", &found);
    entry->name = pstrdup("lor_gt_int32");
    entry->semiring = GxB_LOR_GT_INT32;

    entry = semiringhash_insert(semiringhash, "land_gt_int32", &found);
    entry->name = pstrdup("land_gt_int32");
    entry->semiring = GxB_LAND_GT_INT32;

    entry = semiringhash_insert(semiringhash, "lxor_gt_int32", &found);
    entry->name = pstrdup("lxor_gt_int32");
    entry->semiring = GxB_LXOR_GT_INT32;

    entry = semiringhash_insert(semiringhash, "eq_gt_int32", &found);
    entry->name = pstrdup("eq_gt_int32");
    entry->semiring = GxB_EQ_GT_INT32;

    entry = semiringhash_insert(semiringhash, "any_gt_int32", &found);
    entry->name = pstrdup("any_gt_int32");
    entry->semiring = GxB_ANY_GT_INT32;

    entry = semiringhash_insert(semiringhash, "lor_gt_int64", &found);
    entry->name = pstrdup("lor_gt_int64");
    entry->semiring = GxB_LOR_GT_INT64;

    entry = semiringhash_insert(semiringhash, "land_gt_int64", &found);
    entry->name = pstrdup("land_gt_int64");
    entry->semiring = GxB_LAND_GT_INT64;

    entry = semiringhash_insert(semiringhash, "lxor_gt_int64", &found);
    entry->name = pstrdup("lxor_gt_int64");
    entry->semiring = GxB_LXOR_GT_INT64;

    entry = semiringhash_insert(semiringhash, "eq_gt_int64", &found);
    entry->name = pstrdup("eq_gt_int64");
    entry->semiring = GxB_EQ_GT_INT64;

    entry = semiringhash_insert(semiringhash, "any_gt_int64", &found);
    entry->name = pstrdup("any_gt_int64");
    entry->semiring = GxB_ANY_GT_INT64;

    entry = semiringhash_insert(semiringhash, "lor_gt_uint8", &found);
    entry->name = pstrdup("lor_gt_uint8");
    entry->semiring = GxB_LOR_GT_UINT8;

    entry = semiringhash_insert(semiringhash, "land_gt_uint8", &found);
    entry->name = pstrdup("land_gt_uint8");
    entry->semiring = GxB_LAND_GT_UINT8;

    entry = semiringhash_insert(semiringhash, "lxor_gt_uint8", &found);
    entry->name = pstrdup("lxor_gt_uint8");
    entry->semiring = GxB_LXOR_GT_UINT8;

    entry = semiringhash_insert(semiringhash, "eq_gt_uint8", &found);
    entry->name = pstrdup("eq_gt_uint8");
    entry->semiring = GxB_EQ_GT_UINT8;

    entry = semiringhash_insert(semiringhash, "any_gt_uint8", &found);
    entry->name = pstrdup("any_gt_uint8");
    entry->semiring = GxB_ANY_GT_UINT8;

    entry = semiringhash_insert(semiringhash, "lor_gt_uint16", &found);
    entry->name = pstrdup("lor_gt_uint16");
    entry->semiring = GxB_LOR_GT_UINT16;

    entry = semiringhash_insert(semiringhash, "land_gt_uint16", &found);
    entry->name = pstrdup("land_gt_uint16");
    entry->semiring = GxB_LAND_GT_UINT16;

    entry = semiringhash_insert(semiringhash, "lxor_gt_uint16", &found);
    entry->name = pstrdup("lxor_gt_uint16");
    entry->semiring = GxB_LXOR_GT_UINT16;

    entry = semiringhash_insert(semiringhash, "eq_gt_uint16", &found);
    entry->name = pstrdup("eq_gt_uint16");
    entry->semiring = GxB_EQ_GT_UINT16;

    entry = semiringhash_insert(semiringhash, "any_gt_uint16", &found);
    entry->name = pstrdup("any_gt_uint16");
    entry->semiring = GxB_ANY_GT_UINT16;

    entry = semiringhash_insert(semiringhash, "lor_gt_uint32", &found);
    entry->name = pstrdup("lor_gt_uint32");
    entry->semiring = GxB_LOR_GT_UINT32;

    entry = semiringhash_insert(semiringhash, "land_gt_uint32", &found);
    entry->name = pstrdup("land_gt_uint32");
    entry->semiring = GxB_LAND_GT_UINT32;

    entry = semiringhash_insert(semiringhash, "lxor_gt_uint32", &found);
    entry->name = pstrdup("lxor_gt_uint32");
    entry->semiring = GxB_LXOR_GT_UINT32;

    entry = semiringhash_insert(semiringhash, "eq_gt_uint32", &found);
    entry->name = pstrdup("eq_gt_uint32");
    entry->semiring = GxB_EQ_GT_UINT32;

    entry = semiringhash_insert(semiringhash, "any_gt_uint32", &found);
    entry->name = pstrdup("any_gt_uint32");
    entry->semiring = GxB_ANY_GT_UINT32;

    entry = semiringhash_insert(semiringhash, "lor_gt_uint64", &found);
    entry->name = pstrdup("lor_gt_uint64");
    entry->semiring = GxB_LOR_GT_UINT64;

    entry = semiringhash_insert(semiringhash, "land_gt_uint64", &found);
    entry->name = pstrdup("land_gt_uint64");
    entry->semiring = GxB_LAND_GT_UINT64;

    entry = semiringhash_insert(semiringhash, "lxor_gt_uint64", &found);
    entry->name = pstrdup("lxor_gt_uint64");
    entry->semiring = GxB_LXOR_GT_UINT64;

    entry = semiringhash_insert(semiringhash, "eq_gt_uint64", &found);
    entry->name = pstrdup("eq_gt_uint64");
    entry->semiring = GxB_EQ_GT_UINT64;

    entry = semiringhash_insert(semiringhash, "any_gt_uint64", &found);
    entry->name = pstrdup("any_gt_uint64");
    entry->semiring = GxB_ANY_GT_UINT64;

    entry = semiringhash_insert(semiringhash, "lor_gt_fp32", &found);
    entry->name = pstrdup("lor_gt_fp32");
    entry->semiring = GxB_LOR_GT_FP32;

    entry = semiringhash_insert(semiringhash, "land_gt_fp32", &found);
    entry->name = pstrdup("land_gt_fp32");
    entry->semiring = GxB_LAND_GT_FP32;

    entry = semiringhash_insert(semiringhash, "lxor_gt_fp32", &found);
    entry->name = pstrdup("lxor_gt_fp32");
    entry->semiring = GxB_LXOR_GT_FP32;

    entry = semiringhash_insert(semiringhash, "eq_gt_fp32", &found);
    entry->name = pstrdup("eq_gt_fp32");
    entry->semiring = GxB_EQ_GT_FP32;

    entry = semiringhash_insert(semiringhash, "any_gt_fp32", &found);
    entry->name = pstrdup("any_gt_fp32");
    entry->semiring = GxB_ANY_GT_FP32;

    entry = semiringhash_insert(semiringhash, "lor_gt_fp64", &found);
    entry->name = pstrdup("lor_gt_fp64");
    entry->semiring = GxB_LOR_GT_FP64;

    entry = semiringhash_insert(semiringhash, "land_gt_fp64", &found);
    entry->name = pstrdup("land_gt_fp64");
    entry->semiring = GxB_LAND_GT_FP64;

    entry = semiringhash_insert(semiringhash, "lxor_gt_fp64", &found);
    entry->name = pstrdup("lxor_gt_fp64");
    entry->semiring = GxB_LXOR_GT_FP64;

    entry = semiringhash_insert(semiringhash, "eq_gt_fp64", &found);
    entry->name = pstrdup("eq_gt_fp64");
    entry->semiring = GxB_EQ_GT_FP64;

    entry = semiringhash_insert(semiringhash, "any_gt_fp64", &found);
    entry->name = pstrdup("any_gt_fp64");
    entry->semiring = GxB_ANY_GT_FP64;

    entry = semiringhash_insert(semiringhash, "lor_lt_int8", &found);
    entry->name = pstrdup("lor_lt_int8");
    entry->semiring = GxB_LOR_LT_INT8;

    entry = semiringhash_insert(semiringhash, "land_lt_int8", &found);
    entry->name = pstrdup("land_lt_int8");
    entry->semiring = GxB_LAND_LT_INT8;

    entry = semiringhash_insert(semiringhash, "lxor_lt_int8", &found);
    entry->name = pstrdup("lxor_lt_int8");
    entry->semiring = GxB_LXOR_LT_INT8;

    entry = semiringhash_insert(semiringhash, "eq_lt_int8", &found);
    entry->name = pstrdup("eq_lt_int8");
    entry->semiring = GxB_EQ_LT_INT8;

    entry = semiringhash_insert(semiringhash, "any_lt_int8", &found);
    entry->name = pstrdup("any_lt_int8");
    entry->semiring = GxB_ANY_LT_INT8;

    entry = semiringhash_insert(semiringhash, "lor_lt_int16", &found);
    entry->name = pstrdup("lor_lt_int16");
    entry->semiring = GxB_LOR_LT_INT16;

    entry = semiringhash_insert(semiringhash, "land_lt_int16", &found);
    entry->name = pstrdup("land_lt_int16");
    entry->semiring = GxB_LAND_LT_INT16;

    entry = semiringhash_insert(semiringhash, "lxor_lt_int16", &found);
    entry->name = pstrdup("lxor_lt_int16");
    entry->semiring = GxB_LXOR_LT_INT16;

    entry = semiringhash_insert(semiringhash, "eq_lt_int16", &found);
    entry->name = pstrdup("eq_lt_int16");
    entry->semiring = GxB_EQ_LT_INT16;

    entry = semiringhash_insert(semiringhash, "any_lt_int16", &found);
    entry->name = pstrdup("any_lt_int16");
    entry->semiring = GxB_ANY_LT_INT16;

    entry = semiringhash_insert(semiringhash, "lor_lt_int32", &found);
    entry->name = pstrdup("lor_lt_int32");
    entry->semiring = GxB_LOR_LT_INT32;

    entry = semiringhash_insert(semiringhash, "land_lt_int32", &found);
    entry->name = pstrdup("land_lt_int32");
    entry->semiring = GxB_LAND_LT_INT32;

    entry = semiringhash_insert(semiringhash, "lxor_lt_int32", &found);
    entry->name = pstrdup("lxor_lt_int32");
    entry->semiring = GxB_LXOR_LT_INT32;

    entry = semiringhash_insert(semiringhash, "eq_lt_int32", &found);
    entry->name = pstrdup("eq_lt_int32");
    entry->semiring = GxB_EQ_LT_INT32;

    entry = semiringhash_insert(semiringhash, "any_lt_int32", &found);
    entry->name = pstrdup("any_lt_int32");
    entry->semiring = GxB_ANY_LT_INT32;

    entry = semiringhash_insert(semiringhash, "lor_lt_int64", &found);
    entry->name = pstrdup("lor_lt_int64");
    entry->semiring = GxB_LOR_LT_INT64;

    entry = semiringhash_insert(semiringhash, "land_lt_int64", &found);
    entry->name = pstrdup("land_lt_int64");
    entry->semiring = GxB_LAND_LT_INT64;

    entry = semiringhash_insert(semiringhash, "lxor_lt_int64", &found);
    entry->name = pstrdup("lxor_lt_int64");
    entry->semiring = GxB_LXOR_LT_INT64;

    entry = semiringhash_insert(semiringhash, "eq_lt_int64", &found);
    entry->name = pstrdup("eq_lt_int64");
    entry->semiring = GxB_EQ_LT_INT64;

    entry = semiringhash_insert(semiringhash, "any_lt_int64", &found);
    entry->name = pstrdup("any_lt_int64");
    entry->semiring = GxB_ANY_LT_INT64;

    entry = semiringhash_insert(semiringhash, "lor_lt_uint8", &found);
    entry->name = pstrdup("lor_lt_uint8");
    entry->semiring = GxB_LOR_LT_UINT8;

    entry = semiringhash_insert(semiringhash, "land_lt_uint8", &found);
    entry->name = pstrdup("land_lt_uint8");
    entry->semiring = GxB_LAND_LT_UINT8;

    entry = semiringhash_insert(semiringhash, "lxor_lt_uint8", &found);
    entry->name = pstrdup("lxor_lt_uint8");
    entry->semiring = GxB_LXOR_LT_UINT8;

    entry = semiringhash_insert(semiringhash, "eq_lt_uint8", &found);
    entry->name = pstrdup("eq_lt_uint8");
    entry->semiring = GxB_EQ_LT_UINT8;

    entry = semiringhash_insert(semiringhash, "any_lt_uint8", &found);
    entry->name = pstrdup("any_lt_uint8");
    entry->semiring = GxB_ANY_LT_UINT8;

    entry = semiringhash_insert(semiringhash, "lor_lt_uint16", &found);
    entry->name = pstrdup("lor_lt_uint16");
    entry->semiring = GxB_LOR_LT_UINT16;

    entry = semiringhash_insert(semiringhash, "land_lt_uint16", &found);
    entry->name = pstrdup("land_lt_uint16");
    entry->semiring = GxB_LAND_LT_UINT16;

    entry = semiringhash_insert(semiringhash, "lxor_lt_uint16", &found);
    entry->name = pstrdup("lxor_lt_uint16");
    entry->semiring = GxB_LXOR_LT_UINT16;

    entry = semiringhash_insert(semiringhash, "eq_lt_uint16", &found);
    entry->name = pstrdup("eq_lt_uint16");
    entry->semiring = GxB_EQ_LT_UINT16;

    entry = semiringhash_insert(semiringhash, "any_lt_uint16", &found);
    entry->name = pstrdup("any_lt_uint16");
    entry->semiring = GxB_ANY_LT_UINT16;

    entry = semiringhash_insert(semiringhash, "lor_lt_uint32", &found);
    entry->name = pstrdup("lor_lt_uint32");
    entry->semiring = GxB_LOR_LT_UINT32;

    entry = semiringhash_insert(semiringhash, "land_lt_uint32", &found);
    entry->name = pstrdup("land_lt_uint32");
    entry->semiring = GxB_LAND_LT_UINT32;

    entry = semiringhash_insert(semiringhash, "lxor_lt_uint32", &found);
    entry->name = pstrdup("lxor_lt_uint32");
    entry->semiring = GxB_LXOR_LT_UINT32;

    entry = semiringhash_insert(semiringhash, "eq_lt_uint32", &found);
    entry->name = pstrdup("eq_lt_uint32");
    entry->semiring = GxB_EQ_LT_UINT32;

    entry = semiringhash_insert(semiringhash, "any_lt_uint32", &found);
    entry->name = pstrdup("any_lt_uint32");
    entry->semiring = GxB_ANY_LT_UINT32;

    entry = semiringhash_insert(semiringhash, "lor_lt_uint64", &found);
    entry->name = pstrdup("lor_lt_uint64");
    entry->semiring = GxB_LOR_LT_UINT64;

    entry = semiringhash_insert(semiringhash, "land_lt_uint64", &found);
    entry->name = pstrdup("land_lt_uint64");
    entry->semiring = GxB_LAND_LT_UINT64;

    entry = semiringhash_insert(semiringhash, "lxor_lt_uint64", &found);
    entry->name = pstrdup("lxor_lt_uint64");
    entry->semiring = GxB_LXOR_LT_UINT64;

    entry = semiringhash_insert(semiringhash, "eq_lt_uint64", &found);
    entry->name = pstrdup("eq_lt_uint64");
    entry->semiring = GxB_EQ_LT_UINT64;

    entry = semiringhash_insert(semiringhash, "any_lt_uint64", &found);
    entry->name = pstrdup("any_lt_uint64");
    entry->semiring = GxB_ANY_LT_UINT64;

    entry = semiringhash_insert(semiringhash, "lor_lt_fp32", &found);
    entry->name = pstrdup("lor_lt_fp32");
    entry->semiring = GxB_LOR_LT_FP32;

    entry = semiringhash_insert(semiringhash, "land_lt_fp32", &found);
    entry->name = pstrdup("land_lt_fp32");
    entry->semiring = GxB_LAND_LT_FP32;

    entry = semiringhash_insert(semiringhash, "lxor_lt_fp32", &found);
    entry->name = pstrdup("lxor_lt_fp32");
    entry->semiring = GxB_LXOR_LT_FP32;

    entry = semiringhash_insert(semiringhash, "eq_lt_fp32", &found);
    entry->name = pstrdup("eq_lt_fp32");
    entry->semiring = GxB_EQ_LT_FP32;

    entry = semiringhash_insert(semiringhash, "any_lt_fp32", &found);
    entry->name = pstrdup("any_lt_fp32");
    entry->semiring = GxB_ANY_LT_FP32;

    entry = semiringhash_insert(semiringhash, "lor_lt_fp64", &found);
    entry->name = pstrdup("lor_lt_fp64");
    entry->semiring = GxB_LOR_LT_FP64;

    entry = semiringhash_insert(semiringhash, "land_lt_fp64", &found);
    entry->name = pstrdup("land_lt_fp64");
    entry->semiring = GxB_LAND_LT_FP64;

    entry = semiringhash_insert(semiringhash, "lxor_lt_fp64", &found);
    entry->name = pstrdup("lxor_lt_fp64");
    entry->semiring = GxB_LXOR_LT_FP64;

    entry = semiringhash_insert(semiringhash, "eq_lt_fp64", &found);
    entry->name = pstrdup("eq_lt_fp64");
    entry->semiring = GxB_EQ_LT_FP64;

    entry = semiringhash_insert(semiringhash, "any_lt_fp64", &found);
    entry->name = pstrdup("any_lt_fp64");
    entry->semiring = GxB_ANY_LT_FP64;

    entry = semiringhash_insert(semiringhash, "lor_ge_int8", &found);
    entry->name = pstrdup("lor_ge_int8");
    entry->semiring = GxB_LOR_GE_INT8;

    entry = semiringhash_insert(semiringhash, "land_ge_int8", &found);
    entry->name = pstrdup("land_ge_int8");
    entry->semiring = GxB_LAND_GE_INT8;

    entry = semiringhash_insert(semiringhash, "lxor_ge_int8", &found);
    entry->name = pstrdup("lxor_ge_int8");
    entry->semiring = GxB_LXOR_GE_INT8;

    entry = semiringhash_insert(semiringhash, "eq_ge_int8", &found);
    entry->name = pstrdup("eq_ge_int8");
    entry->semiring = GxB_EQ_GE_INT8;

    entry = semiringhash_insert(semiringhash, "any_ge_int8", &found);
    entry->name = pstrdup("any_ge_int8");
    entry->semiring = GxB_ANY_GE_INT8;

    entry = semiringhash_insert(semiringhash, "lor_ge_int16", &found);
    entry->name = pstrdup("lor_ge_int16");
    entry->semiring = GxB_LOR_GE_INT16;

    entry = semiringhash_insert(semiringhash, "land_ge_int16", &found);
    entry->name = pstrdup("land_ge_int16");
    entry->semiring = GxB_LAND_GE_INT16;

    entry = semiringhash_insert(semiringhash, "lxor_ge_int16", &found);
    entry->name = pstrdup("lxor_ge_int16");
    entry->semiring = GxB_LXOR_GE_INT16;

    entry = semiringhash_insert(semiringhash, "eq_ge_int16", &found);
    entry->name = pstrdup("eq_ge_int16");
    entry->semiring = GxB_EQ_GE_INT16;

    entry = semiringhash_insert(semiringhash, "any_ge_int16", &found);
    entry->name = pstrdup("any_ge_int16");
    entry->semiring = GxB_ANY_GE_INT16;

    entry = semiringhash_insert(semiringhash, "lor_ge_int32", &found);
    entry->name = pstrdup("lor_ge_int32");
    entry->semiring = GxB_LOR_GE_INT32;

    entry = semiringhash_insert(semiringhash, "land_ge_int32", &found);
    entry->name = pstrdup("land_ge_int32");
    entry->semiring = GxB_LAND_GE_INT32;

    entry = semiringhash_insert(semiringhash, "lxor_ge_int32", &found);
    entry->name = pstrdup("lxor_ge_int32");
    entry->semiring = GxB_LXOR_GE_INT32;

    entry = semiringhash_insert(semiringhash, "eq_ge_int32", &found);
    entry->name = pstrdup("eq_ge_int32");
    entry->semiring = GxB_EQ_GE_INT32;

    entry = semiringhash_insert(semiringhash, "any_ge_int32", &found);
    entry->name = pstrdup("any_ge_int32");
    entry->semiring = GxB_ANY_GE_INT32;

    entry = semiringhash_insert(semiringhash, "lor_ge_int64", &found);
    entry->name = pstrdup("lor_ge_int64");
    entry->semiring = GxB_LOR_GE_INT64;

    entry = semiringhash_insert(semiringhash, "land_ge_int64", &found);
    entry->name = pstrdup("land_ge_int64");
    entry->semiring = GxB_LAND_GE_INT64;

    entry = semiringhash_insert(semiringhash, "lxor_ge_int64", &found);
    entry->name = pstrdup("lxor_ge_int64");
    entry->semiring = GxB_LXOR_GE_INT64;

    entry = semiringhash_insert(semiringhash, "eq_ge_int64", &found);
    entry->name = pstrdup("eq_ge_int64");
    entry->semiring = GxB_EQ_GE_INT64;

    entry = semiringhash_insert(semiringhash, "any_ge_int64", &found);
    entry->name = pstrdup("any_ge_int64");
    entry->semiring = GxB_ANY_GE_INT64;

    entry = semiringhash_insert(semiringhash, "lor_ge_uint8", &found);
    entry->name = pstrdup("lor_ge_uint8");
    entry->semiring = GxB_LOR_GE_UINT8;

    entry = semiringhash_insert(semiringhash, "land_ge_uint8", &found);
    entry->name = pstrdup("land_ge_uint8");
    entry->semiring = GxB_LAND_GE_UINT8;

    entry = semiringhash_insert(semiringhash, "lxor_ge_uint8", &found);
    entry->name = pstrdup("lxor_ge_uint8");
    entry->semiring = GxB_LXOR_GE_UINT8;

    entry = semiringhash_insert(semiringhash, "eq_ge_uint8", &found);
    entry->name = pstrdup("eq_ge_uint8");
    entry->semiring = GxB_EQ_GE_UINT8;

    entry = semiringhash_insert(semiringhash, "any_ge_uint8", &found);
    entry->name = pstrdup("any_ge_uint8");
    entry->semiring = GxB_ANY_GE_UINT8;

    entry = semiringhash_insert(semiringhash, "lor_ge_uint16", &found);
    entry->name = pstrdup("lor_ge_uint16");
    entry->semiring = GxB_LOR_GE_UINT16;

    entry = semiringhash_insert(semiringhash, "land_ge_uint16", &found);
    entry->name = pstrdup("land_ge_uint16");
    entry->semiring = GxB_LAND_GE_UINT16;

    entry = semiringhash_insert(semiringhash, "lxor_ge_uint16", &found);
    entry->name = pstrdup("lxor_ge_uint16");
    entry->semiring = GxB_LXOR_GE_UINT16;

    entry = semiringhash_insert(semiringhash, "eq_ge_uint16", &found);
    entry->name = pstrdup("eq_ge_uint16");
    entry->semiring = GxB_EQ_GE_UINT16;

    entry = semiringhash_insert(semiringhash, "any_ge_uint16", &found);
    entry->name = pstrdup("any_ge_uint16");
    entry->semiring = GxB_ANY_GE_UINT16;

    entry = semiringhash_insert(semiringhash, "lor_ge_uint32", &found);
    entry->name = pstrdup("lor_ge_uint32");
    entry->semiring = GxB_LOR_GE_UINT32;

    entry = semiringhash_insert(semiringhash, "land_ge_uint32", &found);
    entry->name = pstrdup("land_ge_uint32");
    entry->semiring = GxB_LAND_GE_UINT32;

    entry = semiringhash_insert(semiringhash, "lxor_ge_uint32", &found);
    entry->name = pstrdup("lxor_ge_uint32");
    entry->semiring = GxB_LXOR_GE_UINT32;

    entry = semiringhash_insert(semiringhash, "eq_ge_uint32", &found);
    entry->name = pstrdup("eq_ge_uint32");
    entry->semiring = GxB_EQ_GE_UINT32;

    entry = semiringhash_insert(semiringhash, "any_ge_uint32", &found);
    entry->name = pstrdup("any_ge_uint32");
    entry->semiring = GxB_ANY_GE_UINT32;

    entry = semiringhash_insert(semiringhash, "lor_ge_uint64", &found);
    entry->name = pstrdup("lor_ge_uint64");
    entry->semiring = GxB_LOR_GE_UINT64;

    entry = semiringhash_insert(semiringhash, "land_ge_uint64", &found);
    entry->name = pstrdup("land_ge_uint64");
    entry->semiring = GxB_LAND_GE_UINT64;

    entry = semiringhash_insert(semiringhash, "lxor_ge_uint64", &found);
    entry->name = pstrdup("lxor_ge_uint64");
    entry->semiring = GxB_LXOR_GE_UINT64;

    entry = semiringhash_insert(semiringhash, "eq_ge_uint64", &found);
    entry->name = pstrdup("eq_ge_uint64");
    entry->semiring = GxB_EQ_GE_UINT64;

    entry = semiringhash_insert(semiringhash, "any_ge_uint64", &found);
    entry->name = pstrdup("any_ge_uint64");
    entry->semiring = GxB_ANY_GE_UINT64;

    entry = semiringhash_insert(semiringhash, "lor_ge_fp32", &found);
    entry->name = pstrdup("lor_ge_fp32");
    entry->semiring = GxB_LOR_GE_FP32;

    entry = semiringhash_insert(semiringhash, "land_ge_fp32", &found);
    entry->name = pstrdup("land_ge_fp32");
    entry->semiring = GxB_LAND_GE_FP32;

    entry = semiringhash_insert(semiringhash, "lxor_ge_fp32", &found);
    entry->name = pstrdup("lxor_ge_fp32");
    entry->semiring = GxB_LXOR_GE_FP32;

    entry = semiringhash_insert(semiringhash, "eq_ge_fp32", &found);
    entry->name = pstrdup("eq_ge_fp32");
    entry->semiring = GxB_EQ_GE_FP32;

    entry = semiringhash_insert(semiringhash, "any_ge_fp32", &found);
    entry->name = pstrdup("any_ge_fp32");
    entry->semiring = GxB_ANY_GE_FP32;

    entry = semiringhash_insert(semiringhash, "lor_ge_fp64", &found);
    entry->name = pstrdup("lor_ge_fp64");
    entry->semiring = GxB_LOR_GE_FP64;

    entry = semiringhash_insert(semiringhash, "land_ge_fp64", &found);
    entry->name = pstrdup("land_ge_fp64");
    entry->semiring = GxB_LAND_GE_FP64;

    entry = semiringhash_insert(semiringhash, "lxor_ge_fp64", &found);
    entry->name = pstrdup("lxor_ge_fp64");
    entry->semiring = GxB_LXOR_GE_FP64;

    entry = semiringhash_insert(semiringhash, "eq_ge_fp64", &found);
    entry->name = pstrdup("eq_ge_fp64");
    entry->semiring = GxB_EQ_GE_FP64;

    entry = semiringhash_insert(semiringhash, "any_ge_fp64", &found);
    entry->name = pstrdup("any_ge_fp64");
    entry->semiring = GxB_ANY_GE_FP64;

    entry = semiringhash_insert(semiringhash, "lor_le_int8", &found);
    entry->name = pstrdup("lor_le_int8");
    entry->semiring = GxB_LOR_LE_INT8;

    entry = semiringhash_insert(semiringhash, "land_le_int8", &found);
    entry->name = pstrdup("land_le_int8");
    entry->semiring = GxB_LAND_LE_INT8;

    entry = semiringhash_insert(semiringhash, "lxor_le_int8", &found);
    entry->name = pstrdup("lxor_le_int8");
    entry->semiring = GxB_LXOR_LE_INT8;

    entry = semiringhash_insert(semiringhash, "eq_le_int8", &found);
    entry->name = pstrdup("eq_le_int8");
    entry->semiring = GxB_EQ_LE_INT8;

    entry = semiringhash_insert(semiringhash, "any_le_int8", &found);
    entry->name = pstrdup("any_le_int8");
    entry->semiring = GxB_ANY_LE_INT8;

    entry = semiringhash_insert(semiringhash, "lor_le_int16", &found);
    entry->name = pstrdup("lor_le_int16");
    entry->semiring = GxB_LOR_LE_INT16;

    entry = semiringhash_insert(semiringhash, "land_le_int16", &found);
    entry->name = pstrdup("land_le_int16");
    entry->semiring = GxB_LAND_LE_INT16;

    entry = semiringhash_insert(semiringhash, "lxor_le_int16", &found);
    entry->name = pstrdup("lxor_le_int16");
    entry->semiring = GxB_LXOR_LE_INT16;

    entry = semiringhash_insert(semiringhash, "eq_le_int16", &found);
    entry->name = pstrdup("eq_le_int16");
    entry->semiring = GxB_EQ_LE_INT16;

    entry = semiringhash_insert(semiringhash, "any_le_int16", &found);
    entry->name = pstrdup("any_le_int16");
    entry->semiring = GxB_ANY_LE_INT16;

    entry = semiringhash_insert(semiringhash, "lor_le_int32", &found);
    entry->name = pstrdup("lor_le_int32");
    entry->semiring = GxB_LOR_LE_INT32;

    entry = semiringhash_insert(semiringhash, "land_le_int32", &found);
    entry->name = pstrdup("land_le_int32");
    entry->semiring = GxB_LAND_LE_INT32;

    entry = semiringhash_insert(semiringhash, "lxor_le_int32", &found);
    entry->name = pstrdup("lxor_le_int32");
    entry->semiring = GxB_LXOR_LE_INT32;

    entry = semiringhash_insert(semiringhash, "eq_le_int32", &found);
    entry->name = pstrdup("eq_le_int32");
    entry->semiring = GxB_EQ_LE_INT32;

    entry = semiringhash_insert(semiringhash, "any_le_int32", &found);
    entry->name = pstrdup("any_le_int32");
    entry->semiring = GxB_ANY_LE_INT32;

    entry = semiringhash_insert(semiringhash, "lor_le_int64", &found);
    entry->name = pstrdup("lor_le_int64");
    entry->semiring = GxB_LOR_LE_INT64;

    entry = semiringhash_insert(semiringhash, "land_le_int64", &found);
    entry->name = pstrdup("land_le_int64");
    entry->semiring = GxB_LAND_LE_INT64;

    entry = semiringhash_insert(semiringhash, "lxor_le_int64", &found);
    entry->name = pstrdup("lxor_le_int64");
    entry->semiring = GxB_LXOR_LE_INT64;

    entry = semiringhash_insert(semiringhash, "eq_le_int64", &found);
    entry->name = pstrdup("eq_le_int64");
    entry->semiring = GxB_EQ_LE_INT64;

    entry = semiringhash_insert(semiringhash, "any_le_int64", &found);
    entry->name = pstrdup("any_le_int64");
    entry->semiring = GxB_ANY_LE_INT64;

    entry = semiringhash_insert(semiringhash, "lor_le_uint8", &found);
    entry->name = pstrdup("lor_le_uint8");
    entry->semiring = GxB_LOR_LE_UINT8;

    entry = semiringhash_insert(semiringhash, "land_le_uint8", &found);
    entry->name = pstrdup("land_le_uint8");
    entry->semiring = GxB_LAND_LE_UINT8;

    entry = semiringhash_insert(semiringhash, "lxor_le_uint8", &found);
    entry->name = pstrdup("lxor_le_uint8");
    entry->semiring = GxB_LXOR_LE_UINT8;

    entry = semiringhash_insert(semiringhash, "eq_le_uint8", &found);
    entry->name = pstrdup("eq_le_uint8");
    entry->semiring = GxB_EQ_LE_UINT8;

    entry = semiringhash_insert(semiringhash, "any_le_uint8", &found);
    entry->name = pstrdup("any_le_uint8");
    entry->semiring = GxB_ANY_LE_UINT8;

    entry = semiringhash_insert(semiringhash, "lor_le_uint16", &found);
    entry->name = pstrdup("lor_le_uint16");
    entry->semiring = GxB_LOR_LE_UINT16;

    entry = semiringhash_insert(semiringhash, "land_le_uint16", &found);
    entry->name = pstrdup("land_le_uint16");
    entry->semiring = GxB_LAND_LE_UINT16;

    entry = semiringhash_insert(semiringhash, "lxor_le_uint16", &found);
    entry->name = pstrdup("lxor_le_uint16");
    entry->semiring = GxB_LXOR_LE_UINT16;

    entry = semiringhash_insert(semiringhash, "eq_le_uint16", &found);
    entry->name = pstrdup("eq_le_uint16");
    entry->semiring = GxB_EQ_LE_UINT16;

    entry = semiringhash_insert(semiringhash, "any_le_uint16", &found);
    entry->name = pstrdup("any_le_uint16");
    entry->semiring = GxB_ANY_LE_UINT16;

    entry = semiringhash_insert(semiringhash, "lor_le_uint32", &found);
    entry->name = pstrdup("lor_le_uint32");
    entry->semiring = GxB_LOR_LE_UINT32;

    entry = semiringhash_insert(semiringhash, "land_le_uint32", &found);
    entry->name = pstrdup("land_le_uint32");
    entry->semiring = GxB_LAND_LE_UINT32;

    entry = semiringhash_insert(semiringhash, "lxor_le_uint32", &found);
    entry->name = pstrdup("lxor_le_uint32");
    entry->semiring = GxB_LXOR_LE_UINT32;

    entry = semiringhash_insert(semiringhash, "eq_le_uint32", &found);
    entry->name = pstrdup("eq_le_uint32");
    entry->semiring = GxB_EQ_LE_UINT32;

    entry = semiringhash_insert(semiringhash, "any_le_uint32", &found);
    entry->name = pstrdup("any_le_uint32");
    entry->semiring = GxB_ANY_LE_UINT32;

    entry = semiringhash_insert(semiringhash, "lor_le_uint64", &found);
    entry->name = pstrdup("lor_le_uint64");
    entry->semiring = GxB_LOR_LE_UINT64;

    entry = semiringhash_insert(semiringhash, "land_le_uint64", &found);
    entry->name = pstrdup("land_le_uint64");
    entry->semiring = GxB_LAND_LE_UINT64;

    entry = semiringhash_insert(semiringhash, "lxor_le_uint64", &found);
    entry->name = pstrdup("lxor_le_uint64");
    entry->semiring = GxB_LXOR_LE_UINT64;

    entry = semiringhash_insert(semiringhash, "eq_le_uint64", &found);
    entry->name = pstrdup("eq_le_uint64");
    entry->semiring = GxB_EQ_LE_UINT64;

    entry = semiringhash_insert(semiringhash, "any_le_uint64", &found);
    entry->name = pstrdup("any_le_uint64");
    entry->semiring = GxB_ANY_LE_UINT64;

    entry = semiringhash_insert(semiringhash, "lor_le_fp32", &found);
    entry->name = pstrdup("lor_le_fp32");
    entry->semiring = GxB_LOR_LE_FP32;

    entry = semiringhash_insert(semiringhash, "land_le_fp32", &found);
    entry->name = pstrdup("land_le_fp32");
    entry->semiring = GxB_LAND_LE_FP32;

    entry = semiringhash_insert(semiringhash, "lxor_le_fp32", &found);
    entry->name = pstrdup("lxor_le_fp32");
    entry->semiring = GxB_LXOR_LE_FP32;

    entry = semiringhash_insert(semiringhash, "eq_le_fp32", &found);
    entry->name = pstrdup("eq_le_fp32");
    entry->semiring = GxB_EQ_LE_FP32;

    entry = semiringhash_insert(semiringhash, "any_le_fp32", &found);
    entry->name = pstrdup("any_le_fp32");
    entry->semiring = GxB_ANY_LE_FP32;

    entry = semiringhash_insert(semiringhash, "lor_le_fp64", &found);
    entry->name = pstrdup("lor_le_fp64");
    entry->semiring = GxB_LOR_LE_FP64;

    entry = semiringhash_insert(semiringhash, "land_le_fp64", &found);
    entry->name = pstrdup("land_le_fp64");
    entry->semiring = GxB_LAND_LE_FP64;

    entry = semiringhash_insert(semiringhash, "lxor_le_fp64", &found);
    entry->name = pstrdup("lxor_le_fp64");
    entry->semiring = GxB_LXOR_LE_FP64;

    entry = semiringhash_insert(semiringhash, "eq_le_fp64", &found);
    entry->name = pstrdup("eq_le_fp64");
    entry->semiring = GxB_EQ_LE_FP64;

    entry = semiringhash_insert(semiringhash, "any_le_fp64", &found);
    entry->name = pstrdup("any_le_fp64");
    entry->semiring = GxB_ANY_LE_FP64;

    entry = semiringhash_insert(semiringhash, "lor_first_bool", &found);
    entry->name = pstrdup("lor_first_bool");
    entry->semiring = GxB_LOR_FIRST_BOOL;

    entry = semiringhash_insert(semiringhash, "land_first_bool", &found);
    entry->name = pstrdup("land_first_bool");
    entry->semiring = GxB_LAND_FIRST_BOOL;

    entry = semiringhash_insert(semiringhash, "lxor_first_bool", &found);
    entry->name = pstrdup("lxor_first_bool");
    entry->semiring = GxB_LXOR_FIRST_BOOL;

    entry = semiringhash_insert(semiringhash, "eq_first_bool", &found);
    entry->name = pstrdup("eq_first_bool");
    entry->semiring = GxB_EQ_FIRST_BOOL;

    entry = semiringhash_insert(semiringhash, "any_first_bool", &found);
    entry->name = pstrdup("any_first_bool");
    entry->semiring = GxB_ANY_FIRST_BOOL;

    entry = semiringhash_insert(semiringhash, "lor_second_bool", &found);
    entry->name = pstrdup("lor_second_bool");
    entry->semiring = GxB_LOR_SECOND_BOOL;

    entry = semiringhash_insert(semiringhash, "land_second_bool", &found);
    entry->name = pstrdup("land_second_bool");
    entry->semiring = GxB_LAND_SECOND_BOOL;

    entry = semiringhash_insert(semiringhash, "lxor_second_bool", &found);
    entry->name = pstrdup("lxor_second_bool");
    entry->semiring = GxB_LXOR_SECOND_BOOL;

    entry = semiringhash_insert(semiringhash, "eq_second_bool", &found);
    entry->name = pstrdup("eq_second_bool");
    entry->semiring = GxB_EQ_SECOND_BOOL;

    entry = semiringhash_insert(semiringhash, "any_second_bool", &found);
    entry->name = pstrdup("any_second_bool");
    entry->semiring = GxB_ANY_SECOND_BOOL;

    entry = semiringhash_insert(semiringhash, "lor_pair_bool", &found);
    entry->name = pstrdup("lor_pair_bool");
    entry->semiring = GxB_LOR_PAIR_BOOL;

    entry = semiringhash_insert(semiringhash, "land_pair_bool", &found);
    entry->name = pstrdup("land_pair_bool");
    entry->semiring = GxB_LAND_PAIR_BOOL;

    entry = semiringhash_insert(semiringhash, "lxor_pair_bool", &found);
    entry->name = pstrdup("lxor_pair_bool");
    entry->semiring = GxB_LXOR_PAIR_BOOL;

    entry = semiringhash_insert(semiringhash, "eq_pair_bool", &found);
    entry->name = pstrdup("eq_pair_bool");
    entry->semiring = GxB_EQ_PAIR_BOOL;

    entry = semiringhash_insert(semiringhash, "any_pair_bool", &found);
    entry->name = pstrdup("any_pair_bool");
    entry->semiring = GxB_ANY_PAIR_BOOL;

    entry = semiringhash_insert(semiringhash, "lor_lor_bool", &found);
    entry->name = pstrdup("lor_lor_bool");
    entry->semiring = GxB_LOR_LOR_BOOL;

    entry = semiringhash_insert(semiringhash, "land_lor_bool", &found);
    entry->name = pstrdup("land_lor_bool");
    entry->semiring = GxB_LAND_LOR_BOOL;

    entry = semiringhash_insert(semiringhash, "lxor_lor_bool", &found);
    entry->name = pstrdup("lxor_lor_bool");
    entry->semiring = GxB_LXOR_LOR_BOOL;

    entry = semiringhash_insert(semiringhash, "eq_lor_bool", &found);
    entry->name = pstrdup("eq_lor_bool");
    entry->semiring = GxB_EQ_LOR_BOOL;

    entry = semiringhash_insert(semiringhash, "any_lor_bool", &found);
    entry->name = pstrdup("any_lor_bool");
    entry->semiring = GxB_ANY_LOR_BOOL;

    entry = semiringhash_insert(semiringhash, "lor_land_bool", &found);
    entry->name = pstrdup("lor_land_bool");
    entry->semiring = GxB_LOR_LAND_BOOL;

    entry = semiringhash_insert(semiringhash, "land_land_bool", &found);
    entry->name = pstrdup("land_land_bool");
    entry->semiring = GxB_LAND_LAND_BOOL;

    entry = semiringhash_insert(semiringhash, "lxor_land_bool", &found);
    entry->name = pstrdup("lxor_land_bool");
    entry->semiring = GxB_LXOR_LAND_BOOL;

    entry = semiringhash_insert(semiringhash, "eq_land_bool", &found);
    entry->name = pstrdup("eq_land_bool");
    entry->semiring = GxB_EQ_LAND_BOOL;

    entry = semiringhash_insert(semiringhash, "any_land_bool", &found);
    entry->name = pstrdup("any_land_bool");
    entry->semiring = GxB_ANY_LAND_BOOL;

    entry = semiringhash_insert(semiringhash, "lor_lxor_bool", &found);
    entry->name = pstrdup("lor_lxor_bool");
    entry->semiring = GxB_LOR_LXOR_BOOL;

    entry = semiringhash_insert(semiringhash, "land_lxor_bool", &found);
    entry->name = pstrdup("land_lxor_bool");
    entry->semiring = GxB_LAND_LXOR_BOOL;

    entry = semiringhash_insert(semiringhash, "lxor_lxor_bool", &found);
    entry->name = pstrdup("lxor_lxor_bool");
    entry->semiring = GxB_LXOR_LXOR_BOOL;

    entry = semiringhash_insert(semiringhash, "eq_lxor_bool", &found);
    entry->name = pstrdup("eq_lxor_bool");
    entry->semiring = GxB_EQ_LXOR_BOOL;

    entry = semiringhash_insert(semiringhash, "any_lxor_bool", &found);
    entry->name = pstrdup("any_lxor_bool");
    entry->semiring = GxB_ANY_LXOR_BOOL;

    entry = semiringhash_insert(semiringhash, "lor_eq_bool", &found);
    entry->name = pstrdup("lor_eq_bool");
    entry->semiring = GxB_LOR_EQ_BOOL;

    entry = semiringhash_insert(semiringhash, "land_eq_bool", &found);
    entry->name = pstrdup("land_eq_bool");
    entry->semiring = GxB_LAND_EQ_BOOL;

    entry = semiringhash_insert(semiringhash, "lxor_eq_bool", &found);
    entry->name = pstrdup("lxor_eq_bool");
    entry->semiring = GxB_LXOR_EQ_BOOL;

    entry = semiringhash_insert(semiringhash, "eq_eq_bool", &found);
    entry->name = pstrdup("eq_eq_bool");
    entry->semiring = GxB_EQ_EQ_BOOL;

    entry = semiringhash_insert(semiringhash, "any_eq_bool", &found);
    entry->name = pstrdup("any_eq_bool");
    entry->semiring = GxB_ANY_EQ_BOOL;

    entry = semiringhash_insert(semiringhash, "lor_gt_bool", &found);
    entry->name = pstrdup("lor_gt_bool");
    entry->semiring = GxB_LOR_GT_BOOL;

    entry = semiringhash_insert(semiringhash, "land_gt_bool", &found);
    entry->name = pstrdup("land_gt_bool");
    entry->semiring = GxB_LAND_GT_BOOL;

    entry = semiringhash_insert(semiringhash, "lxor_gt_bool", &found);
    entry->name = pstrdup("lxor_gt_bool");
    entry->semiring = GxB_LXOR_GT_BOOL;

    entry = semiringhash_insert(semiringhash, "eq_gt_bool", &found);
    entry->name = pstrdup("eq_gt_bool");
    entry->semiring = GxB_EQ_GT_BOOL;

    entry = semiringhash_insert(semiringhash, "any_gt_bool", &found);
    entry->name = pstrdup("any_gt_bool");
    entry->semiring = GxB_ANY_GT_BOOL;

    entry = semiringhash_insert(semiringhash, "lor_lt_bool", &found);
    entry->name = pstrdup("lor_lt_bool");
    entry->semiring = GxB_LOR_LT_BOOL;

    entry = semiringhash_insert(semiringhash, "land_lt_bool", &found);
    entry->name = pstrdup("land_lt_bool");
    entry->semiring = GxB_LAND_LT_BOOL;

    entry = semiringhash_insert(semiringhash, "lxor_lt_bool", &found);
    entry->name = pstrdup("lxor_lt_bool");
    entry->semiring = GxB_LXOR_LT_BOOL;

    entry = semiringhash_insert(semiringhash, "eq_lt_bool", &found);
    entry->name = pstrdup("eq_lt_bool");
    entry->semiring = GxB_EQ_LT_BOOL;

    entry = semiringhash_insert(semiringhash, "any_lt_bool", &found);
    entry->name = pstrdup("any_lt_bool");
    entry->semiring = GxB_ANY_LT_BOOL;

    entry = semiringhash_insert(semiringhash, "lor_ge_bool", &found);
    entry->name = pstrdup("lor_ge_bool");
    entry->semiring = GxB_LOR_GE_BOOL;

    entry = semiringhash_insert(semiringhash, "land_ge_bool", &found);
    entry->name = pstrdup("land_ge_bool");
    entry->semiring = GxB_LAND_GE_BOOL;

    entry = semiringhash_insert(semiringhash, "lxor_ge_bool", &found);
    entry->name = pstrdup("lxor_ge_bool");
    entry->semiring = GxB_LXOR_GE_BOOL;

    entry = semiringhash_insert(semiringhash, "eq_ge_bool", &found);
    entry->name = pstrdup("eq_ge_bool");
    entry->semiring = GxB_EQ_GE_BOOL;

    entry = semiringhash_insert(semiringhash, "any_ge_bool", &found);
    entry->name = pstrdup("any_ge_bool");
    entry->semiring = GxB_ANY_GE_BOOL;

    entry = semiringhash_insert(semiringhash, "lor_le_bool", &found);
    entry->name = pstrdup("lor_le_bool");
    entry->semiring = GxB_LOR_LE_BOOL;

    entry = semiringhash_insert(semiringhash, "land_le_bool", &found);
    entry->name = pstrdup("land_le_bool");
    entry->semiring = GxB_LAND_LE_BOOL;

    entry = semiringhash_insert(semiringhash, "lxor_le_bool", &found);
    entry->name = pstrdup("lxor_le_bool");
    entry->semiring = GxB_LXOR_LE_BOOL;

    entry = semiringhash_insert(semiringhash, "eq_le_bool", &found);
    entry->name = pstrdup("eq_le_bool");
    entry->semiring = GxB_EQ_LE_BOOL;

    entry = semiringhash_insert(semiringhash, "any_le_bool", &found);
    entry->name = pstrdup("any_le_bool");
    entry->semiring = GxB_ANY_LE_BOOL;

    entry = semiringhash_insert(semiringhash, "plus_first_fc32", &found);
    entry->name = pstrdup("plus_first_fc32");
    entry->semiring = GxB_PLUS_FIRST_FC32;

    entry = semiringhash_insert(semiringhash, "times_first_fc32", &found);
    entry->name = pstrdup("times_first_fc32");
    entry->semiring = GxB_TIMES_FIRST_FC32;

    entry = semiringhash_insert(semiringhash, "any_first_fc32", &found);
    entry->name = pstrdup("any_first_fc32");
    entry->semiring = GxB_ANY_FIRST_FC32;

    entry = semiringhash_insert(semiringhash, "plus_first_fc64", &found);
    entry->name = pstrdup("plus_first_fc64");
    entry->semiring = GxB_PLUS_FIRST_FC64;

    entry = semiringhash_insert(semiringhash, "times_first_fc64", &found);
    entry->name = pstrdup("times_first_fc64");
    entry->semiring = GxB_TIMES_FIRST_FC64;

    entry = semiringhash_insert(semiringhash, "any_first_fc64", &found);
    entry->name = pstrdup("any_first_fc64");
    entry->semiring = GxB_ANY_FIRST_FC64;

    entry = semiringhash_insert(semiringhash, "plus_second_fc32", &found);
    entry->name = pstrdup("plus_second_fc32");
    entry->semiring = GxB_PLUS_SECOND_FC32;

    entry = semiringhash_insert(semiringhash, "times_second_fc32", &found);
    entry->name = pstrdup("times_second_fc32");
    entry->semiring = GxB_TIMES_SECOND_FC32;

    entry = semiringhash_insert(semiringhash, "any_second_fc32", &found);
    entry->name = pstrdup("any_second_fc32");
    entry->semiring = GxB_ANY_SECOND_FC32;

    entry = semiringhash_insert(semiringhash, "plus_second_fc64", &found);
    entry->name = pstrdup("plus_second_fc64");
    entry->semiring = GxB_PLUS_SECOND_FC64;

    entry = semiringhash_insert(semiringhash, "times_second_fc64", &found);
    entry->name = pstrdup("times_second_fc64");
    entry->semiring = GxB_TIMES_SECOND_FC64;

    entry = semiringhash_insert(semiringhash, "any_second_fc64", &found);
    entry->name = pstrdup("any_second_fc64");
    entry->semiring = GxB_ANY_SECOND_FC64;

    entry = semiringhash_insert(semiringhash, "plus_pair_fc32", &found);
    entry->name = pstrdup("plus_pair_fc32");
    entry->semiring = GxB_PLUS_PAIR_FC32;

    entry = semiringhash_insert(semiringhash, "times_pair_fc32", &found);
    entry->name = pstrdup("times_pair_fc32");
    entry->semiring = GxB_TIMES_PAIR_FC32;

    entry = semiringhash_insert(semiringhash, "any_pair_fc32", &found);
    entry->name = pstrdup("any_pair_fc32");
    entry->semiring = GxB_ANY_PAIR_FC32;

    entry = semiringhash_insert(semiringhash, "plus_pair_fc64", &found);
    entry->name = pstrdup("plus_pair_fc64");
    entry->semiring = GxB_PLUS_PAIR_FC64;

    entry = semiringhash_insert(semiringhash, "times_pair_fc64", &found);
    entry->name = pstrdup("times_pair_fc64");
    entry->semiring = GxB_TIMES_PAIR_FC64;

    entry = semiringhash_insert(semiringhash, "any_pair_fc64", &found);
    entry->name = pstrdup("any_pair_fc64");
    entry->semiring = GxB_ANY_PAIR_FC64;

    entry = semiringhash_insert(semiringhash, "plus_plus_fc32", &found);
    entry->name = pstrdup("plus_plus_fc32");
    entry->semiring = GxB_PLUS_PLUS_FC32;

    entry = semiringhash_insert(semiringhash, "times_plus_fc32", &found);
    entry->name = pstrdup("times_plus_fc32");
    entry->semiring = GxB_TIMES_PLUS_FC32;

    entry = semiringhash_insert(semiringhash, "any_plus_fc32", &found);
    entry->name = pstrdup("any_plus_fc32");
    entry->semiring = GxB_ANY_PLUS_FC32;

    entry = semiringhash_insert(semiringhash, "plus_plus_fc64", &found);
    entry->name = pstrdup("plus_plus_fc64");
    entry->semiring = GxB_PLUS_PLUS_FC64;

    entry = semiringhash_insert(semiringhash, "times_plus_fc64", &found);
    entry->name = pstrdup("times_plus_fc64");
    entry->semiring = GxB_TIMES_PLUS_FC64;

    entry = semiringhash_insert(semiringhash, "any_plus_fc64", &found);
    entry->name = pstrdup("any_plus_fc64");
    entry->semiring = GxB_ANY_PLUS_FC64;

    entry = semiringhash_insert(semiringhash, "plus_minus_fc32", &found);
    entry->name = pstrdup("plus_minus_fc32");
    entry->semiring = GxB_PLUS_MINUS_FC32;

    entry = semiringhash_insert(semiringhash, "times_minus_fc32", &found);
    entry->name = pstrdup("times_minus_fc32");
    entry->semiring = GxB_TIMES_MINUS_FC32;

    entry = semiringhash_insert(semiringhash, "any_minus_fc32", &found);
    entry->name = pstrdup("any_minus_fc32");
    entry->semiring = GxB_ANY_MINUS_FC32;

    entry = semiringhash_insert(semiringhash, "plus_minus_fc64", &found);
    entry->name = pstrdup("plus_minus_fc64");
    entry->semiring = GxB_PLUS_MINUS_FC64;

    entry = semiringhash_insert(semiringhash, "times_minus_fc64", &found);
    entry->name = pstrdup("times_minus_fc64");
    entry->semiring = GxB_TIMES_MINUS_FC64;

    entry = semiringhash_insert(semiringhash, "any_minus_fc64", &found);
    entry->name = pstrdup("any_minus_fc64");
    entry->semiring = GxB_ANY_MINUS_FC64;

    entry = semiringhash_insert(semiringhash, "plus_times_fc32", &found);
    entry->name = pstrdup("plus_times_fc32");
    entry->semiring = GxB_PLUS_TIMES_FC32;

    entry = semiringhash_insert(semiringhash, "times_times_fc32", &found);
    entry->name = pstrdup("times_times_fc32");
    entry->semiring = GxB_TIMES_TIMES_FC32;

    entry = semiringhash_insert(semiringhash, "any_times_fc32", &found);
    entry->name = pstrdup("any_times_fc32");
    entry->semiring = GxB_ANY_TIMES_FC32;

    entry = semiringhash_insert(semiringhash, "plus_times_fc64", &found);
    entry->name = pstrdup("plus_times_fc64");
    entry->semiring = GxB_PLUS_TIMES_FC64;

    entry = semiringhash_insert(semiringhash, "times_times_fc64", &found);
    entry->name = pstrdup("times_times_fc64");
    entry->semiring = GxB_TIMES_TIMES_FC64;

    entry = semiringhash_insert(semiringhash, "any_times_fc64", &found);
    entry->name = pstrdup("any_times_fc64");
    entry->semiring = GxB_ANY_TIMES_FC64;

    entry = semiringhash_insert(semiringhash, "plus_div_fc32", &found);
    entry->name = pstrdup("plus_div_fc32");
    entry->semiring = GxB_PLUS_DIV_FC32;

    entry = semiringhash_insert(semiringhash, "times_div_fc32", &found);
    entry->name = pstrdup("times_div_fc32");
    entry->semiring = GxB_TIMES_DIV_FC32;

    entry = semiringhash_insert(semiringhash, "any_div_fc32", &found);
    entry->name = pstrdup("any_div_fc32");
    entry->semiring = GxB_ANY_DIV_FC32;

    entry = semiringhash_insert(semiringhash, "plus_div_fc64", &found);
    entry->name = pstrdup("plus_div_fc64");
    entry->semiring = GxB_PLUS_DIV_FC64;

    entry = semiringhash_insert(semiringhash, "times_div_fc64", &found);
    entry->name = pstrdup("times_div_fc64");
    entry->semiring = GxB_TIMES_DIV_FC64;

    entry = semiringhash_insert(semiringhash, "any_div_fc64", &found);
    entry->name = pstrdup("any_div_fc64");
    entry->semiring = GxB_ANY_DIV_FC64;

    entry = semiringhash_insert(semiringhash, "plus_rdiv_fc32", &found);
    entry->name = pstrdup("plus_rdiv_fc32");
    entry->semiring = GxB_PLUS_RDIV_FC32;

    entry = semiringhash_insert(semiringhash, "times_rdiv_fc32", &found);
    entry->name = pstrdup("times_rdiv_fc32");
    entry->semiring = GxB_TIMES_RDIV_FC32;

    entry = semiringhash_insert(semiringhash, "any_rdiv_fc32", &found);
    entry->name = pstrdup("any_rdiv_fc32");
    entry->semiring = GxB_ANY_RDIV_FC32;

    entry = semiringhash_insert(semiringhash, "plus_rdiv_fc64", &found);
    entry->name = pstrdup("plus_rdiv_fc64");
    entry->semiring = GxB_PLUS_RDIV_FC64;

    entry = semiringhash_insert(semiringhash, "times_rdiv_fc64", &found);
    entry->name = pstrdup("times_rdiv_fc64");
    entry->semiring = GxB_TIMES_RDIV_FC64;

    entry = semiringhash_insert(semiringhash, "any_rdiv_fc64", &found);
    entry->name = pstrdup("any_rdiv_fc64");
    entry->semiring = GxB_ANY_RDIV_FC64;

    entry = semiringhash_insert(semiringhash, "plus_rminus_fc32", &found);
    entry->name = pstrdup("plus_rminus_fc32");
    entry->semiring = GxB_PLUS_RMINUS_FC32;

    entry = semiringhash_insert(semiringhash, "times_rminus_fc32", &found);
    entry->name = pstrdup("times_rminus_fc32");
    entry->semiring = GxB_TIMES_RMINUS_FC32;

    entry = semiringhash_insert(semiringhash, "any_rminus_fc32", &found);
    entry->name = pstrdup("any_rminus_fc32");
    entry->semiring = GxB_ANY_RMINUS_FC32;

    entry = semiringhash_insert(semiringhash, "plus_rminus_fc64", &found);
    entry->name = pstrdup("plus_rminus_fc64");
    entry->semiring = GxB_PLUS_RMINUS_FC64;

    entry = semiringhash_insert(semiringhash, "times_rminus_fc64", &found);
    entry->name = pstrdup("times_rminus_fc64");
    entry->semiring = GxB_TIMES_RMINUS_FC64;

    entry = semiringhash_insert(semiringhash, "any_rminus_fc64", &found);
    entry->name = pstrdup("any_rminus_fc64");
    entry->semiring = GxB_ANY_RMINUS_FC64;

    entry = semiringhash_insert(semiringhash, "bor_bor_uint8", &found);
    entry->name = pstrdup("bor_bor_uint8");
    entry->semiring = GxB_BOR_BOR_UINT8;

    entry = semiringhash_insert(semiringhash, "bor_bor_uint16", &found);
    entry->name = pstrdup("bor_bor_uint16");
    entry->semiring = GxB_BOR_BOR_UINT16;

    entry = semiringhash_insert(semiringhash, "bor_bor_uint32", &found);
    entry->name = pstrdup("bor_bor_uint32");
    entry->semiring = GxB_BOR_BOR_UINT32;

    entry = semiringhash_insert(semiringhash, "bor_bor_uint64", &found);
    entry->name = pstrdup("bor_bor_uint64");
    entry->semiring = GxB_BOR_BOR_UINT64;

    entry = semiringhash_insert(semiringhash, "bor_band_uint8", &found);
    entry->name = pstrdup("bor_band_uint8");
    entry->semiring = GxB_BOR_BAND_UINT8;

    entry = semiringhash_insert(semiringhash, "bor_band_uint16", &found);
    entry->name = pstrdup("bor_band_uint16");
    entry->semiring = GxB_BOR_BAND_UINT16;

    entry = semiringhash_insert(semiringhash, "bor_band_uint32", &found);
    entry->name = pstrdup("bor_band_uint32");
    entry->semiring = GxB_BOR_BAND_UINT32;

    entry = semiringhash_insert(semiringhash, "bor_band_uint64", &found);
    entry->name = pstrdup("bor_band_uint64");
    entry->semiring = GxB_BOR_BAND_UINT64;

    entry = semiringhash_insert(semiringhash, "bor_bxor_uint8", &found);
    entry->name = pstrdup("bor_bxor_uint8");
    entry->semiring = GxB_BOR_BXOR_UINT8;

    entry = semiringhash_insert(semiringhash, "bor_bxor_uint16", &found);
    entry->name = pstrdup("bor_bxor_uint16");
    entry->semiring = GxB_BOR_BXOR_UINT16;

    entry = semiringhash_insert(semiringhash, "bor_bxor_uint32", &found);
    entry->name = pstrdup("bor_bxor_uint32");
    entry->semiring = GxB_BOR_BXOR_UINT32;

    entry = semiringhash_insert(semiringhash, "bor_bxor_uint64", &found);
    entry->name = pstrdup("bor_bxor_uint64");
    entry->semiring = GxB_BOR_BXOR_UINT64;

    entry = semiringhash_insert(semiringhash, "bor_bxnor_uint8", &found);
    entry->name = pstrdup("bor_bxnor_uint8");
    entry->semiring = GxB_BOR_BXNOR_UINT8;

    entry = semiringhash_insert(semiringhash, "bor_bxnor_uint16", &found);
    entry->name = pstrdup("bor_bxnor_uint16");
    entry->semiring = GxB_BOR_BXNOR_UINT16;

    entry = semiringhash_insert(semiringhash, "bor_bxnor_uint32", &found);
    entry->name = pstrdup("bor_bxnor_uint32");
    entry->semiring = GxB_BOR_BXNOR_UINT32;

    entry = semiringhash_insert(semiringhash, "bor_bxnor_uint64", &found);
    entry->name = pstrdup("bor_bxnor_uint64");
    entry->semiring = GxB_BOR_BXNOR_UINT64;

    entry = semiringhash_insert(semiringhash, "band_bor_uint8", &found);
    entry->name = pstrdup("band_bor_uint8");
    entry->semiring = GxB_BAND_BOR_UINT8;

    entry = semiringhash_insert(semiringhash, "band_bor_uint16", &found);
    entry->name = pstrdup("band_bor_uint16");
    entry->semiring = GxB_BAND_BOR_UINT16;

    entry = semiringhash_insert(semiringhash, "band_bor_uint32", &found);
    entry->name = pstrdup("band_bor_uint32");
    entry->semiring = GxB_BAND_BOR_UINT32;

    entry = semiringhash_insert(semiringhash, "band_bor_uint64", &found);
    entry->name = pstrdup("band_bor_uint64");
    entry->semiring = GxB_BAND_BOR_UINT64;

    entry = semiringhash_insert(semiringhash, "band_band_uint8", &found);
    entry->name = pstrdup("band_band_uint8");
    entry->semiring = GxB_BAND_BAND_UINT8;

    entry = semiringhash_insert(semiringhash, "band_band_uint16", &found);
    entry->name = pstrdup("band_band_uint16");
    entry->semiring = GxB_BAND_BAND_UINT16;

    entry = semiringhash_insert(semiringhash, "band_band_uint32", &found);
    entry->name = pstrdup("band_band_uint32");
    entry->semiring = GxB_BAND_BAND_UINT32;

    entry = semiringhash_insert(semiringhash, "band_band_uint64", &found);
    entry->name = pstrdup("band_band_uint64");
    entry->semiring = GxB_BAND_BAND_UINT64;

    entry = semiringhash_insert(semiringhash, "band_bxor_uint8", &found);
    entry->name = pstrdup("band_bxor_uint8");
    entry->semiring = GxB_BAND_BXOR_UINT8;

    entry = semiringhash_insert(semiringhash, "band_bxor_uint16", &found);
    entry->name = pstrdup("band_bxor_uint16");
    entry->semiring = GxB_BAND_BXOR_UINT16;

    entry = semiringhash_insert(semiringhash, "band_bxor_uint32", &found);
    entry->name = pstrdup("band_bxor_uint32");
    entry->semiring = GxB_BAND_BXOR_UINT32;

    entry = semiringhash_insert(semiringhash, "band_bxor_uint64", &found);
    entry->name = pstrdup("band_bxor_uint64");
    entry->semiring = GxB_BAND_BXOR_UINT64;

    entry = semiringhash_insert(semiringhash, "band_bxnor_uint8", &found);
    entry->name = pstrdup("band_bxnor_uint8");
    entry->semiring = GxB_BAND_BXNOR_UINT8;

    entry = semiringhash_insert(semiringhash, "band_bxnor_uint16", &found);
    entry->name = pstrdup("band_bxnor_uint16");
    entry->semiring = GxB_BAND_BXNOR_UINT16;

    entry = semiringhash_insert(semiringhash, "band_bxnor_uint32", &found);
    entry->name = pstrdup("band_bxnor_uint32");
    entry->semiring = GxB_BAND_BXNOR_UINT32;

    entry = semiringhash_insert(semiringhash, "band_bxnor_uint64", &found);
    entry->name = pstrdup("band_bxnor_uint64");
    entry->semiring = GxB_BAND_BXNOR_UINT64;

    entry = semiringhash_insert(semiringhash, "bxor_bor_uint8", &found);
    entry->name = pstrdup("bxor_bor_uint8");
    entry->semiring = GxB_BXOR_BOR_UINT8;

    entry = semiringhash_insert(semiringhash, "bxor_bor_uint16", &found);
    entry->name = pstrdup("bxor_bor_uint16");
    entry->semiring = GxB_BXOR_BOR_UINT16;

    entry = semiringhash_insert(semiringhash, "bxor_bor_uint32", &found);
    entry->name = pstrdup("bxor_bor_uint32");
    entry->semiring = GxB_BXOR_BOR_UINT32;

    entry = semiringhash_insert(semiringhash, "bxor_bor_uint64", &found);
    entry->name = pstrdup("bxor_bor_uint64");
    entry->semiring = GxB_BXOR_BOR_UINT64;

    entry = semiringhash_insert(semiringhash, "bxor_band_uint8", &found);
    entry->name = pstrdup("bxor_band_uint8");
    entry->semiring = GxB_BXOR_BAND_UINT8;

    entry = semiringhash_insert(semiringhash, "bxor_band_uint16", &found);
    entry->name = pstrdup("bxor_band_uint16");
    entry->semiring = GxB_BXOR_BAND_UINT16;

    entry = semiringhash_insert(semiringhash, "bxor_band_uint32", &found);
    entry->name = pstrdup("bxor_band_uint32");
    entry->semiring = GxB_BXOR_BAND_UINT32;

    entry = semiringhash_insert(semiringhash, "bxor_band_uint64", &found);
    entry->name = pstrdup("bxor_band_uint64");
    entry->semiring = GxB_BXOR_BAND_UINT64;

    entry = semiringhash_insert(semiringhash, "bxor_bxor_uint8", &found);
    entry->name = pstrdup("bxor_bxor_uint8");
    entry->semiring = GxB_BXOR_BXOR_UINT8;

    entry = semiringhash_insert(semiringhash, "bxor_bxor_uint16", &found);
    entry->name = pstrdup("bxor_bxor_uint16");
    entry->semiring = GxB_BXOR_BXOR_UINT16;

    entry = semiringhash_insert(semiringhash, "bxor_bxor_uint32", &found);
    entry->name = pstrdup("bxor_bxor_uint32");
    entry->semiring = GxB_BXOR_BXOR_UINT32;

    entry = semiringhash_insert(semiringhash, "bxor_bxor_uint64", &found);
    entry->name = pstrdup("bxor_bxor_uint64");
    entry->semiring = GxB_BXOR_BXOR_UINT64;

    entry = semiringhash_insert(semiringhash, "bxor_bxnor_uint8", &found);
    entry->name = pstrdup("bxor_bxnor_uint8");
    entry->semiring = GxB_BXOR_BXNOR_UINT8;

    entry = semiringhash_insert(semiringhash, "bxor_bxnor_uint16", &found);
    entry->name = pstrdup("bxor_bxnor_uint16");
    entry->semiring = GxB_BXOR_BXNOR_UINT16;

    entry = semiringhash_insert(semiringhash, "bxor_bxnor_uint32", &found);
    entry->name = pstrdup("bxor_bxnor_uint32");
    entry->semiring = GxB_BXOR_BXNOR_UINT32;

    entry = semiringhash_insert(semiringhash, "bxor_bxnor_uint64", &found);
    entry->name = pstrdup("bxor_bxnor_uint64");
    entry->semiring = GxB_BXOR_BXNOR_UINT64;

    entry = semiringhash_insert(semiringhash, "bxnor_bor_uint8", &found);
    entry->name = pstrdup("bxnor_bor_uint8");
    entry->semiring = GxB_BXNOR_BOR_UINT8;

    entry = semiringhash_insert(semiringhash, "bxnor_bor_uint16", &found);
    entry->name = pstrdup("bxnor_bor_uint16");
    entry->semiring = GxB_BXNOR_BOR_UINT16;

    entry = semiringhash_insert(semiringhash, "bxnor_bor_uint32", &found);
    entry->name = pstrdup("bxnor_bor_uint32");
    entry->semiring = GxB_BXNOR_BOR_UINT32;

    entry = semiringhash_insert(semiringhash, "bxnor_bor_uint64", &found);
    entry->name = pstrdup("bxnor_bor_uint64");
    entry->semiring = GxB_BXNOR_BOR_UINT64;

    entry = semiringhash_insert(semiringhash, "bxnor_band_uint8", &found);
    entry->name = pstrdup("bxnor_band_uint8");
    entry->semiring = GxB_BXNOR_BAND_UINT8;

    entry = semiringhash_insert(semiringhash, "bxnor_band_uint16", &found);
    entry->name = pstrdup("bxnor_band_uint16");
    entry->semiring = GxB_BXNOR_BAND_UINT16;

    entry = semiringhash_insert(semiringhash, "bxnor_band_uint32", &found);
    entry->name = pstrdup("bxnor_band_uint32");
    entry->semiring = GxB_BXNOR_BAND_UINT32;

    entry = semiringhash_insert(semiringhash, "bxnor_band_uint64", &found);
    entry->name = pstrdup("bxnor_band_uint64");
    entry->semiring = GxB_BXNOR_BAND_UINT64;

    entry = semiringhash_insert(semiringhash, "bxnor_bxor_uint8", &found);
    entry->name = pstrdup("bxnor_bxor_uint8");
    entry->semiring = GxB_BXNOR_BXOR_UINT8;

    entry = semiringhash_insert(semiringhash, "bxnor_bxor_uint16", &found);
    entry->name = pstrdup("bxnor_bxor_uint16");
    entry->semiring = GxB_BXNOR_BXOR_UINT16;

    entry = semiringhash_insert(semiringhash, "bxnor_bxor_uint32", &found);
    entry->name = pstrdup("bxnor_bxor_uint32");
    entry->semiring = GxB_BXNOR_BXOR_UINT32;

    entry = semiringhash_insert(semiringhash, "bxnor_bxor_uint64", &found);
    entry->name = pstrdup("bxnor_bxor_uint64");
    entry->semiring = GxB_BXNOR_BXOR_UINT64;

    entry = semiringhash_insert(semiringhash, "bxnor_bxnor_uint8", &found);
    entry->name = pstrdup("bxnor_bxnor_uint8");
    entry->semiring = GxB_BXNOR_BXNOR_UINT8;

    entry = semiringhash_insert(semiringhash, "bxnor_bxnor_uint16", &found);
    entry->name = pstrdup("bxnor_bxnor_uint16");
    entry->semiring = GxB_BXNOR_BXNOR_UINT16;

    entry = semiringhash_insert(semiringhash, "bxnor_bxnor_uint32", &found);
    entry->name = pstrdup("bxnor_bxnor_uint32");
    entry->semiring = GxB_BXNOR_BXNOR_UINT32;

    entry = semiringhash_insert(semiringhash, "bxnor_bxnor_uint64", &found);
    entry->name = pstrdup("bxnor_bxnor_uint64");
    entry->semiring = GxB_BXNOR_BXNOR_UINT64;

    entry = semiringhash_insert(semiringhash, "min_firsti_int32", &found);
    entry->name = pstrdup("min_firsti_int32");
    entry->semiring = GxB_MIN_FIRSTI_INT32;

    entry = semiringhash_insert(semiringhash, "min_firsti_int64", &found);
    entry->name = pstrdup("min_firsti_int64");
    entry->semiring = GxB_MIN_FIRSTI_INT64;

    entry = semiringhash_insert(semiringhash, "max_firsti_int32", &found);
    entry->name = pstrdup("max_firsti_int32");
    entry->semiring = GxB_MAX_FIRSTI_INT32;

    entry = semiringhash_insert(semiringhash, "max_firsti_int64", &found);
    entry->name = pstrdup("max_firsti_int64");
    entry->semiring = GxB_MAX_FIRSTI_INT64;

    entry = semiringhash_insert(semiringhash, "any_firsti_int32", &found);
    entry->name = pstrdup("any_firsti_int32");
    entry->semiring = GxB_ANY_FIRSTI_INT32;

    entry = semiringhash_insert(semiringhash, "any_firsti_int64", &found);
    entry->name = pstrdup("any_firsti_int64");
    entry->semiring = GxB_ANY_FIRSTI_INT64;

    entry = semiringhash_insert(semiringhash, "plus_firsti_int32", &found);
    entry->name = pstrdup("plus_firsti_int32");
    entry->semiring = GxB_PLUS_FIRSTI_INT32;

    entry = semiringhash_insert(semiringhash, "plus_firsti_int64", &found);
    entry->name = pstrdup("plus_firsti_int64");
    entry->semiring = GxB_PLUS_FIRSTI_INT64;

    entry = semiringhash_insert(semiringhash, "times_firsti_int32", &found);
    entry->name = pstrdup("times_firsti_int32");
    entry->semiring = GxB_TIMES_FIRSTI_INT32;

    entry = semiringhash_insert(semiringhash, "times_firsti_int64", &found);
    entry->name = pstrdup("times_firsti_int64");
    entry->semiring = GxB_TIMES_FIRSTI_INT64;

    entry = semiringhash_insert(semiringhash, "min_firsti1_int32", &found);
    entry->name = pstrdup("min_firsti1_int32");
    entry->semiring = GxB_MIN_FIRSTI1_INT32;

    entry = semiringhash_insert(semiringhash, "min_firsti1_int64", &found);
    entry->name = pstrdup("min_firsti1_int64");
    entry->semiring = GxB_MIN_FIRSTI1_INT64;

    entry = semiringhash_insert(semiringhash, "max_firsti1_int32", &found);
    entry->name = pstrdup("max_firsti1_int32");
    entry->semiring = GxB_MAX_FIRSTI1_INT32;

    entry = semiringhash_insert(semiringhash, "max_firsti1_int64", &found);
    entry->name = pstrdup("max_firsti1_int64");
    entry->semiring = GxB_MAX_FIRSTI1_INT64;

    entry = semiringhash_insert(semiringhash, "any_firsti1_int32", &found);
    entry->name = pstrdup("any_firsti1_int32");
    entry->semiring = GxB_ANY_FIRSTI1_INT32;

    entry = semiringhash_insert(semiringhash, "any_firsti1_int64", &found);
    entry->name = pstrdup("any_firsti1_int64");
    entry->semiring = GxB_ANY_FIRSTI1_INT64;

    entry = semiringhash_insert(semiringhash, "plus_firsti1_int32", &found);
    entry->name = pstrdup("plus_firsti1_int32");
    entry->semiring = GxB_PLUS_FIRSTI1_INT32;

    entry = semiringhash_insert(semiringhash, "plus_firsti1_int64", &found);
    entry->name = pstrdup("plus_firsti1_int64");
    entry->semiring = GxB_PLUS_FIRSTI1_INT64;

    entry = semiringhash_insert(semiringhash, "times_firsti1_int32", &found);
    entry->name = pstrdup("times_firsti1_int32");
    entry->semiring = GxB_TIMES_FIRSTI1_INT32;

    entry = semiringhash_insert(semiringhash, "times_firsti1_int64", &found);
    entry->name = pstrdup("times_firsti1_int64");
    entry->semiring = GxB_TIMES_FIRSTI1_INT64;

    entry = semiringhash_insert(semiringhash, "min_firstj_int32", &found);
    entry->name = pstrdup("min_firstj_int32");
    entry->semiring = GxB_MIN_FIRSTJ_INT32;

    entry = semiringhash_insert(semiringhash, "min_firstj_int64", &found);
    entry->name = pstrdup("min_firstj_int64");
    entry->semiring = GxB_MIN_FIRSTJ_INT64;

    entry = semiringhash_insert(semiringhash, "max_firstj_int32", &found);
    entry->name = pstrdup("max_firstj_int32");
    entry->semiring = GxB_MAX_FIRSTJ_INT32;

    entry = semiringhash_insert(semiringhash, "max_firstj_int64", &found);
    entry->name = pstrdup("max_firstj_int64");
    entry->semiring = GxB_MAX_FIRSTJ_INT64;

    entry = semiringhash_insert(semiringhash, "any_firstj_int32", &found);
    entry->name = pstrdup("any_firstj_int32");
    entry->semiring = GxB_ANY_FIRSTJ_INT32;

    entry = semiringhash_insert(semiringhash, "any_firstj_int64", &found);
    entry->name = pstrdup("any_firstj_int64");
    entry->semiring = GxB_ANY_FIRSTJ_INT64;

    entry = semiringhash_insert(semiringhash, "plus_firstj_int32", &found);
    entry->name = pstrdup("plus_firstj_int32");
    entry->semiring = GxB_PLUS_FIRSTJ_INT32;

    entry = semiringhash_insert(semiringhash, "plus_firstj_int64", &found);
    entry->name = pstrdup("plus_firstj_int64");
    entry->semiring = GxB_PLUS_FIRSTJ_INT64;

    entry = semiringhash_insert(semiringhash, "times_firstj_int32", &found);
    entry->name = pstrdup("times_firstj_int32");
    entry->semiring = GxB_TIMES_FIRSTJ_INT32;

    entry = semiringhash_insert(semiringhash, "times_firstj_int64", &found);
    entry->name = pstrdup("times_firstj_int64");
    entry->semiring = GxB_TIMES_FIRSTJ_INT64;

    entry = semiringhash_insert(semiringhash, "min_firstj1_int32", &found);
    entry->name = pstrdup("min_firstj1_int32");
    entry->semiring = GxB_MIN_FIRSTJ1_INT32;

    entry = semiringhash_insert(semiringhash, "min_firstj1_int64", &found);
    entry->name = pstrdup("min_firstj1_int64");
    entry->semiring = GxB_MIN_FIRSTJ1_INT64;

    entry = semiringhash_insert(semiringhash, "max_firstj1_int32", &found);
    entry->name = pstrdup("max_firstj1_int32");
    entry->semiring = GxB_MAX_FIRSTJ1_INT32;

    entry = semiringhash_insert(semiringhash, "max_firstj1_int64", &found);
    entry->name = pstrdup("max_firstj1_int64");
    entry->semiring = GxB_MAX_FIRSTJ1_INT64;

    entry = semiringhash_insert(semiringhash, "any_firstj1_int32", &found);
    entry->name = pstrdup("any_firstj1_int32");
    entry->semiring = GxB_ANY_FIRSTJ1_INT32;

    entry = semiringhash_insert(semiringhash, "any_firstj1_int64", &found);
    entry->name = pstrdup("any_firstj1_int64");
    entry->semiring = GxB_ANY_FIRSTJ1_INT64;

    entry = semiringhash_insert(semiringhash, "plus_firstj1_int32", &found);
    entry->name = pstrdup("plus_firstj1_int32");
    entry->semiring = GxB_PLUS_FIRSTJ1_INT32;

    entry = semiringhash_insert(semiringhash, "plus_firstj1_int64", &found);
    entry->name = pstrdup("plus_firstj1_int64");
    entry->semiring = GxB_PLUS_FIRSTJ1_INT64;

    entry = semiringhash_insert(semiringhash, "times_firstj1_int32", &found);
    entry->name = pstrdup("times_firstj1_int32");
    entry->semiring = GxB_TIMES_FIRSTJ1_INT32;

    entry = semiringhash_insert(semiringhash, "times_firstj1_int64", &found);
    entry->name = pstrdup("times_firstj1_int64");
    entry->semiring = GxB_TIMES_FIRSTJ1_INT64;

    entry = semiringhash_insert(semiringhash, "min_secondi_int32", &found);
    entry->name = pstrdup("min_secondi_int32");
    entry->semiring = GxB_MIN_SECONDI_INT32;

    entry = semiringhash_insert(semiringhash, "min_secondi_int64", &found);
    entry->name = pstrdup("min_secondi_int64");
    entry->semiring = GxB_MIN_SECONDI_INT64;

    entry = semiringhash_insert(semiringhash, "max_secondi_int32", &found);
    entry->name = pstrdup("max_secondi_int32");
    entry->semiring = GxB_MAX_SECONDI_INT32;

    entry = semiringhash_insert(semiringhash, "max_secondi_int64", &found);
    entry->name = pstrdup("max_secondi_int64");
    entry->semiring = GxB_MAX_SECONDI_INT64;

    entry = semiringhash_insert(semiringhash, "any_secondi_int32", &found);
    entry->name = pstrdup("any_secondi_int32");
    entry->semiring = GxB_ANY_SECONDI_INT32;

    entry = semiringhash_insert(semiringhash, "any_secondi_int64", &found);
    entry->name = pstrdup("any_secondi_int64");
    entry->semiring = GxB_ANY_SECONDI_INT64;

    entry = semiringhash_insert(semiringhash, "plus_secondi_int32", &found);
    entry->name = pstrdup("plus_secondi_int32");
    entry->semiring = GxB_PLUS_SECONDI_INT32;

    entry = semiringhash_insert(semiringhash, "plus_secondi_int64", &found);
    entry->name = pstrdup("plus_secondi_int64");
    entry->semiring = GxB_PLUS_SECONDI_INT64;

    entry = semiringhash_insert(semiringhash, "times_secondi_int32", &found);
    entry->name = pstrdup("times_secondi_int32");
    entry->semiring = GxB_TIMES_SECONDI_INT32;

    entry = semiringhash_insert(semiringhash, "times_secondi_int64", &found);
    entry->name = pstrdup("times_secondi_int64");
    entry->semiring = GxB_TIMES_SECONDI_INT64;

    entry = semiringhash_insert(semiringhash, "min_secondi1_int32", &found);
    entry->name = pstrdup("min_secondi1_int32");
    entry->semiring = GxB_MIN_SECONDI1_INT32;

    entry = semiringhash_insert(semiringhash, "min_secondi1_int64", &found);
    entry->name = pstrdup("min_secondi1_int64");
    entry->semiring = GxB_MIN_SECONDI1_INT64;

    entry = semiringhash_insert(semiringhash, "max_secondi1_int32", &found);
    entry->name = pstrdup("max_secondi1_int32");
    entry->semiring = GxB_MAX_SECONDI1_INT32;

    entry = semiringhash_insert(semiringhash, "max_secondi1_int64", &found);
    entry->name = pstrdup("max_secondi1_int64");
    entry->semiring = GxB_MAX_SECONDI1_INT64;

    entry = semiringhash_insert(semiringhash, "any_secondi1_int32", &found);
    entry->name = pstrdup("any_secondi1_int32");
    entry->semiring = GxB_ANY_SECONDI1_INT32;

    entry = semiringhash_insert(semiringhash, "any_secondi1_int64", &found);
    entry->name = pstrdup("any_secondi1_int64");
    entry->semiring = GxB_ANY_SECONDI1_INT64;

    entry = semiringhash_insert(semiringhash, "plus_secondi1_int32", &found);
    entry->name = pstrdup("plus_secondi1_int32");
    entry->semiring = GxB_PLUS_SECONDI1_INT32;

    entry = semiringhash_insert(semiringhash, "plus_secondi1_int64", &found);
    entry->name = pstrdup("plus_secondi1_int64");
    entry->semiring = GxB_PLUS_SECONDI1_INT64;

    entry = semiringhash_insert(semiringhash, "times_secondi1_int32", &found);
    entry->name = pstrdup("times_secondi1_int32");
    entry->semiring = GxB_TIMES_SECONDI1_INT32;

    entry = semiringhash_insert(semiringhash, "times_secondi1_int64", &found);
    entry->name = pstrdup("times_secondi1_int64");
    entry->semiring = GxB_TIMES_SECONDI1_INT64;

    entry = semiringhash_insert(semiringhash, "min_secondj_int32", &found);
    entry->name = pstrdup("min_secondj_int32");
    entry->semiring = GxB_MIN_SECONDJ_INT32;

    entry = semiringhash_insert(semiringhash, "min_secondj_int64", &found);
    entry->name = pstrdup("min_secondj_int64");
    entry->semiring = GxB_MIN_SECONDJ_INT64;

    entry = semiringhash_insert(semiringhash, "max_secondj_int32", &found);
    entry->name = pstrdup("max_secondj_int32");
    entry->semiring = GxB_MAX_SECONDJ_INT32;

    entry = semiringhash_insert(semiringhash, "max_secondj_int64", &found);
    entry->name = pstrdup("max_secondj_int64");
    entry->semiring = GxB_MAX_SECONDJ_INT64;

    entry = semiringhash_insert(semiringhash, "any_secondj_int32", &found);
    entry->name = pstrdup("any_secondj_int32");
    entry->semiring = GxB_ANY_SECONDJ_INT32;

    entry = semiringhash_insert(semiringhash, "any_secondj_int64", &found);
    entry->name = pstrdup("any_secondj_int64");
    entry->semiring = GxB_ANY_SECONDJ_INT64;

    entry = semiringhash_insert(semiringhash, "plus_secondj_int32", &found);
    entry->name = pstrdup("plus_secondj_int32");
    entry->semiring = GxB_PLUS_SECONDJ_INT32;

    entry = semiringhash_insert(semiringhash, "plus_secondj_int64", &found);
    entry->name = pstrdup("plus_secondj_int64");
    entry->semiring = GxB_PLUS_SECONDJ_INT64;

    entry = semiringhash_insert(semiringhash, "times_secondj_int32", &found);
    entry->name = pstrdup("times_secondj_int32");
    entry->semiring = GxB_TIMES_SECONDJ_INT32;

    entry = semiringhash_insert(semiringhash, "times_secondj_int64", &found);
    entry->name = pstrdup("times_secondj_int64");
    entry->semiring = GxB_TIMES_SECONDJ_INT64;

    entry = semiringhash_insert(semiringhash, "min_secondj1_int32", &found);
    entry->name = pstrdup("min_secondj1_int32");
    entry->semiring = GxB_MIN_SECONDJ1_INT32;

    entry = semiringhash_insert(semiringhash, "min_secondj1_int64", &found);
    entry->name = pstrdup("min_secondj1_int64");
    entry->semiring = GxB_MIN_SECONDJ1_INT64;

    entry = semiringhash_insert(semiringhash, "max_secondj1_int32", &found);
    entry->name = pstrdup("max_secondj1_int32");
    entry->semiring = GxB_MAX_SECONDJ1_INT32;

    entry = semiringhash_insert(semiringhash, "max_secondj1_int64", &found);
    entry->name = pstrdup("max_secondj1_int64");
    entry->semiring = GxB_MAX_SECONDJ1_INT64;

    entry = semiringhash_insert(semiringhash, "any_secondj1_int32", &found);
    entry->name = pstrdup("any_secondj1_int32");
    entry->semiring = GxB_ANY_SECONDJ1_INT32;

    entry = semiringhash_insert(semiringhash, "any_secondj1_int64", &found);
    entry->name = pstrdup("any_secondj1_int64");
    entry->semiring = GxB_ANY_SECONDJ1_INT64;

    entry = semiringhash_insert(semiringhash, "plus_secondj1_int32", &found);
    entry->name = pstrdup("plus_secondj1_int32");
    entry->semiring = GxB_PLUS_SECONDJ1_INT32;

    entry = semiringhash_insert(semiringhash, "plus_secondj1_int64", &found);
    entry->name = pstrdup("plus_secondj1_int64");
    entry->semiring = GxB_PLUS_SECONDJ1_INT64;

    entry = semiringhash_insert(semiringhash, "times_secondj1_int32", &found);
    entry->name = pstrdup("times_secondj1_int32");
    entry->semiring = GxB_TIMES_SECONDJ1_INT32;

    entry = semiringhash_insert(semiringhash, "times_secondj1_int64", &found);
    entry->name = pstrdup("times_secondj1_int64");
    entry->semiring = GxB_TIMES_SECONDJ1_INT64;

    entry = semiringhash_insert(semiringhash, "plus_times_semiring_int8", &found);
    entry->name = pstrdup("plus_times_semiring_int8");
    entry->semiring = GrB_PLUS_TIMES_SEMIRING_INT8;

    entry = semiringhash_insert(semiringhash, "plus_times_semiring_int16", &found);
    entry->name = pstrdup("plus_times_semiring_int16");
    entry->semiring = GrB_PLUS_TIMES_SEMIRING_INT16;

    entry = semiringhash_insert(semiringhash, "plus_times_semiring_int32", &found);
    entry->name = pstrdup("plus_times_semiring_int32");
    entry->semiring = GrB_PLUS_TIMES_SEMIRING_INT32;

    entry = semiringhash_insert(semiringhash, "plus_times_semiring_int64", &found);
    entry->name = pstrdup("plus_times_semiring_int64");
    entry->semiring = GrB_PLUS_TIMES_SEMIRING_INT64;

    entry = semiringhash_insert(semiringhash, "plus_times_semiring_uint8", &found);
    entry->name = pstrdup("plus_times_semiring_uint8");
    entry->semiring = GrB_PLUS_TIMES_SEMIRING_UINT8;

    entry = semiringhash_insert(semiringhash, "plus_times_semiring_uint16", &found);
    entry->name = pstrdup("plus_times_semiring_uint16");
    entry->semiring = GrB_PLUS_TIMES_SEMIRING_UINT16;

    entry = semiringhash_insert(semiringhash, "plus_times_semiring_uint32", &found);
    entry->name = pstrdup("plus_times_semiring_uint32");
    entry->semiring = GrB_PLUS_TIMES_SEMIRING_UINT32;

    entry = semiringhash_insert(semiringhash, "plus_times_semiring_uint64", &found);
    entry->name = pstrdup("plus_times_semiring_uint64");
    entry->semiring = GrB_PLUS_TIMES_SEMIRING_UINT64;

    entry = semiringhash_insert(semiringhash, "plus_times_semiring_fp32", &found);
    entry->name = pstrdup("plus_times_semiring_fp32");
    entry->semiring = GrB_PLUS_TIMES_SEMIRING_FP32;

    entry = semiringhash_insert(semiringhash, "plus_times_semiring_fp64", &found);
    entry->name = pstrdup("plus_times_semiring_fp64");
    entry->semiring = GrB_PLUS_TIMES_SEMIRING_FP64;

    entry = semiringhash_insert(semiringhash, "plus_min_semiring_int8", &found);
    entry->name = pstrdup("plus_min_semiring_int8");
    entry->semiring = GrB_PLUS_MIN_SEMIRING_INT8;

    entry = semiringhash_insert(semiringhash, "plus_min_semiring_int16", &found);
    entry->name = pstrdup("plus_min_semiring_int16");
    entry->semiring = GrB_PLUS_MIN_SEMIRING_INT16;

    entry = semiringhash_insert(semiringhash, "plus_min_semiring_int32", &found);
    entry->name = pstrdup("plus_min_semiring_int32");
    entry->semiring = GrB_PLUS_MIN_SEMIRING_INT32;

    entry = semiringhash_insert(semiringhash, "plus_min_semiring_int64", &found);
    entry->name = pstrdup("plus_min_semiring_int64");
    entry->semiring = GrB_PLUS_MIN_SEMIRING_INT64;

    entry = semiringhash_insert(semiringhash, "plus_min_semiring_uint8", &found);
    entry->name = pstrdup("plus_min_semiring_uint8");
    entry->semiring = GrB_PLUS_MIN_SEMIRING_UINT8;

    entry = semiringhash_insert(semiringhash, "plus_min_semiring_uint16", &found);
    entry->name = pstrdup("plus_min_semiring_uint16");
    entry->semiring = GrB_PLUS_MIN_SEMIRING_UINT16;

    entry = semiringhash_insert(semiringhash, "plus_min_semiring_uint32", &found);
    entry->name = pstrdup("plus_min_semiring_uint32");
    entry->semiring = GrB_PLUS_MIN_SEMIRING_UINT32;

    entry = semiringhash_insert(semiringhash, "plus_min_semiring_uint64", &found);
    entry->name = pstrdup("plus_min_semiring_uint64");
    entry->semiring = GrB_PLUS_MIN_SEMIRING_UINT64;

    entry = semiringhash_insert(semiringhash, "plus_min_semiring_fp32", &found);
    entry->name = pstrdup("plus_min_semiring_fp32");
    entry->semiring = GrB_PLUS_MIN_SEMIRING_FP32;

    entry = semiringhash_insert(semiringhash, "plus_min_semiring_fp64", &found);
    entry->name = pstrdup("plus_min_semiring_fp64");
    entry->semiring = GrB_PLUS_MIN_SEMIRING_FP64;

    entry = semiringhash_insert(semiringhash, "min_plus_semiring_int8", &found);
    entry->name = pstrdup("min_plus_semiring_int8");
    entry->semiring = GrB_MIN_PLUS_SEMIRING_INT8;

    entry = semiringhash_insert(semiringhash, "min_plus_semiring_int16", &found);
    entry->name = pstrdup("min_plus_semiring_int16");
    entry->semiring = GrB_MIN_PLUS_SEMIRING_INT16;

    entry = semiringhash_insert(semiringhash, "min_plus_semiring_int32", &found);
    entry->name = pstrdup("min_plus_semiring_int32");
    entry->semiring = GrB_MIN_PLUS_SEMIRING_INT32;

    entry = semiringhash_insert(semiringhash, "min_plus_semiring_int64", &found);
    entry->name = pstrdup("min_plus_semiring_int64");
    entry->semiring = GrB_MIN_PLUS_SEMIRING_INT64;

    entry = semiringhash_insert(semiringhash, "min_plus_semiring_uint8", &found);
    entry->name = pstrdup("min_plus_semiring_uint8");
    entry->semiring = GrB_MIN_PLUS_SEMIRING_UINT8;

    entry = semiringhash_insert(semiringhash, "min_plus_semiring_uint16", &found);
    entry->name = pstrdup("min_plus_semiring_uint16");
    entry->semiring = GrB_MIN_PLUS_SEMIRING_UINT16;

    entry = semiringhash_insert(semiringhash, "min_plus_semiring_uint32", &found);
    entry->name = pstrdup("min_plus_semiring_uint32");
    entry->semiring = GrB_MIN_PLUS_SEMIRING_UINT32;

    entry = semiringhash_insert(semiringhash, "min_plus_semiring_uint64", &found);
    entry->name = pstrdup("min_plus_semiring_uint64");
    entry->semiring = GrB_MIN_PLUS_SEMIRING_UINT64;

    entry = semiringhash_insert(semiringhash, "min_plus_semiring_fp32", &found);
    entry->name = pstrdup("min_plus_semiring_fp32");
    entry->semiring = GrB_MIN_PLUS_SEMIRING_FP32;

    entry = semiringhash_insert(semiringhash, "min_plus_semiring_fp64", &found);
    entry->name = pstrdup("min_plus_semiring_fp64");
    entry->semiring = GrB_MIN_PLUS_SEMIRING_FP64;

    entry = semiringhash_insert(semiringhash, "min_times_semiring_int8", &found);
    entry->name = pstrdup("min_times_semiring_int8");
    entry->semiring = GrB_MIN_TIMES_SEMIRING_INT8;

    entry = semiringhash_insert(semiringhash, "min_times_semiring_int16", &found);
    entry->name = pstrdup("min_times_semiring_int16");
    entry->semiring = GrB_MIN_TIMES_SEMIRING_INT16;

    entry = semiringhash_insert(semiringhash, "min_times_semiring_int32", &found);
    entry->name = pstrdup("min_times_semiring_int32");
    entry->semiring = GrB_MIN_TIMES_SEMIRING_INT32;

    entry = semiringhash_insert(semiringhash, "min_times_semiring_int64", &found);
    entry->name = pstrdup("min_times_semiring_int64");
    entry->semiring = GrB_MIN_TIMES_SEMIRING_INT64;

    entry = semiringhash_insert(semiringhash, "min_times_semiring_uint8", &found);
    entry->name = pstrdup("min_times_semiring_uint8");
    entry->semiring = GrB_MIN_TIMES_SEMIRING_UINT8;

    entry = semiringhash_insert(semiringhash, "min_times_semiring_uint16", &found);
    entry->name = pstrdup("min_times_semiring_uint16");
    entry->semiring = GrB_MIN_TIMES_SEMIRING_UINT16;

    entry = semiringhash_insert(semiringhash, "min_times_semiring_uint32", &found);
    entry->name = pstrdup("min_times_semiring_uint32");
    entry->semiring = GrB_MIN_TIMES_SEMIRING_UINT32;

    entry = semiringhash_insert(semiringhash, "min_times_semiring_uint64", &found);
    entry->name = pstrdup("min_times_semiring_uint64");
    entry->semiring = GrB_MIN_TIMES_SEMIRING_UINT64;

    entry = semiringhash_insert(semiringhash, "min_times_semiring_fp32", &found);
    entry->name = pstrdup("min_times_semiring_fp32");
    entry->semiring = GrB_MIN_TIMES_SEMIRING_FP32;

    entry = semiringhash_insert(semiringhash, "min_times_semiring_fp64", &found);
    entry->name = pstrdup("min_times_semiring_fp64");
    entry->semiring = GrB_MIN_TIMES_SEMIRING_FP64;

    entry = semiringhash_insert(semiringhash, "min_first_semiring_int8", &found);
    entry->name = pstrdup("min_first_semiring_int8");
    entry->semiring = GrB_MIN_FIRST_SEMIRING_INT8;

    entry = semiringhash_insert(semiringhash, "min_first_semiring_int16", &found);
    entry->name = pstrdup("min_first_semiring_int16");
    entry->semiring = GrB_MIN_FIRST_SEMIRING_INT16;

    entry = semiringhash_insert(semiringhash, "min_first_semiring_int32", &found);
    entry->name = pstrdup("min_first_semiring_int32");
    entry->semiring = GrB_MIN_FIRST_SEMIRING_INT32;

    entry = semiringhash_insert(semiringhash, "min_first_semiring_int64", &found);
    entry->name = pstrdup("min_first_semiring_int64");
    entry->semiring = GrB_MIN_FIRST_SEMIRING_INT64;

    entry = semiringhash_insert(semiringhash, "min_first_semiring_uint8", &found);
    entry->name = pstrdup("min_first_semiring_uint8");
    entry->semiring = GrB_MIN_FIRST_SEMIRING_UINT8;

    entry = semiringhash_insert(semiringhash, "min_first_semiring_uint16", &found);
    entry->name = pstrdup("min_first_semiring_uint16");
    entry->semiring = GrB_MIN_FIRST_SEMIRING_UINT16;

    entry = semiringhash_insert(semiringhash, "min_first_semiring_uint32", &found);
    entry->name = pstrdup("min_first_semiring_uint32");
    entry->semiring = GrB_MIN_FIRST_SEMIRING_UINT32;

    entry = semiringhash_insert(semiringhash, "min_first_semiring_uint64", &found);
    entry->name = pstrdup("min_first_semiring_uint64");
    entry->semiring = GrB_MIN_FIRST_SEMIRING_UINT64;

    entry = semiringhash_insert(semiringhash, "min_first_semiring_fp32", &found);
    entry->name = pstrdup("min_first_semiring_fp32");
    entry->semiring = GrB_MIN_FIRST_SEMIRING_FP32;

    entry = semiringhash_insert(semiringhash, "min_first_semiring_fp64", &found);
    entry->name = pstrdup("min_first_semiring_fp64");
    entry->semiring = GrB_MIN_FIRST_SEMIRING_FP64;

    entry = semiringhash_insert(semiringhash, "min_second_semiring_int8", &found);
    entry->name = pstrdup("min_second_semiring_int8");
    entry->semiring = GrB_MIN_SECOND_SEMIRING_INT8;

    entry = semiringhash_insert(semiringhash, "min_second_semiring_int16", &found);
    entry->name = pstrdup("min_second_semiring_int16");
    entry->semiring = GrB_MIN_SECOND_SEMIRING_INT16;

    entry = semiringhash_insert(semiringhash, "min_second_semiring_int32", &found);
    entry->name = pstrdup("min_second_semiring_int32");
    entry->semiring = GrB_MIN_SECOND_SEMIRING_INT32;

    entry = semiringhash_insert(semiringhash, "min_second_semiring_int64", &found);
    entry->name = pstrdup("min_second_semiring_int64");
    entry->semiring = GrB_MIN_SECOND_SEMIRING_INT64;

    entry = semiringhash_insert(semiringhash, "min_second_semiring_uint8", &found);
    entry->name = pstrdup("min_second_semiring_uint8");
    entry->semiring = GrB_MIN_SECOND_SEMIRING_UINT8;

    entry = semiringhash_insert(semiringhash, "min_second_semiring_uint16", &found);
    entry->name = pstrdup("min_second_semiring_uint16");
    entry->semiring = GrB_MIN_SECOND_SEMIRING_UINT16;

    entry = semiringhash_insert(semiringhash, "min_second_semiring_uint32", &found);
    entry->name = pstrdup("min_second_semiring_uint32");
    entry->semiring = GrB_MIN_SECOND_SEMIRING_UINT32;

    entry = semiringhash_insert(semiringhash, "min_second_semiring_uint64", &found);
    entry->name = pstrdup("min_second_semiring_uint64");
    entry->semiring = GrB_MIN_SECOND_SEMIRING_UINT64;

    entry = semiringhash_insert(semiringhash, "min_second_semiring_fp32", &found);
    entry->name = pstrdup("min_second_semiring_fp32");
    entry->semiring = GrB_MIN_SECOND_SEMIRING_FP32;

    entry = semiringhash_insert(semiringhash, "min_second_semiring_fp64", &found);
    entry->name = pstrdup("min_second_semiring_fp64");
    entry->semiring = GrB_MIN_SECOND_SEMIRING_FP64;

    entry = semiringhash_insert(semiringhash, "min_max_semiring_int8", &found);
    entry->name = pstrdup("min_max_semiring_int8");
    entry->semiring = GrB_MIN_MAX_SEMIRING_INT8;

    entry = semiringhash_insert(semiringhash, "min_max_semiring_int16", &found);
    entry->name = pstrdup("min_max_semiring_int16");
    entry->semiring = GrB_MIN_MAX_SEMIRING_INT16;

    entry = semiringhash_insert(semiringhash, "min_max_semiring_int32", &found);
    entry->name = pstrdup("min_max_semiring_int32");
    entry->semiring = GrB_MIN_MAX_SEMIRING_INT32;

    entry = semiringhash_insert(semiringhash, "min_max_semiring_int64", &found);
    entry->name = pstrdup("min_max_semiring_int64");
    entry->semiring = GrB_MIN_MAX_SEMIRING_INT64;

    entry = semiringhash_insert(semiringhash, "min_max_semiring_uint8", &found);
    entry->name = pstrdup("min_max_semiring_uint8");
    entry->semiring = GrB_MIN_MAX_SEMIRING_UINT8;

    entry = semiringhash_insert(semiringhash, "min_max_semiring_uint16", &found);
    entry->name = pstrdup("min_max_semiring_uint16");
    entry->semiring = GrB_MIN_MAX_SEMIRING_UINT16;

    entry = semiringhash_insert(semiringhash, "min_max_semiring_uint32", &found);
    entry->name = pstrdup("min_max_semiring_uint32");
    entry->semiring = GrB_MIN_MAX_SEMIRING_UINT32;

    entry = semiringhash_insert(semiringhash, "min_max_semiring_uint64", &found);
    entry->name = pstrdup("min_max_semiring_uint64");
    entry->semiring = GrB_MIN_MAX_SEMIRING_UINT64;

    entry = semiringhash_insert(semiringhash, "min_max_semiring_fp32", &found);
    entry->name = pstrdup("min_max_semiring_fp32");
    entry->semiring = GrB_MIN_MAX_SEMIRING_FP32;

    entry = semiringhash_insert(semiringhash, "min_max_semiring_fp64", &found);
    entry->name = pstrdup("min_max_semiring_fp64");
    entry->semiring = GrB_MIN_MAX_SEMIRING_FP64;

    entry = semiringhash_insert(semiringhash, "max_plus_semiring_int8", &found);
    entry->name = pstrdup("max_plus_semiring_int8");
    entry->semiring = GrB_MAX_PLUS_SEMIRING_INT8;

    entry = semiringhash_insert(semiringhash, "max_plus_semiring_int16", &found);
    entry->name = pstrdup("max_plus_semiring_int16");
    entry->semiring = GrB_MAX_PLUS_SEMIRING_INT16;

    entry = semiringhash_insert(semiringhash, "max_plus_semiring_int32", &found);
    entry->name = pstrdup("max_plus_semiring_int32");
    entry->semiring = GrB_MAX_PLUS_SEMIRING_INT32;

    entry = semiringhash_insert(semiringhash, "max_plus_semiring_int64", &found);
    entry->name = pstrdup("max_plus_semiring_int64");
    entry->semiring = GrB_MAX_PLUS_SEMIRING_INT64;

    entry = semiringhash_insert(semiringhash, "max_plus_semiring_uint8", &found);
    entry->name = pstrdup("max_plus_semiring_uint8");
    entry->semiring = GrB_MAX_PLUS_SEMIRING_UINT8;

    entry = semiringhash_insert(semiringhash, "max_plus_semiring_uint16", &found);
    entry->name = pstrdup("max_plus_semiring_uint16");
    entry->semiring = GrB_MAX_PLUS_SEMIRING_UINT16;

    entry = semiringhash_insert(semiringhash, "max_plus_semiring_uint32", &found);
    entry->name = pstrdup("max_plus_semiring_uint32");
    entry->semiring = GrB_MAX_PLUS_SEMIRING_UINT32;

    entry = semiringhash_insert(semiringhash, "max_plus_semiring_uint64", &found);
    entry->name = pstrdup("max_plus_semiring_uint64");
    entry->semiring = GrB_MAX_PLUS_SEMIRING_UINT64;

    entry = semiringhash_insert(semiringhash, "max_plus_semiring_fp32", &found);
    entry->name = pstrdup("max_plus_semiring_fp32");
    entry->semiring = GrB_MAX_PLUS_SEMIRING_FP32;

    entry = semiringhash_insert(semiringhash, "max_plus_semiring_fp64", &found);
    entry->name = pstrdup("max_plus_semiring_fp64");
    entry->semiring = GrB_MAX_PLUS_SEMIRING_FP64;

    entry = semiringhash_insert(semiringhash, "max_times_semiring_int8", &found);
    entry->name = pstrdup("max_times_semiring_int8");
    entry->semiring = GrB_MAX_TIMES_SEMIRING_INT8;

    entry = semiringhash_insert(semiringhash, "max_times_semiring_int16", &found);
    entry->name = pstrdup("max_times_semiring_int16");
    entry->semiring = GrB_MAX_TIMES_SEMIRING_INT16;

    entry = semiringhash_insert(semiringhash, "max_times_semiring_int32", &found);
    entry->name = pstrdup("max_times_semiring_int32");
    entry->semiring = GrB_MAX_TIMES_SEMIRING_INT32;

    entry = semiringhash_insert(semiringhash, "max_times_semiring_int64", &found);
    entry->name = pstrdup("max_times_semiring_int64");
    entry->semiring = GrB_MAX_TIMES_SEMIRING_INT64;

    entry = semiringhash_insert(semiringhash, "max_times_semiring_uint8", &found);
    entry->name = pstrdup("max_times_semiring_uint8");
    entry->semiring = GrB_MAX_TIMES_SEMIRING_UINT8;

    entry = semiringhash_insert(semiringhash, "max_times_semiring_uint16", &found);
    entry->name = pstrdup("max_times_semiring_uint16");
    entry->semiring = GrB_MAX_TIMES_SEMIRING_UINT16;

    entry = semiringhash_insert(semiringhash, "max_times_semiring_uint32", &found);
    entry->name = pstrdup("max_times_semiring_uint32");
    entry->semiring = GrB_MAX_TIMES_SEMIRING_UINT32;

    entry = semiringhash_insert(semiringhash, "max_times_semiring_uint64", &found);
    entry->name = pstrdup("max_times_semiring_uint64");
    entry->semiring = GrB_MAX_TIMES_SEMIRING_UINT64;

    entry = semiringhash_insert(semiringhash, "max_times_semiring_fp32", &found);
    entry->name = pstrdup("max_times_semiring_fp32");
    entry->semiring = GrB_MAX_TIMES_SEMIRING_FP32;

    entry = semiringhash_insert(semiringhash, "max_times_semiring_fp64", &found);
    entry->name = pstrdup("max_times_semiring_fp64");
    entry->semiring = GrB_MAX_TIMES_SEMIRING_FP64;

    entry = semiringhash_insert(semiringhash, "max_first_semiring_int8", &found);
    entry->name = pstrdup("max_first_semiring_int8");
    entry->semiring = GrB_MAX_FIRST_SEMIRING_INT8;

    entry = semiringhash_insert(semiringhash, "max_first_semiring_int16", &found);
    entry->name = pstrdup("max_first_semiring_int16");
    entry->semiring = GrB_MAX_FIRST_SEMIRING_INT16;

    entry = semiringhash_insert(semiringhash, "max_first_semiring_int32", &found);
    entry->name = pstrdup("max_first_semiring_int32");
    entry->semiring = GrB_MAX_FIRST_SEMIRING_INT32;

    entry = semiringhash_insert(semiringhash, "max_first_semiring_int64", &found);
    entry->name = pstrdup("max_first_semiring_int64");
    entry->semiring = GrB_MAX_FIRST_SEMIRING_INT64;

    entry = semiringhash_insert(semiringhash, "max_first_semiring_uint8", &found);
    entry->name = pstrdup("max_first_semiring_uint8");
    entry->semiring = GrB_MAX_FIRST_SEMIRING_UINT8;

    entry = semiringhash_insert(semiringhash, "max_first_semiring_uint16", &found);
    entry->name = pstrdup("max_first_semiring_uint16");
    entry->semiring = GrB_MAX_FIRST_SEMIRING_UINT16;

    entry = semiringhash_insert(semiringhash, "max_first_semiring_uint32", &found);
    entry->name = pstrdup("max_first_semiring_uint32");
    entry->semiring = GrB_MAX_FIRST_SEMIRING_UINT32;

    entry = semiringhash_insert(semiringhash, "max_first_semiring_uint64", &found);
    entry->name = pstrdup("max_first_semiring_uint64");
    entry->semiring = GrB_MAX_FIRST_SEMIRING_UINT64;

    entry = semiringhash_insert(semiringhash, "max_first_semiring_fp32", &found);
    entry->name = pstrdup("max_first_semiring_fp32");
    entry->semiring = GrB_MAX_FIRST_SEMIRING_FP32;

    entry = semiringhash_insert(semiringhash, "max_first_semiring_fp64", &found);
    entry->name = pstrdup("max_first_semiring_fp64");
    entry->semiring = GrB_MAX_FIRST_SEMIRING_FP64;

    entry = semiringhash_insert(semiringhash, "max_second_semiring_int8", &found);
    entry->name = pstrdup("max_second_semiring_int8");
    entry->semiring = GrB_MAX_SECOND_SEMIRING_INT8;

    entry = semiringhash_insert(semiringhash, "max_second_semiring_int16", &found);
    entry->name = pstrdup("max_second_semiring_int16");
    entry->semiring = GrB_MAX_SECOND_SEMIRING_INT16;

    entry = semiringhash_insert(semiringhash, "max_second_semiring_int32", &found);
    entry->name = pstrdup("max_second_semiring_int32");
    entry->semiring = GrB_MAX_SECOND_SEMIRING_INT32;

    entry = semiringhash_insert(semiringhash, "max_second_semiring_int64", &found);
    entry->name = pstrdup("max_second_semiring_int64");
    entry->semiring = GrB_MAX_SECOND_SEMIRING_INT64;

    entry = semiringhash_insert(semiringhash, "max_second_semiring_uint8", &found);
    entry->name = pstrdup("max_second_semiring_uint8");
    entry->semiring = GrB_MAX_SECOND_SEMIRING_UINT8;

    entry = semiringhash_insert(semiringhash, "max_second_semiring_uint16", &found);
    entry->name = pstrdup("max_second_semiring_uint16");
    entry->semiring = GrB_MAX_SECOND_SEMIRING_UINT16;

    entry = semiringhash_insert(semiringhash, "max_second_semiring_uint32", &found);
    entry->name = pstrdup("max_second_semiring_uint32");
    entry->semiring = GrB_MAX_SECOND_SEMIRING_UINT32;

    entry = semiringhash_insert(semiringhash, "max_second_semiring_uint64", &found);
    entry->name = pstrdup("max_second_semiring_uint64");
    entry->semiring = GrB_MAX_SECOND_SEMIRING_UINT64;

    entry = semiringhash_insert(semiringhash, "max_second_semiring_fp32", &found);
    entry->name = pstrdup("max_second_semiring_fp32");
    entry->semiring = GrB_MAX_SECOND_SEMIRING_FP32;

    entry = semiringhash_insert(semiringhash, "max_second_semiring_fp64", &found);
    entry->name = pstrdup("max_second_semiring_fp64");
    entry->semiring = GrB_MAX_SECOND_SEMIRING_FP64;

    entry = semiringhash_insert(semiringhash, "max_min_semiring_int8", &found);
    entry->name = pstrdup("max_min_semiring_int8");
    entry->semiring = GrB_MAX_MIN_SEMIRING_INT8;

    entry = semiringhash_insert(semiringhash, "max_min_semiring_int16", &found);
    entry->name = pstrdup("max_min_semiring_int16");
    entry->semiring = GrB_MAX_MIN_SEMIRING_INT16;

    entry = semiringhash_insert(semiringhash, "max_min_semiring_int32", &found);
    entry->name = pstrdup("max_min_semiring_int32");
    entry->semiring = GrB_MAX_MIN_SEMIRING_INT32;

    entry = semiringhash_insert(semiringhash, "max_min_semiring_int64", &found);
    entry->name = pstrdup("max_min_semiring_int64");
    entry->semiring = GrB_MAX_MIN_SEMIRING_INT64;

    entry = semiringhash_insert(semiringhash, "max_min_semiring_uint8", &found);
    entry->name = pstrdup("max_min_semiring_uint8");
    entry->semiring = GrB_MAX_MIN_SEMIRING_UINT8;

    entry = semiringhash_insert(semiringhash, "max_min_semiring_uint16", &found);
    entry->name = pstrdup("max_min_semiring_uint16");
    entry->semiring = GrB_MAX_MIN_SEMIRING_UINT16;

    entry = semiringhash_insert(semiringhash, "max_min_semiring_uint32", &found);
    entry->name = pstrdup("max_min_semiring_uint32");
    entry->semiring = GrB_MAX_MIN_SEMIRING_UINT32;

    entry = semiringhash_insert(semiringhash, "max_min_semiring_uint64", &found);
    entry->name = pstrdup("max_min_semiring_uint64");
    entry->semiring = GrB_MAX_MIN_SEMIRING_UINT64;

    entry = semiringhash_insert(semiringhash, "max_min_semiring_fp32", &found);
    entry->name = pstrdup("max_min_semiring_fp32");
    entry->semiring = GrB_MAX_MIN_SEMIRING_FP32;

    entry = semiringhash_insert(semiringhash, "max_min_semiring_fp64", &found);
    entry->name = pstrdup("max_min_semiring_fp64");
    entry->semiring = GrB_MAX_MIN_SEMIRING_FP64;

    entry = semiringhash_insert(semiringhash, "lor_land_semiring_bool", &found);
    entry->name = pstrdup("lor_land_semiring_bool");
    entry->semiring = GrB_LOR_LAND_SEMIRING_BOOL;

    entry = semiringhash_insert(semiringhash, "land_lor_semiring_bool", &found);
    entry->name = pstrdup("land_lor_semiring_bool");
    entry->semiring = GrB_LAND_LOR_SEMIRING_BOOL;

    entry = semiringhash_insert(semiringhash, "lxor_land_semiring_bool", &found);
    entry->name = pstrdup("lxor_land_semiring_bool");
    entry->semiring = GrB_LXOR_LAND_SEMIRING_BOOL;

    entry = semiringhash_insert(semiringhash, "lxnor_lor_semiring_bool", &found);
    entry->name = pstrdup("lxnor_lor_semiring_bool");
    entry->semiring = GrB_LXNOR_LOR_SEMIRING_BOOL;
    }

GrB_Semiring lookup_semiring(char *name)
{
    semiring_entry *entry;
	entry = semiringhash_lookup(semiringhash, name);
    if (entry)
        return entry->semiring;
    return NULL;
    }
