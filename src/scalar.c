#include "scalar.h"
PG_MODULE_MAGIC;

static Size scalar_get_flat_size(ExpandedObjectHeader *eohptr) {
	onesparse_Scalar *scalar;
	int32_t type_code;
	GrB_Index nvals;
	Size data_size;

	LOGF();

	scalar = (onesparse_Scalar*) eohptr;
	Assert(scalar->em_magic == scalar_MAGIC);

	if (scalar->flat_size)
		return scalar->flat_size;

	data_size = 0;

	ERRORIF(GrB_get(scalar->scalar, &type_code, GrB_EL_TYPE_CODE) != GrB_SUCCESS,
			"Cannot get Scalar Type code.");

	ERRORIF(GrB_Scalar_nvals(&nvals, scalar->scalar) != GrB_SUCCESS,
			"Error extracting scalar nvals.");

	if (nvals)
	{
		if (type_code == GrB_INT64_CODE)
		{
			data_size = sizeof(int64_t);
		}
		else if (type_code == GrB_INT32_CODE)
		{
			data_size = sizeof(int32_t);
		}
		else
			elog(ERROR, "Unknown Type Code");
	}

	scalar->flat_size = ONESPARSE_SCALAR_FLATSIZE() + data_size;
	elog(INFO, "flat size %zu", scalar->flat_size);

	return scalar->flat_size;
}

/* Flatten scalar into a pre-allocated result buffer that is
   allocated_size in bytes.  */
static void flatten_scalar(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size)
{
	GrB_Index nvals;
	onesparse_Scalar *scalar;
	onesparse_FlatScalar *flat;
	void* data;

	LOGF();

	scalar = (onesparse_Scalar *) eohptr;
	flat = (onesparse_FlatScalar *) result;

	Assert(scalar->em_magic == scalar_MAGIC);
	Assert(allocated_size == scalar->flat_size);
	memset(flat, 0, allocated_size);

	ERRORIF(GrB_get(scalar->scalar, &flat->type_code, GrB_EL_TYPE_CODE) != GrB_SUCCESS,
			"Cannot get Scalar Type code.");

	ERRORIF(GrB_Scalar_nvals(&nvals, scalar->scalar) != GrB_SUCCESS,
			"Error extracting scalar nvals.");

	flat->nvals = nvals ? 1 : 0;
	if (flat->nvals)
	{
		data = ONESPARSE_SCALAR_DATA(flat);
		if (flat->type_code == GrB_INT64_CODE)
		{
			ERRORIF(GrB_Scalar_extractElement((int64_t*)data, scalar->scalar) != GrB_SUCCESS,
					"Cannot extract Scalar element.");
		}
		else if (flat->type_code == GrB_INT32_CODE)
		{
			ERRORIF(GrB_Scalar_extractElement((int32_t*)data, scalar->scalar) != GrB_SUCCESS,
					"Cannot extract Scalar element.");
		}
		else
			elog(ERROR, "Unknown scalar type.");
	}
	SET_VARSIZE(flat, allocated_size);
}

/* Construct an empty expanded scalar. */
onesparse_Scalar* new_scalar(
	GrB_Type type,
	MemoryContext parentcontext,
	GrB_Scalar _scalar)
{
	onesparse_Scalar *scalar;

	MemoryContext objcxt, oldcxt;
	MemoryContextCallback *ctxcb;

	LOGF();
	objcxt = AllocSetContextCreate(parentcontext,
								   "expanded scalar",
								   ALLOCSET_DEFAULT_SIZES);

	scalar = MemoryContextAlloc(objcxt,	sizeof(onesparse_Scalar));

	EOH_init_header(&scalar->hdr, &scalar_methods, objcxt);

	oldcxt = MemoryContextSwitchTo(objcxt);

	scalar->em_magic = scalar_MAGIC;
	scalar->flat_size = 0;

	if (_scalar == NULL)
	{
		ERRORIF(GrB_Scalar_new(&scalar->scalar, type) != GrB_SUCCESS,
				"Cannot create new Scalar.");
	}
	else
	{
		scalar->scalar = _scalar;
	}

	ctxcb = MemoryContextAlloc(objcxt, sizeof(MemoryContextCallback));

	ctxcb->func = context_callback_scalar_free;
	ctxcb->arg = scalar;

	MemoryContextRegisterResetCallback(objcxt, ctxcb);

	MemoryContextSwitchTo(oldcxt);
	return scalar;
}

