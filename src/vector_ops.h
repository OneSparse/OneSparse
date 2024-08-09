
Datum FN(vector_out)(OS_Vector *vec);
Datum FN(vector_ewise_mult)(OS_Vector *A,
							OS_Vector *B,
							OS_Vector *C,
							OS_Vector *mask,
							GrB_BinaryOp binop,
							GrB_BinaryOp accum,
							GrB_Descriptor desc);
Datum FN(vector_ewise_add)(OS_Vector *A,
						   OS_Vector *B,
						   OS_Vector *C,
						   OS_Vector *mask,
						   GrB_BinaryOp binop,
						   GrB_BinaryOp accum,
						   GrB_Descriptor desc);

Datum FN(vector_eq)(OS_Vector *A,
					OS_Vector *B);

Datum FN(vector_ne)(OS_Vector *A,
					OS_Vector *B);

GrB_Index *FN(extract_indexes)(OS_Vector *A, GrB_Index size);

PG_FUNCTION_INFO_V1(FN(vector));
PG_FUNCTION_INFO_V1(FN(vector_empty));
PG_FUNCTION_INFO_V1(FN(vector_elements));
PG_FUNCTION_INFO_V1(FN(vector_reduce));
PG_FUNCTION_INFO_V1(FN(vector_assign));
PG_FUNCTION_INFO_V1(FN(vector_set_element));

Datum
FN(vector_elements)(PG_FUNCTION_ARGS)
{
	GrB_Info info;
	FuncCallContext  *funcctx;
	TupleDesc tupdesc;
	Datum result;

	Datum values[3];
	bool nulls[2] = {false, false};
	HeapTuple tuple;
	GrB_Index size = 0;
	OS_Vector *vec;
	FN(OS_Vector_ExtractState) *state;

	if (SRF_IS_FIRSTCALL())
	{
		MemoryContext oldcontext;

		funcctx = SRF_FIRSTCALL_INIT();
		oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
		vec = (OS_Vector *) OS_GETARG_VECTOR(0);

		state = (FN(OS_Vector_ExtractState)*)palloc(sizeof(FN(OS_Vector_ExtractState)));
		CHECKD(GrB_Vector_size(&size, vec->V), vec->V);

		state->indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * size);
		state->vals = (PG_TYPE*) palloc0(sizeof(PG_TYPE) * size);

		CHECKD(GrB_Vector_extractTuples(state->indices,
										state->vals,
										&size,
										vec->V), vec->V);
		state->vec = vec;
		funcctx->max_calls = size;
		funcctx->user_fctx = (void*)state;

		/* One-time setup code appears here: */
		if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
			ereport(ERROR,
					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
					 errmsg("function returning record called in context "
							"that cannot accept type record")));
		BlessTupleDesc(tupdesc);
		funcctx->tuple_desc = tupdesc;

		MemoryContextSwitchTo(oldcontext);
	}

	funcctx = SRF_PERCALL_SETUP();
	state = (FN(OS_Vector_ExtractState)*)funcctx->user_fctx;
	vec = state->vec;

	if (funcctx->call_cntr < funcctx->max_calls)
	{
		values[0] = Int64GetDatum(state->indices[funcctx->call_cntr]);
		values[1] = PG_TGD(state->vals[funcctx->call_cntr]);

		tuple = heap_form_tuple(funcctx->tuple_desc, values, nulls);
		result = HeapTupleGetDatum(tuple);
		SRF_RETURN_NEXT(funcctx, result);
	}
	else
	{
		SRF_RETURN_DONE(funcctx);
	}
}

Datum
FN(vector_ewise_mult)(OS_Vector *A,
					  OS_Vector *B,
					  OS_Vector *C,
					  OS_Vector *mask,
					  GrB_BinaryOp binop,
					  GrB_BinaryOp accum,
					  GrB_Descriptor desc)
{
	GrB_Info info;
	GrB_Index size;

	if (C == NULL)
	{
		CHECKD(GrB_Vector_size(&size, A->V), A->V);
		C = FN(construct_empty_expanded_vector)(size, CurrentMemoryContext);
	}
	CHECKD(GrB_eWiseMult(C->V, mask ? mask->V : NULL, accum, binop, A->V, B->V, desc), C->V);
	OS_RETURN_VECTOR(C);
}

Datum
FN(vector_ewise_add)(OS_Vector *A,
					 OS_Vector *B,
					 OS_Vector *C,
					 OS_Vector *mask,
					 GrB_BinaryOp binop,
					 GrB_BinaryOp accum,
					 GrB_Descriptor desc)
{
	GrB_Info info;
	GrB_Index size;

	if (C == NULL)
	{
		CHECKD(GrB_Vector_size(&size, A->V), A->V);
		C = FN(construct_empty_expanded_vector)(size, CurrentMemoryContext);
	}
	CHECKD(GrB_eWiseAdd(C->V, mask ? mask->V : NULL, accum, GB_ADD, A->V, B->V, desc), C->V);
	OS_RETURN_VECTOR(C);
}

GrB_Index *FN(extract_indexes)(OS_Vector *A,
							   GrB_Index size)
{
	GrB_Index *row_indices;
	PG_TYPE *values;
	GrB_Info info;

	row_indices = (GrB_Index*) palloc0(sizeof(GrB_Index) * size);
	values = (PG_TYPE*) palloc0(sizeof(PG_TYPE) * size);

	CHECKD(GrB_Vector_extractTuples(row_indices,
									values,
									&size,
									A->V), A->V);
	return row_indices;
}

Datum
FN(vector_reduce)(PG_FUNCTION_ARGS)
{
	GrB_Info info;
	OS_Vector *A;
	GrB_Semiring semiring;
	GrB_Monoid monoid;
	char *semiring_name;
	PG_TYPE val;

	A = OS_GETARG_VECTOR(0);
	semiring_name = PG_ARGISNULL(1) ? NULL : text_to_cstring(PG_GETARG_TEXT_PP(1));

	semiring = semiring_name ? lookup_semiring(semiring_name) : GB_RNG;
	CHECKD(GxB_Semiring_add(&monoid, semiring), semiring);
	CHECKD(GrB_reduce(&val, NULL, monoid, A->V, NULL), A->V);
	PG_RET(val);
}

Datum
FN(vector_assign)(PG_FUNCTION_ARGS)
{
	GrB_Info info;
	OS_Vector *A, *B, *mask;
	GrB_Index size, *indexes = NULL;
	PG_TYPE val;

	A = OS_GETARG_VECTOR(0);
	val = PG_GET(1);
	B = PG_ARGISNULL(2) ? NULL : OS_GETARG_VECTOR(2);
	mask = PG_ARGISNULL(3)? NULL : OS_GETARG_VECTOR(3);

	CHECKD(GrB_Vector_size(&size, A->V), A->V);

	if (B != NULL)
		indexes = FN(extract_indexes)(B, size);

	CHECKD(GrB_assign(A->V, mask? mask->V: NULL, NULL, val, indexes ? indexes : GrB_ALL, size, NULL), A->V);
	OS_RETURN_VECTOR(A);
}

Datum
FN(vector_set_element)(PG_FUNCTION_ARGS)
{
	GrB_Info info;
	OS_Vector *A;
	GrB_Index index;
	PG_TYPE val;

	A = OS_GETARG_VECTOR(0);
	index = PG_GETARG_INT64(1);
	val = PG_GET(2);

	CHECKD(GrB_Vector_setElement(A->V, val, index), A->V);
	OS_RETURN_VECTOR(A);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
