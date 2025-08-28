typedef struct type_entry
{
	uint32 status;
	char *name;
    GrB_Type type;
} type_entry;

static uint32 hash_string_pointer(char *s);

static uint32
hash_string_pointer(char *s)
{
	unsigned char *ss = (unsigned char *) s;

	return hash_bytes(ss, strlen(s));
    }

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
{
    bool found;
    type_entry *entry;

    typehash = typehash_create(TopMemoryContext, TYPEHASH_INITIAL_SIZE, NULL);
    
    entry = typehash_insert(typehash, "bool", &found);
    entry->name = strdup("bool");
    entry->type = GrB_BOOL;

    entry = typehash_insert(typehash, "int8", &found);
    entry->name = strdup("int8");
    entry->type = GrB_INT8;

    entry = typehash_insert(typehash, "int16", &found);
    entry->name = strdup("int16");
    entry->type = GrB_INT16;

    entry = typehash_insert(typehash, "int32", &found);
    entry->name = strdup("int32");
    entry->type = GrB_INT32;

    entry = typehash_insert(typehash, "int64", &found);
    entry->name = strdup("int64");
    entry->type = GrB_INT64;

    entry = typehash_insert(typehash, "uint8", &found);
    entry->name = strdup("uint8");
    entry->type = GrB_UINT8;

    entry = typehash_insert(typehash, "uint16", &found);
    entry->name = strdup("uint16");
    entry->type = GrB_UINT16;

    entry = typehash_insert(typehash, "uint32", &found);
    entry->name = strdup("uint32");
    entry->type = GrB_UINT32;

    entry = typehash_insert(typehash, "uint64", &found);
    entry->name = strdup("uint64");
    entry->type = GrB_UINT64;

    entry = typehash_insert(typehash, "fp32", &found);
    entry->name = strdup("fp32");
    entry->type = GrB_FP32;

    entry = typehash_insert(typehash, "fp64", &found);
    entry->name = strdup("fp64");
    entry->type = GrB_FP64;

    entry = typehash_insert(typehash, "fc32", &found);
    entry->name = strdup("fc32");
    entry->type = GxB_FC32;

    entry = typehash_insert(typehash, "fc64", &found);
    entry->name = strdup("fc64");
    entry->type = GxB_FC64;
    }

GrB_Type lookup_type(char *name)
{
    type_entry *entry;
	bool connected;
    GrB_Type type;
	Oid argtypes[1];
	Datum values[1];
	char nulls[1];
	SPIPlanPtr plan;
	int execres;

	TupleDesc tupdesc;
	SPITupleTable *tuptable;
	HeapTuple tuple;
	bool isnull;
	Datum d_type_def;

	char *type_def;

	entry = typehash_lookup(typehash, name);

    if (entry)
	{
        return entry->type;
	}

	type = NULL;
	connected = spi_ensure_connected();

	argtypes[0] = TEXTOID;
	values[0] = CStringGetTextDatum(name);
	nulls[0] = ' ';

	plan = SPI_prepare(
		"select type_def "
		"from onesparse.user_defined_type "
		"where name = $1 "
		"limit 1",
		1,
		argtypes
        );

	if (plan == NULL)
	{
		elog(ERROR, "lookup_type: SPI_prepare failed");
	}

	execres = SPI_execute_plan(plan, values, nulls, true, 1);
	if (execres != SPI_OK_SELECT)
	{
		elog(ERROR, "lookup_type: SPI_execute_plan failed");
	}

	if (SPI_processed == 1)
	{
        tupdesc = SPI_tuptable->tupdesc;
		tuptable = SPI_tuptable;
		tuple = tuptable->vals[0];

		d_type_def = SPI_getbinval(tuple, tupdesc, 1, &isnull);
		if (isnull)
		{
			elog(ERROR, "lookup_type: type_def is NULL");
		}

		type_def = TextDatumGetCString(d_type_def);

		OS_CHECK(GxB_Type_new(
					 &type,
					 0,
					 name,
					 type_def
					 ),
				 type,
				 "Failed to create new user defined type.");
	}

	if (connected)
    {
        if (SPI_finish() != SPI_OK_FINISH)
        {
            elog(ERROR, "lookup_type: SPI_finish failed");
		}
	}

    return type;
    }
