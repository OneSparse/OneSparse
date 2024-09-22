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
PG_FUNCTION_INFO_V1(matrix_ewise_add);
PG_FUNCTION_INFO_V1(matrix_ewise_mult);
PG_FUNCTION_INFO_V1(matrix_ewise_union);

static Size matrix_get_flat_size(ExpandedObjectHeader *eohptr) {
	onesparse_Matrix *matrix;
	void *serialized_data;
	GrB_Index serialized_size;

	LOGF();

	matrix = (onesparse_Matrix*) eohptr;
	Assert(matrix->em_magic == matrix_MAGIC);

	if (matrix->flat_size)
	{
		Assert(matrix->serialized_data != NULL);
		Assert(matrix->serialized_size > 0);
		return matrix->flat_size;
	}

	ERRORIF(GxB_Matrix_serialize(&serialized_data, &serialized_size, matrix->matrix, NULL) != GrB_SUCCESS,
			"Error serializing matrix");

	matrix->serialized_data = serialized_data;
	matrix->serialized_size = serialized_size;
	matrix->flat_size = ONESPARSE_MATRIX_FLATSIZE() + serialized_size;
	return matrix->flat_size;
}

/* Flatten matrix into a pre-allocated result buffer that is
   allocated_size in bytes.  */
static void flatten_matrix(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size)
{
	onesparse_Matrix *matrix;
	onesparse_FlatMatrix *flat;
	void* data;
//	void (*free_function)(void *p);

	LOGF();

	matrix = (onesparse_Matrix *) eohptr;
	flat = (onesparse_FlatMatrix *) result;

	Assert(matrix->em_magic == matrix_MAGIC);
	Assert(allocated_size == matrix->flat_size);
	Assert(matrix->serialized_data != NULL);
	Assert(matrix->serialized_size > 0);

	memset(flat, 0, allocated_size);

	ERRORIF(GrB_get(matrix->matrix, &flat->type_code, GrB_EL_TYPE_CODE) != GrB_SUCCESS,
			"Cannot get Matrix Type code.");

	ERRORIF(GrB_Matrix_nrows(&flat->nrows, matrix->matrix) != GrB_SUCCESS,
			"Error extracting matrix nrows.");

	ERRORIF(GrB_Matrix_ncols(&flat->ncols, matrix->matrix) != GrB_SUCCESS,
			"Error extracting matrix ncols.");

	data = ONESPARSE_MATRIX_DATA(flat);

	memcpy(data, matrix->serialized_data, matrix->serialized_size);
	flat->serialized_size = matrix->serialized_size;

	/* free_function = NULL; */
	/* ERRORIF(GrB_get(GrB_Global, (void*)free_function, GxB_FREE_FUNCTION) != GrB_SUCCESS, */
	/* 		"Cannot get SuiteSparse free function."); */

	/* free_function(matrix->serialized_data); */
	SET_VARSIZE(flat, allocated_size);
}

