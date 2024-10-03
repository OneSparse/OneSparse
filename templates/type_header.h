typedef struct type_entry
{{
	uint32 status;
	char *name;
    GrB_Type type;
}} type_entry;

static uint32 hash_string_pointer(char *s);

static uint32
hash_string_pointer(char *s)
{{
	unsigned char *ss = (unsigned char *) s;

	return hash_bytes(ss, strlen(s));
    }}

#define TYPEHASH_INITIAL_SIZE 2000

#define SH_PREFIX		typehash
#define SH_ELEMENT_TYPE type_entry
#define SH_KEY_TYPE		char *
#define	SH_KEY			name
#define SH_HASH_KEY(tb, key)	hash_string_pointer(key)
#define SH_EQUAL(tb, a, b)		(strcmp(a, b) == 0)
#define	SH_SCOPE		static inline
#define SH_DECLARE
#define SH_DEFINE
#include "lib/simplehash.h"

static typehash_hash *typehash;

void initialize_types()
{{
    bool found;
    type_entry *entry;

    typehash = typehash_create(TopMemoryContext, TYPEHASH_INITIAL_SIZE, NULL);
    {decls}
    }}

GrB_Type lookup_type(char *name)
{{
    type_entry *entry;
	entry = typehash_lookup(typehash, name);
    if (entry)
        return entry->type;
    return NULL;
    }}
