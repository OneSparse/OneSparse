#include "onesparse.h"

static void context_callback_matrix_free(void*);
static Size matrix_get_flat_size(ExpandedObjectHeader *eohptr);

static void flatten_matrix(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size);

static const ExpandedObjectMethods matrix_methods = {
	matrix_get_flat_size,
	flatten_matrix
};

PG_FUNCTION_INFO_V1(matrix_in);
PG_FUNCTION_INFO_V1(matrix_out);
PG_FUNCTION_INFO_V1(matrix_new);
PG_FUNCTION_INFO_V1(matrix_agg_final);
PG_FUNCTION_INFO_V1(matrix_elements);
PG_FUNCTION_INFO_V1(matrix_nvals);
PG_FUNCTION_INFO_V1(matrix_nrows);
PG_FUNCTION_INFO_V1(matrix_ncols);
PG_FUNCTION_INFO_V1(matrix_wait);
PG_FUNCTION_INFO_V1(matrix_dup);
PG_FUNCTION_INFO_V1(matrix_clear);
PG_FUNCTION_INFO_V1(matrix_eadd);
PG_FUNCTION_INFO_V1(matrix_emult);
PG_FUNCTION_INFO_V1(matrix_eunion);
PG_FUNCTION_INFO_V1(matrix_reduce_vector);
PG_FUNCTION_INFO_V1(matrix_reduce_scalar);
PG_FUNCTION_INFO_V1(matrix_assign_matrix);
PG_FUNCTION_INFO_V1(matrix_extract_matrix);
PG_FUNCTION_INFO_V1(matrix_mxm);
PG_FUNCTION_INFO_V1(matrix_mxv);
PG_FUNCTION_INFO_V1(matrix_vxm);
PG_FUNCTION_INFO_V1(matrix_transpose);
PG_FUNCTION_INFO_V1(matrix_kron);
PG_FUNCTION_INFO_V1(matrix_select);
PG_FUNCTION_INFO_V1(matrix_apply);
PG_FUNCTION_INFO_V1(matrix_apply_first);
PG_FUNCTION_INFO_V1(matrix_apply_second);
PG_FUNCTION_INFO_V1(matrix_eq);
PG_FUNCTION_INFO_V1(matrix_get_element);
PG_FUNCTION_INFO_V1(matrix_set_element);
PG_FUNCTION_INFO_V1(matrix_remove_element);
PG_FUNCTION_INFO_V1(matrix_contains);
PG_FUNCTION_INFO_V1(matrix_resize);
PG_FUNCTION_INFO_V1(matrix_info);
PG_FUNCTION_INFO_V1(matrix_type);

static Size matrix_get_flat_size(ExpandedObjectHeader *eohptr) {
	os_Matrix *matrix;
	void *serialized_data;
	GrB_Index serialized_size;

	LOGF();

	matrix = (os_Matrix*) eohptr;
	Assert(matrix->em_magic == matrix_MAGIC);

	if (matrix->flat_size)
	{
		Assert(matrix->serialized_data != NULL);
		Assert(OS_MATRIX_FLATSIZE() + matrix->serialized_size == matrix->flat_size);
		return matrix->flat_size;
	}

    OS_CHECK(GrB_wait(matrix->matrix, GrB_MATERIALIZE),
		  matrix->matrix,
		  "Error waiting to materialize matrix.");

	OS_CHECK(GxB_Matrix_serialize(
			  &serialized_data,
			  &serialized_size,
			  matrix->matrix, NULL),
		  matrix->matrix,
		  "Error serializing matrix");

	matrix->serialized_data = serialized_data;
	matrix->serialized_size = serialized_size;
	matrix->flat_size = OS_MATRIX_FLATSIZE() + serialized_size;
	return matrix->flat_size;
}

/* Flatten matrix into a pre-allocated result buffer that is
   allocated_size in bytes.	 */
static void flatten_matrix(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size)
{
	os_Matrix *matrix;
	os_FlatMatrix *flat;
	void* data;

	LOGF();

	matrix = (os_Matrix *) eohptr;
	flat = (os_FlatMatrix *) result;

	Assert(matrix->em_magic == matrix_MAGIC);
	Assert(allocated_size == matrix->flat_size);
	Assert(matrix->serialized_data != NULL);
	Assert(matrix->serialized_size > 0);

	memset(flat, 0, allocated_size);

	OS_CHECK(GrB_get(
			  matrix->matrix,
			  &flat->type_code,
			  GrB_EL_TYPE_CODE),
		  matrix->matrix,
		  "Cannot get Matrix Type code.");

	OS_MNROWS(flat->nrows, matrix);
	OS_MNCOLS(flat->ncols, matrix);
	data = OS_MATRIX_DATA(flat);

	memcpy(data, matrix->serialized_data, matrix->serialized_size);
	flat->serialized_size = matrix->serialized_size;

	/* free_function = NULL; */
	/* ERRORIF(GrB_get(GrB_Global, (void*)free_function, GxB_FREE_FUNCTION) != GrB_SUCCESS, */
	/*		"Cannot get SuiteSparse free function."); */

	/* free_function(matrix->serialized_data); */
	SET_VARSIZE(flat, allocated_size);
}

/* Construct an empty expanded matrix. */
os_Matrix* new_matrix(
	GrB_Type type,
	GrB_Index nrows,
	GrB_Index ncols,
	MemoryContext parentcontext,
	GrB_Matrix _matrix)
{
	os_Matrix *matrix;

	MemoryContext objcxt, oldcxt;
	MemoryContextCallback *ctxcb;

	LOGF();
	objcxt = AllocSetContextCreate(parentcontext,
								   "expanded matrix",
								   ALLOCSET_DEFAULT_SIZES);

	matrix = MemoryContextAlloc(objcxt,	sizeof(os_Matrix));

	EOH_init_header(&matrix->hdr, &matrix_methods, objcxt);

	oldcxt = MemoryContextSwitchTo(objcxt);

	matrix->em_magic = matrix_MAGIC;
	matrix->flat_size = 0;
	matrix->serialized_data = NULL;
	matrix->serialized_size = 0;

	if (_matrix == NULL)
	{
		OS_CHECK(GrB_Matrix_new(&matrix->matrix, type, nrows, ncols),
			  matrix->matrix,
			  "Cannot create new Matrix.");
	}
	else
	{
		matrix->matrix = _matrix;
	}

	ctxcb = MemoryContextAlloc(objcxt, sizeof(MemoryContextCallback));

	ctxcb->func = context_callback_matrix_free;
	ctxcb->arg = matrix;

	MemoryContextRegisterResetCallback(objcxt, ctxcb);

	MemoryContextSwitchTo(oldcxt);
	return matrix;
}

