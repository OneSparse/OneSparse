typedef struct descriptor_entry
{{
	uint32 status;
	char *name;
    GrB_Descriptor descriptor;
}} descriptor_entry;

static uint32 hash_string_pointer(char *s);

static uint32
hash_string_pointer(char *s)
{{
	unsigned char *ss = (unsigned char *) s;

	return hash_bytes(ss, strlen(s));
    }}

#define DESCRIPTORHASH_INITIAL_SIZE 2000

#define SH_PREFIX		descriptorhash
#define SH_ELEMENT_TYPE descriptor_entry
#define SH_KEY_TYPE		char *
#define	SH_KEY			name
#define SH_HASH_KEY(tb, key)	hash_string_pointer(key)
#define SH_EQUAL(tb, a, b)		(strcmp(a, b) == 0)
#define	SH_SCOPE		static inline
#define SH_DECLARE
#define SH_DEFINE
#include "lib/simplehash.h"

static descriptorhash_hash *descriptorhash;

void initialize_descriptors()
{{
    bool found;
    descriptor_entry *entry;

    descriptorhash = descriptorhash_create(TopMemoryContext, DESCRIPTORHASH_INITIAL_SIZE, NULL);
    {decls}
    }}

GrB_Descriptor lookup_descriptor(char *name)
{{
    descriptor_entry *entry;
	entry = descriptorhash_lookup(descriptorhash, name);
    if (entry)
        return entry->descriptor;
    return NULL;
    }}
