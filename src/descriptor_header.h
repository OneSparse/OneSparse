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
    
    entry = descriptorhash_insert(descriptorhash, "desc_t1", &found);
    entry->name = strdup("desc_t1");
    entry->descriptor = GrB_DESC_T1;

    entry = descriptorhash_insert(descriptorhash, "desc_t0", &found);
    entry->name = strdup("desc_t0");
    entry->descriptor = GrB_DESC_T0;

    entry = descriptorhash_insert(descriptorhash, "desc_t0t1", &found);
    entry->name = strdup("desc_t0t1");
    entry->descriptor = GrB_DESC_T0T1;

    entry = descriptorhash_insert(descriptorhash, "desc_c", &found);
    entry->name = strdup("desc_c");
    entry->descriptor = GrB_DESC_C;

    entry = descriptorhash_insert(descriptorhash, "desc_ct1", &found);
    entry->name = strdup("desc_ct1");
    entry->descriptor = GrB_DESC_CT1;

    entry = descriptorhash_insert(descriptorhash, "desc_ct0", &found);
    entry->name = strdup("desc_ct0");
    entry->descriptor = GrB_DESC_CT0;

    entry = descriptorhash_insert(descriptorhash, "desc_ct0t1", &found);
    entry->name = strdup("desc_ct0t1");
    entry->descriptor = GrB_DESC_CT0T1;

    entry = descriptorhash_insert(descriptorhash, "desc_s", &found);
    entry->name = strdup("desc_s");
    entry->descriptor = GrB_DESC_S;

    entry = descriptorhash_insert(descriptorhash, "desc_st1", &found);
    entry->name = strdup("desc_st1");
    entry->descriptor = GrB_DESC_ST1;

    entry = descriptorhash_insert(descriptorhash, "desc_st0", &found);
    entry->name = strdup("desc_st0");
    entry->descriptor = GrB_DESC_ST0;

    entry = descriptorhash_insert(descriptorhash, "desc_st0t1", &found);
    entry->name = strdup("desc_st0t1");
    entry->descriptor = GrB_DESC_ST0T1;

    entry = descriptorhash_insert(descriptorhash, "desc_sc", &found);
    entry->name = strdup("desc_sc");
    entry->descriptor = GrB_DESC_SC;

    entry = descriptorhash_insert(descriptorhash, "desc_sct1", &found);
    entry->name = strdup("desc_sct1");
    entry->descriptor = GrB_DESC_SCT1;

    entry = descriptorhash_insert(descriptorhash, "desc_sct0", &found);
    entry->name = strdup("desc_sct0");
    entry->descriptor = GrB_DESC_SCT0;

    entry = descriptorhash_insert(descriptorhash, "desc_sct0t1", &found);
    entry->name = strdup("desc_sct0t1");
    entry->descriptor = GrB_DESC_SCT0T1;

    entry = descriptorhash_insert(descriptorhash, "desc_r", &found);
    entry->name = strdup("desc_r");
    entry->descriptor = GrB_DESC_R;

    entry = descriptorhash_insert(descriptorhash, "desc_rt1", &found);
    entry->name = strdup("desc_rt1");
    entry->descriptor = GrB_DESC_RT1;

    entry = descriptorhash_insert(descriptorhash, "desc_rt0", &found);
    entry->name = strdup("desc_rt0");
    entry->descriptor = GrB_DESC_RT0;

    entry = descriptorhash_insert(descriptorhash, "desc_rt0t1", &found);
    entry->name = strdup("desc_rt0t1");
    entry->descriptor = GrB_DESC_RT0T1;

    entry = descriptorhash_insert(descriptorhash, "desc_rc", &found);
    entry->name = strdup("desc_rc");
    entry->descriptor = GrB_DESC_RC;

    entry = descriptorhash_insert(descriptorhash, "desc_rct1", &found);
    entry->name = strdup("desc_rct1");
    entry->descriptor = GrB_DESC_RCT1;

    entry = descriptorhash_insert(descriptorhash, "desc_rct0", &found);
    entry->name = strdup("desc_rct0");
    entry->descriptor = GrB_DESC_RCT0;

    entry = descriptorhash_insert(descriptorhash, "desc_rct0t1", &found);
    entry->name = strdup("desc_rct0t1");
    entry->descriptor = GrB_DESC_RCT0T1;

    entry = descriptorhash_insert(descriptorhash, "desc_rs", &found);
    entry->name = strdup("desc_rs");
    entry->descriptor = GrB_DESC_RS;

    entry = descriptorhash_insert(descriptorhash, "desc_rst1", &found);
    entry->name = strdup("desc_rst1");
    entry->descriptor = GrB_DESC_RST1;

    entry = descriptorhash_insert(descriptorhash, "desc_rst0", &found);
    entry->name = strdup("desc_rst0");
    entry->descriptor = GrB_DESC_RST0;

    entry = descriptorhash_insert(descriptorhash, "desc_rst0t1", &found);
    entry->name = strdup("desc_rst0t1");
    entry->descriptor = GrB_DESC_RST0T1;

    entry = descriptorhash_insert(descriptorhash, "desc_rsc", &found);
    entry->name = strdup("desc_rsc");
    entry->descriptor = GrB_DESC_RSC;

    entry = descriptorhash_insert(descriptorhash, "desc_rsct1", &found);
    entry->name = strdup("desc_rsct1");
    entry->descriptor = GrB_DESC_RSCT1;

    entry = descriptorhash_insert(descriptorhash, "desc_rsct0", &found);
    entry->name = strdup("desc_rsct0");
    entry->descriptor = GrB_DESC_RSCT0;

    entry = descriptorhash_insert(descriptorhash, "desc_rsct0t1", &found);
    entry->name = strdup("desc_rsct0t1");
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
