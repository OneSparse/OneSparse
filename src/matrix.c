
/* The same "header template" matrix.h is used to generate the various
   type specific functions. */

#define SUFFIX _int64				// suffix for names
#define PG_TYPE int64				// postgres type
#define GB_TYPE GrB_INT64			// graphblas vector type
#define GB_DUP GrB_SECOND_INT64	  // default duplicate index resolver
#define GB_MUL GrB_TIMES_INT64	   // times bin op
#define GB_ADD GrB_PLUS_INT64	   // times bin op
#define PG_GET PG_GETARG_INT64	   // how to get value args
#define PG_RET PG_RETURN_INT64
#define PG_DGT DatumGetInt64		 // datum get type
#define PG_TGD Int64GetDatum		 // type get datum
#define PRINT_FMT(v) "%lu", v		// printf fmt
#include "matrix.h"				  // "call" template

#define SUFFIX _bool
#define PG_TYPE bool
#define GB_TYPE GrB_BOOL
#define GB_DUP GrB_SECOND_BOOL
#define GB_MUL GrB_TIMES_BOOL
#define GB_ADD GrB_PLUS_BOOL
#define PG_GET PG_GETARG_BOOL
#define PG_RET PG_RETURN_BOOL
#define PG_DGT DatumGetBool
#define PG_TGD BoolGetDatum
#define PRINT_FMT(v) "%s", v ? "t" : "f"
#include "matrix.h"

#define SUFFIX _int8
#define PG_TYPE int8
#define GB_TYPE GrB_INT8
#define GB_DUP GrB_SECOND_INT8
#define GB_MUL GrB_TIMES_INT8
#define GB_ADD GrB_PLUS_INT8
#define PG_GET PG_GETARG_CHAR
#define PG_RET PG_RETURN_CHAR
#define PG_DGT DatumGetChar
#define PG_TGD CharGetDatum
#define PRINT_FMT(v) "%i", v
#include "matrix.h"

#define SUFFIX _int16
#define PG_TYPE int16
#define GB_TYPE GrB_INT16
#define GB_DUP GrB_SECOND_INT16
#define GB_MUL GrB_TIMES_INT16
#define GB_ADD GrB_PLUS_INT16
#define PG_GET PG_GETARG_INT16
#define PG_RET PG_RETURN_INT16
#define PG_DGT DatumGetInt16
#define PG_TGD Int16GetDatum
#define PRINT_FMT(v) "%i", v
#include "matrix.h"

#define SUFFIX _int32
#define PG_TYPE int32
#define GB_TYPE GrB_INT32
#define GB_DUP GrB_SECOND_INT32
#define GB_MUL GrB_TIMES_INT32
#define GB_ADD GrB_PLUS_INT32
#define PG_GET PG_GETARG_INT32
#define PG_RET PG_RETURN_INT32
#define PG_DGT DatumGetInt32
#define PG_TGD Int32GetDatum
#define PRINT_FMT(v) "%i", v
#include "matrix.h"

#define SUFFIX _float4
#define PG_TYPE float4
#define GB_TYPE GrB_FP32
#define GB_DUP GrB_SECOND_FP32
#define GB_MUL GrB_TIMES_FP32
#define GB_ADD GrB_PLUS_FP32
#define PG_GET PG_GETARG_FLOAT4
#define PG_RET PG_RETURN_FLOAT4
#define PG_DGT DatumGetFloat4
#define PG_TGD Float4GetDatum
#define PRINT_FMT(v) "%f", v
#include "matrix.h"

#define SUFFIX _float8
#define PG_TYPE float8
#define GB_TYPE GrB_FP64
#define GB_DUP GrB_SECOND_FP64
#define GB_MUL GrB_TIMES_FP64
#define GB_ADD GrB_PLUS_FP64
#define PG_GET PG_GETARG_FLOAT8
#define PG_RET PG_RETURN_FLOAT8
#define PG_DGT DatumGetFloat8
#define PG_TGD Float8GetDatum
#define PRINT_FMT(v) "%f", v
#include "matrix.h"

/* MemoryContextCallback function to free matrices */
static void
context_callback_matrix_free(void* m)
{
	OS_Matrix *mat = (OS_Matrix *) m;
	GrB_Matrix_free(&mat->M);
}

/* Construct an empty flat matrix. */
OS_FlatMatrix *
construct_empty_flat_matrix(GrB_Index nrows,
							GrB_Index ncols,
							GrB_Type type)
{
	OS_FlatMatrix *result;

	result = (OS_FlatMatrix *) palloc0(sizeof(OS_FlatMatrix));
	SET_VARSIZE(result, sizeof(OS_FlatMatrix));
	result->nrows = ncols;
	result->ncols = nrows;
	result->nvals = 0;
	result->type = type;
	return result;
}

