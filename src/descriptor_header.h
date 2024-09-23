typedef struct descriptor_entry
{
	uint32 status;
	char *name;
    GrB_Descriptor descriptor;
} descriptor_entry;

static uint32 hash_string_pointer(char *s);

static uint32
hash_string_pointer(char *s)
{
	unsigned char *ss = (unsigned char *) s;

	return hash_bytes(ss, strlen(s));
    }

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
{
    bool found;
    descriptor_entry *entry;

    descriptorhash = descriptorhash_create(TopMemoryContext, DESCRIPTORHASH_INITIAL_SIZE, NULL);
    
    entry = descriptorhash_insert(descriptorhash, "t1", &found);
    entry->name = strdup("t1");
    entry->descriptor = GrB_DESC_T1;

    entry = descriptorhash_insert(descriptorhash, "t0", &found);
    entry->name = strdup("t0");
    entry->descriptor = GrB_DESC_T0;

    entry = descriptorhash_insert(descriptorhash, "t0t1", &found);
    entry->name = strdup("t0t1");
    entry->descriptor = GrB_DESC_T0T1;

    entry = descriptorhash_insert(descriptorhash, "c", &found);
    entry->name = strdup("c");
    entry->descriptor = GrB_DESC_C;

    entry = descriptorhash_insert(descriptorhash, "ct1", &found);
    entry->name = strdup("ct1");
    entry->descriptor = GrB_DESC_CT1;

    entry = descriptorhash_insert(descriptorhash, "ct0", &found);
    entry->name = strdup("ct0");
    entry->descriptor = GrB_DESC_CT0;

    entry = descriptorhash_insert(descriptorhash, "ct0t1", &found);
    entry->name = strdup("ct0t1");
    entry->descriptor = GrB_DESC_CT0T1;

    entry = descriptorhash_insert(descriptorhash, "s", &found);
    entry->name = strdup("s");
    entry->descriptor = GrB_DESC_S;

    entry = descriptorhash_insert(descriptorhash, "st1", &found);
    entry->name = strdup("st1");
    entry->descriptor = GrB_DESC_ST1;

    entry = descriptorhash_insert(descriptorhash, "st0", &found);
    entry->name = strdup("st0");
    entry->descriptor = GrB_DESC_ST0;

    entry = descriptorhash_insert(descriptorhash, "st0t1", &found);
    entry->name = strdup("st0t1");
    entry->descriptor = GrB_DESC_ST0T1;

    entry = descriptorhash_insert(descriptorhash, "sc", &found);
    entry->name = strdup("sc");
    entry->descriptor = GrB_DESC_SC;

    entry = descriptorhash_insert(descriptorhash, "sct1", &found);
    entry->name = strdup("sct1");
    entry->descriptor = GrB_DESC_SCT1;

    entry = descriptorhash_insert(descriptorhash, "sct0", &found);
    entry->name = strdup("sct0");
    entry->descriptor = GrB_DESC_SCT0;

    entry = descriptorhash_insert(descriptorhash, "sct0t1", &found);
    entry->name = strdup("sct0t1");
    entry->descriptor = GrB_DESC_SCT0T1;

    entry = descriptorhash_insert(descriptorhash, "r", &found);
    entry->name = strdup("r");
    entry->descriptor = GrB_DESC_R;

    entry = descriptorhash_insert(descriptorhash, "rt1", &found);
    entry->name = strdup("rt1");
    entry->descriptor = GrB_DESC_RT1;

    entry = descriptorhash_insert(descriptorhash, "rt0", &found);
    entry->name = strdup("rt0");
    entry->descriptor = GrB_DESC_RT0;

    entry = descriptorhash_insert(descriptorhash, "rt0t1", &found);
    entry->name = strdup("rt0t1");
    entry->descriptor = GrB_DESC_RT0T1;

    entry = descriptorhash_insert(descriptorhash, "rc", &found);
    entry->name = strdup("rc");
    entry->descriptor = GrB_DESC_RC;

    entry = descriptorhash_insert(descriptorhash, "rct1", &found);
    entry->name = strdup("rct1");
    entry->descriptor = GrB_DESC_RCT1;

    entry = descriptorhash_insert(descriptorhash, "rct0", &found);
    entry->name = strdup("rct0");
    entry->descriptor = GrB_DESC_RCT0;

    entry = descriptorhash_insert(descriptorhash, "rct0t1", &found);
    entry->name = strdup("rct0t1");
    entry->descriptor = GrB_DESC_RCT0T1;

    entry = descriptorhash_insert(descriptorhash, "rs", &found);
    entry->name = strdup("rs");
    entry->descriptor = GrB_DESC_RS;

    entry = descriptorhash_insert(descriptorhash, "rst1", &found);
    entry->name = strdup("rst1");
    entry->descriptor = GrB_DESC_RST1;

    entry = descriptorhash_insert(descriptorhash, "rst0", &found);
    entry->name = strdup("rst0");
    entry->descriptor = GrB_DESC_RST0;

    entry = descriptorhash_insert(descriptorhash, "rst0t1", &found);
    entry->name = strdup("rst0t1");
    entry->descriptor = GrB_DESC_RST0T1;

    entry = descriptorhash_insert(descriptorhash, "rsc", &found);
    entry->name = strdup("rsc");
    entry->descriptor = GrB_DESC_RSC;

    entry = descriptorhash_insert(descriptorhash, "rsct1", &found);
    entry->name = strdup("rsct1");
    entry->descriptor = GrB_DESC_RSCT1;

    entry = descriptorhash_insert(descriptorhash, "rsct0", &found);
    entry->name = strdup("rsct0");
    entry->descriptor = GrB_DESC_RSCT0;

    entry = descriptorhash_insert(descriptorhash, "rsct0t1", &found);
    entry->name = strdup("rsct0t1");
    entry->descriptor = GrB_DESC_RSCT0T1;
    }

GrB_Descriptor lookup_descriptor(char *name)
{
    descriptor_entry *entry;
	entry = descriptorhash_lookup(descriptorhash, name);
    if (entry)
        return entry->descriptor;
    return NULL;
    }
