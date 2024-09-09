#include "scalar.h"

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
		else if (type_code == GrB_INT16_CODE)
		{
			data_size = sizeof(int16_t);
		}
		else if (type_code == GrB_FP64_CODE)
		{
			data_size = sizeof(double);
		}
		else if (type_code == GrB_FP32_CODE)
		{
			data_size = sizeof(float);
		}
		else if (type_code == GrB_BOOL_CODE)
		{
			data_size = sizeof(bool);
		}
		else
			elog(ERROR, "Unknown Type Code");
	}

	scalar->flat_size = ONESPARSE_SCALAR_FLATSIZE() + data_size;
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
		else if (flat->type_code == GrB_INT16_CODE)
		{
			ERRORIF(GrB_Scalar_extractElement((int16_t*)data, scalar->scalar) != GrB_SUCCESS,
					"Cannot extract Scalar element.");
		}
		else if (flat->type_code == GrB_FP64_CODE)
		{
			ERRORIF(GrB_Scalar_extractElement((double*)data, scalar->scalar) != GrB_SUCCESS,
					"Cannot extract Scalar element.");
		}
		else if (flat->type_code == GrB_FP32_CODE)
		{
			ERRORIF(GrB_Scalar_extractElement((double*)data, scalar->scalar) != GrB_SUCCESS,
					"Cannot extract Scalar element.");
		}
		else if (flat->type_code == GrB_BOOL_CODE)
		{
			ERRORIF(GrB_Scalar_extractElement((bool*)data, scalar->scalar) != GrB_SUCCESS,
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
	else if (flat->type_code == GrB_INT16_CODE)
	{
		type = GrB_INT16;
	}
	else if (flat->type_code == GrB_FP64_CODE)
	{
		type = GrB_FP64;
	}
	else if (flat->type_code == GrB_FP32_CODE)
	{
		type = GrB_FP32;
	}
	else if (flat->type_code == GrB_BOOL_CODE)
	{
		type = GrB_BOOL;
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
		else if (type == GrB_INT16)
		{
			ERRORIF(GrB_Scalar_setElement(scalar->scalar, *(int16_t*)data) != GrB_SUCCESS,
					"Cannot set scalar element in expand.");
		}
		else if (type == GrB_FP64)
		{
			ERRORIF(GrB_Scalar_setElement(scalar->scalar, *(double*)data) != GrB_SUCCESS,
					"Cannot set scalar element in expand.");
		}
		else if (type == GrB_FP32)
		{
			ERRORIF(GrB_Scalar_setElement(scalar->scalar, *(float*)data) != GrB_SUCCESS,
					"Cannot set scalar element in expand.");
		}
		else if (type == GrB_BOOL)
		{
			ERRORIF(GrB_Scalar_setElement(scalar->scalar, *(bool*)data) != GrB_SUCCESS,
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

Datum _scalar_in(char *input)
{
	onesparse_Scalar *scalar;
	size_t len;
	GrB_Type typ;
	char str_val[GxB_MAX_NAME_LEN];
	char str_type[GxB_MAX_NAME_LEN];
	char *fmt = "";

	len = strlen(input);

	sscanf(input, "%[^:]:%s", str_type, str_val);

	if (strcmp(str_type, "i8") == 0)
	{
		int64_t value;
		typ = GrB_INT64;
		fmt = "%ld";
		scalar = new_scalar(typ, CurrentMemoryContext, NULL);
		if (len)
		{
			if (sscanf(str_val, fmt, &value) == 1)
			{
				ERRORIF(GrB_Scalar_setElement(scalar->scalar, value) != GrB_SUCCESS,
						"Cannot set scalar element in expand.");
			}
			else
				elog(ERROR, "Invalid format for %s %s", fmt, str_val);
		}
	}
	else if (strcmp(str_type, "i4") == 0)
	{
		int32_t value;
		typ = GrB_INT32;
		fmt = "%i";
		scalar = new_scalar(typ, CurrentMemoryContext, NULL);
		if (len)
		{
			if (sscanf(str_val, fmt, &value) == 1)
			{
				ERRORIF(GrB_Scalar_setElement(scalar->scalar, value) != GrB_SUCCESS,
						"Cannot set scalar element in expand.");
			}
			else
				elog(ERROR, "Invalid format for %s %s", fmt, str_val);
		}
	}
	else if (strcmp(str_type, "i2") == 0)
	{
		int16_t value;
		typ = GrB_INT16;
		fmt = "%i";
		scalar = new_scalar(typ, CurrentMemoryContext, NULL);
		if (len)
		{
			if (sscanf(str_val, fmt, &value) == 1)
			{
				ERRORIF(GrB_Scalar_setElement(scalar->scalar, value) != GrB_SUCCESS,
						"Cannot set scalar element in expand.");
			}
			else
				elog(ERROR, "Invalid format for %s %s", fmt, str_val);
		}
	}
	else if (strcmp(str_type, "f8") == 0)
	{
		double value;
		typ = GrB_FP64;
		fmt = "%lf";
		scalar = new_scalar(typ, CurrentMemoryContext, NULL);
		if (len)
		{
			if (sscanf(str_val, fmt, &value) == 1)
			{
				ERRORIF(GrB_Scalar_setElement(scalar->scalar, value) != GrB_SUCCESS,
						"Cannot set scalar element in expand.");
			}
			else
				elog(ERROR, "Invalid format for %s %s", fmt, str_val);
		}
	}
	else if (strcmp(str_type, "f4") == 0)
	{
		float value;
		typ = GrB_FP32;
		fmt = "%f";
		scalar = new_scalar(typ, CurrentMemoryContext, NULL);
		if (len)
		{
			if (sscanf(str_val, fmt, &value) == 1)
			{
				ERRORIF(GrB_Scalar_setElement(scalar->scalar, value) != GrB_SUCCESS,
						"Cannot set scalar element in expand.");
			}
			else
				elog(ERROR, "Invalid format for %s %s", fmt, str_val);
		}
	}
	else if (strcmp(str_type, "b") == 0)
	{
		char value;
		typ = GrB_BOOL;
		fmt = "%c";
		scalar = new_scalar(typ, CurrentMemoryContext, NULL);
		if (len)
		{
			if (sscanf(str_val, fmt, &value) == 1)
			{
				bool _value;
				if (value == 't')
					_value = true;
				else if (value == 'f')
					_value = false;
				else
					elog(ERROR, "Invalid value for bool %c", value);
				ERRORIF(GrB_Scalar_setElement(scalar->scalar, _value) != GrB_SUCCESS,
						"Cannot set scalar element in expand.");
			}
			else
				elog(ERROR, "Invalid format for %s %s", fmt, str_val);
		}
	}
	else
		elog(ERROR, "Unknown type code %s", str_type);
	ONESPARSE_RETURN_SCALAR(scalar);
}

Datum scalar_in(PG_FUNCTION_ARGS)
{
	char *input;
	input = PG_GETARG_CSTRING(0);
	return _scalar_in(input);
}

Datum scalar_out(PG_FUNCTION_ARGS)
{
	char *result, *sname;
	int32_t type_code;
	onesparse_Scalar *scalar;
	GrB_Index nvals;

	LOGF();
	scalar = ONESPARSE_GETARG_SCALAR(0);

	nvals = 0;
	ERRORIF(GrB_Scalar_nvals(&nvals, scalar->scalar) != GrB_SUCCESS,
			"Error extracting scalar nvals.");

	ERRORIF(GrB_get(scalar->scalar, &type_code, GrB_EL_TYPE_CODE) != GrB_SUCCESS,
			"Cannot get Scalar Type code.");

	sname = short_name(type_code);

	if (nvals)
	{
		if (type_code == GrB_INT64_CODE)
		{
			int64_t value;
			ERRORIF(GrB_Scalar_extractElement(&value, scalar->scalar) != GrB_SUCCESS,
					"Error extracting scalar element.");
			result = palloc(23);
			snprintf(result, 23, "%s:" "%" PRIi64, sname, value);
		}
		else if (type_code == GrB_INT32_CODE)
		{
			int32_t value;
			ERRORIF(GrB_Scalar_extractElement(&value, scalar->scalar) != GrB_SUCCESS,
					"Error extracting scalar element.");
			result = palloc(13);
			snprintf(result, 13, "%s:" "%" PRIi32, sname, value);
		}
		else if (type_code == GrB_INT16_CODE)
		{
			int16_t value;
			ERRORIF(GrB_Scalar_extractElement(&value, scalar->scalar) != GrB_SUCCESS,
					"Error extracting scalar element.");
			result = palloc(8);
			snprintf(result, 8, "%s:" "%" PRIi16, sname, value);
		}
		else if (type_code == GrB_FP64_CODE)
		{
			double value;
			ERRORIF(GrB_Scalar_extractElement(&value, scalar->scalar) != GrB_SUCCESS,
					"Error extracting scalar element.");
			result = palloc(21);
			snprintf(result, 21, "%s:%f", sname, value);
		}
		else if (type_code == GrB_FP32_CODE)
		{
			float value;
			ERRORIF(GrB_Scalar_extractElement(&value, scalar->scalar) != GrB_SUCCESS,
					"Error extracting scalar element.");
			result = palloc(12);
			snprintf(result, 12, "%s:%f", sname, value);
		}
		else if (type_code == GrB_BOOL_CODE)
		{
			bool value;
			ERRORIF(GrB_Scalar_extractElement(&value, scalar->scalar) != GrB_SUCCESS,
					"Error extracting scalar element.");
			result = palloc(7);
			snprintf(result, 7, "%s:%s", sname, value ? "t" : "f");
		}
		else
			elog(ERROR, "Unsupported type code %i.", type_code);
	}
	else
	{
		result = palloc(4);
		snprintf(result, 4, "%s:", sname);
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

#define SUFFIX _int16                // suffix for names
#define PG_TYPE int16                // postgres type
#define GB_TYPE GrB_INT16            // graphblas vector type
#define PG_GETARG PG_GETARG_INT16       // how to get value args
#define PG_RETURN PG_RETURN_INT16
#include "scalar_ops.h"

#define SUFFIX _fp64                // suffix for names
#define PG_TYPE float8                // postgres type
#define GB_TYPE GrB_FP64            // graphblas vector type
#define PG_GETARG PG_GETARG_FLOAT8       // how to get value args
#define PG_RETURN PG_RETURN_FLOAT8
#include "scalar_ops.h"

#define SUFFIX _fp32                // suffix for names
#define PG_TYPE float4                // postgres type
#define GB_TYPE GrB_FP32            // graphblas vector type
#define PG_GETARG PG_GETARG_FLOAT4       // how to get value args
#define PG_RETURN PG_RETURN_FLOAT4
#include "scalar_ops.h"

#define SUFFIX _bool                // suffix for names
#define PG_TYPE bool                // postgres type
#define GB_TYPE GrB_BOOL            // graphblas vector type
#define PG_GETARG PG_GETARG_BOOL       // how to get value args
#define PG_RETURN PG_RETURN_BOOL
#define NO_SCALAR_MATH
#include "scalar_ops.h"

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