/* Expand a flat matrix in to an Expanded one, return as Postgres Datum. */
Datum expand_matrix(os_FlatMatrix *flat, MemoryContext parentcontext)
{
	GrB_Type type;
	os_Matrix *matrix;
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

	matrix = new_matrix(type, flat->nrows, flat->ncols, parentcontext, NULL);
	data = OS_MATRIX_DATA(flat);

	OS_CHECK(GxB_Matrix_deserialize(
			  &matrix->matrix,
			  type,
			  data,
			  flat->serialized_size, NULL),
		  matrix->matrix,
		  "Error deserializing matrix");

	OS_RETURN_MATRIX(matrix);
}

static void
context_callback_matrix_free(void* ptr)
{
	os_Matrix *matrix = (os_Matrix *) ptr;
	LOGF();

	OS_CHECK(GrB_Matrix_free(&matrix->matrix),
		  matrix->matrix,
		  "Cannot GrB_Free Matrix");
}

/* Helper function to always expand datum

   This is used by PG_GETARG_MATRIX */
os_Matrix* DatumGetMatrix(Datum datum)
{
	os_Matrix *matrix;
	os_FlatMatrix *flat;

	LOGF();
	if (VARATT_IS_EXTERNAL_EXPANDED_RW(DatumGetPointer(datum))) {
		matrix = MatrixGetEOHP(datum);
		Assert(matrix->em_magic == matrix_MAGIC);
		return matrix;
	}
	flat = OS_DETOAST_MATRIX(datum);
	datum = expand_matrix(flat, CurrentMemoryContext);
	return MatrixGetEOHP(datum);
}

Datum matrix_in(PG_FUNCTION_ARGS)
{
	os_Matrix *matrix;
	char *input;
	GrB_Type typ;
	char *short_name;
	char *input_copy;
	char *token;
	char *saveptr;
	char *endptr;
	char *number_token;
	char *number_saveptr;
	int row, col;
	GrB_Index nrows, ncols;
	int matched;

	input = PG_GETARG_CSTRING(0);
	input_copy = strdup(input);
	token = strtok_r(input_copy, "[", &saveptr);

	if (token == NULL)
		elog(ERROR, "Matrix parse error, no short type code prefix.");

	short_name = palloc(strlen(token)+1);

	nrows = ncols = GrB_INDEX_MAX+1;

	if (strstr(input, "(:)"))
	{
		nrows = ncols = GrB_INDEX_MAX+1;
	}
	else if (strstr(input, "(:"))
	{
		matched = sscanf(token, "%99[^()](:%lu)", short_name, &ncols);
		if (matched != 2)
			elog(ERROR, "Invalid short name %s", token);
		nrows = GrB_INDEX_MAX+1;
	}
	else if (strstr(input, ")"))
	{
		matched = sscanf(token, "%99[^()](%lu:%lu)", short_name, &nrows, &ncols);
		if (matched == 2)
		{
			ncols = GrB_INDEX_MAX+1;
		}
		else if (matched != 3)
		{
			elog(ERROR, "Invalid prefix %s", token);
		}
	}
	else
	{
		matched = sscanf(token, "%s", short_name);
		if (matched != 1)
			elog(ERROR, "Invalid short name %s", token);
	}


	typ = short_type(short_name);
	matrix = new_matrix(typ, nrows, ncols, CurrentMemoryContext, NULL);

	strcpy(input_copy, input);
	strtok_r(input_copy, "[", &saveptr);
	token = strtok_r(NULL, "]", &saveptr);
	if (token == NULL)
	{
		OS_RETURN_MATRIX(matrix);
	}

	row = 0;
	col = 0;
	for (number_token = strtok_r(token, " ", &number_saveptr);
		 number_token != NULL;
		 number_token = strtok_r(NULL, " ", &number_saveptr))
	{
		errno = 0;

		if (sscanf(number_token, "%d:%d:%s", &row, &col, number_token) != 3)
			elog(ERROR, "Invalid matrix entry %s", number_token);

		if (typ == GrB_INT64) {
			int64_t num = strtoll(number_token, &endptr, 10);
			if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid INT64 %s", number_token);
			OS_CHECK(GrB_Matrix_setElement(matrix->matrix, num, row, col),
				  matrix->matrix,
				  "Error setting Matrix Element");
		}
		else if (typ == GrB_INT32) {
			long num = strtol(number_token, &endptr, 10);
			if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid INT32 %s", number_token);
			OS_CHECK(GrB_Matrix_setElement(matrix->matrix, num, row, col),
				  matrix->matrix,
				  "Error setting Matrix Element");
		}
		else if (typ == GrB_INT16) {
			int num = (int)strtol(number_token, &endptr, 10);
			if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid INT16 %s", number_token);
			OS_CHECK(GrB_Matrix_setElement(matrix->matrix, num, row, col),
				  matrix->matrix,
				  "Error setting Matrix Element");
		}
		else if (typ == GrB_FP64)
		{
			double num = strtod(number_token, &endptr);
			if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid FP64 %s", number_token);
			OS_CHECK(GrB_Matrix_setElement(matrix->matrix, num, row, col),
				  matrix->matrix,
				  "Error setting Matrix Element");
		}
		else if (typ == GrB_FP32)
		{
			float num = strtof(number_token, &endptr);
			if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid FP32 %s", number_token);
			OS_CHECK(GrB_Matrix_setElement(matrix->matrix, num, row, col),
				  matrix->matrix,
				  "Error setting Matrix Element");
		}
		else if (typ == GrB_BOOL)
		{
			bool num;
			if (strcmp(number_token, "t") == 0)
				num = true;
			else if	 (strcmp(number_token, "f") == 0)
				num = false;
			else
				elog(ERROR, "Invalid BOOL %s", number_token);
			OS_CHECK(GrB_Matrix_setElement(matrix->matrix, num, row, col),
				  matrix->matrix,
				  "Error setting Matrix Element");
		}
	}
	OS_RETURN_MATRIX(matrix);
}

