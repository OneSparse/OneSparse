
PG_FUNCTION_INFO_V1(FN(scalar));
PG_FUNCTION_INFO_V1(FN(set_scalar));
PG_FUNCTION_INFO_V1(FN(cast_scalar));

PG_FUNCTION_INFO_V1(FN(scalar_plus));
PG_FUNCTION_INFO_V1(FN(plus_scalar));
PG_FUNCTION_INFO_V1(FN(scalar_minus));
PG_FUNCTION_INFO_V1(FN(minus_scalar));
PG_FUNCTION_INFO_V1(FN(scalar_mult));
PG_FUNCTION_INFO_V1(FN(mult_scalar));
PG_FUNCTION_INFO_V1(FN(scalar_div));
PG_FUNCTION_INFO_V1(FN(div_scalar));

Datum
FN(scalar) (PG_FUNCTION_ARGS)
{
	os_Scalar  *scalar;
	PG_TYPE		val;

	LOGF();
	scalar = new_scalar(GB_TYPE, CurrentMemoryContext, NULL);
	if (!PG_ARGISNULL(0))
	{
		val = PG_GETARG(0);
		OS_CHECK(GrB_Scalar_setElement(
									   scalar->scalar,
									   val),
				 scalar->scalar,
				 "Error setting Scalar element");
	}
	OS_RETURN_SCALAR(scalar);
}

Datum
FN(set_scalar) (PG_FUNCTION_ARGS)
{
	os_Scalar  *scalar;
	PG_TYPE		val;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);
	scalar = OS_GETARG_SCALAR(0);
	val = PG_GETARG(1);
	OS_CHECK(GrB_Scalar_setElement(scalar->scalar, val),
			 scalar->scalar,
			 "Error setting Scalar element");
	OS_RETURN_SCALAR(scalar);
}

/*  SUPPORT_FN(FN(set_scalar_support), linitial); */

Datum
FN(cast_scalar) (PG_FUNCTION_ARGS)
{
	os_Scalar  *scalar;
	PG_TYPE		value;

	LOGF();
	ERRORNULL(0);

	scalar = OS_GETARG_SCALAR(0);
	OS_CHECK(GrB_Scalar_extractElement(
									   &value,
									   scalar->scalar),
			 scalar->scalar,
			 "Error extracting Scalar element");

	PG_RETURN(value);
}

#ifndef NO_SCALAR_MATH
Datum
FN(scalar_plus) (PG_FUNCTION_ARGS)
{
	os_Scalar  *scalar,
			   *result;
	PG_TYPE		left,
				right;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	scalar = OS_GETARG_SCALAR(0);
	right = PG_GETARG(1);

	OS_CHECK(GrB_Scalar_extractElement(
									   &left,
									   scalar->scalar),
			 scalar->scalar,
			 "Error extracting Scalar element");

	result = new_scalar(GB_TYPE, CurrentMemoryContext, NULL);
	OS_CHECK(GrB_Scalar_setElement(
								   result->scalar,
								   left + right),
			 result->scalar,
			 "Error setting Scalar element");

	OS_RETURN_SCALAR(result);
}

Datum
FN(plus_scalar) (PG_FUNCTION_ARGS)
{
	os_Scalar  *scalar;
	PG_TYPE		left,
				right;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	left = PG_GETARG(0);
	scalar = OS_GETARG_SCALAR(1);

	OS_CHECK(GrB_Scalar_extractElement(
									   &right,
									   scalar->scalar),
			 scalar->scalar,
			 "Error extracting Scalar element");

	PG_RETURN(left + right);
}

Datum
FN(scalar_minus) (PG_FUNCTION_ARGS)
{
	os_Scalar  *scalar,
			   *result;
	PG_TYPE		left,
				right;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	scalar = OS_GETARG_SCALAR(0);
	right = PG_GETARG(1);

	OS_CHECK(GrB_Scalar_extractElement(
									   &left,
									   scalar->scalar),
			 scalar->scalar,
			 "Error extracting Scalar element");

	result = new_scalar(GB_TYPE, CurrentMemoryContext, NULL);
	OS_CHECK(GrB_Scalar_setElement(
								   result->scalar,
								   left - right),
			 result->scalar,
			 "Error setting Scalar element");

	OS_RETURN_SCALAR(result);
}

Datum
FN(minus_scalar) (PG_FUNCTION_ARGS)
{
	os_Scalar  *scalar;
	PG_TYPE		left,
				right;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	left = PG_GETARG(0);
	scalar = OS_GETARG_SCALAR(1);

	OS_CHECK(GrB_Scalar_extractElement(
									   &right,
									   scalar->scalar),
			 scalar->scalar,
			 "Error extracting Scalar element");

	PG_RETURN(left - right);
}

Datum
FN(scalar_mult) (PG_FUNCTION_ARGS)
{
	os_Scalar  *scalar,
			   *result;
	PG_TYPE		left,
				right;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	scalar = OS_GETARG_SCALAR(0);
	right = PG_GETARG(1);

	OS_CHECK(GrB_Scalar_extractElement(
									   &left,
									   scalar->scalar),
			 scalar->scalar,
			 "Error extracting Scalar element");

	result = new_scalar(GB_TYPE, CurrentMemoryContext, NULL);
	OS_CHECK(GrB_Scalar_setElement(
								   result->scalar,
								   left * right),
			 result->scalar,
			 "Error setting Scalar element");

	OS_RETURN_SCALAR(result);
}

Datum
FN(mult_scalar) (PG_FUNCTION_ARGS)
{
	os_Scalar  *scalar;
	PG_TYPE		left,
				right;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	left = PG_GETARG(0);
	scalar = OS_GETARG_SCALAR(1);

	OS_CHECK(GrB_Scalar_extractElement(
									   &right,
									   scalar->scalar),
			 scalar->scalar,
			 "Error extracting Scalar element");

	PG_RETURN(left * right);
}

Datum
FN(scalar_div) (PG_FUNCTION_ARGS)
{
	os_Scalar  *scalar,
			   *result;
	PG_TYPE		left,
				right;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	scalar = OS_GETARG_SCALAR(0);
	right = PG_GETARG(1);

	OS_CHECK(GrB_Scalar_extractElement(
									   &left,
									   scalar->scalar),
			 scalar->scalar,
			 "Error extracting Scalar element");

	result = new_scalar(GB_TYPE, CurrentMemoryContext, NULL);
	OS_CHECK(GrB_Scalar_setElement(
								   result->scalar,
								   left / right),
			 result->scalar,
			 "Error setting Scalar element");

	OS_RETURN_SCALAR(result);
}

Datum
FN(div_scalar) (PG_FUNCTION_ARGS)
{
	os_Scalar  *scalar;
	PG_TYPE		left,
				right;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	left = PG_GETARG(0);
	scalar = OS_GETARG_SCALAR(1);

	OS_CHECK(GrB_Scalar_extractElement(
									   &right,
									   scalar->scalar),
			 scalar->scalar,
			 "Error extracting Scalar element");

	PG_RETURN(left / right);
}
#endif

#undef SUFFIX
#undef PG_TYPE
#undef GB_TYPE
#undef PG_GETARG
#undef PG_RETURN
#undef NO_SCALAR_MATH
