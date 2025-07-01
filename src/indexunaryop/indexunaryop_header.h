typedef struct indexunaryop_entry
{
	uint32 status;
	char *name;
    GrB_IndexUnaryOp indexunaryop;
} indexunaryop_entry;

static uint32 hash_string_pointer(char *s);

static uint32
hash_string_pointer(char *s)
{
	unsigned char *ss = (unsigned char *) s;

	return hash_bytes(ss, strlen(s));
    }

#define INDEXUNARYOPHASH_INITIAL_SIZE 2000

#define SH_PREFIX		indexunaryophash
#define SH_ELEMENT_TYPE indexunaryop_entry
#define SH_KEY_TYPE		char *
#define	SH_KEY			name
#define SH_HASH_KEY(tb, key)	hash_string_pointer(key)
#define SH_EQUAL(tb, a, b)		(strcmp(a, b) == 0)
#define	SH_SCOPE		static inline
#define SH_DECLARE
#define SH_DEFINE
#include "lib/simplehash.h"

static indexunaryophash_hash *indexunaryophash;

void rand_r_int32_f(int *z, const int *x, GrB_Index i, GrB_Index j, const int *y);
void rand_r_int32_f(int *z, const int *x, GrB_Index i, GrB_Index j, const int *y)
{
        *z = rand_r((unsigned int*)x);
    }

#define RAND_R_INT32_DEFN \
"void rand_r_int32_f(int *z, const int *x, GrB_Index i, GrB_Index j, const int *y)\n" \
"{\n" \
"        *z = rand_r((unsigned int*)x);\n" \
"    }\n"