Datum matrix_out(PG_FUNCTION_ARGS)
{
	GrB_Info info;
	GrB_Index row, col, nrows, ncols, nvals;
	GxB_Iterator iterator;
	os_Matrix *matrix;
	StringInfoData buf;
	int32_t type_code;

	LOGF();
	matrix = OS_GETARG_MATRIX(0);

	GxB_Iterator_new(&iterator);
	OS_CHECK(GxB_Matrix_Iterator_attach(iterator, matrix->matrix, NULL),
		  matrix->matrix,
		  "Cannot attach matrix iterator.");

	initStringInfo(&buf);
	OS_CHECK(GrB_get(matrix->matrix, &type_code, GrB_EL_TYPE_CODE),
		  matrix->matrix,
		  "Cannot get Matrix Type code.");

	appendStringInfo(&buf, "%s", short_code(type_code));

	OS_MNROWS(nrows, matrix);
	OS_MNCOLS(ncols, matrix);
	if (nrows < GrB_INDEX_MAX+1 || ncols < GrB_INDEX_MAX+1)
	{
		appendStringInfo(&buf, "(");
		if (nrows < GrB_INDEX_MAX+1)
		{
			appendStringInfo(&buf, "%lu", nrows);
		}
		appendStringInfo(&buf, ":");
		if (ncols < GrB_INDEX_MAX+1)
		{
			appendStringInfo(&buf, "%lu", ncols);
		}
		appendStringInfo(&buf, ")");
	}
	OS_MNVALS(nvals, matrix);

	if (nvals == 0)
	{
		PG_RETURN_CSTRING(buf.data);
	}

	appendStringInfo(&buf, "[");

	info = GxB_Matrix_Iterator_seek(iterator, 0);
	while (info != GxB_EXHAUSTED)
	{
		GxB_Matrix_Iterator_getIndex(iterator, &row, &col);
		switch(type_code)
		{
			case GrB_INT64_CODE:
				{
				int64_t vi64 = GxB_Iterator_get_INT64(iterator);
				appendStringInfo(&buf, "%lu:%lu:%ld", row, col, vi64);
				break;
				}
			case GrB_INT32_CODE:
				{
				int32_t vi32 = GxB_Iterator_get_INT32(iterator);
				appendStringInfo(&buf, "%lu:%lu:%d", row, col, vi32);
				break;
				}
			case GrB_INT16_CODE:
				{
				int16_t vi16 = GxB_Iterator_get_INT16(iterator);
				appendStringInfo(&buf, "%lu:%lu:%d", row, col, vi16);
				break;
				}
			case GrB_FP64_CODE:
				{
				double vf64 = GxB_Iterator_get_FP64(iterator);
				appendStringInfo(&buf, "%lu:%lu:%f", row, col, vf64);
				break;
				}
			case GrB_FP32_CODE:
				{
				float vf32 = GxB_Iterator_get_FP32(iterator);
				appendStringInfo(&buf, "%lu:%lu:%f", row, col, vf32);
				break;
				}
			case GrB_BOOL_CODE:
				{
				bool b = GxB_Iterator_get_BOOL(iterator);
				appendStringInfo(&buf, "%lu:%lu:%s", row, col, b ? "t" : "f");
				break;
				}
		}
		info = GxB_Matrix_Iterator_next(iterator);
		if (info != GxB_EXHAUSTED)
			appendStringInfo(&buf, " ");
	}
	GrB_free(&iterator);

	appendStringInfo(&buf, "]");
	PG_RETURN_CSTRING(buf.data);
}

Datum matrix_new(PG_FUNCTION_ARGS)
{
	os_Type *type;
	GrB_Index nrows, ncols;
	os_Matrix *A;

	type = OS_GETARG_TYPE(0);
	nrows = PG_GETARG_INT64(1);
	ncols = PG_GETARG_INT64(2);
	if (nrows == -1)
	{
		nrows = GrB_INDEX_MAX+1;
	}
	if (ncols == -1)
		ncols = GrB_INDEX_MAX+1;
	A = new_matrix(type->type, nrows, ncols, CurrentMemoryContext, NULL);
	OS_RETURN_MATRIX(A);
}

Datum matrix_elements(PG_FUNCTION_ARGS)
{
	FuncCallContext  *funcctx;
	TupleDesc tupdesc;
	Datum result;

	Datum values[3];
	bool nulls[3] = {false, false, false};
	HeapTuple tuple;
	GrB_Index nvals, row, col;
	os_Matrix *matrix;
	os_Scalar *scalar;
	os_Matrix_ExtractState *state;

	if (SRF_IS_FIRSTCALL()) {
		MemoryContext oldcontext;

		funcctx = SRF_FIRSTCALL_INIT();
		oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
		matrix = OS_GETARG_MATRIX(0);

		state = (os_Matrix_ExtractState*)palloc(sizeof(os_Matrix_ExtractState));
		OS_MNVALS(nvals, matrix);

		state->matrix = matrix;
		GxB_Iterator_new(&(state->iterator));
		OS_CHECK(GxB_Matrix_Iterator_attach(state->iterator, matrix->matrix, NULL),
				 matrix->matrix,
				 "Cannot attach matrix iterator.");
		OS_MTYPE(state->type, matrix);
		state->info = GxB_Matrix_Iterator_seek(state->iterator, 0);
		funcctx->max_calls = nvals;
		funcctx->user_fctx = (void*)state;

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
	state = (os_Matrix_ExtractState*)funcctx->user_fctx;
	if (state->info == GxB_EXHAUSTED || funcctx->call_cntr > funcctx->max_calls)
	{
		SRF_RETURN_DONE(funcctx);
	}
	else
	{
		GxB_Matrix_Iterator_getIndex(state->iterator, &row, &col);
		values[0] = Int64GetDatum(row);
		values[1] = Int64GetDatum(col);
		scalar = new_scalar(state->type, CurrentMemoryContext, NULL);
		matrix = state->matrix;
		OS_CHECK(GrB_Matrix_extractElement(scalar->scalar, matrix->matrix, row, col),
				 matrix->matrix,
				 "Error extracting setting matrix element.");
		values[2] = EOHPGetRWDatum(&(scalar)->hdr);
		tuple = heap_form_tuple(funcctx->tuple_desc, values, nulls);
		result = HeapTupleGetDatum(tuple);
		state->info = GxB_Matrix_Iterator_next(state->iterator);
		SRF_RETURN_NEXT(funcctx, result);
	}
}

Datum matrix_nvals(PG_FUNCTION_ARGS)
{
	GrB_Index result;
	os_Matrix *matrix;

	LOGF();
	ERRORNULL(0);
	matrix = OS_GETARG_MATRIX(0);
	OS_MNVALS(result, matrix);
	PG_RETURN_INT64(result);
}

Datum matrix_nrows(PG_FUNCTION_ARGS)
{
	GrB_Index result;
	os_Matrix *matrix;

	LOGF();
	ERRORNULL(0);
	matrix = OS_GETARG_MATRIX(0);
	OS_MNROWS(result, matrix);
	PG_RETURN_INT64(result);
}

Datum matrix_ncols(PG_FUNCTION_ARGS)
{
	GrB_Index result;
	os_Matrix *matrix;

	LOGF();
	ERRORNULL(0);
	matrix = OS_GETARG_MATRIX(0);
	OS_MNCOLS(result, matrix);
	PG_RETURN_INT64(result);
}

Datum matrix_eadd(PG_FUNCTION_ARGS)
{
	GrB_Type utype, vtype, wtype;
	os_Matrix *u, *v, *w;
	GrB_Matrix mask;
	GrB_Descriptor descriptor;
	GrB_BinaryOp op, accum;
	GrB_Index nrows, ncols;
	int nargs;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	nargs = PG_NARGS();
	u = OS_GETARG_MATRIX(0);
	v = OS_GETARG_MATRIX(1);

	OS_MTYPE(utype, u);
	OS_MTYPE(vtype, v);
	wtype = type_promote(utype, vtype);

	op = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 2);
	if (op == NULL)
	{
		op = default_binaryop(wtype);
	}

	if (PG_ARGISNULL(3))
	{
		OS_MNROWS(nrows, u);
		OS_MNCOLS(ncols, u);
		w = new_matrix(wtype, nrows, ncols, CurrentMemoryContext, NULL);
	}
	else
		w = OS_GETARG_MATRIX(3);

	mask = OS_GETARG_MATRIX_HANDLE_OR_NULL(nargs, 4);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_eWiseAdd(w->matrix,
						  mask,
						  accum,
						  op,
						  u->matrix,
						  v->matrix,
						  descriptor),
		  w->matrix,
		  "Error matrix eWiseAdd.");

	OS_RETURN_MATRIX(w);
}

