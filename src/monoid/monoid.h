#pragma once

#define monoid_MAGIC 689276819

typedef struct os_FlatMonoid
{
	int32		vl_len_;
	char		name[GxB_MAX_NAME_LEN];
}			os_FlatMonoid;

typedef struct os_Monoid
{
	ExpandedObjectHeader hdr;
	int			em_magic;
	Size		flat_size;
	GrB_Monoid	monoid;
	char	   *name;
}			os_Monoid;

Datum		expand_monoid(os_FlatMonoid * flat, MemoryContext parentcontext);

os_Monoid  *new_monoid(
					   char *name,
					   MemoryContext parentcontext);

os_Monoid  *DatumGetMonoid(Datum d);

#define OS_DETOAST_MONOID(_datum) (os_FlatMonoid*)PG_DETOAST_DATUM(datum)
#define OS_GETARG_MONOID(_arg_num)  DatumGetMonoid(PG_GETARG_DATUM(_arg_num))
#define OS_GETARG_MONOID_HANDLE_OR_NULL(_nargs, _arg_num) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_MONOID(_arg_num)->monoid : NULL;
#define OS_RETURN_MONOID(_monoid) return EOHPGetRWDatum(&(_monoid)->hdr)
#define OS_MONOID_FLATSIZE() MAXALIGN(sizeof(os_FlatMonoid))

#define OS_MONOID_DATA(_flat) ((char*)(_flat) + OS_MONOID_FLATSIZE())
#define MonoidGetEOHP(_datum) (os_Monoid *) DatumGetEOHP(_datum)
