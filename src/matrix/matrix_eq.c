#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_eq);
Datum matrix_eq(PG_FUNCTION_ARGS)
{
	GrB_Type atype, btype;
	os_Matrix *a, *b, *c;
	GrB_Index anrows, ancols, anvals, bnrows, bncols, bnvals;
	bool result;
    struct timeval start, end;

	OS_START_BENCH();
	ERRORNULL(0);
	ERRORNULL(1);

	a = OS_GETARG_MATRIX(0);
	b = OS_GETARG_MATRIX_A(1, a);
	if (a == b)
		PG_RETURN_BOOL(true);

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
	OS_END_BENCH();
	PG_RETURN_BOOL(result);
}