/* Helper function to always expanded datum

   This is used by OS_GETARG_MATRIX */
OS_Matrix *
DatumGetMatrix(Datum d)
{
	OS_Matrix *A;
	OS_FlatMatrix *flat;

	if (VARATT_IS_EXTERNAL_EXPANDED(DatumGetPointer(d)))
	{
		A = MatrixGetEOHP(d);
		Assert(A->em_magic == matrix_MAGIC);
		return A;
	}
	flat = (OS_FlatMatrix*)PG_DETOAST_DATUM(d);
	TYPE_APPLY(d, flat->type, expand_flat_matrix, flat, CurrentMemoryContext);
	return MatrixGetEOHP(d);
}

Datum
matrix_in(PG_FUNCTION_ARGS)
{
	OS_FlatMatrix *flat;
	char *input;
	size_t len;
	int bc;
	Datum d;

	input = PG_GETARG_CSTRING(0);
	len = strlen(input);
	bc = (len) / 2 + VARHDRSZ;
	flat = palloc(bc);
	hex_decode(input, len, (char*)flat);
	TYPE_APPLY(d, flat->type, expand_flat_matrix, flat, CurrentMemoryContext);
	return d;
}

Datum
matrix_out(PG_FUNCTION_ARGS)
{
	Size size;
	char *rp, *result, *buf;
	OS_Matrix *A = OS_GETARG_MATRIX(0);
	size = EOH_get_flat_size(&A->hdr);
	buf = palloc(size);
	EOH_flatten_into(&A->hdr, buf, size);
	rp = result = palloc((size * 2) + 1);
	rp += hex_encode(buf, size, rp);
	*rp = '\0';
	PG_RETURN_CSTRING(result);
}

Datum
matrix_nrows(PG_FUNCTION_ARGS)
{
	GrB_Info info;
	OS_Matrix *mat;
	GrB_Index count;
	mat = OS_GETARG_MATRIX(0);
	CHECKD(GrB_Matrix_nrows(&count, mat->M), mat->M);
	return Int64GetDatum(count);
}

Datum
matrix_ncols(PG_FUNCTION_ARGS)
{
	GrB_Info info;
	OS_Matrix *mat;
	GrB_Index count;
	mat = OS_GETARG_MATRIX(0);
	CHECKD(GrB_Matrix_ncols(&count, mat->M), mat->M);
	return Int64GetDatum(count);
}

Datum
matrix_nvals(PG_FUNCTION_ARGS)
{
	GrB_Info info;
	OS_Matrix *mat;
	GrB_Index count;
	mat = OS_GETARG_MATRIX(0);
	CHECKD(GrB_Matrix_nvals(&count, mat->M), mat->M);
	return Int64GetDatum(count);
}

Datum
matrix_eq(PG_FUNCTION_ARGS)
{
	GrB_Info info;
	OS_Matrix *A, *B;
	bool result;

	A = OS_GETARG_MATRIX(0);
	B = OS_GETARG_MATRIX(1);

	result = true;
	PG_RETURN_BOOL(result);
}

Datum
matrix_ne(PG_FUNCTION_ARGS)
{
	GrB_Info info;
	OS_Matrix *A, *B;
	bool result;

	A = OS_GETARG_MATRIX(0);
	B = OS_GETARG_MATRIX(1);

	result = true;
	PG_RETURN_BOOL(!result);
}

Datum
matrix_ewise_mult(PG_FUNCTION_ARGS)
{
	OS_Matrix *A, *B, *C = NULL, *mask = NULL;
	Datum d;
	char *binop_name, *accum_name;
	GrB_BinaryOp binop = NULL, accum = NULL;
	GrB_Descriptor desc = NULL;
	GrB_Info info;

	A = OS_GETARG_MATRIX(0);
	B = OS_GETARG_MATRIX(1);

	binop_name = matrix_times_binop(A, B);
	accum_name = NULL;

	if (PG_NARGS() > 2) {
		C = PG_ARGISNULL(2) ? NULL : OS_GETARG_MATRIX(2);
		mask = PG_ARGISNULL(3) ? NULL : OS_GETARG_MATRIX(3);
		binop_name = PG_ARGISNULL(4) ?
			binop_name : text_to_cstring(PG_GETARG_TEXT_PP(4));
		accum_name = PG_ARGISNULL(5) ?
			accum_name : text_to_cstring(PG_GETARG_TEXT_PP(5));
		if (PG_NARGS() > 6) {
			GET_DESCRIPTOR(6, desc);
		}
	}
	binop = lookup_binop(binop_name);
	if (binop == NULL)
		elog(ERROR, "unknown binop name %s", binop_name);
	if (accum_name != NULL)
		accum = lookup_binop(accum_name);

	TYPE_APPLY(d, A->type, matrix_ewise_mult, A, B, C, mask, binop, accum, desc);
	return d;
}

