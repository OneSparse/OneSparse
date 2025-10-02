
PG_FUNCTION_INFO_V1(FN(vector_agg));

Datum
FN(vector_agg) (PG_FUNCTION_ARGS)
{
	GrB_Index	i;
	PG_TYPE		value;
	os_Vector  *state;
	MemoryContext aggcontext;

	LOGF();
	if (PG_ARGISNULL(0))
	{
		if (!AggCheckCallContext(fcinfo, &aggcontext))
		{
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

Datum
FN(vector) (PG_FUNCTION_ARGS)
{
	os_Vector  *v;
	ArrayType  *arr;
	GrB_Index	size;
	Oid			eltype;
	int16		elmlen;
	bool		elmbyval;
	char		elmalign;
	int			ndims;
	int		   *dims;
	int			n;
	Datum	   *elems = NULL;
	bool	   *nulls = NULL;
	int			nelems = 0;
	int			i;
	PG_TYPE		val;

	LOGF();
	ERRORNULL(0);

	arr = PG_GETARG_ARRAYTYPE_P(0);

	ndims = ARR_NDIM(arr);
	dims = ARR_DIMS(arr);
	eltype = ARR_ELEMTYPE(arr);

	if (ndims != 1)
	{
		ereport(ERROR,
				(errcode(ERRCODE_DATATYPE_MISMATCH),
				 errmsg("expected a 1-D array")));
	}

	if (eltype != PG_TYPEOID)
	{
		ereport(ERROR,
				(errcode(ERRCODE_DATATYPE_MISMATCH),
				 errmsg("array type does not match cast function")));
	}

	n = dims[0];
	size = (GrB_Index) n;

	get_typlenbyvalalign(eltype, &elmlen, &elmbyval, &elmalign);
	deconstruct_array(arr, eltype, elmlen, elmbyval, elmalign,
					  &elems, &nulls, &nelems);

	if (nelems != n)
	{
		ereport(ERROR,
				(errcode(ERRCODE_DATA_EXCEPTION),
				 errmsg("array length mismatch")));
	}

	v = new_vector(GB_TYPE, size, CurrentMemoryContext, NULL);

	for (i = 0; i < nelems; i++)
	{
		if (nulls[i])
			continue;

		val = PG_DATUM_GET(elems[i]);
		OS_CHECK(GrB_Vector_setElement(v->vector, val, (GrB_Index) i),
				 v->vector,
				 "Cannot set vector element.");
	}

	OS_RETURN_VECTOR(v);
}

Datum
FN(cast_vector) (PG_FUNCTION_ARGS)
{
	os_Vector  *vector;
	ArrayType  *arr;
	Datum	   *elems;
	bool	   *nulls;
	int			dims[1];
	int			lbs[1];
	int16		elmlen;
	bool		elmbyval;
	char		elmalign;
	GxB_Iterator iterator;
	GrB_Info	info;
	GrB_Index	n,
				i;
	PG_TYPE		val;

	LOGF();
	ERRORNULL(0);

	vector = OS_GETARG_VECTOR(0);
	OS_VSIZE(n, vector);

	GxB_Iterator_new(&iterator);
	OS_CHECK(GxB_Vector_Iterator_attach(iterator, vector->vector, NULL),
			 vector->vector,
			 "Cannot attach vector iterator.");

	elems = palloc(n * sizeof(Datum));
	nulls = palloc(n * sizeof(bool));
	memset(nulls, true, n * sizeof(bool));

	for (info = GxB_Vector_Iterator_seek(iterator, 0);
		 info != GxB_EXHAUSTED;
		 info = GxB_Vector_Iterator_next(iterator))
	{
		i = GxB_Vector_Iterator_getIndex(iterator);
		OS_CHECK(GrB_Vector_extractElement(&val, vector->vector, i),
				 vector->vector,
				 "Error extracting vector element in array cast.");
		elems[i] = PG_GET_DATUM(val);
		nulls[i] = false;
	}

	dims[0] = n;
	lbs[0] = 1;

	get_typlenbyvalalign(PG_TYPEOID, &elmlen, &elmbyval, &elmalign);

	arr = construct_md_array(elems,
							 nulls,
							 1,
							 dims,
							 lbs,
							 PG_TYPEOID,
							 elmlen,
							 elmbyval,
							 elmalign);

	PG_RETURN_ARRAYTYPE_P(arr);
}

#undef SUFFIX
#undef PG_TYPE
#undef GB_TYPE
#undef PG_TYPEOID
#undef PG_GETARG
#undef PG_RETURN
#undef NO_SCALAR_MATH
#undef PG_GET_DATUM
#undef PG_DATUM_GET
