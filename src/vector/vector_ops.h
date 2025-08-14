
PG_FUNCTION_INFO_V1(FN(vector_agg));

Datum FN(vector_agg)(PG_FUNCTION_ARGS)
{
	GrB_Index i;
	PG_TYPE value;
    os_Vector *state;
	MemoryContext aggcontext;

	LOGF();
    if (PG_ARGISNULL(0))
    {
        if (!AggCheckCallContext(fcinfo, &aggcontext)) {
            elog(ERROR, "aggregate function called in non-aggregate context");
        }
		state = new_vector(GB_TYPE, GxB_INDEX_MAX, aggcontext, NULL);
    }
    else
    {
        state = OS_GETARG_VECTOR(0);
    }
    if (PG_ARGISNULL(1) || PG_ARGISNULL(2))
    {
		PG_RETURN_POINTER(state);
	}
	i = PG_GETARG_INT64(1);
	value = PG_GETARG(2);
	OS_CHECK(GrB_Vector_setElement(state->vector, value, i),
		  state->vector,
		  "Error setting vector element in aggregate.");
    OS_RETURN_VECTOR(state);
}

PG_FUNCTION_INFO_V1(FN(vector));
PG_FUNCTION_INFO_V1(FN(cast_vector));

Datum FN(vector)(PG_FUNCTION_ARGS)
{
	os_Vector* v;
	ArrayType *arr;
	GrB_Index size;

	LOGF();
	if (!PG_ARGISNULL(0))
	{
		arr = PG_GETARG_ARRAYTYPE_P(0);
	}
	v = new_vector(GB_TYPE, size, CurrentMemoryContext, NULL);
	OS_RETURN_VECTOR(v);
}

Datum FN(cast_vector)(PG_FUNCTION_ARGS)
{
	os_Vector* v;
	ArrayType *arr;
	int n = 0;
    int16 elmlen;
	bool elmbyval;
	char elmalign;
	Datum *elems;

	LOGF();
	ERRORNULL(0);

	v = OS_GETARG_VECTOR(0);

	elems = palloc(n * sizeof(Datum));
    get_typlenbyvalalign(INT8OID, &elmlen, &elmbyval, &elmalign);

    arr = construct_array(elems, n, INT8OID, elmlen, elmbyval, elmalign);
	PG_RETURN_ARRAYTYPE_P(arr);
}

#undef SUFFIX
#undef PG_TYPE
#undef GB_TYPE
#undef PG_TYPEOID
#undef PG_GETARG
#undef PG_RETURN
#undef NO_SCALAR_MATH

