
PG_FUNCTION_INFO_V1(FN(matrix_agg));

Datum FN(matrix_agg)(PG_FUNCTION_ARGS)
{
	GrB_Index i, j;
	PG_TYPE value;
    os_Matrix *state;
	MemoryContext aggcontext;

    if (PG_ARGISNULL(0))
    {
        if (!AggCheckCallContext(fcinfo, &aggcontext)) {
            elog(ERROR, "aggregate function called in non-aggregate context");
        }
		state = new_matrix(GB_TYPE, GxB_INDEX_MAX, GxB_INDEX_MAX, aggcontext, NULL);
    }
    else
    {
        state = OS_GETARG_MATRIX(0);
    }
    if (PG_ARGISNULL(1) || PG_ARGISNULL(2) || PG_ARGISNULL(3))
    {
		PG_RETURN_POINTER(state);
	}
	i = PG_GETARG_INT64(1);
	j = PG_GETARG_INT64(2);
	value = PG_GETARG(3);
	OS_CHECK(GrB_Matrix_setElement(state->matrix, value, i, j),
		  state->matrix,
		  "Error setting matrix element in aggregate.");
    OS_RETURN_MATRIX(state);
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