Datum matrix_emult(PG_FUNCTION_ARGS)
{
	GrB_Type utype, vtype, wtype;
	os_Matrix *u, *v, *w;
	GrB_Matrix mask;
	GrB_Descriptor descriptor;
	GrB_BinaryOp op, accum;
	GrB_Index nrows, ncols;
	int nargs;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	nargs = PG_NARGS();
	u = OS_GETARG_MATRIX(0);
	v = OS_GETARG_MATRIX(1);
	OS_MTYPE(utype, u);
	OS_MTYPE(vtype, v);
	wtype = type_promote(utype, vtype);

	op = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 2);
	if (op == NULL)
	{
		op = default_binaryop(wtype);
	}

	if (PG_ARGISNULL(3))
	{
		OS_MNROWS(nrows, u);
		OS_MNCOLS(ncols, u);
		w = new_matrix(wtype, nrows, ncols, CurrentMemoryContext, NULL);
	}
	else
		w = OS_GETARG_MATRIX(3);

	mask = OS_GETARG_MATRIX_HANDLE_OR_NULL(nargs, 4);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_eWiseMult(w->matrix,
						   mask,
						   accum,
						   op,
						   u->matrix,
						   v->matrix,
						   descriptor),
			 w->matrix,
			 "Error matrix eWiseMult.");

	OS_RETURN_MATRIX(w);
}

Datum matrix_eunion(PG_FUNCTION_ARGS)
{
	GrB_Type utype, vtype, wtype;
	os_Matrix *u, *v, *w;
	GrB_Matrix mask;
	os_Scalar *a, *b;
	GrB_Descriptor descriptor;
	GrB_BinaryOp op;
	GrB_BinaryOp accum;
	GrB_Index nrows, ncols;
	int nargs;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);
	ERRORNULL(3);

	nargs = PG_NARGS();
	u = OS_GETARG_MATRIX(0);
	a = OS_GETARG_SCALAR(1);
	v = OS_GETARG_MATRIX(2);
	b = OS_GETARG_SCALAR(3);

	OS_MTYPE(utype, u);
	OS_MTYPE(vtype, v);
	wtype = type_promote(utype, vtype);

	op = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 4);
	if (op == NULL)
	{
		op = default_binaryop(wtype);
	}
	if (PG_ARGISNULL(5))
	{
		OS_MNROWS(nrows, u);
		OS_MNCOLS(ncols, u);
		w = new_matrix(wtype, nrows, ncols, CurrentMemoryContext, NULL);
	}
	else
		w = OS_GETARG_MATRIX(5);

	mask = OS_GETARG_MATRIX_HANDLE_OR_NULL(nargs, 6);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 7);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 8);

	OS_CHECK(GxB_eWiseUnion(w->matrix,
							mask,
							accum,
							op,
							u->matrix,
							a->scalar,
							v->matrix,
							b->scalar,
							descriptor),
			 w->matrix,
			 "Error matrix eWiseUnion.");
	OS_RETURN_MATRIX(w);
}

Datum matrix_mxm(PG_FUNCTION_ARGS)
{
	GrB_Type atype, btype, ctype;
	os_Matrix *a, *b, *c;
	GrB_Matrix mask;
	GrB_Descriptor descriptor;
	GrB_BinaryOp accum;
	GrB_Semiring semiring;
	GrB_Index nrows, ncols;
	int nargs;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	nargs = PG_NARGS();
	a = OS_GETARG_MATRIX(0);
	b = OS_GETARG_MATRIX(1);

	OS_MTYPE(atype, a);
	OS_MTYPE(btype, b);
	ctype = type_promote(atype, btype);

	semiring = OS_GETARG_SEMIRING_HANDLE_OR_NULL(nargs, 2);
	if (semiring == NULL)
	{
		semiring = default_semiring(ctype);
	}

	if (PG_ARGISNULL(3))
	{
		OS_MNROWS(nrows, a);
		OS_MNCOLS(ncols, b);
		c = new_matrix(ctype, nrows, ncols, CurrentMemoryContext, NULL);
	}
	else
		c = OS_GETARG_MATRIX(3);

	mask = OS_GETARG_MATRIX_HANDLE_OR_NULL(nargs, 4);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_mxm(c->matrix,
					 mask,
					 accum,
					 semiring,
					 a->matrix,
					 b->matrix,
					 descriptor),
		  c->matrix,
		  "Error matrix mxm.");

	OS_RETURN_MATRIX(c);
}

