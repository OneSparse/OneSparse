#include "../onesparse.h"

static void
emit_field(StringInfoData *out, const char *ctype, const char *fname)
{
    const char *arr;
	size_t baselen;

	arr = strchr(ctype, '[');

    if (arr == NULL)
    {
        appendStringInfo(out, "    %s %s;\n", ctype, fname);
        return;
    }

    /* Base type is everything before the first '[' (trim trailing spaces). */
    baselen = (size_t) (arr - ctype);
    while (baselen > 0 && isspace((unsigned char) ctype[baselen - 1]))
    {
        baselen--;
    }

    /* Print as: <base> <name><array-suffixes>; e.g., "char f_bpchar[16];" */
    appendStringInfo(out, "    %.*s %s%s;\n",
                     (int) baselen, ctype, fname, arr);
}

static const char *
map_type(Oid typid, int32 typmod, StringInfo scratch)
{
    HeapTuple typtup;
    Form_pg_type tform;
    Oid basetypid;
    int32 basetypmod;

    typtup = SearchSysCache1(TYPEOID, ObjectIdGetDatum(typid));
    if (!HeapTupleIsValid(typtup))
    {
        ereport(ERROR, (errmsg("type OID %u not found", typid)));
    }
    tform = (Form_pg_type) GETSTRUCT(typtup);

    basetypid = typid;
    basetypmod = typmod;

    if (tform->typbasetype != InvalidOid)
    {
        Oid base;
        int32 bmod;

        base = tform->typbasetype;
        bmod = tform->typtypmod;

        ReleaseSysCache(typtup);

        typtup = SearchSysCache1(TYPEOID, ObjectIdGetDatum(base));
        if (!HeapTupleIsValid(typtup))
        {
            ereport(ERROR, (errmsg("base type OID %u not found", base)));
        }
        tform = (Form_pg_type) GETSTRUCT(typtup);
        basetypid = base;
        basetypmod = bmod;
    }

    switch (basetypid)
    {
        case BOOLOID:
            ReleaseSysCache(typtup);
            return "bool";
        case INT2OID:
            ReleaseSysCache(typtup);
            return "int16_t";
        case INT4OID:
            ReleaseSysCache(typtup);
            return "int32_t";
        case INT8OID:
            ReleaseSysCache(typtup);
            return "int64_t";
        case FLOAT4OID:
            ReleaseSysCache(typtup);
            return "float";
        case FLOAT8OID:
            ReleaseSysCache(typtup);
            return "double";
        case OIDOID:
            ReleaseSysCache(typtup);
            return "Oid";
        case UUIDOID:
            ReleaseSysCache(typtup);
            return "pg_uuid_t";
        case CASHOID:
            ReleaseSysCache(typtup);
            return "int64_t";
        default:
            break;
    }

    if (basetypid == BPCHAROID)
    {
        int32 n;

        n = (basetypmod < (int32) VARHDRSZ) ? -1 : basetypmod - VARHDRSZ;
        ReleaseSysCache(typtup);
        if (n <= 0)
        {
            ereport(ERROR, (errmsg("bpchar without length is not fixed-size")));
        }
        resetStringInfo(scratch);
        appendStringInfo(scratch, "char[%d]", n);
        return scratch->data;
    }

    if (basetypid == BITOID || basetypid == VARBITOID)
    {
        int32 nbits;
        int32 nbytes;

        nbits = (basetypmod < 0) ? -1 : basetypmod;
        ReleaseSysCache(typtup);
        if (nbits <= 0)
        {
            ereport(ERROR, (errmsg("bit/varbit without length is not fixed-size")));
        }
        nbytes = (nbits + 7) / 8;
        resetStringInfo(scratch);
        appendStringInfo(scratch, "unsigned char[%d]", nbytes);
        return scratch->data;
    }

    if (tform->typelem != InvalidOid && tform->typlen == -1)
    {
        ReleaseSysCache(typtup);
        ereport(ERROR, (errmsg("arrays are unsupported unless a fixed bound is encoded elsewhere")));
    }

    ReleaseSysCache(typtup);
    ereport(ERROR, (errmsg("unsupported type OID %u", basetypid)));
    return NULL;
}

PG_FUNCTION_INFO_V1(jit_type);
Datum
jit_type(PG_FUNCTION_ARGS)
{
    Oid type_oid;
    HeapTuple typtup;
    Form_pg_type tform;
    Oid nspid;
    Oid relid;
    TupleDesc tupdesc;
    StringInfoData out;
    StringInfoData scratch;
    const char *nspname;
    const char *compname;

    type_oid = PG_GETARG_OID(0);

    typtup = SearchSysCache1(TYPEOID, ObjectIdGetDatum(type_oid));
    if (!HeapTupleIsValid(typtup))
    {
        ereport(ERROR, (errmsg("type OID %u not found", type_oid)));
    }
    tform = (Form_pg_type) GETSTRUCT(typtup);

    if (tform->typtype != TYPTYPE_COMPOSITE || tform->typrelid == InvalidOid)
    {
        ReleaseSysCache(typtup);
        ereport(ERROR, (errmsg("OID %u is not a composite type", type_oid)));
    }

    nspid = tform->typnamespace;
    relid = tform->typrelid;
    ReleaseSysCache(typtup);

    tupdesc = lookup_rowtype_tupdesc(type_oid, -1);

    initStringInfo(&out);
    initStringInfo(&scratch);

    nspname = get_namespace_name(nspid);
    if (nspname == NULL)
    {
        nspname = "public";
    }

    compname = get_rel_name(relid);
    if (compname == NULL)
    {
        compname = "type";
    }

    appendStringInfo(&out, "typedef struct {\n");

    for (int i = 0; i < tupdesc->natts; i++)
    {
        Form_pg_attribute a;
        const char *ctype;

        a = TupleDescAttr(tupdesc, i);
        if (a->attisdropped)
        {
            continue;
        }
        if (a->attnum <= 0)
        {
            continue;
        }

        ctype = map_type(a->atttypid, a->atttypmod, &scratch);
		emit_field(&out, ctype, NameStr(a->attname));
        //appendStringInfo(&out, "    %s %s;\n", ctype, NameStr(a->attname));
    }

    appendStringInfo(&out, "} %s;", compname);

    ReleaseTupleDesc(tupdesc);

    PG_RETURN_TEXT_P(cstring_to_text_with_len(out.data, out.len));
}

