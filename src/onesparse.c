
#include "onesparse.h"
PG_MODULE_MAGIC;

// Helper function to convert a PostgreSQL bigint[] to a C array of uint64_t
uint64_t* get_c_array_from_pg_array(FunctionCallInfo fcinfo, int arg_number, uint64_t *out_nelems) {
    ArrayType *input_array;
    Datum *elements;
    bool *nulls;
    int nelems;           // Intermediate variable to store the int number of elements
    uint64_t *c_array;
    int i;
    int64_t temp_value;

    // Fetch the input argument as a PostgreSQL array
    input_array = PG_GETARG_ARRAYTYPE_P(arg_number);

    // Deconstruct the PostgreSQL array into its components
    deconstruct_array(input_array,
                      INT8OID,       // OID for int64
                      8,             // size of int64 (8 bytes)
                      true,          // it is passed by value
                      'd',           // default alignment
                      &elements,     // array of Datums to be filled
                      &nulls,        // array of null flags to be filled
                      &nelems);      // number of elements (stored as int initially)

    // Allocate memory for the C array
    c_array = (uint64_t *) palloc(nelems * sizeof(uint64_t));

    // Convert each PostgreSQL Datum to uint64_t and store it in the C array
    for (i = 0; i < nelems; i++) {
        if (!nulls[i]) {
            // Convert the datum to int64_t
            temp_value = DatumGetInt64(elements[i]);

            // Throw an error if the value is negative
            if (temp_value < 0) {
                ereport(ERROR,
                        (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
                         errmsg("Negative values are not allowed in uint64_t array")));
            }

            // Store the non-negative value as uint64_t
            c_array[i] = (uint64_t) temp_value;
        } else {
            // Handle null values if needed (this example sets to 0)
            c_array[i] = 0;
        }
    }

    // Set the output parameter for the number of elements as uint64_t
    *out_nelems = (uint64_t) nelems;

    // Return the C array
    return c_array;
}

char* short_code(GrB_Type_Code code)
{
	switch(code)
	{
		case GrB_BOOL_CODE:
			return "bool";
		case GrB_INT64_CODE:
			return "int64";
		case GrB_UINT64_CODE:
			return "uint64";
		case GrB_INT32_CODE:
			return "int32";
		case GrB_UINT32_CODE:
			return "uint32";
		case GrB_INT16_CODE:
			return "int16";
		case GrB_UINT16_CODE:
			return "uint16";
		case GrB_INT8_CODE:
			return "int8";
		case GrB_UINT8_CODE:
			return "uint8";
		case GrB_FP64_CODE:
			return "fp64";
		case GrB_FP32_CODE:
			return "fp32";
		case GxB_FC32_CODE:
			return "fc32";
		case GxB_FC64_CODE:
			return "fc64";
		case GrB_UDT_CODE:
			return "utd";
	}
	elog(ERROR, "Unknown type code");
}

GrB_Type short_type(char *name)
{
	if (strcmp(name, "bool") == 0)
		return GrB_BOOL;
	if (strcmp(name, "int64") == 0)
		return GrB_INT64;
	if (strcmp(name, "uint64") == 0)
		return GrB_UINT64;
	if (strcmp(name, "int32") == 0)
		return GrB_INT32;
	if (strcmp(name, "uint32") == 0)
		return GrB_UINT32;
	if (strcmp(name, "int16") == 0)
		return GrB_INT16;
	if (strcmp(name, "uint16") == 0)
		return GrB_UINT16;
	if (strcmp(name, "fp64") == 0)
		return GrB_FP64;
	if (strcmp(name, "fp32") == 0)
		return GrB_FP32;
	if (strcmp(name, "fc64") == 0)
		return GxB_FC64;
	if (strcmp(name, "fc32") == 0)
		return GxB_FC32;
	elog(ERROR, "Unknown short name %s", name);
}

const char* error_name(GrB_Info info)
{
	switch(info)
	{
		case GrB_SUCCESS:
			return "SUCCESS";
		case GrB_NO_VALUE:
			return "NO_VALUE";
		case GxB_EXHAUSTED:
			return "EXHAUSTED";
		case GrB_UNINITIALIZED_OBJECT:
			return "UNINITIALIZED_OBJECT";
		case GrB_NULL_POINTER:
			return "NULL_POINTER";
		case GrB_INVALID_VALUE:
			return "INVALID_VALUE";
		case GrB_INVALID_INDEX:
			return "INVALID_INDEX";
		case GrB_DOMAIN_MISMATCH:
			return "DOMAIN_MISMATCH";
		case GrB_DIMENSION_MISMATCH:
			return "DIMENSION_MISMATCH";
		case GrB_OUTPUT_NOT_EMPTY:
			return "OUTPUT_NOT_EMPTY";
		case GrB_NOT_IMPLEMENTED:
			return "NOT_IMPLEMENTED";
		case GrB_ALREADY_SET:
			return "ALREADY_SET";
		case GrB_PANIC:
			return "PANIC";
		case GrB_OUT_OF_MEMORY:
			return "OUT_OF_MEMORY";
		case GrB_INSUFFICIENT_SPACE:
			return "INSUFFICIENT_SPACE";
		case GrB_INVALID_OBJECT:
			return "INVALID_OBJECT";
		case GrB_INDEX_OUT_OF_BOUNDS:
			return "INDEX_OUT_OF_BOUNDS";
		case GrB_EMPTY_OBJECT:
			return "EMPTY_OBJECT";
	}
	return "Unknown Info enum value";
}