Datum matrix_mxv(PG_FUNCTION_ARGS)
{
	GrB_Type atype, btype, ctype;
	os_Matrix *a;
	os_Vector *b, *c;
	GrB_Vector mask;
	GrB_Descriptor descriptor;
	GrB_BinaryOp accum;
	GrB_Semiring semiring;
	GrB_Index bsize;
	int nargs;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	nargs = PG_NARGS();
	a = OS_GETARG_MATRIX(0);
	b = OS_GETARG_VECTOR(1);

	OS_MTYPE(atype, a);
	OS_VTYPE(btype, b);
	ctype = type_promote(atype, btype);

	semiring = OS_GETARG_SEMIRING_HANDLE_OR_NULL(nargs, 2);
	if (semiring == NULL)
	{
		semiring = default_semiring(ctype);
	}

	if (PG_ARGISNULL(3))
	{
		OS_VSIZE(bsize, b);
		c = new_vector(ctype, bsize, CurrentMemoryContext, NULL);
	}
	else
		c = OS_GETARG_VECTOR(3);

	mask = OS_GETARG_VECTOR_HANDLE_OR_NULL(nargs, 4);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_mxv(c->vector,
					 mask,
					 accum,
					 semiring,
					 a->matrix,
					 b->vector,
					 descriptor),
		  c->vector,
		  "Error matrix mxv.");

	OS_RETURN_VECTOR(c);
}

Datum matrix_vxm(PG_FUNCTION_ARGS)
{
	GrB_Type atype, btype, ctype;
	os_Vector *a, *c;
	os_Matrix *b;
	GrB_Vector mask;
	GrB_Descriptor descriptor;
	GrB_BinaryOp accum;
	GrB_Semiring semiring;
	GrB_Index asize;
	int nargs;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	nargs = PG_NARGS();
	a = OS_GETARG_VECTOR(0);
	b = OS_GETARG_MATRIX(1);

	OS_VTYPE(atype, a);
	OS_MTYPE(btype, b);
	ctype = type_promote(atype, btype);

	semiring = OS_GETARG_SEMIRING_HANDLE_OR_NULL(nargs, 2);
	if (semiring == NULL)
	{
		semiring = default_semiring(ctype);
	}

	if (PG_ARGISNULL(3))
	{
		OS_VSIZE(asize, a);
		c = new_vector(ctype, asize, CurrentMemoryContext, NULL);
	}
	else
		c = OS_GETARG_VECTOR(3);

	mask = OS_GETARG_VECTOR_HANDLE_OR_NULL(nargs, 4);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_vxm(c->vector,
					 mask,
					 accum,
					 semiring,
					 a->vector,
					 b->matrix,
					 descriptor),
		  c->vector,
		  "Error matrix mxv.");

	OS_RETURN_VECTOR(c);
}

Datum matrix_kron(PG_FUNCTION_ARGS)
{
	GrB_Type atype, btype, ctype;
	os_Matrix *a, *b, *c;
	GrB_Matrix mask;
	GrB_Descriptor descriptor;
	GrB_BinaryOp accum;
	GrB_Semiring semiring;
	GrB_Index anrows, ancols, bnrows, bncols;
	int nargs;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	nargs = PG_NARGS();
	a = OS_GETARG_MATRIX(0);
	b = OS_GETARG_MATRIX(1);

	OS_MTYPE(atype, a);
	OS_MTYPE(btype, b);
	ctype = type_promote(atype, btype);

	semiring = OS_GETARG_SEMIRING_HANDLE_OR_NULL(nargs, 2);
	if (semiring == NULL)
	{
		semiring = default_semiring(ctype);
	}

	if (PG_ARGISNULL(3))
	{
		OS_MNROWS(anrows, a);
		OS_MNROWS(bnrows, b);
		OS_MNCOLS(ancols, a);
		OS_MNCOLS(bncols, b);
		ctype = type_promote(atype, btype);
		c = new_matrix(ctype, anrows * bncols, ancols * bnrows, CurrentMemoryContext, NULL);
	}
	else
		c = OS_GETARG_MATRIX(3);

	mask = OS_GETARG_MATRIX_HANDLE_OR_NULL(nargs, 4);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_kronecker(c->matrix,
						   mask,
						   accum,
						   semiring,
						   a->matrix,
						   b->matrix,
						   descriptor),
			 c->matrix,
			 "Error matrix kron.");
	OS_RETURN_MATRIX(c);
}

Datum matrix_reduce_vector(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *a;
	os_Vector *w;
	GrB_Vector mask;
	GrB_Descriptor descriptor;
	GrB_Monoid monoid;
	GrB_BinaryOp accum;
	GrB_Index vsize;
	int nargs;

	LOGF();
	ERRORNULL(0);

	nargs = PG_NARGS();
	a = OS_GETARG_MATRIX(0);

	OS_MTYPE(type, a);

	monoid = OS_GETARG_MONOID_HANDLE_OR_NULL(nargs, 1);
	if (monoid == NULL)
	{
		monoid = default_monoid(type);
	}

	if (PG_ARGISNULL(2))
	{
		OS_MNROWS(vsize, a);
		w = new_vector(type, vsize, CurrentMemoryContext, NULL);
	}
	else
		w = OS_GETARG_VECTOR(2);

	mask = OS_GETARG_VECTOR_HANDLE_OR_NULL(nargs, 3);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 4);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 5);

	OS_CHECK(GrB_Matrix_reduce_Monoid(
				 w->vector,
				 mask,
				 accum,
				 monoid,
				 a->matrix,
				 descriptor),
			 w->vector,
			 "Error matrix vector reduce.");

	OS_RETURN_VECTOR(w);
}

Datum
matrix_reduce_scalar(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *A;
	GrB_Monoid monoid;
	GrB_BinaryOp accum;
	GrB_Descriptor descriptor;
	os_Scalar *result;
	int nargs;

	nargs = PG_NARGS();
	A = OS_GETARG_MATRIX(0);
	OS_MTYPE(type, A);

	monoid = OS_GETARG_MONOID_HANDLE_OR_NULL(nargs, 1);
	if (monoid == NULL)
	{
		monoid = default_monoid(type);
	}

	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 2);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 3);

	result = new_scalar(type, CurrentMemoryContext, NULL);

	OS_CHECK(GrB_Matrix_reduce_Monoid_Scalar(
				 result->scalar,
				 accum,
				 monoid,
				 A->matrix,
				 descriptor),
			 result->scalar,
			 "Cannot reduce matrix to scalar");

	OS_RETURN_SCALAR(result);
}

