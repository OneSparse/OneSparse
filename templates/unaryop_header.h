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
	bool connected;
    GrB_UnaryOp op;
	Oid argtypes[1];
	Datum values[1];
	char nulls[1];
	SPIPlanPtr plan;
	int execres;

	TupleDesc tupdesc;
	SPITupleTable *tuptable;
	HeapTuple tuple;
	bool isnull;
	Datum d_func;
	Datum d_ztype;
	Datum d_xtype;

	char *func;
	char *ztype_name;
	char *xtype_name;

	GrB_Type ztype;
	GrB_Type xtype;

	entry = unaryophash_lookup(unaryophash, name);

    if (entry)
	{{
        return entry->unaryop;
	}}

	op = NULL;

	connected = spi_ensure_connected();

	argtypes[0] = TEXTOID;
	values[0] = CStringGetTextDatum(name);
	nulls[0] = ' ';

	plan = SPI_prepare(
		"select ztype, xtype, func "
		"from onesparse.user_defined_unaryop "
		"where name = $1 "
		"limit 1",
		1,
		argtypes
        );

	if (plan == NULL)
	{{
		elog(ERROR, "lookup_unaryop: SPI_prepare failed");
	}}

	execres = SPI_execute_plan(plan, values, nulls, true, 1);
	if (execres != SPI_OK_SELECT)
	{{
		elog(ERROR, "lookup_unaryop: SPI_execute_plan failed");
	}}

	if (SPI_processed == 1)
	{{
        tupdesc = SPI_tuptable->tupdesc;
		tuptable = SPI_tuptable;
		tuple = tuptable->vals[0];

		d_ztype = SPI_getbinval(tuple, tupdesc, 1, &isnull);
		if (isnull)
		{{
			elog(ERROR, "lookup_unaryop: ztype is NULL");
		}}
		d_xtype = SPI_getbinval(tuple, tupdesc, 2, &isnull);
		if (isnull)
		{{
			elog(ERROR, "lookup_unaryop: xtype is NULL");
		}}
		d_func = SPI_getbinval(tuple, tupdesc, 3, &isnull);
		if (isnull)
		{{
			elog(ERROR, "lookup_unaryop: func is NULL");
		}}

		ztype_name = TextDatumGetCString(d_ztype);
		xtype_name = TextDatumGetCString(d_xtype);
		func = TextDatumGetCString(d_func);

		ztype = lookup_type(ztype_name);
		xtype = lookup_type(xtype_name);

		if (ztype == NULL || xtype == NULL)
		{{
			elog(ERROR, "lookup_unaryop: unknown GraphBLAS type(s) for '%s'", name);
		}}

		OS_CHECK(GxB_UnaryOp_new(
					 &op,
					 NULL,
					 ztype,
					 xtype,
					 name,
					 func
					 ),
				 op,
				 "Failed to create new user defined unaryop.");
	}}

	if (connected)
    {{
        if (SPI_finish() != SPI_OK_FINISH)
        {{
            elog(ERROR, "lookup_unaryop: SPI_finish failed");
		}}
	}}
    return op;
}}