GrB_Type type_promote(GrB_Type left, GrB_Type right)
{
	int i, i1, i2;
	GrB_Type types[] =
		{GrB_INT16, GrB_UINT16,
		 GrB_INT32, GrB_UINT32,
		 GrB_INT64, GrB_UINT64,
		 GrB_FP32, GrB_FP64};

	i1 = i2 = -1;
	for (i = 0; i < 8; i++)
	{
        if (left == types[i]) i1 = i;
        if (right == types[i]) i2 = i;
    }
	if (i1 == -1 || i2 == -1)
		elog(ERROR, "Cannot promote types");

    return (i1 > i2) ? left : right;
}

GrB_BinaryOp default_binaryop(GrB_Type type)
{
	if (type == GrB_INT64)
		return GrB_TIMES_INT64;
	else if (type == GrB_INT32)
		return GrB_TIMES_INT32;
	else if (type == GrB_INT16)
		return GrB_TIMES_INT16;
	else if (type == GrB_INT8)
		return GrB_TIMES_INT8;
	if (type == GrB_UINT64)
		return GrB_TIMES_UINT64;
	else if (type == GrB_UINT32)
		return GrB_TIMES_UINT32;
	else if (type == GrB_UINT16)
		return GrB_TIMES_UINT16;
	else if (type == GrB_UINT8)
		return GrB_TIMES_UINT8;
	if (type == GrB_FP64)
		return GrB_TIMES_FP64;
	else if (type == GrB_FP32)
		return GrB_TIMES_FP32;
	else if (type == GrB_BOOL)
		return GxB_PAIR_BOOL;
	return NULL;
}

GrB_Monoid default_monoid(GrB_Type type)
{
	if (type == GrB_INT64)
		return GrB_PLUS_MONOID_INT64;
	else if (type == GrB_INT32)
		return GrB_PLUS_MONOID_INT32;
	else if (type == GrB_INT16)
		return GrB_PLUS_MONOID_INT16;
	else if (type == GrB_INT8)
		return GrB_PLUS_MONOID_INT8;
	if (type == GrB_UINT64)
		return GrB_PLUS_MONOID_UINT64;
	else if (type == GrB_UINT32)
		return GrB_PLUS_MONOID_UINT32;
	else if (type == GrB_UINT16)
		return GrB_PLUS_MONOID_UINT16;
	else if (type == GrB_UINT8)
		return GrB_PLUS_MONOID_UINT8;
	if (type == GrB_FP64)
		return GrB_PLUS_MONOID_FP64;
	else if (type == GrB_FP32)
		return GrB_PLUS_MONOID_FP32;
	else if (type == GrB_BOOL)
		return GxB_ANY_BOOL_MONOID;
	return NULL;
}

GrB_Semiring default_semiring(GrB_Type type)
{
	if (type == GrB_INT64)
		return GrB_PLUS_TIMES_SEMIRING_INT64;
	else if (type == GrB_INT32)
		return GrB_PLUS_TIMES_SEMIRING_INT32;
	else if (type == GrB_INT16)
		return GrB_PLUS_TIMES_SEMIRING_INT16;
	else if (type == GrB_INT8)
		return GrB_PLUS_TIMES_SEMIRING_INT8;
	if (type == GrB_UINT64)
		return GrB_PLUS_TIMES_SEMIRING_UINT64;
	else if (type == GrB_UINT32)
		return GrB_PLUS_TIMES_SEMIRING_UINT32;
	else if (type == GrB_UINT16)
		return GrB_PLUS_TIMES_SEMIRING_UINT16;
	else if (type == GrB_UINT8)
		return GrB_PLUS_TIMES_SEMIRING_UINT8;
	if (type == GrB_FP64)
		return GrB_PLUS_TIMES_SEMIRING_FP64;
	else if (type == GrB_FP32)
		return GrB_PLUS_TIMES_SEMIRING_FP32;
	else if (type == GrB_BOOL)
		return GxB_ANY_PAIR_BOOL;
	return NULL;
}

void *calloc_function(size_t num, size_t size) {
  MemoryContext oldcxt;
  void *p;
  size_t total;
  total = num * size;
  oldcxt = MemoryContextSwitchTo(TopMemoryContext);
  p = palloc_extended(total, MCXT_ALLOC_HUGE | MCXT_ALLOC_ZERO);
  MemoryContextSwitchTo(oldcxt);
  return p;
}

void *malloc_function(size_t size) {
  MemoryContext oldcxt;
  void *p;
  oldcxt = MemoryContextSwitchTo(TopMemoryContext);

  p = palloc_extended(size, MCXT_ALLOC_HUGE);
  MemoryContextSwitchTo(oldcxt);
  return p;
}

void *realloc_function(void *p, size_t size) {
  MemoryContext oldcxt;
  oldcxt = MemoryContextSwitchTo(TopMemoryContext);
  p = repalloc_huge(p, size);
  MemoryContextSwitchTo(oldcxt);
  return p;
}

void free_function(void *p) {
  //  MemoryContext oldcxt;
  //  oldcxt = MemoryContextSwitchTo(TopMemoryContext);
  pfree(p);
  //  MemoryContextSwitchTo(oldcxt);
}

void _PG_init(void)
{
	if (GrB_init(GrB_NONBLOCKING) != GrB_SUCCESS)
		elog(ERROR, "Cannot initialize GraphBLAS");

	initialize_types();
	initialize_descriptors();
	initialize_unaryops();
	initialize_indexunaryops();
	initialize_binaryops();
	initialize_monoids();
	initialize_semirings();
}


/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