Datum
matrix_assign_matrix(PG_FUNCTION_ARGS)
{
	os_Matrix *A, *B, *mask;
	os_BinaryOp *accum;
	os_Descriptor *descriptor;
	GrB_Index nvals, *rows = NULL, *cols = NULL;
	int nargs;

	nargs = PG_NARGS();
	A = OS_GETARG_MATRIX(0);
	B = OS_GETARG_MATRIX(1);

	accum = OS_GETARG_BINARYOP_OR_NULL(nargs, 2);
	mask = OS_GETARG_MATRIX_OR_NULL(nargs, 3);
	descriptor = OS_GETARG_DESCRIPTOR_OR_NULL(nargs, 4);

	OS_MNVALS(nvals, B);
	rows = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);
	cols = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);

	OS_CHECK(GrB_Matrix_extractTuples(rows,
								   cols,
								   NULL,
								   &nvals,
								   B->matrix),
		  B->matrix,
		  "Error extracting tuples.");

	OS_CHECK(GrB_assign(A->matrix,
					 mask ? mask->matrix: NULL,
					 accum ? accum->binaryop : NULL,
					 B->matrix,
					 rows,
					 nvals,
					 cols,
					 nvals,
					 descriptor ? descriptor->descriptor : NULL),
		  A->matrix,
		  "Error in assign matrix.");

	OS_RETURN_MATRIX(A);
}

Datum
matrix_extract_matrix(PG_FUNCTION_ARGS)
{
	os_Matrix *A, *B, *mask;
	os_BinaryOp *accum;
	os_Descriptor *descriptor;
	GrB_Index nvals, *rows = NULL, *cols = NULL;
	int nargs;

	nargs = PG_NARGS();
	A = OS_GETARG_MATRIX(0);
	B = OS_GETARG_MATRIX(1);

	accum = OS_GETARG_BINARYOP_OR_NULL(nargs, 2);
	mask = OS_GETARG_MATRIX_OR_NULL(nargs, 3);
	descriptor = OS_GETARG_DESCRIPTOR_OR_NULL(nargs, 4);

	if (B != NULL)
	{
		OS_MNVALS(nvals, B);
		rows = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);
		cols = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);

		OS_CHECK(GrB_Matrix_extractTuples(rows,
									   cols,
									   NULL,
									   &nvals,
									   A->matrix),
			  A->matrix,
			  "Error extracting tuples.");
	}

	OS_CHECK(GrB_extract(A->matrix,
					  mask ? mask->matrix: NULL,
					  accum ? accum->binaryop : NULL,
					  B->matrix,
					  rows ? rows : GrB_ALL,
					  nvals,
					  cols ? cols : GrB_ALL,
					  nvals,
					  descriptor ? descriptor->descriptor : NULL),
		  A->matrix,
		  "Error in assign matrix.");

	OS_RETURN_MATRIX(A);
}

Datum matrix_select(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *A, *C, *mask;
	os_Scalar *y;
	os_Descriptor *descriptor;
	os_BinaryOp *accum;
	os_IndexUnaryOp *op;
	GrB_Index nrows, ncols;
	int nargs;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	nargs = PG_NARGS();
	A = OS_GETARG_MATRIX(0);
	op = OS_GETARG_INDEXUNARYOP(1);
	y = OS_GETARG_SCALAR(2);

	if (PG_ARGISNULL(3))
	{
		OS_MTYPE(type, A);
		OS_MNROWS(nrows, A);
		OS_MNCOLS(ncols, A);
		C = new_matrix(type, nrows, ncols, CurrentMemoryContext, NULL);
	}
	else
		C = OS_GETARG_MATRIX(3);

	mask = OS_GETARG_MATRIX_OR_NULL(nargs, 4);
	accum = OS_GETARG_BINARYOP_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_OR_NULL(nargs, 6);

	OS_CHECK(GrB_select(C->matrix,
					 mask ? mask->matrix : NULL,
					 accum ? accum->binaryop : NULL,
					 op->indexunaryop,
					 A->matrix,
					 y ? y->scalar : NULL,
					 descriptor ? descriptor->descriptor : NULL),
		  C->matrix,
		  "Error in GrB_select");

	OS_RETURN_MATRIX(C);
}

Datum matrix_transpose(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *a, *c;
	GrB_Matrix mask;
	GrB_Descriptor descriptor;
	GrB_BinaryOp accum;
	GrB_Index nrows, ncols;
	int nargs;

	LOGF();
	ERRORNULL(0);

	nargs = PG_NARGS();
	a = OS_GETARG_MATRIX(0);

	if (PG_ARGISNULL(1))
	{
		OS_MTYPE(type, a);
		OS_MNROWS(nrows, a);
		OS_MNCOLS(ncols, a);
		c = new_matrix(type, nrows, ncols, CurrentMemoryContext, NULL);
	}
	else
		c = OS_GETARG_MATRIX(2);

	mask = OS_GETARG_MATRIX_HANDLE_OR_NULL(nargs, 2);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 3);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 4);

	OS_CHECK(GrB_transpose(c->matrix,
						   mask,
						   accum,
						   a->matrix,
						   descriptor),
		  c->matrix,
		  "Error in transpose");
	OS_RETURN_MATRIX(c);
}

Datum matrix_apply(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *A, *C, *mask;
	os_Descriptor *descriptor;
	os_BinaryOp *accum;
	os_UnaryOp *op;
	GrB_Index nrows, ncols;
	int nargs;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	nargs = PG_NARGS();
	A = OS_GETARG_MATRIX(0);
	op = OS_GETARG_UNARYOP(1);

	if (PG_ARGISNULL(2))
	{
		OS_MTYPE(type, A);
		OS_MNROWS(nrows, A);
		OS_MNCOLS(ncols, A);
		C = new_matrix(type, nrows, ncols, CurrentMemoryContext, NULL);
	}
	else
		C = OS_GETARG_MATRIX(2);

	mask = OS_GETARG_MATRIX_OR_NULL(nargs, 3);
	accum = OS_GETARG_BINARYOP_OR_NULL(nargs, 4);
	descriptor = OS_GETARG_DESCRIPTOR_OR_NULL(nargs, 5);

	OS_CHECK(GrB_apply(C->matrix,
					mask ? mask->matrix : NULL,
					accum ? accum->binaryop : NULL,
					op->unaryop,
					A->matrix,
					descriptor ? descriptor->descriptor : NULL),
		  C->matrix,
		  "Error in GrB_Matrix_apply");

	OS_RETURN_MATRIX(C);
}