/* Construct an empty expanded matrix. */
onesparse_Matrix* new_matrix(
	GrB_Type type,
	GrB_Index nrows,
	GrB_Index ncols,
	MemoryContext parentcontext,
	GrB_Matrix _matrix)
{
	onesparse_Matrix *matrix;

	MemoryContext objcxt, oldcxt;
	MemoryContextCallback *ctxcb;

	LOGF();
	objcxt = AllocSetContextCreate(parentcontext,
								   "expanded matrix",
								   ALLOCSET_DEFAULT_SIZES);

	matrix = MemoryContextAlloc(objcxt,	sizeof(onesparse_Matrix));

	EOH_init_header(&matrix->hdr, &matrix_methods, objcxt);

	oldcxt = MemoryContextSwitchTo(objcxt);

	matrix->em_magic = matrix_MAGIC;
	matrix->flat_size = 0;
	matrix->serialized_data = NULL;
	matrix->serialized_size = 0;

	if (_matrix == NULL)
	{
		ERRORIF(GrB_Matrix_new(&matrix->matrix, type, nrows, ncols) != GrB_SUCCESS,
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
Datum expand_matrix(onesparse_FlatMatrix *flat, MemoryContext parentcontext)
{
	GrB_Type type;
	onesparse_Matrix *matrix;
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
	data = ONESPARSE_MATRIX_DATA(flat);
	ERRORIF(GxB_Matrix_deserialize(&matrix->matrix, type, data, flat->serialized_size, NULL) != GrB_SUCCESS,
			"Error deserializing matrix");
	ONESPARSE_RETURN_MATRIX(matrix);
}

static void
context_callback_matrix_free(void* ptr)
{
	onesparse_Matrix *matrix = (onesparse_Matrix *) ptr;
	LOGF();

	ERRORIF(GrB_Matrix_free(&matrix->matrix) != GrB_SUCCESS,
			"Cannot GrB_Free Matrix");
}

/* Helper function to always expand datum

   This is used by PG_GETARG_MATRIX */
onesparse_Matrix* DatumGetMatrix(Datum datum)
{
	onesparse_Matrix *matrix;
	onesparse_FlatMatrix *flat;

	LOGF();
	if (VARATT_IS_EXTERNAL_EXPANDED_RW(DatumGetPointer(datum))) {
		matrix = MatrixGetEOHP(datum);
		Assert(matrix->em_magic == matrix_MAGIC);
		return matrix;
	}
	flat = ONESPARSE_DETOAST_MATRIX(datum);
	datum = expand_matrix(flat, CurrentMemoryContext);
	return MatrixGetEOHP(datum);
}

Datum matrix_in(PG_FUNCTION_ARGS)
{
	onesparse_Matrix *matrix;
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

	input = PG_GETARG_CSTRING(0);
	input_copy = strdup(input);
    token = strtok_r(input_copy, "[", &saveptr);

    if (token != NULL)
	{
		short_name = palloc(strlen(token)+1);
        strcpy(short_name, token);
		typ = short_type(short_name);
    }
	else
		elog(ERROR, "Matrix parse error, no short type code prefix.");

	matrix = new_matrix(typ, GrB_INDEX_MAX+1, GrB_INDEX_MAX+1, CurrentMemoryContext, NULL);

    strcpy(input_copy, input);
    strtok_r(input_copy, "[", &saveptr);
    token = strtok_r(NULL, "]", &saveptr);

	row = 0;
	col = 0;
    for (number_token = strtok_r(token, " ", &number_saveptr); number_token != NULL;
         number_token = strtok_r(NULL, " ", &number_saveptr))
	{
        errno = 0;

		if (sscanf(number_token, "%d:%d:%s", &row, &col, number_token) != 3)
			elog(ERROR, "Invalid matrix entry %s", number_token);

        if (typ == GrB_INT64) {
            int64_t num = strtoll(number_token, &endptr, 10);
            if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid INT64 %s", number_token);
			ERRORIF(GrB_Matrix_setElement(matrix->matrix, num, row, col) != GrB_SUCCESS,
				"Error setting Matrix Element");
        }
        else if (typ == GrB_INT32) {
            long num = strtol(number_token, &endptr, 10);
            if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid INT32 %s", number_token);
			ERRORIF(GrB_Matrix_setElement(matrix->matrix, num, row, col) != GrB_SUCCESS,
					"Error setting Matrix Element");
        }
        else if (typ == GrB_INT16) {
            int num = (int)strtol(number_token, &endptr, 10);
            if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid INT16 %s", number_token);
			ERRORIF(GrB_Matrix_setElement(matrix->matrix, num, row, col) != GrB_SUCCESS,
					"Error setting Matrix Element");
        }
		else if (typ == GrB_FP64)
		{
            double num = strtod(number_token, &endptr);
            if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid FP64 %s", number_token);
			ERRORIF(GrB_Matrix_setElement(matrix->matrix, num, row, col) != GrB_SUCCESS,
					"Error setting Matrix Element");
        }
		else if (typ == GrB_FP32)
		{
            float num = strtof(number_token, &endptr);
            if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid FP32 %s", number_token);
			ERRORIF(GrB_Matrix_setElement(matrix->matrix, num, row, col) != GrB_SUCCESS,
					"Error setting Matrix Element");
        }
		else if (typ == GrB_BOOL)
		{
			bool num;
            if (strcmp(number_token, "t") == 0)
				num = true;
			else if  (strcmp(number_token, "f") == 0)
				num = false;
			else
				elog(ERROR, "Invalid BOOL %s", number_token);
			ERRORIF(GrB_Matrix_setElement(matrix->matrix, num, row, col) != GrB_SUCCESS,
					"Error setting Matrix Element");
        }
    }
	ONESPARSE_RETURN_MATRIX(matrix);
}

Datum matrix_out(PG_FUNCTION_ARGS)
{
	GrB_Info info;
	GrB_Index row, col;
    GxB_Iterator iterator;
	onesparse_Matrix *matrix;
    StringInfoData buf;
	int32_t type_code;

	LOGF();
	matrix = ONESPARSE_GETARG_MATRIX(0);

    GxB_Iterator_new(&iterator);
	ERRORIF(GxB_Matrix_Iterator_attach(iterator, matrix->matrix, NULL) != GrB_SUCCESS,
			"Cannot attach matrix iterator.");

    initStringInfo(&buf);
	ERRORIF(GrB_get(matrix->matrix, &type_code, GrB_EL_TYPE_CODE) != GrB_SUCCESS,
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
	onesparse_Matrix *matrix;

	LOGF();
	ERRORNULL(0);

	matrix = ONESPARSE_GETARG_MATRIX(0);

	ERRORIF(GrB_Matrix_nvals(&result, matrix->matrix) != GrB_SUCCESS,
			"Error extracting matrix nvals.");
	PG_RETURN_INT64(result);
}

Datum matrix_nrows(PG_FUNCTION_ARGS)
{
	GrB_Index result;
	onesparse_Matrix *matrix;

	LOGF();
	ERRORNULL(0);

	matrix = ONESPARSE_GETARG_MATRIX(0);

	ERRORIF(GrB_Matrix_nrows(&result, matrix->matrix) != GrB_SUCCESS,
			"Error extracting matrix size.");
	PG_RETURN_INT64(result);
}

Datum matrix_ncols(PG_FUNCTION_ARGS)
{
	GrB_Index result;
	onesparse_Matrix *matrix;

	LOGF();
	ERRORNULL(0);

	matrix = ONESPARSE_GETARG_MATRIX(0);

	ERRORIF(GrB_Matrix_ncols(&result, matrix->matrix) != GrB_SUCCESS,
			"Error extracting matrix size.");
	PG_RETURN_INT64(result);
}

Datum matrix_ewise_add(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	onesparse_Matrix *u, *v, *w, *mask;
	onesparse_Descriptor *descriptor;
	onesparse_BinaryOp *op, *accum;
	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	u = ONESPARSE_GETARG_MATRIX(0);
	v = ONESPARSE_GETARG_MATRIX(1);
	op = ONESPARSE_GETARG_BINARYOP(2);

	mask = NULL;
	accum = NULL;
	descriptor = NULL;

	if (PG_NARGS() > 3)
		mask = PG_ARGISNULL(3) ? NULL : ONESPARSE_GETARG_MATRIX(3);
	if (PG_NARGS() > 4)
		 accum = PG_ARGISNULL(4) ? NULL : ONESPARSE_GETARG_BINARYOP(4);
	if (PG_NARGS() > 5)
		 descriptor = PG_ARGISNULL(5) ? NULL : ONESPARSE_GETARG_DESCRIPTOR(5);

	ERRORIF(GxB_Matrix_type(&type, u->matrix) != GrB_SUCCESS,
			"Cannot get matrix type");

	w = new_matrix(type, GrB_INDEX_MAX+1, GrB_INDEX_MAX+1, CurrentMemoryContext, NULL);

	ERRORIF(GrB_eWiseAdd(w->matrix,
						 mask ? mask->matrix : NULL,
						 accum ? accum->binaryop : NULL,
						 op->binaryop,
						 u->matrix,
						 v->matrix,
						 descriptor ? descriptor->descriptor : NULL) != GrB_SUCCESS,
			"Error matrix eWiseAdd.");

	ONESPARSE_RETURN_MATRIX(w);
}

Datum matrix_ewise_mult(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	onesparse_Matrix *u, *v, *w, *mask;
	onesparse_Descriptor *descriptor;
	onesparse_BinaryOp *op, *accum;
	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);

	u = ONESPARSE_GETARG_MATRIX(0);
	v = ONESPARSE_GETARG_MATRIX(1);
	op = ONESPARSE_GETARG_BINARYOP(2);

	mask = NULL;
	accum = NULL;
	descriptor = NULL;

	if (PG_NARGS() > 3)
		mask = PG_ARGISNULL(3) ? NULL : ONESPARSE_GETARG_MATRIX(3);
	if (PG_NARGS() > 4)
		 accum = PG_ARGISNULL(4) ? NULL : ONESPARSE_GETARG_BINARYOP(4);
	if (PG_NARGS() > 5)
		 descriptor = PG_ARGISNULL(5) ? NULL : ONESPARSE_GETARG_DESCRIPTOR(5);

	ERRORIF(GxB_Matrix_type(&type, u->matrix) != GrB_SUCCESS,
			"Cannot get matrix type");

	w = new_matrix(type, GrB_INDEX_MAX+1, GrB_INDEX_MAX+1, CurrentMemoryContext, NULL);

	ERRORIF(GrB_eWiseMult(w->matrix,
						  mask ? mask->matrix : NULL,
						  accum ? accum->binaryop : NULL,
						  op->binaryop,
						  u->matrix,
						  v->matrix,
						  descriptor ? descriptor->descriptor : NULL) != GrB_SUCCESS,
			"Error matrix eWiseMult.");

	ONESPARSE_RETURN_MATRIX(w);
}

Datum matrix_ewise_union(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	onesparse_Matrix *u, *v, *w, *mask;
	onesparse_Scalar *a, *b;
	onesparse_Descriptor *descriptor;
	onesparse_BinaryOp *op, *accum;
	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	ERRORNULL(2);
	ERRORNULL(3);
	ERRORNULL(4);

	u = ONESPARSE_GETARG_MATRIX(0);
	a = ONESPARSE_GETARG_SCALAR(1);
	v = ONESPARSE_GETARG_MATRIX(2);
	b = ONESPARSE_GETARG_SCALAR(3);
	op = ONESPARSE_GETARG_BINARYOP(4);

	mask = NULL;
	accum = NULL;
	descriptor = NULL;

	if (PG_NARGS() > 5)
		mask = PG_ARGISNULL(5) ? NULL : ONESPARSE_GETARG_MATRIX(5);
	if (PG_NARGS() > 6)
		 accum = PG_ARGISNULL(6) ? NULL : ONESPARSE_GETARG_BINARYOP(6);
	if (PG_NARGS() > 7)
		 descriptor = PG_ARGISNULL(7) ? NULL : ONESPARSE_GETARG_DESCRIPTOR(7);

	ERRORIF(GxB_Matrix_type(&type, u->matrix) != GrB_SUCCESS,
			"Cannot get matrix type");

	w = new_matrix(type, GrB_INDEX_MAX+1, GrB_INDEX_MAX+1, CurrentMemoryContext, NULL);

	ERRORIF(GxB_eWiseUnion(w->matrix,
						   mask ? mask->matrix : NULL,
						   accum ? accum->binaryop : NULL,
						   op->binaryop,
						   u->matrix,
						   a->scalar,
						   v->matrix,
						   b->scalar,
						   descriptor ? descriptor->descriptor : NULL) != GrB_SUCCESS,
			"Error matrix eWiseUnion.");

	ONESPARSE_RETURN_MATRIX(w);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
