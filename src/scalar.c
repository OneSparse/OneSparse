#include "onesparse.h"

static void context_callback_scalar_free(void*);
static Size scalar_get_flat_size(ExpandedObjectHeader *eohptr);
Datum _scalar_in(char *input);

static void flatten_scalar(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size);

static const ExpandedObjectMethods scalar_methods = {
	scalar_get_flat_size,
	flatten_scalar
};

PG_FUNCTION_INFO_V1(scalar_in);
PG_FUNCTION_INFO_V1(scalar_out);
PG_FUNCTION_INFO_V1(scalar_nvals);
PG_FUNCTION_INFO_V1(scalar_wait);
PG_FUNCTION_INFO_V1(scalar_dup);
PG_FUNCTION_INFO_V1(scalar_clear);
PG_FUNCTION_INFO_V1(scalar_print);
PG_FUNCTION_INFO_V1(scalar_type);

static Size scalar_get_flat_size(ExpandedObjectHeader *eohptr) {
	os_Scalar *scalar;
	int32_t type_code;
	GrB_Index nvals;
	Size data_size;

	LOGF();

	scalar = (os_Scalar*) eohptr;
	Assert(scalar->em_magic == scalar_MAGIC);

	if (scalar->flat_size)
		return scalar->flat_size;

	data_size = 0;

	OS_CHECK(GrB_get(scalar->scalar, &type_code, GrB_EL_TYPE_CODE),
		  scalar->scalar,
		  "Cannot get Scalar Type code.");

	OS_CHECK(GrB_Scalar_nvals(&nvals, scalar->scalar),
		  scalar->scalar,
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

	scalar->flat_size = OS_SCALAR_FLATSIZE() + data_size;
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
	os_Scalar *scalar;
	os_FlatScalar *flat;
	void* data;

	LOGF();

	scalar = (os_Scalar *) eohptr;
	flat = (os_FlatScalar *) result;

	Assert(scalar->em_magic == scalar_MAGIC);
	Assert(allocated_size == scalar->flat_size);
	memset(flat, 0, allocated_size);

	OS_CHECK(GrB_get(scalar->scalar, &flat->type_code, GrB_EL_TYPE_CODE),
		  scalar->scalar,
		  "Cannot get Scalar Type code.");

	OS_CHECK(GrB_Scalar_nvals(&nvals, scalar->scalar),
		  scalar->scalar,
		  "Error extracting scalar nvals.");

	flat->nvals = nvals ? 1 : 0;
	if (flat->nvals)
	{
		data = OS_SCALAR_DATA(flat);
		if (flat->type_code == GrB_INT64_CODE)
		{
			OS_CHECK(GrB_Scalar_extractElement((int64_t*)data, scalar->scalar),
				  scalar->scalar,
				  "Cannot extract Scalar element.");
		}
		else if (flat->type_code == GrB_INT32_CODE)
		{
			OS_CHECK(GrB_Scalar_extractElement((int32_t*)data, scalar->scalar),
				  scalar->scalar,
				  "Cannot extract Scalar element.");
		}
		else if (flat->type_code == GrB_INT16_CODE)
		{
			OS_CHECK(GrB_Scalar_extractElement((int16_t*)data, scalar->scalar),
				  scalar->scalar,
				  "Cannot extract Scalar element.");
		}
		else if (flat->type_code == GrB_FP64_CODE)
		{
			OS_CHECK(GrB_Scalar_extractElement((double*)data, scalar->scalar),
				  scalar->scalar,
				  "Cannot extract Scalar element.");
		}
		else if (flat->type_code == GrB_FP32_CODE)
		{
			OS_CHECK(GrB_Scalar_extractElement((float*)data, scalar->scalar),
				  scalar->scalar,
				  "Cannot extract Scalar element.");
		}
		else if (flat->type_code == GrB_BOOL_CODE)
		{
			OS_CHECK(GrB_Scalar_extractElement((bool*)data, scalar->scalar),
				  scalar->scalar,
				  "Cannot extract Scalar element.");
		}
		else
			elog(ERROR, "Unknown scalar type.");
	}
	SET_VARSIZE(flat, allocated_size);
}

/* Construct an empty expanded scalar. */
os_Scalar* new_scalar(
	GrB_Type type,
	MemoryContext parentcontext,
	GrB_Scalar _scalar)
{
	os_Scalar *scalar;

	MemoryContext objcxt, oldcxt;
	MemoryContextCallback *ctxcb;

	LOGF();
	objcxt = AllocSetContextCreate(parentcontext,
								   "expanded scalar",
								   ALLOCSET_DEFAULT_SIZES);

	scalar = MemoryContextAlloc(objcxt,	sizeof(os_Scalar));

	EOH_init_header(&scalar->hdr, &scalar_methods, objcxt);

	oldcxt = MemoryContextSwitchTo(objcxt);

	scalar->em_magic = scalar_MAGIC;
	scalar->flat_size = 0;

	if (_scalar == NULL)
	{
		OS_CHECK(GrB_Scalar_new(&scalar->scalar, type),
			  scalar->scalar,
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
Datum expand_scalar(os_FlatScalar *flat, MemoryContext parentcontext)
{
	GrB_Type type;
	os_Scalar *scalar;
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
		data = OS_SCALAR_DATA(flat);
		if (type == GrB_INT64)
		{
			OS_CHECK(GrB_Scalar_setElement(scalar->scalar, *(int64_t*)data),
				  scalar->scalar,
				  "Cannot set scalar element in expand.");
		}
		else if (type == GrB_INT32)
		{
			OS_CHECK(GrB_Scalar_setElement(scalar->scalar, *(int32_t*)data),
				  scalar->scalar,
				  "Cannot set scalar element in expand.");
		}
		else if (type == GrB_INT16)
		{
			OS_CHECK(GrB_Scalar_setElement(scalar->scalar, *(int16_t*)data),
				  scalar->scalar,
				  "Cannot set scalar element in expand.");
		}
		else if (type == GrB_FP64)
		{
			OS_CHECK(GrB_Scalar_setElement(scalar->scalar, *(double*)data),
				  scalar->scalar,
				  "Cannot set scalar element in expand.");
		}
		else if (type == GrB_FP32)
		{
			OS_CHECK(GrB_Scalar_setElement(scalar->scalar, *(float*)data),
				  scalar->scalar,
				  "Cannot set scalar element in expand.");
		}
		else if (type == GrB_BOOL)
		{
			OS_CHECK(GrB_Scalar_setElement(scalar->scalar, *(bool*)data),
				  scalar->scalar,
				  "Cannot set scalar element in expand.");
		}
		else
			elog(ERROR, "Unknown type code.");
	}
	OS_RETURN_SCALAR(scalar);
}

static void
context_callback_scalar_free(void* ptr)
{
	os_Scalar *scalar = (os_Scalar *) ptr;
	LOGF();

	OS_CHECK(GrB_Scalar_free(&scalar->scalar),
		  scalar->scalar,
		  "Cannot GrB_Free Scalar");
}

/* Helper function to always expand datum

   This is used by PG_GETARG_SCALAR */
os_Scalar* DatumGetScalar(Datum datum)
{
	os_Scalar *scalar;
	os_FlatScalar *flat;

	LOGF();
	if (VARATT_IS_EXTERNAL_EXPANDED_RW(DatumGetPointer(datum))) {
		scalar = ScalarGetEOHP(datum);
		Assert(scalar->em_magic == scalar_MAGIC);
		return scalar;
	}
	flat = OS_DETOAST_SCALAR(datum);
	datum = expand_scalar(flat, CurrentMemoryContext);
	return ScalarGetEOHP(datum);
}

Datum _scalar_in(char *input)
{
	os_Scalar *scalar;
	size_t len;
	GrB_Type typ;
	char str_val[GxB_MAX_NAME_LEN];
	char str_type[GxB_MAX_NAME_LEN];
	char *fmt = "";

	len = strlen(input);

	sscanf(input, "%[^:]:%s", str_type, str_val);

	if (strcmp(str_type, "int64") == 0)
	{
		int64_t value;
		typ = GrB_INT64;
		fmt = "%ld";
		scalar = new_scalar(typ, CurrentMemoryContext, NULL);
		if (len)
		{
			if (sscanf(str_val, fmt, &value) == 1)
			{
				OS_CHECK(GrB_Scalar_setElement(scalar->scalar, value),
					  scalar->scalar,
					  "Cannot set scalar element in expand.");
			}
			else
				elog(ERROR, "Invalid format for %s %s", fmt, str_val);
		}
	}
	else if (strcmp(str_type, "int32") == 0)
	{
		int32_t value;
		typ = GrB_INT32;
		fmt = "%i";
		scalar = new_scalar(typ, CurrentMemoryContext, NULL);
		if (len)
		{
			if (sscanf(str_val, fmt, &value) == 1)
			{
				OS_CHECK(GrB_Scalar_setElement(scalar->scalar, value),
					  scalar->scalar,
					  "Cannot set scalar element in expand.");
			}
			else
				elog(ERROR, "Invalid format for %s %s", fmt, str_val);
		}
	}
	else if (strcmp(str_type, "int16") == 0)
	{
		int16_t value;
		typ = GrB_INT16;
		fmt = "%i";
		scalar = new_scalar(typ, CurrentMemoryContext, NULL);
		if (len)
		{
			if (sscanf(str_val, fmt, &value) == 1)
			{
				OS_CHECK(GrB_Scalar_setElement(scalar->scalar, value),
					  scalar->scalar,
					  "Cannot set scalar element in expand.");
			}
			else
				elog(ERROR, "Invalid format for %s %s", fmt, str_val);
		}
	}
	else if (strcmp(str_type, "fp64") == 0)
	{
		double value;
		typ = GrB_FP64;
		fmt = "%lf";
		scalar = new_scalar(typ, CurrentMemoryContext, NULL);
		if (len)
		{
			if (sscanf(str_val, fmt, &value) == 1)
			{
				OS_CHECK(GrB_Scalar_setElement(scalar->scalar, value),
					  scalar->scalar,
					  "Cannot set scalar element in expand.");
			}
			else
				elog(ERROR, "Invalid format for %s %s", fmt, str_val);
		}
	}
	else if (strcmp(str_type, "fp32") == 0)
	{
		float value;
		typ = GrB_FP32;
		fmt = "%f";
		scalar = new_scalar(typ, CurrentMemoryContext, NULL);
		if (len)
		{
			if (sscanf(str_val, fmt, &value) == 1)
			{
				OS_CHECK(GrB_Scalar_setElement(scalar->scalar, value),
					  scalar->scalar,
					  "Cannot set scalar element in expand.");
			}
			else
				elog(ERROR, "Invalid format for %s %s", fmt, str_val);
		}
	}
	else if (strcmp(str_type, "bool") == 0)
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
				OS_CHECK(GrB_Scalar_setElement(scalar->scalar, _value),
					  scalar->scalar,
					  "Cannot set scalar element in expand.");
			}
			else
				elog(ERROR, "Invalid format for %s %s", fmt, str_val);
		}
	}
	else
		elog(ERROR, "Unknown type code %s", str_type);
	OS_RETURN_SCALAR(scalar);
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
	os_Scalar *scalar;
	GrB_Index nvals;

	LOGF();
	scalar = OS_GETARG_SCALAR(0);

	nvals = 0;
	OS_CHECK(GrB_Scalar_nvals(&nvals, scalar->scalar),
		  scalar->scalar,
		  "Error extracting scalar nvals.");

	OS_CHECK(GrB_get(scalar->scalar, &type_code, GrB_EL_TYPE_CODE),
		  scalar->scalar,
		  "Cannot get Scalar Type code.");

	sname = short_code(type_code);

	if (nvals)
	{
		if (type_code == GrB_INT64_CODE)
		{
			int64_t value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:" "%" PRIi64, sname, value);
		}
		else if (type_code == GrB_INT32_CODE)
		{
			int32_t value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:" "%" PRIi32, sname, value);
		}
		else if (type_code == GrB_INT16_CODE)
		{
			int16_t value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:" "%" PRIi16, sname, value);
		}
		else if (type_code == GrB_FP64_CODE)
		{
			double value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:%f", sname, value);
		}
		else if (type_code == GrB_FP32_CODE)
		{
			float value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				  scalar->scalar,
				  "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:%f", sname, value);
		}
		else if (type_code == GrB_BOOL_CODE)
		{
			bool value;
			OS_CHECK(GrB_Scalar_extractElement(&value, scalar->scalar),
				   scalar->scalar,
				   "Error extracting scalar element.");
			result = palloc(GxB_MAX_NAME_LEN);
			snprintf(result, GxB_MAX_NAME_LEN, "%s:%s", sname, value ? "t" : "f");
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
	os_Scalar *scalar;

	LOGF();
	ERRORNULL(0);

	scalar = OS_GETARG_SCALAR(0);

	OS_CHECK(GrB_Scalar_nvals(&result, scalar->scalar),
		  scalar->scalar,
		  "Error extracting scalar nvals.");
	PG_RETURN_INT16(result ? 1 : 0);
}

Datum scalar_wait(PG_FUNCTION_ARGS)
{
	os_Scalar *scalar;
	int waitmode;

	LOGF();
	ERRORNULL(0);

	scalar = OS_GETARG_SCALAR(0);
	waitmode = PG_GETARG_INT32(1);

	OS_CHECK(GrB_Scalar_wait(scalar->scalar, waitmode),
		  scalar->scalar,
		  "Error waiting for scalar.");
	OS_RETURN_SCALAR(scalar);
}

Datum scalar_dup(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Scalar *scalar;
	os_Scalar *result;

	LOGF();
	ERRORNULL(0);

	scalar = OS_GETARG_SCALAR(0);
	OS_CHECK(GxB_Scalar_type(&type, scalar->scalar),
		  scalar->scalar,
		  "Cannot get scalar type");

	result = new_scalar(type, CurrentMemoryContext, NULL);

	OS_CHECK(GrB_Scalar_dup(&result->scalar, scalar->scalar),
		  scalar->scalar,
		  "Error duping scalar.");
	OS_RETURN_SCALAR(result);
}

Datum scalar_clear(PG_FUNCTION_ARGS)
{
	os_Scalar *scalar;

	LOGF();
	ERRORNULL(0);

	scalar = OS_GETARG_SCALAR(0);

	OS_CHECK(GrB_Scalar_clear(scalar->scalar),
		  scalar->scalar,
		  "Error clearing scalar.");
	OS_RETURN_SCALAR(scalar);
}

Datum scalar_print(PG_FUNCTION_ARGS) {
	int32 type_code;
	os_Scalar *s;
	GrB_Type type;
    StringInfoData buf;
	s = OS_GETARG_SCALAR(0);
	OS_STYPE(type, s);
    initStringInfo(&buf);

	OS_CHECK(GrB_get(s->scalar, &type_code, GrB_EL_TYPE_CODE),
		  s->scalar,
		  "Cannot get Scalar Type code.");

	switch(type_code)
	{
		case GrB_INT64_CODE:
			{
				int64_t vi64;
				OS_CHECK(GrB_Scalar_extractElement(&vi64, s->scalar),
					  s->scalar,
					  "Error extracting scalar value.");
				appendStringInfo(&buf, "%ld", vi64);
				break;
			}
		case GrB_INT32_CODE:
			{
				int32_t vi32;
				OS_CHECK(GrB_Scalar_extractElement(&vi32, s->scalar),
					  s->scalar,
					  "Error extracting scalar value.");
				appendStringInfo(&buf, "%d", vi32);
				break;
			}
		case GrB_INT16_CODE:
			{
				int32_t vi16;
				OS_CHECK(GrB_Scalar_extractElement(&vi16, s->scalar),
					  s->scalar,
					  "Error extracting scalar value.");
				appendStringInfo(&buf, "%d", vi16);
				break;
			}
		case GrB_FP64_CODE:
			{
				double fp64;
				OS_CHECK(GrB_Scalar_extractElement(&fp64, s->scalar),
					  s->scalar,
					  "Error extracting scalar value.");
				appendStringInfo(&buf, "%f", fp64);
				break;
			}
		case GrB_FP32_CODE:
			{
				float fp64;
				OS_CHECK(GrB_Scalar_extractElement(&fp64, s->scalar),
					  s->scalar,
					  "Error extracting scalar value.");
				appendStringInfo(&buf, "%f", fp64);
				break;
			}
	}

	PG_RETURN_TEXT_P(cstring_to_text(buf.data));
}

Datum scalar_type(PG_FUNCTION_ARGS) {
	os_Scalar *A;
	GrB_Type type;
	char *type_name;
	size_t type_name_len;
	os_Type *result;

	ERRORNULL(0);

	A = OS_GETARG_SCALAR(0);

	OS_CHECK(GxB_Scalar_type(&type, A->scalar),
		  A->scalar,
		  "Cannot get scalar type");

	OS_CHECK(GrB_get(type, &type_name_len, GrB_NAME),
		  type,
		  "Cannot get type name len.");

	type_name = palloc(type_name_len);

	OS_CHECK(GrB_get(type, type_name, GrB_NAME),
		  type,
		  "Cannot get type name.");

	for (int i = 4; i < type_name_len; i++)
	{
        type_name[i - 4] = tolower(type_name[i]);
    }
    type_name[type_name_len - 4] = '\0';
	result = new_type(type_name, CurrentMemoryContext);
	OS_RETURN_TYPE(result);
}

#define SUFFIX _int64                // suffix for names
#define PG_TYPE int64                // postgres type
#define GB_TYPE GrB_INT64            // graphblas scalar type
#define PG_GETARG PG_GETARG_INT64       // how to get value args
#define PG_RETURN PG_RETURN_INT64
#include "scalar_ops.h"

#define SUFFIX _int32                // suffix for names
#define PG_TYPE int32                // postgres type
#define GB_TYPE GrB_INT32            // graphblas scalar type
#define PG_GETARG PG_GETARG_INT32       // how to get value args
#define PG_RETURN PG_RETURN_INT32
#include "scalar_ops.h"

#define SUFFIX _int16                // suffix for names
#define PG_TYPE int16                // postgres type
#define GB_TYPE GrB_INT16            // graphblas scalar type
#define PG_GETARG PG_GETARG_INT16       // how to get value args
#define PG_RETURN PG_RETURN_INT16
#include "scalar_ops.h"

#define SUFFIX _fp64                // suffix for names
#define PG_TYPE float8                // postgres type
#define GB_TYPE GrB_FP64            // graphblas scalar type
#define PG_GETARG PG_GETARG_FLOAT8       // how to get value args
#define PG_RETURN PG_RETURN_FLOAT8
#include "scalar_ops.h"

#define SUFFIX _fp32                // suffix for names
#define PG_TYPE float4                // postgres type
#define GB_TYPE GrB_FP32            // graphblas scalar type
#define PG_GETARG PG_GETARG_FLOAT4       // how to get value args
#define PG_RETURN PG_RETURN_FLOAT4
#include "scalar_ops.h"

#define SUFFIX _bool                // suffix for names
#define PG_TYPE bool                // postgres type
#define GB_TYPE GrB_BOOL            // graphblas scalar type
#define PG_GETARG PG_GETARG_BOOL       // how to get value args
#define PG_RETURN PG_RETURN_BOOL
#define NO_SCALAR_MATH
#include "scalar_ops.h"

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