Datum matrix_apply_first(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *a, *c;
	os_Scalar *s;
	GrB_Matrix mask;
	GrB_Descriptor descriptor;
	GrB_BinaryOp accum;
	GrB_BinaryOp binaryop;
	GrB_Index nrows, ncols;
	int nargs;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	nargs = PG_NARGS();
	s = OS_GETARG_SCALAR(0);
	a = OS_GETARG_MATRIX(1);

	OS_MTYPE(type, a);
	binaryop = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 2);
	if (binaryop == NULL)
	{
		binaryop = default_binaryop(type);
	}

	if (PG_ARGISNULL(3))
	{
		OS_MNROWS(nrows, a);
		OS_MNCOLS(ncols, a);
		c = new_matrix(type, nrows, ncols, CurrentMemoryContext, NULL);
	}
	else
		c = OS_GETARG_MATRIX(3);

	mask = OS_GETARG_MATRIX_HANDLE_OR_NULL(nargs, 4);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_apply(c->matrix,
					   mask,
					   accum,
					   binaryop,
					   s->scalar,
					   a->matrix,
					   descriptor),
			 c->matrix,
			 "Error in grb_matrix_apply_binaryop1st");
	OS_RETURN_MATRIX(c);
}

Datum matrix_apply_second(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *a, *c;
	os_Scalar *s;
	GrB_Matrix mask;
	GrB_Descriptor descriptor;
	GrB_BinaryOp accum;
	GrB_BinaryOp binaryop;
	GrB_Index nrows, ncols;
	int nargs;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	nargs = PG_NARGS();
	a = OS_GETARG_MATRIX(0);
	s = OS_GETARG_SCALAR(1);

	OS_MTYPE(type, a);
	binaryop = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 2);
	if (binaryop == NULL)
	{
		binaryop = default_binaryop(type);
	}

	if (PG_ARGISNULL(3))
	{
		OS_MNROWS(nrows, a);
		OS_MNCOLS(ncols, a);
		c = new_matrix(type, nrows, ncols, CurrentMemoryContext, NULL);
	}
	else
		c = OS_GETARG_MATRIX(3);

	mask = OS_GETARG_MATRIX_HANDLE_OR_NULL(nargs, 4);
	accum = OS_GETARG_BINARYOP_HANDLE_OR_NULL(nargs, 5);
	descriptor = OS_GETARG_DESCRIPTOR_HANDLE_OR_NULL(nargs, 6);

	OS_CHECK(GrB_apply(c->matrix,
					   mask,
					   accum,
					   binaryop,
					   a->matrix,
					   s->scalar,
					   descriptor),
			 c->matrix,
			 "Error in grb_matrix_apply_binaryop1st");
	OS_RETURN_MATRIX(c);
}

Datum matrix_eq(PG_FUNCTION_ARGS)
{
	GrB_Type atype, btype;
	os_Matrix *a, *b, *c;
	GrB_Index anrows, ancols, anvals, bnrows, bncols, bnvals;
	bool result;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	a = OS_GETARG_MATRIX(0);
	b = OS_GETARG_MATRIX(1);

	OS_MTYPE(atype, a);
	OS_MTYPE(btype, b);
	if (atype != btype)
		PG_RETURN_BOOL(false);

	OS_MNROWS(anrows, a);
	OS_MNROWS(bnrows, b);
	if (anrows != bnrows)
		PG_RETURN_BOOL(false);

	OS_MNCOLS(ancols, a);
	OS_MNCOLS(bncols, b);
	if (ancols != bncols)
		PG_RETURN_BOOL(false);

	OS_MNVALS(anvals, a);
	OS_MNVALS(bnvals, b);
	if (anvals != bnvals)
		PG_RETURN_BOOL(false);

	c = new_matrix(GrB_BOOL, anrows, ancols, CurrentMemoryContext, NULL);
	OS_CHECK(GrB_eWiseMult(c->matrix,
						  NULL,
						  NULL,
						  GrB_EQ_BOOL,
						  a->matrix,
						  b->matrix,
						  NULL),
			 c->matrix,
			 "Error matrix eWiseAdd in eq.");

	OS_MNVALS(anvals, a);
	OS_MNVALS(bnvals, c);
	if (anvals != bnvals)
		PG_RETURN_BOOL(false);

	OS_CHECK(GrB_Matrix_reduce_BOOL(
			  &result,
			  NULL,
			  GrB_LAND_MONOID_BOOL,
			  c->matrix,
			  NULL),
			 c->matrix,
			 "Cannot reduce matrix to scalar in eq");

	PG_RETURN_BOOL(result);
}

Datum matrix_set_element(PG_FUNCTION_ARGS)
{
	os_Matrix *matrix;
	os_Scalar *scalar;
	GrB_Index i, j;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);
	ERRORNULL(3);

	matrix = OS_GETARG_MATRIX(0);
	i = PG_GETARG_INT64(1);
	j = PG_GETARG_INT64(2);
	scalar = OS_GETARG_SCALAR(3);

	OS_CHECK(GrB_Matrix_setElement(matrix->matrix, scalar->scalar, i, j),
		  matrix->matrix,
		  "Error setting matrix element.");

	OS_RETURN_MATRIX(matrix);
}

Datum matrix_remove_element(PG_FUNCTION_ARGS)
{
	os_Matrix *matrix;
	GrB_Index i, j;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	matrix = OS_GETARG_MATRIX(0);
	i = PG_GETARG_INT64(1);
	j = PG_GETARG_INT64(2);

	OS_CHECK(GrB_Matrix_removeElement(matrix->matrix, i, j),
		  matrix->matrix,
		  "Error setting matrix element.");

    OS_CHECK(GrB_wait(matrix->matrix, GrB_MATERIALIZE),
		  matrix->matrix,
		  "Error waiting to materialize matrix.");

	OS_RETURN_MATRIX(matrix);
}