Datum
matrix_ewise_add(PG_FUNCTION_ARGS)
{
	OS_Matrix *A, *B, *C = NULL, *mask = NULL;
	Datum d;
	char *binop_name, *accum_name;
	GrB_BinaryOp binop = NULL, accum = NULL;
	GrB_Descriptor desc = NULL;
	GrB_Info info;

	A = OS_GETARG_MATRIX(0);
	B = OS_GETARG_MATRIX(1);

	accum_name = NULL;
	binop_name = matrix_plus_binop(A, B);

	if (PG_NARGS() > 2)
	{
		C = PG_ARGISNULL(2) ? NULL : OS_GETARG_MATRIX(2);
		mask = PG_ARGISNULL(3) ? NULL : OS_GETARG_MATRIX(3);
		binop_name = PG_ARGISNULL(4) ?
			binop_name : text_to_cstring(PG_GETARG_TEXT_PP(4));
		accum_name = PG_ARGISNULL(5) ?
			accum_name : text_to_cstring(PG_GETARG_TEXT_PP(5));
		if (PG_NARGS() > 6)
		{
			GET_DESCRIPTOR(6, desc);
		}
	}

	binop = lookup_binop(binop_name);
	if (binop == NULL)
		elog(ERROR, "unknown binop name %s", binop_name);
	if (accum_name != NULL)
		accum = lookup_binop(accum_name);

	TYPE_APPLY(d, A->type, matrix_ewise_add, A, B, C, mask, binop, accum, desc);
	return d;
}

Datum
mxm(PG_FUNCTION_ARGS)
{
	OS_Matrix *A, *B, *C = NULL, *mask = NULL;
	GrB_Info info;
	GrB_Type type;
	Datum d;
	char *semiring_name, *binop_name;
	GrB_Semiring semiring;
	GrB_BinaryOp binop = NULL;
	GrB_Descriptor desc = NULL;

	A = OS_GETARG_MATRIX(0);
	B = OS_GETARG_MATRIX(1);

	semiring_name = mxm_semiring(A, B);
	binop_name = NULL;

	if (PG_NARGS() > 2)
	{
		C = PG_ARGISNULL(2) ? NULL : OS_GETARG_MATRIX(2);
		mask = PG_ARGISNULL(3) ? NULL : OS_GETARG_MATRIX(3);
		semiring_name = PG_ARGISNULL(4) ?
			semiring_name :
			text_to_cstring(PG_GETARG_TEXT_PP(4));
		binop_name = PG_ARGISNULL(5) ?
			NULL :
			text_to_cstring(PG_GETARG_TEXT_PP(5));

		if (PG_NARGS() > 6)
		{
			GET_DESCRIPTOR(6, desc);
		}
	}

	semiring = lookup_semiring(semiring_name);
	if (semiring == NULL)
		elog(ERROR, "unknown semiring name %s", semiring_name);

	if (binop_name != NULL)
		binop = lookup_binop(binop_name);

	if (semiring == NULL)
		elog(ERROR, "unknown semiring name %s", semiring_name);

	type = mxm_type(A, B);

	TYPE_APPLY(d, type, mxm, A, B, C, mask, semiring, binop, desc);
	return d;
}

