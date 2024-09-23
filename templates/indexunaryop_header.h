typedef struct indexunaryop_entry
{{
	uint32 status;
	char *name;
    GrB_IndexUnaryOp indexunaryop;
}} indexunaryop_entry;

static uint32 hash_string_pointer(char *s);

static uint32
hash_string_pointer(char *s)
{{
	unsigned char *ss = (unsigned char *) s;

	return hash_bytes(ss, strlen(s));
    }}

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

void initialize_indexunaryops()
{{
    bool found;
    indexunaryop_entry *entry;

    indexunaryophash = indexunaryophash_create(TopMemoryContext, INDEXUNARYOPHASH_INITIAL_SIZE, NULL);
    {decls}
    }}

GrB_IndexUnaryOp lookup_indexunaryop(char *name)
{{
    indexunaryop_entry *entry;
	entry = indexunaryophash_lookup(indexunaryophash, name);
    if (entry)
        return entry->indexunaryop;
    return NULL;
    }}
