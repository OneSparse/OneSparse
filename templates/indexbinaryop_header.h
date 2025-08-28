typedef struct indexbinaryop_entry
{{
	uint32 status;
	char *name;
    GxB_IndexBinaryOp indexbinaryop;
}} indexbinaryop_entry;

static uint32 hash_string_pointer(char *s);

static uint32
hash_string_pointer(char *s)
{{
	unsigned char *ss = (unsigned char *) s;

	return hash_bytes(ss, strlen(s));
    }}

#define INDEXBINARYOPHASH_INITIAL_SIZE 2000

#define SH_PREFIX		indexbinaryophash
#define SH_ELEMENT_TYPE indexbinaryop_entry
#define SH_KEY_TYPE		char *
#define	SH_KEY			name
#define SH_HASH_KEY(tb, key)	hash_string_pointer(key)
#define SH_EQUAL(tb, a, b)		(strcmp(a, b) == 0)
#define	SH_SCOPE		static inline
#define SH_DECLARE
#define SH_DEFINE
#include "lib/simplehash.h"

static indexbinaryophash_hash *indexbinaryophash;

void initialize_indexbinaryops()
{{
    bool found;
    indexbinaryop_entry *entry;

    indexbinaryophash = indexbinaryophash_create(TopMemoryContext, INDEXBINARYOPHASH_INITIAL_SIZE, NULL);
    {decls}
    }}

GxB_IndexBinaryOp lookup_indexbinaryop(char *name)
{{
    indexbinaryop_entry *entry;
	bool connected;
    GxB_IndexBinaryOp op;
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
	Datum d_ytype;
	Datum d_ttype;

	char *func;
	char *ztype_name;
	char *xtype_name;
	char *ytype_name;
	char *ttype_name;

	GrB_Type ztype;
	GrB_Type xtype;
	GrB_Type ytype;
	GrB_Type ttype;

	entry = indexbinaryophash_lookup(indexbinaryophash, name);

    if (entry)
	{{
        return entry->indexbinaryop;
	}}

	op = NULL;
	connected = spi_ensure_connected();

	argtypes[0] = TEXTOID;
	values[0] = CStringGetTextDatum(name);
	nulls[0] = ' ';

	plan = SPI_prepare(
		"select ztype, xtype, ytype, ttype, func "
		"from onesparse.user_defined_indexbinaryop "
		"where name = $1 "
		"limit 1",
		1,
		argtypes
        );

	if (plan == NULL)
	{{
		elog(ERROR, "lookup_indexbinaryop: SPI_prepare failed");
	}}

	execres = SPI_execute_plan(plan, values, nulls, true, 1);
	if (execres != SPI_OK_SELECT)
	{{
		elog(ERROR, "lookup_indexbinaryop: SPI_execute_plan failed");
	}}

	if (SPI_processed == 1)
	{{
        tupdesc = SPI_tuptable->tupdesc;
		tuptable = SPI_tuptable;
		tuple = tuptable->vals[0];

		d_ztype = SPI_getbinval(tuple, tupdesc, 1, &isnull);
		if (isnull)
		{{
			elog(ERROR, "lookup_indexbinaryop: ztype is NULL");
		}}
		d_xtype = SPI_getbinval(tuple, tupdesc, 2, &isnull);
		if (isnull)
		{{
			elog(ERROR, "lookup_indexbinaryop: xtype is NULL");
		}}
		d_ytype = SPI_getbinval(tuple, tupdesc, 3, &isnull);
		if (isnull)
		{{
			elog(ERROR, "lookup_indexbinaryop: ytype is NULL");
		}}
		d_ttype = SPI_getbinval(tuple, tupdesc, 4, &isnull);
		if (isnull)
		{{
			elog(ERROR, "lookup_indexbinaryop: ttype is NULL");
		}}
		d_func = SPI_getbinval(tuple, tupdesc, 5, &isnull);
		if (isnull)
		{{
			elog(ERROR, "lookup_indexbinaryop: func is NULL");
		}}

		ztype_name = TextDatumGetCString(d_ztype);
		xtype_name = TextDatumGetCString(d_xtype);
		ytype_name = TextDatumGetCString(d_ytype);
		ttype_name = TextDatumGetCString(d_ttype);
		func = TextDatumGetCString(d_func);

		ztype = lookup_type(ztype_name);
		xtype = lookup_type(xtype_name);
		ytype = lookup_type(ytype_name);
		ttype = lookup_type(ttype_name);

		if (ztype == NULL || xtype == NULL || ytype == NULL ||  ttype == NULL)
		{{
			elog(ERROR, "lookup_indexbinaryop: unknown GraphBLAS type(s) for '%s'", name);
		}}

		OS_CHECK(GxB_IndexBinaryOp_new(
					 &op,
					 NULL,
					 ztype,
					 xtype,
					 ytype,
					 ttype,
					 name,
					 func
					 ),
				 op,
				 "Failed to create new user defined binary op.");
	}}

	if (connected)
    {{
        if (SPI_finish() != SPI_OK_FINISH)
        {{
            elog(ERROR, "lookup_indexbinaryop: SPI_finish failed");
		}}
	}}
    return op;
}}