Datum
mxv(PG_FUNCTION_ARGS)
{
	GrB_Info info;
	OS_Matrix *A;
	OS_Vector *B, *C = NULL, *mask = NULL;
	GrB_Type type;
	Datum d;
	char *semiring_name, *binop_name;
	GrB_Semiring semiring;
	GrB_BinaryOp binop = NULL;
	GrB_Descriptor desc = NULL;

	A = OS_GETARG_MATRIX(0);
	B = OS_GETARG_VECTOR(1);

	semiring_name = mxv_semiring(A, B);
	binop_name = NULL;

	if (PG_NARGS() > 2)
	{
		C = PG_ARGISNULL(2) ? NULL : OS_GETARG_VECTOR(2);
		mask = PG_ARGISNULL(3) ? NULL : OS_GETARG_VECTOR(3);
		semiring_name = PG_ARGISNULL(4) ?
			semiring_name :
			text_to_cstring(PG_GETARG_TEXT_PP(4));
		binop_name = PG_ARGISNULL(5) ?
			NULL :
			text_to_cstring(PG_GETARG_TEXT_PP(5));
		if (PG_NARGS() > 6) {
			GET_DESCRIPTOR(6, desc);
		}
	}

	if (binop_name != NULL)
		binop = lookup_binop(binop_name);

	semiring = lookup_semiring(semiring_name);
	if (semiring == NULL)
		elog(ERROR, "unknown semiring name %s", semiring_name);

	type = mxv_type(A, B);

	TYPE_APPLY(d, type, mxv, A, B, C, mask, semiring, binop, desc);
	return d;
}

Datum
vxm(PG_FUNCTION_ARGS)
{
	OS_Matrix *B;
	OS_Vector *A, *C = NULL, *mask = NULL;
	GrB_Type type;
	GrB_Info info;
	Datum d;
	char *semiring_name, *binop_name;
	GrB_Semiring semiring;
	GrB_BinaryOp binop = NULL;
	GrB_Descriptor desc = NULL;

	A = OS_GETARG_VECTOR(0);
	B = OS_GETARG_MATRIX(1);

	semiring_name = vxm_semiring(A, B);
	binop_name = NULL;

	if (PG_NARGS() > 2)
	{
		C = PG_ARGISNULL(2) ? NULL : OS_GETARG_VECTOR(2);
		mask = PG_ARGISNULL(3) ? NULL : OS_GETARG_VECTOR(3);
		semiring_name = PG_ARGISNULL(4) ? semiring_name : PG_GETARG_CSTRING(4);
		binop_name = PG_ARGISNULL(5) ?
			NULL :
			text_to_cstring(PG_GETARG_TEXT_PP(5));
		if (PG_NARGS() > 6)
		{
			GET_DESCRIPTOR(6, desc);
		}
	}

	if (binop_name != NULL)
		binop = lookup_binop(binop_name);

	semiring = lookup_semiring(semiring_name);
	if (semiring == NULL)
		elog(ERROR, "unknown semiring name %s", semiring_name);

	type = vxm_type(A, B);

	TYPE_APPLY(d, type, vxm, A, B, C, mask, semiring, binop, desc);
	return d;
}

Datum
matrix_reduce_vector(PG_FUNCTION_ARGS)
{
	GrB_Info info;
	OS_Matrix *A;
	GrB_Semiring semiring;
	GrB_Monoid monoid;
	char *semiring_name;
	OS_Vector *val;
	GrB_Index size;
	GrB_Type type;

	A = OS_GETARG_MATRIX(0);
	semiring_name = PG_ARGISNULL(1)?
		NULL:
		text_to_cstring(PG_GETARG_TEXT_PP(1));

	if (semiring_name == NULL)
	{
		CHECKD(GxB_Matrix_type(&type, A->M), A->M);
		semiring_name = DEFAULT_SEMIRING(type);
	}

	semiring = lookup_semiring(semiring_name);
	CHECKD(GxB_Semiring_add(&monoid, semiring), semiring);

	CHECKD(GrB_Matrix_ncols(&size, A->M), A->M);
	TYPE_APPLY(val, A->type, construct_empty_expanded_vector, size, CurrentMemoryContext);

	CHECKD(GrB_reduce(val->V, NULL, monoid, A->M, NULL), A->M);
	OS_RETURN_VECTOR(val);
}

Datum
matrix_transpose(PG_FUNCTION_ARGS) {
	GrB_Info info;
	OS_Matrix *A, *C = NULL, *mask;
	GrB_Index m, n;
	GrB_Descriptor desc = NULL;

	A = OS_GETARG_MATRIX(0);

	if (PG_NARGS() > 0) {
		C = PG_ARGISNULL(1) ? NULL : OS_GETARG_MATRIX(2);
		mask = PG_ARGISNULL(2) ? NULL : OS_GETARG_MATRIX(3);
		if (PG_NARGS() > 3) {
			GET_DESCRIPTOR(3, desc);
		}
	}

	if (C == NULL) {
		CHECKD(GrB_Matrix_nrows(&m, A->M), A->M);
		CHECKD(GrB_Matrix_ncols(&n, A->M), A->M);
		TYPE_APPLY(C, A->type, construct_empty_expanded_matrix, m, n, CurrentMemoryContext);
	}

	CHECKD(GrB_transpose(C->M, mask? mask->M:NULL, NULL, A->M, desc), A->M);
	OS_RETURN_MATRIX(C);
}

