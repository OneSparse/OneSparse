
PG_FUNCTION_INFO_V1(FN(vector_agg));

Datum FN(vector_agg)(PG_FUNCTION_ARGS)
{
	GrB_Index i;
	PG_TYPE value;
    os_Vector *state;
	MemoryContext aggcontext;

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

#undef SUFFIX
#undef PG_TYPE
#undef GB_TYPE
#undef PG_GETARG
#undef PG_RETURN
#undef NO_SCALAR_MATH

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
