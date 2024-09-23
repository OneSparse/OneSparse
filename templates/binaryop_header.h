typedef struct binaryop_entry
{{
	uint32 status;
	char *name;
    GrB_BinaryOp binaryop;
}} binaryop_entry;

static uint32 hash_string_pointer(char *s);

static uint32
hash_string_pointer(char *s)
{{
	unsigned char *ss = (unsigned char *) s;

	return hash_bytes(ss, strlen(s));
    }}

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
{{
    bool found;
    binaryop_entry *entry;

    binaryophash = binaryophash_create(TopMemoryContext, BINARYOPHASH_INITIAL_SIZE, NULL);
    {decls}
    }}

GrB_BinaryOp lookup_binaryop(char *name)
{{
    binaryop_entry *entry;
	entry = binaryophash_lookup(binaryophash, name);
    if (entry)
        return entry->binaryop;
    return NULL;
    }}