Datum matrix_contains(PG_FUNCTION_ARGS)
{
	os_Matrix *matrix;
	GrB_Index i, j;
	GrB_Info info;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	matrix = OS_GETARG_MATRIX(0);
	i = PG_GETARG_INT64(1);
	j = PG_GETARG_INT64(2);

	info = GxB_Matrix_isStoredElement(matrix->matrix, i, j);
	if (info == GrB_SUCCESS)
		PG_RETURN_BOOL(true);
	else if (info == GrB_NO_VALUE)
		PG_RETURN_BOOL(false);
	else
		elog(ERROR, "Error checking stored element.");
	PG_RETURN_BOOL(false);
}

Datum matrix_get_element(PG_FUNCTION_ARGS)
{
	os_Matrix *matrix;
	os_Scalar *scalar;
	GrB_Index i, j;
	GrB_Type type;


	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	matrix = OS_GETARG_MATRIX(0);
	i = PG_GETARG_INT64(1);
	j = PG_GETARG_INT64(2);

	OS_MTYPE(type, matrix);
	scalar = new_scalar(type, CurrentMemoryContext, NULL);

	OS_CHECK(GrB_Matrix_extractElement(scalar->scalar, matrix->matrix, i, j),
		  matrix->matrix,
		  "Error extracting setting matrix element.");
	OS_RETURN_SCALAR(scalar);
}

Datum matrix_wait(PG_FUNCTION_ARGS)
{
	os_Matrix *A;
	int waitmode;

	LOGF();
	ERRORNULL(0);

	A = OS_GETARG_MATRIX(0);
	waitmode = PG_GETARG_INT32(1);

	OS_CHECK(GrB_Matrix_wait(A->matrix, waitmode),
		  A->matrix,
		  "Error waiting for matrix.");
	OS_RETURN_MATRIX(A);
}

Datum matrix_dup(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *matrix;
	os_Matrix *result;
	GrB_Index nrows, ncols;

	LOGF();
	ERRORNULL(0);

	matrix = OS_GETARG_MATRIX(0);
	OS_MTYPE(type, matrix);
	OS_MNROWS(nrows, matrix);
	OS_MNCOLS(ncols, matrix);
	result = new_matrix(type, nrows, ncols, CurrentMemoryContext, NULL);

	OS_CHECK(GrB_Matrix_dup(&result->matrix, matrix->matrix),
		  result->matrix,
		  "Error duping matrix.");
	OS_RETURN_MATRIX(result);
}

Datum matrix_clear(PG_FUNCTION_ARGS)
{
	os_Matrix *A;

	LOGF();
	ERRORNULL(0);

	A = OS_GETARG_MATRIX(0);

	OS_CHECK(GrB_Matrix_clear(A->matrix),
		  A->matrix,
		  "Error clearing matrix.");
	OS_RETURN_MATRIX(A);
}

Datum matrix_resize(PG_FUNCTION_ARGS)
{
	os_Matrix *A;
	GrB_Index i, j;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	A = OS_GETARG_MATRIX(0);
	i = PG_GETARG_INT64(1);
	j = PG_GETARG_INT64(2);

	OS_CHECK(GrB_Matrix_resize(A->matrix, i, j),
		  A->matrix,
		  "Error resizing matrix.");
	OS_RETURN_MATRIX(A);
}

Datum matrix_info(PG_FUNCTION_ARGS) {
	os_Matrix *A;
	char *result, *buf;
	size_t size;
	FILE *fp;
	int level;
	A = OS_GETARG_MATRIX(0);
	level = PG_GETARG_INT32(1);
	if (level > 5)
	{
		elog(ERROR, "Print level is from 0 to 5");
	}

	fp = open_memstream(&buf, &size);
	if (fp == NULL)
		elog(ERROR, "unable to open memstream for matrix_print");
	GxB_fprint(A->matrix, level, fp);
	fflush(fp);
	result = palloc(size + 1);
	memcpy(result, buf, size+1);
	free(buf);
	PG_RETURN_TEXT_P(cstring_to_text_with_len(result, size+1));
}

Datum matrix_type(PG_FUNCTION_ARGS) {
	os_Matrix *A;
	GrB_Type type;
	char *type_name;
	size_t type_name_len;
	os_Type *result;

	ERRORNULL(0);

	A = OS_GETARG_MATRIX(0);

	OS_MTYPE(type, A);

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

Datum
matrix_agg_final(PG_FUNCTION_ARGS)
{
    os_Matrix *state = OS_GETARG_MATRIX(0);

    // If no inputs were processed, return NULL
    if (state == NULL)
        PG_RETURN_NULL();

    OS_RETURN_MATRIX(state);
}

#define SUFFIX _int64                // suffix for names
#define PG_TYPE int64                // postgres type
#define GB_TYPE GrB_INT64            // graphblas matrix type
#define PG_GETARG PG_GETARG_INT64       // how to get value args
#define PG_RETURN PG_RETURN_INT64
#include "matrix_ops.h"

#define SUFFIX _int32                // suffix for names
#define PG_TYPE int32                // postgres type
#define GB_TYPE GrB_INT32            // graphblas matrix type
#define PG_GETARG PG_GETARG_INT32       // how to get value args
#define PG_RETURN PG_RETURN_INT32
#include "matrix_ops.h"

#define SUFFIX _int16                // suffix for names
#define PG_TYPE int16                // postgres type
#define GB_TYPE GrB_INT16            // graphblas matrix type
#define PG_GETARG PG_GETARG_INT16       // how to get value args
#define PG_RETURN PG_RETURN_INT16
#include "matrix_ops.h"

#define SUFFIX _fp64                // suffix for names
#define PG_TYPE float8                // postgres type
#define GB_TYPE GrB_FP64            // graphblas matrix type
#define PG_GETARG PG_GETARG_FLOAT8       // how to get value args
#define PG_RETURN PG_RETURN_FLOAT8
#include "matrix_ops.h"

#define SUFFIX _fp32                // suffix for names
#define PG_TYPE float4                // postgres type
#define GB_TYPE GrB_FP32            // graphblas matrix type
#define PG_GETARG PG_GETARG_FLOAT4       // how to get value args
#define PG_RETURN PG_RETURN_FLOAT4
#include "matrix_ops.h"

#define SUFFIX _bool                // suffix for names
#define PG_TYPE bool                // postgres type
#define GB_TYPE GrB_BOOL            // graphblas matrix type
#define PG_GETARG PG_GETARG_BOOL       // how to get value args
#define PG_RETURN PG_RETURN_BOOL
#include "matrix_ops.h"

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