/* Expand a flat scalar in to an Expanded one, return as Postgres Datum. */
Datum expand_scalar(onesparse_FlatScalar *flat, MemoryContext parentcontext)
{
	GrB_Type type;
	onesparse_Scalar *scalar;
	void* data;

	LOGF();

	if (flat->type_code == GrB_INT64_CODE)
	{
		type = GrB_INT64;
	}
	else if (flat->type_code == GrB_INT32_CODE)
	{
		type = GrB_INT32;
	}
	else
		elog(ERROR, "Unknown type code.");

	scalar = new_scalar(type, parentcontext, NULL);
	if (flat->nvals)
	{
		data = ONESPARSE_SCALAR_DATA(flat);
		if (type == GrB_INT64)
		{
			ERRORIF(GrB_Scalar_setElement(scalar->scalar, *(int64_t*)data) != GrB_SUCCESS,
					"Cannot set scalar element in expand.");
		}
		else if (type == GrB_INT32)
		{
			ERRORIF(GrB_Scalar_setElement(scalar->scalar, *(int32_t*)data) != GrB_SUCCESS,
					"Cannot set scalar element in expand.");
		}
		else
			elog(ERROR, "Unknown type code.");
	}
	ONESPARSE_RETURN_SCALAR(scalar);
}

static void
context_callback_scalar_free(void* ptr)
{
	onesparse_Scalar *scalar = (onesparse_Scalar *) ptr;
	LOGF();

	ERRORIF(GrB_Scalar_free(&scalar->scalar) != GrB_SUCCESS,
			"Cannot GrB_Free Scalar");
}

/* Helper function to always expand datum

   This is used by PG_GETARG_SCALAR */
onesparse_Scalar* DatumGetScalar(Datum datum)
{
	onesparse_Scalar *scalar;
	onesparse_FlatScalar *flat;

	LOGF();
	if (VARATT_IS_EXTERNAL_EXPANDED_RW(DatumGetPointer(datum))) {
		scalar = ScalarGetEOHP(datum);
		Assert(scalar->em_magic == scalar_MAGIC);
		return scalar;
	}
	flat = ONESPARSE_DETOAST_SCALAR(datum);
	datum = expand_scalar(flat, CurrentMemoryContext);
	return ScalarGetEOHP(datum);
}

Datum scalar_in(PG_FUNCTION_ARGS) {
	onesparse_FlatScalar *flat;
	char *input;
	size_t len;
	int bc;
	Datum d;

	input = PG_GETARG_CSTRING(0);
	len = strlen(input);
	bc = (len) / 2 + VARHDRSZ;
	flat = palloc(bc);
	hex_decode(input, len, (char*)flat);
	d = expand_scalar(flat, CurrentMemoryContext);
	return d;
}

Datum scalar_out(PG_FUNCTION_ARGS)
{
	char *result;
	int32_t type_code;
	onesparse_Scalar *scalar;
	GrB_Index nvals;

	LOGF();
	scalar = ONESPARSE_GETARG_SCALAR(0);

	nvals = 0;
	ERRORIF(GrB_Scalar_nvals(&nvals, scalar->scalar) != GrB_SUCCESS,
			"Error extracting scalar nvals.");

	if (nvals)
	{
		ERRORIF(GrB_get(scalar->scalar, &type_code, GrB_EL_TYPE_CODE) != GrB_SUCCESS,
				"Cannot get Scalar Type code.");

		if (type_code == GrB_INT64_CODE)
		{
			int64_t value;
			ERRORIF(GrB_Scalar_extractElement(&value, scalar->scalar) != GrB_SUCCESS,
					"Error extracting scalar element.");
			result = palloc(20);
			snprintf(result, 20, "%" PRIi64, value);
		}
		if (type_code == GrB_INT32_CODE)
		{
			int32_t value;
			ERRORIF(GrB_Scalar_extractElement(&value, scalar->scalar) != GrB_SUCCESS,
					"Error extracting scalar element.");
			result = palloc(10);
			snprintf(result, 10, "%" PRIi32, value);
		}
		else
			elog(ERROR, "Unsupported type code.");
	}
	else
	{
		result = palloc(1);
		snprintf(result, 1, "");
	}
	PG_RETURN_CSTRING(result);
}

Datum scalar_nvals(PG_FUNCTION_ARGS)
{
	GrB_Index result;
	onesparse_Scalar *scalar;

	LOGF();
	ERRORNULL(0);

	scalar = ONESPARSE_GETARG_SCALAR(0);

	ERRORIF(GrB_Scalar_nvals(&result, scalar->scalar) != GrB_SUCCESS,
			"Error extracting scalar nvals.");
	PG_RETURN_INT16(result ? 1 : 0);
}

#define SUFFIX _int64                // suffix for names
#define PG_TYPE int64                // postgres type
#define GB_TYPE GrB_INT64            // graphblas vector type
#define PG_GETARG PG_GETARG_INT64       // how to get value args
#define PG_RETURN PG_RETURN_INT64
#include "scalar_ops.h"

#define SUFFIX _int32                // suffix for names
#define PG_TYPE int32                // postgres type
#define GB_TYPE GrB_INT32            // graphblas vector type
#define PG_GETARG PG_GETARG_INT32       // how to get value args
#define PG_RETURN PG_RETURN_INT32
#include "scalar_ops.h"

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
