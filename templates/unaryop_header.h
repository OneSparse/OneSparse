typedef struct unaryop_entry
{{
	uint32 status;
	char *name;
    GrB_UnaryOp unaryop;
}} unaryop_entry;

static uint32 hash_string_pointer(char *s);

static uint32
hash_string_pointer(char *s)
{{
	unsigned char *ss = (unsigned char *) s;

	return hash_bytes(ss, strlen(s));
    }}

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
{{
    bool found;
    unaryop_entry *entry;

    unaryophash = unaryophash_create(TopMemoryContext, UNARYOPHASH_INITIAL_SIZE, NULL);
    {decls}
    }}

GrB_UnaryOp lookup_unaryop(char *name)
{{
    unaryop_entry *entry;
	entry = unaryophash_lookup(unaryophash, name);
    if (entry)
        return entry->unaryop;
    return NULL;
    }}
