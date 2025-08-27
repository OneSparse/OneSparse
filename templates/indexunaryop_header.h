typedef struct indexunaryop_entry
{{
	uint32 status;
	char *name;
    GrB_IndexUnaryOp indexunaryop;
}} indexunaryop_entry;

static uint32 hash_string_pointer(char *s);

static uint32
hash_string_pointer(char *s)
{{
	unsigned char *ss = (unsigned char *) s;

	return hash_bytes(ss, strlen(s));
    }}

#define INDEXUNARYOPHASH_INITIAL_SIZE 2000

#define SH_PREFIX		indexunaryophash
#define SH_ELEMENT_TYPE indexunaryop_entry
#define SH_KEY_TYPE		char *
#define	SH_KEY			name
#define SH_HASH_KEY(tb, key)	hash_string_pointer(key)
#define SH_EQUAL(tb, a, b)		(strcmp(a, b) == 0)
#define	SH_SCOPE		static inline
#define SH_DECLARE
#define SH_DEFINE
#include "lib/simplehash.h"

static indexunaryophash_hash *indexunaryophash;

void rand_r_int32_f(int *z, const int *x, GrB_Index i, GrB_Index j, const int *y);
void rand_r_int32_f(int *z, const int *x, GrB_Index i, GrB_Index j, const int *y)
{{
        *z = rand_r((unsigned int*)x);
    }}

#define RAND_R_INT32_DEFN \
"void rand_r_int32_f(int *z, const int *x, GrB_Index i, GrB_Index j, const int *y)\n" \
"{{\n" \
"        *z = rand_r((unsigned int*)x);\n" \
"    }}\n"

void initialize_indexunaryops()
{{
    bool found;
    indexunaryop_entry *entry;
    GrB_IndexUnaryOp op;

    indexunaryophash = indexunaryophash_create(TopMemoryContext, INDEXUNARYOPHASH_INITIAL_SIZE, NULL);

    OS_CHECK(GxB_IndexUnaryOp_new(&op,
                                  (GxB_index_unary_function) rand_r_int32_f,
                                  GrB_INT32,
                                  GrB_INT32,
                                  GrB_INT32,
                                  "rand_r_int32",
                                  RAND_R_INT32_DEFN
                                   ),
             op,
             "Cannot create rand_r index unary op" );

    entry = indexunaryophash_insert(indexunaryophash, "rand_r_int32", &found);
    entry->name = strdup("rand_r_int32");
    entry->indexunaryop = op;

    {decls}
    }}

GrB_IndexUnaryOp lookup_indexunaryop(char *name)
{{
    indexunaryop_entry *entry;

	bool connected;
    GrB_IndexUnaryOp op;
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
	Datum d_ttype;

	char *func;
	char *ztype_name;
	char *xtype_name;
	char *ttype_name;

	GrB_Type ztype;
	GrB_Type xtype;
	GrB_Type ttype;

	entry = indexunaryophash_lookup(indexunaryophash, name);
    if (entry)
	{{
        return entry->indexunaryop;
	}}

	op = NULL;
	connected = spi_ensure_connected();

	argtypes[0] = TEXTOID;
	values[0] = CStringGetTextDatum(name);
	nulls[0] = ' ';

	plan = SPI_prepare(
		"select ztype, xtype, ttype, func "
		"from onesparse.user_defined_indexunaryop "
		"where name = $1 "
		"limit 1",
		1,
		argtypes
        );

	if (plan == NULL)
	{{
		elog(ERROR, "lookup_indexunaryop: SPI_prepare failed");
	}}

	execres = SPI_execute_plan(plan, values, nulls, true, 1);
	if (execres != SPI_OK_SELECT)
	{{
		elog(ERROR, "lookup_indexunaryop: SPI_execute_plan failed");
	}}

	if (SPI_processed == 1)
	{{
        tupdesc = SPI_tuptable->tupdesc;
		tuptable = SPI_tuptable;
		tuple = tuptable->vals[0];

		d_ztype = SPI_getbinval(tuple, tupdesc, 1, &isnull);
		if (isnull)
		{{
			elog(ERROR, "lookup_indexunaryop: ztype is NULL");
		}}
		d_xtype = SPI_getbinval(tuple, tupdesc, 2, &isnull);
		if (isnull)
		{{
			elog(ERROR, "lookup_indexunaryop: xtype is NULL");
		}}
		d_ttype = SPI_getbinval(tuple, tupdesc, 3, &isnull);
		if (isnull)
		{{
			elog(ERROR, "lookup_indexunaryop: ttype is NULL");
		}}
		d_func = SPI_getbinval(tuple, tupdesc, 4, &isnull);
		if (isnull)
		{{
			elog(ERROR, "lookup_indexunaryop: func is NULL");
		}}

		ztype_name = TextDatumGetCString(d_ztype);
		xtype_name = TextDatumGetCString(d_xtype);
		ttype_name = TextDatumGetCString(d_ttype);
		func = TextDatumGetCString(d_func);

		ztype = lookup_type(ztype_name);
		xtype = lookup_type(xtype_name);
		ttype = lookup_type(ttype_name);

		if (ztype == NULL || xtype == NULL || ttype == NULL)
		{{
			elog(ERROR, "lookup_indexunaryop: unknown GraphBLAS type(s) for '%s'", name);
		}}

		OS_CHECK(GxB_IndexUnaryOp_new(
					 &op,
					 NULL,
					 ztype,
					 xtype,
					 ttype,
					 name,
					 func
					 ),
				 op,
				 "Failed to create new user defined indexunaryop.");
	}}

	if (connected)
    {{
        if (SPI_finish() != SPI_OK_FINISH)
        {{
            elog(ERROR, "lookup_indexunaryop: SPI_finish failed");
		}}
	}}
    return op;
    }}
