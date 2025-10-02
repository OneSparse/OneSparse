#include "../onesparse.h"

static inline Size
buf_align(Size off, Size a)
{
	Size		mask;

	if (a <= 1)
	{
		return off;
	}
	mask = a - 1;
	return (off + mask) & ~mask;
}

/* produce '0'/'1' string (nbits + 1 for NUL) from big-endian bit order */
static char *
bytes_to_bit_cstr(const unsigned char *src, int nbits)
{
	char	   *s;
	int			j;

	s = palloc(nbits + 1);
	for (j = 0; j < nbits; j++)
	{
		int			byte_index = j >> 3;
		int			bit_in_byte = 7 - (j & 7);	/* MSB first */
		int			bit = (src[byte_index] >> bit_in_byte) & 1;

		s[j] = bit ? '1' : '0';
	}
	s[nbits] = '\0';
	return s;
}

/* Build a Datum for bpchar(bit/varbit) via the type's input function */
static Datum
cstring_to_typed_datum(Oid atttypid, int32 atttypmod, const char *cstr)
{
	Oid			inputfunc;
	Oid			typioparam;

	getTypeInputInfo(atttypid, &inputfunc, &typioparam);
	return OidInputFunctionCall(inputfunc, (char *) cstr, typioparam, atttypmod);
}

