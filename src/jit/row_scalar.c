#include "../onesparse.h"

PG_FUNCTION_INFO_V1(row_scalar);
Datum row_scalar(PG_FUNCTION_ARGS)
{
    HeapTupleHeader rec;
    Oid             typoid;
    int32           typmod;
    TupleDesc       tupdesc;
    HeapTupleData   tuple;
    int             natts;

    Size            maxalign = 1;
    Size            off = 0;
    Size            total;
    char           *buf;
    bytea          *out;
    FieldLayout *layouts;
    Size       *offsets;

    rec = PG_GETARG_HEAPTUPLEHEADER(0);

    typoid = HeapTupleHeaderGetTypeId(rec);
    typmod = HeapTupleHeaderGetTypMod(rec);
    tupdesc = lookup_rowtype_tupdesc(typoid, typmod);

    tuple.t_len  = HeapTupleHeaderGetDatumLength(rec);
    tuple.t_data = rec;

    natts = tupdesc->natts;

    layouts = palloc(sizeof(FieldLayout) * natts);
    offsets  = palloc(sizeof(Size) * natts);

    for (int i = 0; i < natts; i++)
    {
        Form_pg_attribute a;

        a = TupleDescAttr(tupdesc, i);
        if (a->attisdropped || a->attnum <= 0)
        {
            offsets[i] = ~((Size) 0);
            continue;
        }

        get_field_layout(a->atttypid, a->atttypmod, &layouts[i], i);

        off = TYPEALIGN(layouts[i].align, off);
        offsets[i] = off;
        off += layouts[i].size;

        if (layouts[i].align > maxalign)
        {
            maxalign = layouts[i].align;
        }
    }

    total = TYPEALIGN(maxalign, off);
    buf = palloc0(total);

    for (int i = 0; i < natts; i++)
    {
        Form_pg_attribute a;
        Size field_off;

        a = TupleDescAttr(tupdesc, i);
        field_off = offsets[i];

        if (a->attisdropped || a->attnum <= 0)
        {
            continue;
        }
        if (field_off == ~((Size) 0))
        {
            continue;
        }

        switch (layouts[i].kind)
        {
            case FK_BOOL:
            {
                bool v = DatumGetBool(get_notnull_attr(&tuple, tupdesc, a->attnum, NameStr(a->attname)));
                memcpy(buf + field_off, &v, sizeof(v));
                break;
            }

            case FK_INT16:
            {
                int16 v = DatumGetInt16(get_notnull_attr(&tuple, tupdesc, a->attnum, NameStr(a->attname)));
                memcpy(buf + field_off, &v, sizeof(v));
                break;
            }

            case FK_INT32:
            {
                int32 v = DatumGetInt32(get_notnull_attr(&tuple, tupdesc, a->attnum, NameStr(a->attname)));
                memcpy(buf + field_off, &v, sizeof(v));
                break;
            }

            case FK_INT64:
            {
                int64 v = DatumGetInt64(get_notnull_attr(&tuple, tupdesc, a->attnum, NameStr(a->attname)));
                memcpy(buf + field_off, &v, sizeof(v));
                break;
            }

            case FK_FLOAT4:
            {
                float4 v = DatumGetFloat4(get_notnull_attr(&tuple, tupdesc, a->attnum, NameStr(a->attname)));
                memcpy(buf + field_off, &v, sizeof(v));
                break;
            }

            case FK_FLOAT8:
            {
                float8 v = DatumGetFloat8(get_notnull_attr(&tuple, tupdesc, a->attnum, NameStr(a->attname)));
                memcpy(buf + field_off, &v, sizeof(v));
                break;
            }

            case FK_OID:
            {
                Oid v = DatumGetObjectId(get_notnull_attr(&tuple, tupdesc, a->attnum, NameStr(a->attname)));
                memcpy(buf + field_off, &v, sizeof(v));
                break;
            }

            case FK_UUID:
            {
                pg_uuid_t *u = DatumGetUUIDP(get_notnull_attr(&tuple, tupdesc, a->attnum, NameStr(a->attname)));
                memcpy(buf + field_off, u->data, 16);
                break;
            }

            case FK_MONEY:
            {
                int64 v = DatumGetInt64(get_notnull_attr(&tuple, tupdesc, a->attnum, NameStr(a->attname)));
                memcpy(buf + field_off, &v, sizeof(v));
                break;
            }

            case FK_BPCHAR:
            {
                Datum d = get_notnull_attr(&tuple, tupdesc, a->attnum, NameStr(a->attname));
                struct varlena *vl = (struct varlena *) PG_DETOAST_DATUM_PACKED(d);
                int32 ndecl = (a->atttypmod < (int32) VARHDRSZ) ? -1 : a->atttypmod - VARHDRSZ;
                int payload = VARSIZE_ANY_EXHDR(vl);
                char *p = VARDATA_ANY(vl);

                if (ndecl <= 0)
                {
                    ereport(ERROR, (errmsg("bpchar without fixed length is unsupported")));
                }
                if ((Size) ndecl != layouts[i].size)
                {
                    ereport(ERROR, (errmsg("bpchar length mismatch")));
                }
                if (payload > ndecl)
                {
                    payload = ndecl;
                }

                memset(buf + field_off, 0, layouts[i].size);
                memcpy(buf + field_off, p, payload);
                break;
            }

            case FK_BITBYTES:
            {
                Datum d = get_notnull_attr(&tuple, tupdesc, a->attnum, NameStr(a->attname));
                VarBit *vb = DatumGetVarBitP(d);
                int nbits = VARBITLEN(vb);
                int nbytes = (nbits + 7) / 8;

                if ((Size) nbytes != layouts[i].size)
                {
                    ereport(ERROR, (errmsg("bit/varbit length mismatch: value %d bits", nbits)));
                }
                memcpy(buf + field_off, VARBITS(vb), nbytes);
                break;
            }
        }
    }

    /* TODO: use `buf` (size `total`) as your C-ABI struct memory. */

    out = (bytea *) palloc(total + VARHDRSZ);
    SET_VARSIZE(out, total + VARHDRSZ);
    memcpy(VARDATA(out), buf, total);

    ReleaseTupleDesc(tupdesc);
    PG_RETURN_BYTEA_P(out);
}