Datum
matrix_assign_matrix(PG_FUNCTION_ARGS) {
	GrB_Info info;
	OS_Matrix *A, *B, *mask;
	GrB_Index nvals, *rows = NULL, *cols = NULL;

	A = OS_GETARG_MATRIX(0);
	B = OS_GETARG_MATRIX(1);
	mask = PG_ARGISNULL(2)? NULL : OS_GETARG_MATRIX(2);

	CHECKD(GrB_Matrix_nvals(&nvals, B->M), B->M);

	if (B != NULL) {
		TYPE_APPLY(nvals, B->type, extract_rowscols, B, &rows, &cols, nvals);
	}

	CHECKD(GrB_assign(A->M,
					  mask? mask->M: NULL,
					  NULL,
					  B->M,
					  rows? rows : GrB_ALL,
					  nvals,
					  cols? cols : GrB_ALL,
					  nvals,
					  NULL), A->M);
	OS_RETURN_MATRIX(A);
}

Datum
matrix_kron(PG_FUNCTION_ARGS) {
	OS_Matrix *A, *B, *C = NULL, *mask = NULL;
	GrB_Info info;
	GrB_Type type;
	Datum d;
	char *accum_name, *mulop_name;
	GrB_BinaryOp accum = NULL, mulop = NULL;
	GrB_Descriptor desc = NULL;

	A = OS_GETARG_MATRIX(0);
	B = OS_GETARG_MATRIX(1);
	accum_name = NULL;
	mulop_name = NULL;

	if (PG_NARGS() > 2) {
		C = PG_ARGISNULL(2) ? NULL : OS_GETARG_MATRIX(2);
		mask = PG_ARGISNULL(3) ? NULL : OS_GETARG_MATRIX(3);
		accum_name = PG_ARGISNULL(4) ?
			NULL :
			text_to_cstring(PG_GETARG_TEXT_PP(4));
		mulop_name = PG_ARGISNULL(5) ?
			NULL :
			text_to_cstring(PG_GETARG_TEXT_PP(5));

		if (PG_NARGS() > 6) {
			GET_DESCRIPTOR(6, desc);
		}
	}

	if (accum_name != NULL)
		accum = lookup_binop(accum_name);

	if (mulop_name == NULL)
		mulop_name = matrix_times_binop(A, NULL);

	mulop = lookup_binop(mulop_name);

	type = mxm_type(A, B);

	TYPE_APPLY(d, type, matrix_kron, A, B, C, mask, accum, mulop, desc);
	return d;
}

Datum
matrix_xtract(PG_FUNCTION_ARGS) {
	GrB_Info info;
	OS_Matrix *A, *B, *C = NULL;
	GrB_Index nrows, ncols, nvals, *rows, *cols;
	GrB_Descriptor desc = NULL;

	A = OS_GETARG_MATRIX(0);
	B = OS_GETARG_MATRIX(1);
	C = PG_ARGISNULL(2) ? NULL : OS_GETARG_MATRIX(2);
	GET_DESCRIPTOR(3, desc);

	CHECKD(GrB_Matrix_nrows(&nrows, A->M), A->M);
	CHECKD(GrB_Matrix_ncols(&ncols, A->M), A->M);

	if (C == NULL) {
		TYPE_APPLY(C, A->type, construct_empty_expanded_matrix, nrows, ncols, CurrentMemoryContext);
	}

	CHECKD(GrB_Matrix_nvals(&nvals, A->M), A->M);
	TYPE_APPLY(nvals, A->type, extract_rowscols, B, &rows, &cols, nvals);

	CHECKD(GrB_Matrix_extract(C->M, NULL, NULL, A->M, rows, nrows, cols, ncols, NULL), A->M);
	OS_RETURN_MATRIX(C);
}

Datum
matrix_print(PG_FUNCTION_ARGS) {
	OS_Matrix *A;
	char *result, *buf;
	size_t size;
	FILE *fp;
	int level;
	A = OS_GETARG_MATRIX(0);
	level = PG_GETARG_INT32(1);

	fp = open_memstream(&buf, &size);
	if (fp == NULL)
		elog(ERROR, "unable to open memstream for matrix_print");
	GxB_fprint(A->M, level, fp);
	fflush(fp);
	result = palloc(size + 1);
	memcpy(result, buf, size+1);
	free(buf);
	PG_RETURN_TEXT_P(cstring_to_text_with_len(result, size+1));
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