PG_FUNCTION_INFO_V1(scalar_row);
Datum
scalar_row(PG_FUNCTION_ARGS)
{
	bytea	   *data;
	Oid			rettype;
	TupleDesc	tupdesc;
	HeapTuple	tuple;
	Datum	   *values;
	bool	   *nulls;

	const char *buf;
	Size		len;
	Size		off = 0;
	Size		maxalign = 1;

	int			natts;
	FieldLayout *layouts;
	Size	   *offsets;

	if (PG_NARGS() != 2)
	{
		ereport(ERROR, (errmsg("expected 2 arguments")));
	}

	/*
	 * 1) get the requested return type from call context (anchored by
	 * anyelement arg)
	 */
	rettype = get_fn_expr_argtype(fcinfo->flinfo, 1);
	if (!OidIsValid(rettype))
	{
		ereport(ERROR,
				(errmsg("could not resolve return type from anchor argument"),
				 errhint("Call as struct_row(data, NULL::<your_composite_type>).")));
	}
	/* 2) validate return type is composite and obtain tuple descriptor */
	{
		HeapTuple	typtup;
		Form_pg_type tform;

		typtup = SearchSysCache1(TYPEOID, ObjectIdGetDatum(rettype));
		if (!HeapTupleIsValid(typtup))
		{
			ereport(ERROR, (errmsg("return type OID %u not found", rettype)));
		}
		tform = (Form_pg_type) GETSTRUCT(typtup);
		if (tform->typtype != TYPTYPE_COMPOSITE || tform->typrelid == InvalidOid)
		{
			ReleaseSysCache(typtup);
			ereport(ERROR, (errmsg("return type must be a composite type")));
		}
		ReleaseSysCache(typtup);
	}

	tupdesc = lookup_rowtype_tupdesc(rettype, -1);

	/* 3) get buffer */
	data = PG_GETARG_BYTEA_PP(0);
	buf = VARDATA_ANY(data);
	len = VARSIZE_ANY_EXHDR(data);

	natts = tupdesc->natts;
	layouts = palloc(sizeof(FieldLayout) * natts);
	offsets = palloc(sizeof(Size) * natts);

	/* 4) compute layout and offsets exactly like the forward packer */
	for (int i = 0; i < natts; i++)
	{
		Form_pg_attribute a;

		a = TupleDescAttr(tupdesc, i);
		if (a->attisdropped || a->attnum <= 0)
		{
			layouts[i].kind = -1;
			layouts[i].size = 0;
			layouts[i].align = 1;
			offsets[i] = ~((Size) 0);
			continue;
		}

		get_field_layout(a->atttypid, a->atttypmod, &layouts[i], i);

		off = buf_align(off, layouts[i].align);
		offsets[i] = off;
		off += layouts[i].size;

		if (layouts[i].align > maxalign)
		{
			maxalign = layouts[i].align;
		}
	}

	off = buf_align(off, maxalign);

	if (len < off)
	{
		ReleaseTupleDesc(tupdesc);
		ereport(ERROR,
				(errmsg("input bytea too small for computed struct layout"),
				 errdetail("needed %zu bytes, got %zu bytes", (size_t) off, (size_t) len)));
	}

	values = palloc0(sizeof(Datum) * natts);
	nulls = palloc0(sizeof(bool) * natts);

	/* 5) reconstruct each attribute Datum from the buffer */
	for (int i = 0; i < natts; i++)
	{
		Form_pg_attribute a;
		const char *p;

		a = TupleDescAttr(tupdesc, i);
		if (a->attisdropped || a->attnum <= 0)
		{
			nulls[i] = true;
			continue;
		}

		if (offsets[i] == ~((Size) 0))
		{
			nulls[i] = true;
			continue;
		}

		p = buf + offsets[i];

		switch (layouts[i].kind)
		{
			case FK_BOOL:
				{
					bool		v;

					memcpy(&v, p, sizeof(v));
					values[i] = BoolGetDatum(v);
					break;
				}
			case FK_INT16:
				{
					int16		v;

					memcpy(&v, p, sizeof(v));
					values[i] = Int16GetDatum(v);
					break;
				}
			case FK_INT32:
				{
					int32		v;

					memcpy(&v, p, sizeof(v));
					values[i] = Int32GetDatum(v);
					break;
				}
			case FK_INT64:
				{
					int64		v;

					memcpy(&v, p, sizeof(v));
					values[i] = Int64GetDatum(v);
					break;
				}
			case FK_FLOAT4:
				{
					float4		v;

					memcpy(&v, p, sizeof(v));
					values[i] = Float4GetDatum(v);
					break;
				}
			case FK_FLOAT8:
				{
					float8		v;

					memcpy(&v, p, sizeof(v));
					values[i] = Float8GetDatum(v);
					break;
				}
			case FK_OID:
				{
					Oid			v;

					memcpy(&v, p, sizeof(v));
					values[i] = ObjectIdGetDatum(v);
					break;
				}
			case FK_UUID:
				{
					pg_uuid_t  *u;

					u = palloc(sizeof(pg_uuid_t));
					memcpy(u->data, p, 16);
					values[i] = UUIDPGetDatum(u);
					break;
				}
			case FK_MONEY:
				{
					int64		v;

					memcpy(&v, p, sizeof(v));
					values[i] = Int64GetDatum(v);
					break;
				}
			case FK_BPCHAR:
				{
					/*
					 * Our packed bytes are a raw n-byte char[]; turn it into
					 * bpchar via input.
					 */
					int			ndecl;
					int			n = (int) layouts[i].size;
					char	   *tmp;
					int			copylen = n;

					/*
					 * Treat trailing zero bytes as padding -> trim for
					 * cstring
					 */
					while (copylen > 0 && p[copylen - 1] == '\0')
					{
						copylen--;
					}

					tmp = palloc(copylen + 1);
					memcpy(tmp, p, copylen);
					tmp[copylen] = '\0';

					ndecl = (a->atttypmod < (int32) VARHDRSZ) ? -1 : a->atttypmod - VARHDRSZ;
					if (ndecl <= 0 || ndecl != n)
					{
						pfree(tmp);
						ReleaseTupleDesc(tupdesc);
						ereport(ERROR, (errmsg("bpchar length mismatch")));
					}

					values[i] = cstring_to_typed_datum(a->atttypid, a->atttypmod, tmp);
					pfree(tmp);
					break;
				}
			case FK_BITBYTES:
				{
					/*
					 * Convert raw bytes back to a '0'/'1' string using nbits
					 * from typmod
					 */
					int32		nbits = a->atttypmod;	/* for bit/varbit typmod
														 * is the bit length */
					char	   *bitstr;

					if (nbits <= 0)
					{
						ReleaseTupleDesc(tupdesc);
						ereport(ERROR, (errmsg("bit/varbit typmod not fixed-length")));
					}
					bitstr = bytes_to_bit_cstr((const unsigned char *) p, nbits);
					values[i] = cstring_to_typed_datum(a->atttypid, a->atttypmod, bitstr);
					pfree(bitstr);
					break;
				}
			default:
				{
					ReleaseTupleDesc(tupdesc);
					ereport(ERROR, (errmsg("unhandled field kind")));
				}
		}

		nulls[i] = false;
	}

	tuple = heap_form_tuple(tupdesc, values, nulls);

	ReleaseTupleDesc(tupdesc);
	PG_RETURN_DATUM(HeapTupleGetDatum(tuple));
}
