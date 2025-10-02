typedef enum FieldKind
{
	FK_BOOL,
	FK_INT16,
	FK_INT32,
	FK_INT64,
	FK_FLOAT4,
	FK_FLOAT8,
	FK_OID,
	FK_UUID,
	FK_MONEY,
	FK_BPCHAR,
	FK_BITBYTES
}			FieldKind;

typedef struct FieldLayout
{
	FieldKind	kind;
	Size		size;
	Size		align;
}			FieldLayout;

void		get_field_layout(Oid typid, int32 typmod, FieldLayout * out, int attidx);
Datum		get_notnull_attr(HeapTuple tuple, TupleDesc tupdesc, int attno, const char *name);
