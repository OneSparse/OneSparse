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
PG_FUNCTION_INFO_V1(matrix_nvals);
PG_FUNCTION_INFO_V1(matrix_nrows);
PG_FUNCTION_INFO_V1(matrix_ncols);
PG_FUNCTION_INFO_V1(matrix_wait);
PG_FUNCTION_INFO_V1(matrix_dup);
PG_FUNCTION_INFO_V1(matrix_clear);
PG_FUNCTION_INFO_V1(matrix_ewise_add);
PG_FUNCTION_INFO_V1(matrix_ewise_mult);
PG_FUNCTION_INFO_V1(matrix_ewise_union);
PG_FUNCTION_INFO_V1(matrix_reduce_vector);
PG_FUNCTION_INFO_V1(matrix_reduce_scalar);
PG_FUNCTION_INFO_V1(matrix_assign_matrix);
PG_FUNCTION_INFO_V1(matrix_extract_matrix);
PG_FUNCTION_INFO_V1(matrix_mxm);
PG_FUNCTION_INFO_V1(matrix_mxv);
PG_FUNCTION_INFO_V1(matrix_vxm);
PG_FUNCTION_INFO_V1(matrix_select);

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
		Assert(matrix->serialized_size > 0);
		return matrix->flat_size;
	}

	CHECK(GxB_Matrix_serialize(
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
//	void (*free_function)(void *p);

	LOGF();

	matrix = (os_Matrix *) eohptr;
	flat = (os_FlatMatrix *) result;

	Assert(matrix->em_magic == matrix_MAGIC);
	Assert(allocated_size == matrix->flat_size);
	Assert(matrix->serialized_data != NULL);
	Assert(matrix->serialized_size > 0);

	memset(flat, 0, allocated_size);

	CHECK(GrB_get(
			  matrix->matrix,
			  &flat->type_code,
			  GrB_EL_TYPE_CODE),
		  matrix->matrix,
		  "Cannot get Matrix Type code.");

	CHECK(GrB_Matrix_nrows(&flat->nrows, matrix->matrix),
		  matrix->matrix,
		  "Error extracting matrix nrows.");

	CHECK(GrB_Matrix_ncols(&flat->ncols, matrix->matrix),
		  matrix->matrix,
		  "Error extracting matrix ncols.");

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
		CHECK(GrB_Matrix_new(&matrix->matrix, type, nrows, ncols),
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
	CHECK(GxB_Matrix_deserialize(
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

	CHECK(GrB_Matrix_free(&matrix->matrix),
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
			CHECK(GrB_Matrix_setElement(matrix->matrix, num, row, col),
				  matrix->matrix,
				  "Error setting Matrix Element");
		}
		else if (typ == GrB_INT32) {
			long num = strtol(number_token, &endptr, 10);
			if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid INT32 %s", number_token);
			CHECK(GrB_Matrix_setElement(matrix->matrix, num, row, col),
				  matrix->matrix,
				  "Error setting Matrix Element");
		}
		else if (typ == GrB_INT16) {
			int num = (int)strtol(number_token, &endptr, 10);
			if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid INT16 %s", number_token);
			CHECK(GrB_Matrix_setElement(matrix->matrix, num, row, col),
				  matrix->matrix,
				  "Error setting Matrix Element");
		}
		else if (typ == GrB_FP64)
		{
			double num = strtod(number_token, &endptr);
			if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid FP64 %s", number_token);
			CHECK(GrB_Matrix_setElement(matrix->matrix, num, row, col),
				  matrix->matrix,
				  "Error setting Matrix Element");
		}
		else if (typ == GrB_FP32)
		{
			float num = strtof(number_token, &endptr);
			if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid FP32 %s", number_token);
			CHECK(GrB_Matrix_setElement(matrix->matrix, num, row, col),
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
			CHECK(GrB_Matrix_setElement(matrix->matrix, num, row, col),
				  matrix->matrix,
				  "Error setting Matrix Element");
		}
	}
	OS_RETURN_MATRIX(matrix);
}

Datum matrix_out(PG_FUNCTION_ARGS)
{
	GrB_Info info;
	GrB_Index row, col;
	GxB_Iterator iterator;
	os_Matrix *matrix;
	StringInfoData buf;
	int32_t type_code;

	LOGF();
	matrix = OS_GETARG_MATRIX(0);

	GxB_Iterator_new(&iterator);
	CHECK(GxB_Matrix_Iterator_attach(iterator, matrix->matrix, NULL),
		  matrix->matrix,
		  "Cannot attach matrix iterator.");

	initStringInfo(&buf);
	CHECK(GrB_get(matrix->matrix, &type_code, GrB_EL_TYPE_CODE),
		  matrix->matrix,
		  "Cannot get Matrix Type code.");

	appendStringInfo(&buf, "%s[", short_code(type_code));

	info = GxB_Matrix_Iterator_seek(iterator, 0);
	while (info != GxB_EXHAUSTED)
	{
		GxB_Matrix_Iterator_getIndex(iterator, &row, &col);
		switch(type_code)
		{
			case GrB_INT64_CODE:
				int64_t vi64 = GxB_Iterator_get_INT64(iterator);
				appendStringInfo(&buf, "%lu:%lu:%ld", row, col, vi64);
				break;
			case GrB_INT32_CODE:
				int32_t vi32 = GxB_Iterator_get_INT32(iterator);
				appendStringInfo(&buf, "%lu:%lu:%d", row, col, vi32);
				break;
			case GrB_INT16_CODE:
				int16_t vi16 = GxB_Iterator_get_INT16(iterator);
				appendStringInfo(&buf, "%lu:%lu:%d", row, col, vi16);
				break;
			case GrB_FP64_CODE:
				double vf64 = GxB_Iterator_get_FP64(iterator);
				appendStringInfo(&buf, "%lu:%lu:%f", row, col, vf64);
				break;
			case GrB_FP32_CODE:
				float vf32 = GxB_Iterator_get_FP32(iterator);
				appendStringInfo(&buf, "%lu:%lu:%f", row, col, vf32);
				break;
		}
		info = GxB_Matrix_Iterator_next(iterator);
		if (info != GxB_EXHAUSTED)
			appendStringInfo(&buf, " ");
	}
	GrB_free(&iterator);

	appendStringInfo(&buf, "]");
	PG_RETURN_CSTRING(buf.data);
}

Datum matrix_nvals(PG_FUNCTION_ARGS)
{
	GrB_Index result;
	os_Matrix *matrix;

	LOGF();
	ERRORNULL(0);

	matrix = OS_GETARG_MATRIX(0);

	CHECK(GrB_Matrix_nvals(&result, matrix->matrix),
		  matrix->matrix,
		  "Error extracting matrix nvals.");
	PG_RETURN_INT64(result);
}

Datum matrix_nrows(PG_FUNCTION_ARGS)
{
	GrB_Index result;
	os_Matrix *matrix;

	LOGF();
	ERRORNULL(0);

	matrix = OS_GETARG_MATRIX(0);

	CHECK(GrB_Matrix_nrows(&result, matrix->matrix),
		  matrix->matrix,
		  "Error extracting matrix size.");
	PG_RETURN_INT64(result);
}

Datum matrix_ncols(PG_FUNCTION_ARGS)
{
	GrB_Index result;
	os_Matrix *matrix;

	LOGF();
	ERRORNULL(0);

	matrix = OS_GETARG_MATRIX(0);

	CHECK(GrB_Matrix_ncols(&result, matrix->matrix),
		  matrix->matrix,
		  "Error extracting matrix size.");
	PG_RETURN_INT64(result);
}

Datum matrix_ewise_add(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *u, *v, *w, *mask;
	os_Descriptor *descriptor;
	os_BinaryOp *op, *accum;
	GrB_Index nrows, ncols;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	u = OS_GETARG_MATRIX(0);
	v = OS_GETARG_MATRIX(1);
	op = OS_GETARG_BINARYOP(2);

	mask = NULL;
	accum = NULL;
	descriptor = NULL;

	if (PG_NARGS() > 3)
	{
		if (PG_ARGISNULL(3))
		{
			CHECK(GxB_Matrix_type(&type, u->matrix),
				  u->matrix,
				  "Cannot get matrix type");

			CHECK(GrB_Matrix_nrows(&nrows, u->matrix),
				  u->matrix,
				  "Error extracting matrix nrows.");

			CHECK(GrB_Matrix_ncols(&ncols, u->matrix),
				  u->matrix,
				  "Error extracting matrix ncols.");

			w = new_matrix(type, nrows, ncols, CurrentMemoryContext, NULL);
		}
		else
			w = OS_GETARG_MATRIX(3);
	}
	if (PG_NARGS() > 4)
		mask = PG_ARGISNULL(4) ? NULL : OS_GETARG_MATRIX(4);
	if (PG_NARGS() > 5)
		accum = PG_ARGISNULL(5) ? NULL : OS_GETARG_BINARYOP(5);
	if (PG_NARGS() >6)
		descriptor = PG_ARGISNULL(6) ? NULL : OS_GETARG_DESCRIPTOR(6);

	CHECK(GrB_eWiseAdd(w->matrix,
					   mask ? mask->matrix : NULL,
					   accum ? accum->binaryop : NULL,
					   op->binaryop,
					   u->matrix,
					   v->matrix,
					   descriptor ? descriptor->descriptor : NULL),
		  w->matrix,
		  "Error matrix eWiseAdd.");

	OS_RETURN_MATRIX(w);
}

Datum matrix_ewise_mult(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *u, *v, *w, *mask;
	os_Descriptor *descriptor;
	os_BinaryOp *op, *accum;
	GrB_Index nrows, ncols;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	u = OS_GETARG_MATRIX(0);
	v = OS_GETARG_MATRIX(1);
	op = OS_GETARG_BINARYOP(2);

	mask = NULL;
	accum = NULL;
	descriptor = NULL;

	if (PG_NARGS() > 3)
	{
		if (PG_ARGISNULL(3))
		{
			CHECK(GxB_Matrix_type(&type, u->matrix),
				  u->matrix,
				  "Cannot get matrix type");

			CHECK(GrB_Matrix_nrows(&nrows, u->matrix),
				  u->matrix,
				  "Error extracting matrix nrows.");

			CHECK(GrB_Matrix_ncols(&ncols, u->matrix),
				  u->matrix,
				  "Error extracting matrix ncols.");

			w = new_matrix(type, nrows, ncols, CurrentMemoryContext, NULL);
		}
		else
			w = OS_GETARG_MATRIX(3);
	}
	if (PG_NARGS() > 4)
		mask = PG_ARGISNULL(4) ? NULL : OS_GETARG_MATRIX(4);
	if (PG_NARGS() > 5)
		accum = PG_ARGISNULL(5) ? NULL : OS_GETARG_BINARYOP(5);
	if (PG_NARGS() > 6)
		descriptor = PG_ARGISNULL(6) ? NULL : OS_GETARG_DESCRIPTOR(6);

	CHECK(GrB_eWiseMult(w->matrix,
						mask ? mask->matrix : NULL,
						accum ? accum->binaryop : NULL,
						  op->binaryop,
						u->matrix,
						v->matrix,
						descriptor ? descriptor->descriptor : NULL),
		  w->matrix,
		  "Error matrix eWiseMult.");

	OS_RETURN_MATRIX(w);
}

Datum matrix_ewise_union(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *u, *v, *w, *mask;
	os_Scalar *a, *b;
	os_Descriptor *descriptor;
	os_BinaryOp *op, *accum;
	GrB_Index nrows, ncols;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);
	ERRORNULL(3);
	ERRORNULL(4);

	u = OS_GETARG_MATRIX(0);
	a = OS_GETARG_SCALAR(1);
	v = OS_GETARG_MATRIX(2);
	b = OS_GETARG_SCALAR(3);
	op = OS_GETARG_BINARYOP(4);

	mask = NULL;
	accum = NULL;
	descriptor = NULL;

	if (PG_NARGS() > 5)
	{
		if (PG_ARGISNULL(5))
		{
			CHECK(GxB_Matrix_type(&type, u->matrix),
				  u->matrix,
				  "Cannot get matrix type");

			CHECK(GrB_Matrix_nrows(&nrows, u->matrix),
				  u->matrix,
				  "Error extracting matrix nrows.");

			CHECK(GrB_Matrix_ncols(&ncols, u->matrix),
				  u->matrix,
				  "Error extracting matrix ncols.");

			w = new_matrix(type, nrows, ncols, CurrentMemoryContext, NULL);
		}
		else
			w = OS_GETARG_MATRIX(5);
	}
	if (PG_NARGS() > 6)
		mask = PG_ARGISNULL(6) ? NULL : OS_GETARG_MATRIX(6);
	if (PG_NARGS() > 7)
		accum = PG_ARGISNULL(7) ? NULL : OS_GETARG_BINARYOP(7);
	if (PG_NARGS() > 8)
		descriptor = PG_ARGISNULL(8) ? NULL : OS_GETARG_DESCRIPTOR(8);

	CHECK(GxB_eWiseUnion(w->matrix,
						 mask ? mask->matrix : NULL,
						 accum ? accum->binaryop : NULL,
						 op->binaryop,
						 u->matrix,
						 a->scalar,
						 v->matrix,
						 b->scalar,
						 descriptor ? descriptor->descriptor : NULL),
		  w->matrix,
		  "Error matrix eWiseUnion.");
	OS_RETURN_MATRIX(w);
}

Datum matrix_mxm(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *u, *v, *w, *mask;
	os_Descriptor *descriptor;
	os_BinaryOp *accum;
	os_Semiring *op;
	GrB_Index nrows, ncols;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	u = OS_GETARG_MATRIX(0);
	v = OS_GETARG_MATRIX(1);
	op = OS_GETARG_SEMIRING(2);

	mask = NULL;
	accum = NULL;
	descriptor = NULL;

	if (PG_NARGS() > 3)
	{
		if (PG_ARGISNULL(3))
		{
			CHECK(GxB_Matrix_type(&type, u->matrix),
				  u->matrix,
				  "Cannot get matrix type");

			CHECK(GrB_Matrix_nrows(&nrows, u->matrix),
				  u->matrix,
				  "Error extracting matrix nrows.");

			CHECK(GrB_Matrix_ncols(&ncols, v->matrix),
				  v->matrix,
				  "Error extracting matrix ncols.");

			w = new_matrix(type, nrows, ncols, CurrentMemoryContext, NULL);
		}
		else
			w = OS_GETARG_MATRIX(3);
	}
	if (PG_NARGS() > 4)
		mask = PG_ARGISNULL(4) ? NULL : OS_GETARG_MATRIX(4);
	if (PG_NARGS() > 5)
		accum = PG_ARGISNULL(5) ? NULL : OS_GETARG_BINARYOP(5);
	if (PG_NARGS() > 6)
		descriptor = PG_ARGISNULL(6) ? NULL : OS_GETARG_DESCRIPTOR(6);

	CHECK(GrB_mxm(w->matrix,
				  mask ? mask->matrix : NULL,
				  accum ? accum->binaryop : NULL,
				  op->semiring,
				  u->matrix,
				  v->matrix,
				  descriptor ? descriptor->descriptor : NULL),
		  w->matrix,
		  "Error matrix mxm.");

	OS_RETURN_MATRIX(w);
}

Datum matrix_mxv(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *u;
	os_Vector *v, *w, *mask;
	os_Descriptor *descriptor;
	os_BinaryOp *accum;
	os_Semiring *op;
	GrB_Index wsize;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	u = OS_GETARG_MATRIX(0);
	v = OS_GETARG_VECTOR(1);
	op = OS_GETARG_SEMIRING(2);

	mask = NULL;
	accum = NULL;
	descriptor = NULL;

	if (PG_NARGS() > 3)
	{
		if (PG_ARGISNULL(3))
		{
			CHECK(GxB_Matrix_type(&type, u->matrix),
				  u->matrix,
				  "Cannot get matrix type");

			CHECK(GrB_Vector_size(&wsize, v->vector),
				  v->vector,
				  "Cannot get matrix type");

			w = new_vector(type, wsize, CurrentMemoryContext, NULL);
		}
		else
			w = OS_GETARG_VECTOR(3);
	}
	if (PG_NARGS() > 4)
		mask = PG_ARGISNULL(4) ? NULL : OS_GETARG_VECTOR(4);
	if (PG_NARGS() > 5)
		accum = PG_ARGISNULL(5) ? NULL : OS_GETARG_BINARYOP(5);
	if (PG_NARGS() > 6)
		descriptor = PG_ARGISNULL(6) ? NULL : OS_GETARG_DESCRIPTOR(6);

	CHECK(GrB_mxv(w->vector,
				  mask ? mask->vector : NULL,
				  accum ? accum->binaryop : NULL,
				  op->semiring,
				  u->matrix,
				  v->vector,
				  descriptor ? descriptor->descriptor : NULL),
		  w->vector,
		  "Error matrix mxm.");

	OS_RETURN_VECTOR(w);
}

Datum matrix_vxm(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Vector *u, *w, *mask;
	os_Matrix *v;
	os_Descriptor *descriptor;
	os_BinaryOp *accum;
	os_Semiring *op;
	GrB_Index wsize;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	u = OS_GETARG_VECTOR(0);
	v = OS_GETARG_MATRIX(1);
	op = OS_GETARG_SEMIRING(2);

	mask = NULL;
	accum = NULL;
	descriptor = NULL;

	if (PG_NARGS() > 3)
	{
		if (PG_ARGISNULL(3))
		{
			CHECK(GxB_Vector_type(&type, u->vector),
				  u->vector,
				  "Cannot get vector type");

			CHECK(GrB_Vector_size(&wsize, u->vector),
				  u->vector,
				  "Cannot get matrix type");

			w = new_vector(type, wsize, CurrentMemoryContext, NULL);
		}
		else
			w = OS_GETARG_VECTOR(3);
	}
	if (PG_NARGS() > 4)
		mask = PG_ARGISNULL(4) ? NULL : OS_GETARG_VECTOR(4);
	if (PG_NARGS() > 5)
		accum = PG_ARGISNULL(5) ? NULL : OS_GETARG_BINARYOP(5);
	if (PG_NARGS() > 6)
		descriptor = PG_ARGISNULL(6) ? NULL : OS_GETARG_DESCRIPTOR(6);

	CHECK(GrB_vxm(w->vector,
				  mask ? mask->vector : NULL,
				  accum ? accum->binaryop : NULL,
				  op->semiring,
				  u->vector,
				  v->matrix,
				  descriptor ? descriptor->descriptor : NULL),
		  w->vector,
		  "Error matrix mxm.");

	OS_RETURN_VECTOR(w);
}

Datum matrix_reduce_vector(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Matrix *u;
	os_Vector *w, *mask;
	os_Descriptor *descriptor;
	os_Monoid *op;
	os_BinaryOp *accum;
	GrB_Index wsize;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	u = OS_GETARG_MATRIX(0);
	op = OS_GETARG_MONOID(1);

	descriptor = NULL;

	if (PG_NARGS() > 2)
	{
		if (PG_ARGISNULL(2))
		{
			CHECK(GxB_Matrix_type(&type, u->matrix),
				  u->matrix,
				  "Cannot get matrix type");

			CHECK(GrB_Matrix_nrows(&wsize, u->matrix),
				  u->matrix,
				  "Error extracting matrix ncols.");

			w = new_vector(type, wsize, CurrentMemoryContext, NULL);
		}
		else
			w = OS_GETARG_VECTOR(2);
	}
	if (PG_NARGS() > 3)
		mask = PG_ARGISNULL(3) ? NULL : OS_GETARG_VECTOR(3);
	if (PG_NARGS() > 4)
		accum = PG_ARGISNULL(4) ? NULL : OS_GETARG_BINARYOP(4);
	if (PG_NARGS() > 5)
		descriptor = PG_ARGISNULL(5) ? NULL : OS_GETARG_DESCRIPTOR(5);

	CHECK(GrB_Matrix_reduce_Monoid(
			  w->vector,
			  mask ? mask->vector : NULL,
			  accum ? accum->binaryop : NULL,
			  op->monoid,
			  u->matrix,
			  descriptor ? descriptor->descriptor : NULL),
		  w->vector,
		  "Error matrix vector reduce.");

	OS_RETURN_VECTOR(w);
}

Datum
matrix_reduce_scalar(PG_FUNCTION_ARGS)
{
	os_Matrix *A;
	os_Monoid *monoid;
	os_BinaryOp *accum;
	os_Descriptor *descriptor;
	os_Scalar *result;
	GrB_Type type;

	A = OS_GETARG_MATRIX(0);
	monoid = OS_GETARG_MONOID(1);

	if (PG_NARGS() > 2)
		accum = PG_ARGISNULL(2) ? NULL : OS_GETARG_BINARYOP(2);
	if (PG_NARGS() > 3)
		descriptor = PG_ARGISNULL(3) ? NULL : OS_GETARG_DESCRIPTOR(3);

	CHECK(GxB_Matrix_type(&type, A->matrix),
		  A->matrix,
		  "Cannot get Matrix type");

	result = new_scalar(type, CurrentMemoryContext, NULL);

	CHECK(GrB_Matrix_reduce_Monoid_Scalar(
			  result->scalar,
			  accum ? accum->binaryop : NULL,
			  monoid->monoid,
			  A->matrix,
			  descriptor ? descriptor->descriptor : NULL),
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

	A = OS_GETARG_MATRIX(0);
	B = OS_GETARG_MATRIX(1);

	if (PG_NARGS() > 2)
		accum = PG_ARGISNULL(2) ? NULL : OS_GETARG_BINARYOP(2);
	if (PG_NARGS() > 3)
		mask = PG_ARGISNULL(3) ? NULL : OS_GETARG_MATRIX(3);
	if (PG_NARGS() > 4)
		descriptor = PG_ARGISNULL(4) ? NULL : OS_GETARG_DESCRIPTOR(4);

	CHECK(GrB_Matrix_nvals(&nvals, B->matrix),
		  B->matrix,
		  "Error getting nvals.");

	rows = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);
	cols = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);

	CHECK(GrB_Matrix_extractTuples(rows,
								   cols,
								   NULL,
								   &nvals,
								   B->matrix),
		  B->matrix,
		  "Error extracting tuples.");

	CHECK(GrB_assign(A->matrix,
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

	A = OS_GETARG_MATRIX(0);
	B = OS_GETARG_MATRIX(1);

	if (PG_NARGS() > 2)
		accum = PG_ARGISNULL(2) ? NULL : OS_GETARG_BINARYOP(2);
	if (PG_NARGS() > 3)
		mask = PG_ARGISNULL(3) ? NULL : OS_GETARG_MATRIX(3);
	if (PG_NARGS() > 4)
		descriptor = PG_ARGISNULL(4) ? NULL : OS_GETARG_DESCRIPTOR(4);

	if (B != NULL)
	{
		CHECK(GrB_Matrix_nvals(&nvals, B->matrix),
			  B->matrix,
			  "Error getting nvals.");

		rows = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);
		cols = (GrB_Index*) palloc0(sizeof(GrB_Index) * nvals);

		CHECK(GrB_Matrix_extractTuples(rows,
									   cols,
									   NULL,
									   &nvals,
									   A->matrix),
			  A->matrix,
			  "Error extracting tuples.");
	}

	CHECK(GrB_extract(A->matrix,
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

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	A = OS_GETARG_MATRIX(0);
	op = OS_GETARG_INDEXUNARYOP(1);
	y = OS_GETARG_SCALAR(2);

	mask = NULL;
	accum = NULL;
	descriptor = NULL;

	if (PG_NARGS() > 3)
	{
		if (PG_ARGISNULL(3))
		{
			CHECK(GxB_Matrix_type(&type, A->matrix),
				  A->matrix,
				  "Cannot get matrix type");

			CHECK(GrB_Matrix_nrows(&nrows, A->matrix),
				  A->matrix,
				  "Cannot get matrix nrows");

			CHECK(GrB_Matrix_ncols(&ncols, A->matrix),
				  A->matrix,
				  "Cannot get matrix ncols");

			C = new_matrix(type, nrows, ncols, CurrentMemoryContext, NULL);
		}
		else
			C = OS_GETARG_MATRIX(3);
	}
	if (PG_NARGS() > 4)
		mask = PG_ARGISNULL(4) ? NULL : OS_GETARG_MATRIX(4);
	if (PG_NARGS() > 5)
		accum = PG_ARGISNULL(5) ? NULL : OS_GETARG_BINARYOP(5);
	if (PG_NARGS() > 6)
		descriptor = PG_ARGISNULL(6) ? NULL : OS_GETARG_DESCRIPTOR(6);

	CHECK(GrB_select(C->matrix,
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

Datum matrix_wait(PG_FUNCTION_ARGS)
{
	os_Matrix *A;
	int waitmode;

	LOGF();
	ERRORNULL(0);

	A = OS_GETARG_MATRIX(0);
	waitmode = PG_GETARG_INT32(1);

	CHECK(GrB_Matrix_wait(A->matrix, waitmode),
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
	CHECK(GxB_Matrix_type(&type, matrix->matrix),
		  matrix->matrix,
		  "Cannot get matrix type");

	CHECK(GrB_Matrix_nrows(&nrows, matrix->matrix),
		  matrix->matrix,
		  "Error extracting matrix nrows.");

	CHECK(GrB_Matrix_ncols(&ncols, matrix->matrix),
		  matrix->matrix,
		  "Error extracting matrix ncols.");

	result = new_matrix(type, nrows, ncols, CurrentMemoryContext, NULL);

	CHECK(GrB_Matrix_dup(&result->matrix, matrix->matrix),
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

	CHECK(GrB_Matrix_clear(A->matrix),
		  A->matrix,
		  "Error clearing matrix.");
	OS_RETURN_MATRIX(A);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
