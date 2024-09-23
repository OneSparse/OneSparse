typedef struct semiring_entry
{{
	uint32 status;
	char *name;
    GrB_Semiring semiring;
}} semiring_entry;

static uint32 hash_string_pointer(char *s);

static uint32
hash_string_pointer(char *s)
{{
	unsigned char *ss = (unsigned char *) s;

	return hash_bytes(ss, strlen(s));
    }}

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
{{
    bool found;
    semiring_entry *entry;

    semiringhash = semiringhash_create(TopMemoryContext, SEMIRINGHASH_INITIAL_SIZE, NULL);
    {decls}
    }}

GrB_Semiring lookup_semiring(char *name)
{{
    semiring_entry *entry;
	entry = semiringhash_lookup(semiringhash, name);
    if (entry)
        return entry->semiring;
    return NULL;
    }}
