typedef struct monoid_entry
{{
	uint32 status;
	char *name;
    GrB_Monoid monoid;
}} monoid_entry;

static uint32 hash_string_pointer(char *s);

static uint32
hash_string_pointer(char *s)
{{
	unsigned char *ss = (unsigned char *) s;

	return hash_bytes(ss, strlen(s));
    }}

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
{{
    bool found;
    monoid_entry *entry;

    monoidhash = monoidhash_create(TopMemoryContext, MONOIDHASH_INITIAL_SIZE, NULL);
    {decls}
    }}

GrB_Monoid lookup_monoid(char *name)
{{
    monoid_entry *entry;
	entry = monoidhash_lookup(monoidhash, name);
    if (entry)
        return entry->monoid;
    return NULL;
    }}