void initialize_indexunaryops()
{
    bool found;
    indexunaryop_entry *entry;
    GrB_IndexUnaryOp op;

    indexunaryophash = indexunaryophash_create(TopMemoryContext, INDEXUNARYOPHASH_INITIAL_SIZE, NULL);

    OS_CHECK(GxB_IndexUnaryOp_new(&op,
                                  (GxB_index_unary_function) rand_r_int32_f,
                                  GrB_INT32,
                                  GrB_INT32,
                                  GrB_INT32,
                                  "rand_r_int32",
                                  RAND_R_INT32_DEFN
                                   ),
             op,
             "Cannot create rand_r index unary op" );

    entry = indexunaryophash_insert(indexunaryophash, "rand_r_int32", &found);
    entry->name = strdup("rand_r_int32");
    entry->indexunaryop = op;

    
    entry = indexunaryophash_insert(indexunaryophash, "rowindex_int32", &found);
    entry->name = strdup("rowindex_int32");
    entry->indexunaryop = GrB_ROWINDEX_INT32;

    entry = indexunaryophash_insert(indexunaryophash, "rowindex_int64", &found);
    entry->name = strdup("rowindex_int64");
    entry->indexunaryop = GrB_ROWINDEX_INT64;

    entry = indexunaryophash_insert(indexunaryophash, "colindex_int32", &found);
    entry->name = strdup("colindex_int32");
    entry->indexunaryop = GrB_COLINDEX_INT32;

    entry = indexunaryophash_insert(indexunaryophash, "colindex_int64", &found);
    entry->name = strdup("colindex_int64");
    entry->indexunaryop = GrB_COLINDEX_INT64;

    entry = indexunaryophash_insert(indexunaryophash, "diagindex_int32", &found);
    entry->name = strdup("diagindex_int32");
    entry->indexunaryop = GrB_DIAGINDEX_INT32;

    entry = indexunaryophash_insert(indexunaryophash, "diagindex_int64", &found);
    entry->name = strdup("diagindex_int64");
    entry->indexunaryop = GrB_DIAGINDEX_INT64;

    entry = indexunaryophash_insert(indexunaryophash, "tril", &found);
    entry->name = strdup("tril");
    entry->indexunaryop = GrB_TRIL;

    entry = indexunaryophash_insert(indexunaryophash, "triu", &found);
    entry->name = strdup("triu");
    entry->indexunaryop = GrB_TRIU;

    entry = indexunaryophash_insert(indexunaryophash, "diag", &found);
    entry->name = strdup("diag");
    entry->indexunaryop = GrB_DIAG;

    entry = indexunaryophash_insert(indexunaryophash, "offdiag", &found);
    entry->name = strdup("offdiag");
    entry->indexunaryop = GrB_OFFDIAG;

    entry = indexunaryophash_insert(indexunaryophash, "colle", &found);
    entry->name = strdup("colle");
    entry->indexunaryop = GrB_COLLE;

    entry = indexunaryophash_insert(indexunaryophash, "colgt", &found);
    entry->name = strdup("colgt");
    entry->indexunaryop = GrB_COLGT;

    entry = indexunaryophash_insert(indexunaryophash, "rowle", &found);
    entry->name = strdup("rowle");
    entry->indexunaryop = GrB_ROWLE;

    entry = indexunaryophash_insert(indexunaryophash, "rowgt", &found);
    entry->name = strdup("rowgt");
    entry->indexunaryop = GrB_ROWGT;

    entry = indexunaryophash_insert(indexunaryophash, "valueeq_int8", &found);
    entry->name = strdup("valueeq_int8");
    entry->indexunaryop = GrB_VALUEEQ_INT8;

    entry = indexunaryophash_insert(indexunaryophash, "valueeq_uint8", &found);
    entry->name = strdup("valueeq_uint8");
    entry->indexunaryop = GrB_VALUEEQ_UINT8;

    entry = indexunaryophash_insert(indexunaryophash, "valueeq_fp32", &found);
    entry->name = strdup("valueeq_fp32");
    entry->indexunaryop = GrB_VALUEEQ_FP32;

    entry = indexunaryophash_insert(indexunaryophash, "valueeq_bool", &found);
    entry->name = strdup("valueeq_bool");
    entry->indexunaryop = GrB_VALUEEQ_BOOL;

    entry = indexunaryophash_insert(indexunaryophash, "valueeq_int16", &found);
    entry->name = strdup("valueeq_int16");
    entry->indexunaryop = GrB_VALUEEQ_INT16;

    entry = indexunaryophash_insert(indexunaryophash, "valueeq_uint16", &found);
    entry->name = strdup("valueeq_uint16");
    entry->indexunaryop = GrB_VALUEEQ_UINT16;

    entry = indexunaryophash_insert(indexunaryophash, "valueeq_fp64", &found);
    entry->name = strdup("valueeq_fp64");
    entry->indexunaryop = GrB_VALUEEQ_FP64;

    entry = indexunaryophash_insert(indexunaryophash, "valueeq_int32", &found);
    entry->name = strdup("valueeq_int32");
    entry->indexunaryop = GrB_VALUEEQ_INT32;

    entry = indexunaryophash_insert(indexunaryophash, "valueeq_uint32", &found);
    entry->name = strdup("valueeq_uint32");
    entry->indexunaryop = GrB_VALUEEQ_UINT32;

    entry = indexunaryophash_insert(indexunaryophash, "valueeq_fc32", &found);
    entry->name = strdup("valueeq_fc32");
    entry->indexunaryop = GxB_VALUEEQ_FC32;

    entry = indexunaryophash_insert(indexunaryophash, "valueeq_int64", &found);
    entry->name = strdup("valueeq_int64");
    entry->indexunaryop = GrB_VALUEEQ_INT64;

    entry = indexunaryophash_insert(indexunaryophash, "valueeq_uint64", &found);
    entry->name = strdup("valueeq_uint64");
    entry->indexunaryop = GrB_VALUEEQ_UINT64;

    entry = indexunaryophash_insert(indexunaryophash, "valueeq_fc64", &found);
    entry->name = strdup("valueeq_fc64");
    entry->indexunaryop = GxB_VALUEEQ_FC64;

    entry = indexunaryophash_insert(indexunaryophash, "valuene_int8", &found);
    entry->name = strdup("valuene_int8");
    entry->indexunaryop = GrB_VALUENE_INT8;

    entry = indexunaryophash_insert(indexunaryophash, "valuene_uint8", &found);
    entry->name = strdup("valuene_uint8");
    entry->indexunaryop = GrB_VALUENE_UINT8;

    entry = indexunaryophash_insert(indexunaryophash, "valuene_fp32", &found);
    entry->name = strdup("valuene_fp32");
    entry->indexunaryop = GrB_VALUENE_FP32;

    entry = indexunaryophash_insert(indexunaryophash, "valuene_bool", &found);
    entry->name = strdup("valuene_bool");
    entry->indexunaryop = GrB_VALUENE_BOOL;

    entry = indexunaryophash_insert(indexunaryophash, "valuene_int16", &found);
    entry->name = strdup("valuene_int16");
    entry->indexunaryop = GrB_VALUENE_INT16;

    entry = indexunaryophash_insert(indexunaryophash, "valuene_uint16", &found);
    entry->name = strdup("valuene_uint16");
    entry->indexunaryop = GrB_VALUENE_UINT16;

    entry = indexunaryophash_insert(indexunaryophash, "valuene_fp64", &found);
    entry->name = strdup("valuene_fp64");
    entry->indexunaryop = GrB_VALUENE_FP64;

    entry = indexunaryophash_insert(indexunaryophash, "valuene_int32", &found);
    entry->name = strdup("valuene_int32");
    entry->indexunaryop = GrB_VALUENE_INT32;

    entry = indexunaryophash_insert(indexunaryophash, "valuene_uint32", &found);
    entry->name = strdup("valuene_uint32");
    entry->indexunaryop = GrB_VALUENE_UINT32;

    entry = indexunaryophash_insert(indexunaryophash, "valuene_fc32", &found);
    entry->name = strdup("valuene_fc32");
    entry->indexunaryop = GxB_VALUENE_FC32;

    entry = indexunaryophash_insert(indexunaryophash, "valuene_int64", &found);
    entry->name = strdup("valuene_int64");
    entry->indexunaryop = GrB_VALUENE_INT64;

    entry = indexunaryophash_insert(indexunaryophash, "valuene_uint64", &found);
    entry->name = strdup("valuene_uint64");
    entry->indexunaryop = GrB_VALUENE_UINT64;

    entry = indexunaryophash_insert(indexunaryophash, "valuene_fc64", &found);
    entry->name = strdup("valuene_fc64");
    entry->indexunaryop = GxB_VALUENE_FC64;

    entry = indexunaryophash_insert(indexunaryophash, "valuelt_int8", &found);
    entry->name = strdup("valuelt_int8");
    entry->indexunaryop = GrB_VALUELT_INT8;

    entry = indexunaryophash_insert(indexunaryophash, "valuelt_uint8", &found);
    entry->name = strdup("valuelt_uint8");
    entry->indexunaryop = GrB_VALUELT_UINT8;

    entry = indexunaryophash_insert(indexunaryophash, "valuelt_fp32", &found);
    entry->name = strdup("valuelt_fp32");
    entry->indexunaryop = GrB_VALUELT_FP32;

    entry = indexunaryophash_insert(indexunaryophash, "valuelt_bool", &found);
    entry->name = strdup("valuelt_bool");
    entry->indexunaryop = GrB_VALUELT_BOOL;

    entry = indexunaryophash_insert(indexunaryophash, "valuelt_int16", &found);
    entry->name = strdup("valuelt_int16");
    entry->indexunaryop = GrB_VALUELT_INT16;

    entry = indexunaryophash_insert(indexunaryophash, "valuelt_uint16", &found);
    entry->name = strdup("valuelt_uint16");
    entry->indexunaryop = GrB_VALUELT_UINT16;

    entry = indexunaryophash_insert(indexunaryophash, "valuelt_fp64", &found);
    entry->name = strdup("valuelt_fp64");
    entry->indexunaryop = GrB_VALUELT_FP64;

    entry = indexunaryophash_insert(indexunaryophash, "valuelt_int32", &found);
    entry->name = strdup("valuelt_int32");
    entry->indexunaryop = GrB_VALUELT_INT32;

    entry = indexunaryophash_insert(indexunaryophash, "valuelt_uint32", &found);
    entry->name = strdup("valuelt_uint32");
    entry->indexunaryop = GrB_VALUELT_UINT32;

    entry = indexunaryophash_insert(indexunaryophash, "valuelt_int64", &found);
    entry->name = strdup("valuelt_int64");
    entry->indexunaryop = GrB_VALUELT_INT64;

    entry = indexunaryophash_insert(indexunaryophash, "valuelt_uint64", &found);
    entry->name = strdup("valuelt_uint64");
    entry->indexunaryop = GrB_VALUELT_UINT64;

    entry = indexunaryophash_insert(indexunaryophash, "valuele_int8", &found);
    entry->name = strdup("valuele_int8");
    entry->indexunaryop = GrB_VALUELE_INT8;

    entry = indexunaryophash_insert(indexunaryophash, "valuele_uint8", &found);
    entry->name = strdup("valuele_uint8");
    entry->indexunaryop = GrB_VALUELE_UINT8;

    entry = indexunaryophash_insert(indexunaryophash, "valuele_fp32", &found);
    entry->name = strdup("valuele_fp32");
    entry->indexunaryop = GrB_VALUELE_FP32;

    entry = indexunaryophash_insert(indexunaryophash, "valuele_bool", &found);
    entry->name = strdup("valuele_bool");
    entry->indexunaryop = GrB_VALUELE_BOOL;

    entry = indexunaryophash_insert(indexunaryophash, "valuele_int16", &found);
    entry->name = strdup("valuele_int16");
    entry->indexunaryop = GrB_VALUELE_INT16;

    entry = indexunaryophash_insert(indexunaryophash, "valuele_uint16", &found);
    entry->name = strdup("valuele_uint16");
    entry->indexunaryop = GrB_VALUELE_UINT16;

    entry = indexunaryophash_insert(indexunaryophash, "valuele_fp64", &found);
    entry->name = strdup("valuele_fp64");
    entry->indexunaryop = GrB_VALUELE_FP64;

    entry = indexunaryophash_insert(indexunaryophash, "valuele_int32", &found);
    entry->name = strdup("valuele_int32");
    entry->indexunaryop = GrB_VALUELE_INT32;

    entry = indexunaryophash_insert(indexunaryophash, "valuele_uint32", &found);
    entry->name = strdup("valuele_uint32");
    entry->indexunaryop = GrB_VALUELE_UINT32;

    entry = indexunaryophash_insert(indexunaryophash, "valuele_int64", &found);
    entry->name = strdup("valuele_int64");
    entry->indexunaryop = GrB_VALUELE_INT64;

    entry = indexunaryophash_insert(indexunaryophash, "valuele_uint64", &found);
    entry->name = strdup("valuele_uint64");
    entry->indexunaryop = GrB_VALUELE_UINT64;

    entry = indexunaryophash_insert(indexunaryophash, "valuegt_int8", &found);
    entry->name = strdup("valuegt_int8");
    entry->indexunaryop = GrB_VALUEGT_INT8;

    entry = indexunaryophash_insert(indexunaryophash, "valuegt_uint8", &found);
    entry->name = strdup("valuegt_uint8");
    entry->indexunaryop = GrB_VALUEGT_UINT8;

    entry = indexunaryophash_insert(indexunaryophash, "valuegt_fp32", &found);
    entry->name = strdup("valuegt_fp32");
    entry->indexunaryop = GrB_VALUEGT_FP32;

    entry = indexunaryophash_insert(indexunaryophash, "valuegt_bool", &found);
    entry->name = strdup("valuegt_bool");
    entry->indexunaryop = GrB_VALUEGT_BOOL;

    entry = indexunaryophash_insert(indexunaryophash, "valuegt_int16", &found);
    entry->name = strdup("valuegt_int16");
    entry->indexunaryop = GrB_VALUEGT_INT16;

    entry = indexunaryophash_insert(indexunaryophash, "valuegt_uint16", &found);
    entry->name = strdup("valuegt_uint16");
    entry->indexunaryop = GrB_VALUEGT_UINT16;

    entry = indexunaryophash_insert(indexunaryophash, "valuegt_fp64", &found);
    entry->name = strdup("valuegt_fp64");
    entry->indexunaryop = GrB_VALUEGT_FP64;

    entry = indexunaryophash_insert(indexunaryophash, "valuegt_int32", &found);
    entry->name = strdup("valuegt_int32");
    entry->indexunaryop = GrB_VALUEGT_INT32;

    entry = indexunaryophash_insert(indexunaryophash, "valuegt_uint32", &found);
    entry->name = strdup("valuegt_uint32");
    entry->indexunaryop = GrB_VALUEGT_UINT32;

    entry = indexunaryophash_insert(indexunaryophash, "valuegt_int64", &found);
    entry->name = strdup("valuegt_int64");
    entry->indexunaryop = GrB_VALUEGT_INT64;

    entry = indexunaryophash_insert(indexunaryophash, "valuegt_uint64", &found);
    entry->name = strdup("valuegt_uint64");
    entry->indexunaryop = GrB_VALUEGT_UINT64;

    entry = indexunaryophash_insert(indexunaryophash, "valuege_int8", &found);
    entry->name = strdup("valuege_int8");
    entry->indexunaryop = GrB_VALUEGE_INT8;

    entry = indexunaryophash_insert(indexunaryophash, "valuege_uint8", &found);
    entry->name = strdup("valuege_uint8");
    entry->indexunaryop = GrB_VALUEGE_UINT8;

    entry = indexunaryophash_insert(indexunaryophash, "valuege_fp32", &found);
    entry->name = strdup("valuege_fp32");
    entry->indexunaryop = GrB_VALUEGE_FP32;

    entry = indexunaryophash_insert(indexunaryophash, "valuege_bool", &found);
    entry->name = strdup("valuege_bool");
    entry->indexunaryop = GrB_VALUEGE_BOOL;

    entry = indexunaryophash_insert(indexunaryophash, "valuege_int16", &found);
    entry->name = strdup("valuege_int16");
    entry->indexunaryop = GrB_VALUEGE_INT16;

    entry = indexunaryophash_insert(indexunaryophash, "valuege_uint16", &found);
    entry->name = strdup("valuege_uint16");
    entry->indexunaryop = GrB_VALUEGE_UINT16;

    entry = indexunaryophash_insert(indexunaryophash, "valuege_fp64", &found);
    entry->name = strdup("valuege_fp64");
    entry->indexunaryop = GrB_VALUEGE_FP64;

    entry = indexunaryophash_insert(indexunaryophash, "valuege_int32", &found);
    entry->name = strdup("valuege_int32");
    entry->indexunaryop = GrB_VALUEGE_INT32;

    entry = indexunaryophash_insert(indexunaryophash, "valuege_uint32", &found);
    entry->name = strdup("valuege_uint32");
    entry->indexunaryop = GrB_VALUEGE_UINT32;

    entry = indexunaryophash_insert(indexunaryophash, "valuege_int64", &found);
    entry->name = strdup("valuege_int64");
    entry->indexunaryop = GrB_VALUEGE_INT64;

    entry = indexunaryophash_insert(indexunaryophash, "valuege_uint64", &found);
    entry->name = strdup("valuege_uint64");
    entry->indexunaryop = GrB_VALUEGE_UINT64;
    }

GrB_IndexUnaryOp lookup_indexunaryop(char *name)
{
    indexunaryop_entry *entry;
	entry = indexunaryophash_lookup(indexunaryophash, name);
    if (entry)
        return entry->indexunaryop;
    return NULL;
    }
