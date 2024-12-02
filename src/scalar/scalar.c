#include "../onesparse.h"

static void context_callback_scalar_free(void*);
static Size scalar_get_flat_size(ExpandedObjectHeader *eohptr);

static void flatten_scalar(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size);

static const ExpandedObjectMethods scalar_methods = {
	scalar_get_flat_size,
	flatten_scalar
};

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

	OS_CHECK(GrB_get(scalar->scalar, &type_code, GrB_EL_TYPE_CODE),
		  scalar->scalar,
		  "Cannot get Scalar Type code.");

	OS_CHECK(GrB_Scalar_nvals(&nvals, scalar->scalar),
		  scalar->scalar,
		  "Error extracting scalar nvals.");

	data_size = 0;
	if (nvals)
		data_size = code_size(type_code);

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
		else if (flat->type_code == GrB_UINT64_CODE)
		{
			OS_CHECK(GrB_Scalar_extractElement((uint64_t*)data, scalar->scalar),
				  scalar->scalar,
				  "Cannot extract Scalar element.");
		}
		else if (flat->type_code == GrB_INT32_CODE)
		{
			OS_CHECK(GrB_Scalar_extractElement((int32_t*)data, scalar->scalar),
				  scalar->scalar,
				  "Cannot extract Scalar element.");
		}
		else if (flat->type_code == GrB_UINT32_CODE)
		{
			OS_CHECK(GrB_Scalar_extractElement((uint32_t*)data, scalar->scalar),
				  scalar->scalar,
				  "Cannot extract Scalar element.");
		}
		else if (flat->type_code == GrB_INT16_CODE)
		{
			OS_CHECK(GrB_Scalar_extractElement((int16_t*)data, scalar->scalar),
				  scalar->scalar,
				  "Cannot extract Scalar element.");
		}
		else if (flat->type_code == GrB_UINT16_CODE)
		{
			OS_CHECK(GrB_Scalar_extractElement((uint16_t*)data, scalar->scalar),
				  scalar->scalar,
				  "Cannot extract Scalar element.");
		}
		else if (flat->type_code == GrB_INT8_CODE)
		{
			OS_CHECK(GrB_Scalar_extractElement((int8_t*)data, scalar->scalar),
				  scalar->scalar,
				  "Cannot extract Scalar element.");
		}
		else if (flat->type_code == GrB_UINT8_CODE)
		{
			OS_CHECK(GrB_Scalar_extractElement((uint8_t*)data, scalar->scalar),
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

	type = code_type(flat->type_code);

	scalar = new_scalar(type, parentcontext, NULL);
	if (flat->nvals)
	{
		data = OS_SCALAR_DATA(flat);
		if (type == GrB_UINT64)
		{
			OS_CHECK(GrB_Scalar_setElement(scalar->scalar, *(uint64_t*)data),
				  scalar->scalar,
				  "Cannot set scalar element in expand.");
		}
		else if (type == GrB_INT64)
		{
			OS_CHECK(GrB_Scalar_setElement(scalar->scalar, *(int64_t*)data),
				  scalar->scalar,
				  "Cannot set scalar element in expand.");
		}
		else if (type == GrB_UINT32)
		{
			OS_CHECK(GrB_Scalar_setElement(scalar->scalar, *(uint32_t*)data),
				  scalar->scalar,
				  "Cannot set scalar element in expand.");
		}
		else if (type == GrB_INT32)
		{
			OS_CHECK(GrB_Scalar_setElement(scalar->scalar, *(int32_t*)data),
				  scalar->scalar,
				  "Cannot set scalar element in expand.");
		}
		else if (type == GrB_UINT16)
		{
			OS_CHECK(GrB_Scalar_setElement(scalar->scalar, *(uint16_t*)data),
				  scalar->scalar,
				  "Cannot set scalar element in expand.");
		}
		else if (type == GrB_INT16)
		{
			OS_CHECK(GrB_Scalar_setElement(scalar->scalar, *(int16_t*)data),
				  scalar->scalar,
				  "Cannot set scalar element in expand.");
		}
		else if (type == GrB_UINT8)
		{
			OS_CHECK(GrB_Scalar_setElement(scalar->scalar, *(uint8_t*)data),
				  scalar->scalar,
				  "Cannot set scalar element in expand.");
		}
		else if (type == GrB_INT8)
		{
			OS_CHECK(GrB_Scalar_setElement(scalar->scalar, *(int8_t*)data),
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

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