void get_field_layout(Oid typid, int32 typmod, FieldLayout *out, int attidx)
{
    HeapTuple typtup;
    Form_pg_type tform;
    Oid base;
    int32 bmod;

    typtup = SearchSysCache1(TYPEOID, ObjectIdGetDatum(typid));
    if (!HeapTupleIsValid(typtup))
    {
        ereport(ERROR, (errmsg("type OID %u not found", typid)));
    }
    tform = (Form_pg_type) GETSTRUCT(typtup);

    base = typid;
    bmod = typmod;

    if (tform->typbasetype != InvalidOid)
    {
        base = tform->typbasetype;
        bmod = tform->typtypmod;
        ReleaseSysCache(typtup);

        typtup = SearchSysCache1(TYPEOID, ObjectIdGetDatum(base));
        if (!HeapTupleIsValid(typtup))
        {
            ereport(ERROR, (errmsg("base type OID %u not found", base)));
        }
        tform = (Form_pg_type) GETSTRUCT(typtup);
    }

    if (tform->typelem != InvalidOid && tform->typlen == -1)
    {
        ReleaseSysCache(typtup);
        ereport(ERROR,
                (errmsg("attribute %d: array types are not supported", attidx + 1)));
    }

    switch (base)
    {
        case BOOLOID:
            out->kind  = FK_BOOL;
            out->size  = sizeof(bool);
            out->align = _Alignof(bool);
            break;

        case INT2OID:
            out->kind  = FK_INT16;
            out->size  = sizeof(int16);
            out->align = _Alignof(int16);
            break;

        case INT4OID:
            out->kind  = FK_INT32;
            out->size  = sizeof(int32);
            out->align = _Alignof(int32);
            break;

        case INT8OID:
            out->kind  = FK_INT64;
            out->size  = sizeof(int64);
            out->align = _Alignof(int64);
            break;

        case FLOAT4OID:
            out->kind  = FK_FLOAT4;
            out->size  = sizeof(float4);
            out->align = _Alignof(float4);
            break;

        case FLOAT8OID:
            out->kind  = FK_FLOAT8;
            out->size  = sizeof(float8);
            out->align = _Alignof(float8);
            break;

        case OIDOID:
            out->kind  = FK_OID;
            out->size  = sizeof(Oid);
            out->align = _Alignof(Oid);
            break;

        case UUIDOID:
            out->kind  = FK_UUID;
            out->size  = 16;
            out->align = _Alignof(pg_uuid_t);
            break;

        case CASHOID:
            out->kind  = FK_MONEY;
            out->size  = sizeof(int64);
            out->align = _Alignof(int64);
            break;

        case BPCHAROID:
        {
            int32 ndecl;

            ndecl = (bmod < (int32) VARHDRSZ) ? -1 : bmod - VARHDRSZ;
            if (ndecl <= 0)
            {
                ReleaseSysCache(typtup);
                ereport(ERROR, (errmsg("bpchar without fixed length is unsupported")));
            }
            out->kind  = FK_BPCHAR;
            out->size  = (Size) ndecl;
            out->align = _Alignof(char);
            break;
        }

        case BITOID:
        case VARBITOID:
        {
            int32 nbits;
            int32 nbytes;

            nbits = (bmod < 0) ? -1 : bmod;
            if (nbits <= 0)
            {
                ReleaseSysCache(typtup);
                ereport(ERROR, (errmsg("bit/varbit without fixed length is unsupported")));
            }
            nbytes = (nbits + 7) / 8;

            out->kind  = FK_BITBYTES;
            out->size  = (Size) nbytes;
            out->align = _Alignof(unsigned char);
            break;
        }

        default:
            ReleaseSysCache(typtup);
            ereport(ERROR, (errmsg("unsupported attribute type OID %u", base)));
    }

    ReleaseSysCache(typtup);
}

Datum get_notnull_attr(HeapTuple tuple, TupleDesc tupdesc, int attno, const char *name)
{
    bool isnull;
    Datum d;

    d = heap_getattr(tuple, attno, tupdesc, &isnull);
    if (isnull)
    {
        ereport(ERROR,
                (errmsg("attribute \"%s\" cannot be NULL", name)));
    }
    return d;
}
