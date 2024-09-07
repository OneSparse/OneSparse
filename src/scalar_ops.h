
PG_FUNCTION_INFO_V1(FN(scalar));
PG_FUNCTION_INFO_V1(FN(cast_scalar));

PG_FUNCTION_INFO_V1(FN(scalar_plus));
PG_FUNCTION_INFO_V1(FN(plus_scalar));
PG_FUNCTION_INFO_V1(FN(scalar_minus));
PG_FUNCTION_INFO_V1(FN(minus_scalar));
PG_FUNCTION_INFO_V1(FN(scalar_mult));
PG_FUNCTION_INFO_V1(FN(mult_scalar));
PG_FUNCTION_INFO_V1(FN(scalar_div));
PG_FUNCTION_INFO_V1(FN(div_scalar));

Datum FN(scalar)(PG_FUNCTION_ARGS)
{
	onesparse_Scalar* scalar;
	PG_TYPE val;
	LOGF();
	scalar = new_scalar(GB_TYPE, CurrentMemoryContext, NULL);
	if (!PG_ARGISNULL(0))
	{
		val = PG_GETARG(0);
		ERRORIF(GrB_Scalar_setElement(
					scalar->scalar,
					val),
				"Error setting Scalar element");
	}
	ONESPARSE_RETURN_SCALAR(scalar);
}

Datum FN(cast_scalar)(PG_FUNCTION_ARGS)
{
	onesparse_Scalar* scalar;
	PG_TYPE value;

	LOGF();
	ERRORNULL(0);

	scalar = ONESPARSE_GETARG_SCALAR(0);
	ERRORIF(GrB_Scalar_extractElement(
				&value,
				scalar->scalar),
			"Error extracting Scalar element");

	PG_RETURN(value);
}

Datum FN(scalar_plus)(PG_FUNCTION_ARGS)
{
	onesparse_Scalar *scalar, *result;
	PG_TYPE left, right;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	scalar = ONESPARSE_GETARG_SCALAR(0);
	right = PG_GETARG(1);

	ERRORIF(GrB_Scalar_extractElement(
				&left,
				scalar->scalar),
			"Error extracting Scalar element");

	result = new_scalar(GB_TYPE, CurrentMemoryContext, NULL);
	ERRORIF(GrB_Scalar_setElement(
				result->scalar,
				left + right),
			"Error setting Scalar element");

	ONESPARSE_RETURN_SCALAR(result);
}

Datum FN(plus_scalar)(PG_FUNCTION_ARGS)
{
	onesparse_Scalar *scalar;
	PG_TYPE left, right;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	left = PG_GETARG(0);
	scalar = ONESPARSE_GETARG_SCALAR(1);

	ERRORIF(GrB_Scalar_extractElement(
				&right,
				scalar->scalar),
			"Error extracting Scalar element");

	PG_RETURN(left + right);
}

Datum FN(scalar_minus)(PG_FUNCTION_ARGS)
{
	onesparse_Scalar *scalar, *result;
	PG_TYPE left, right;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	scalar = ONESPARSE_GETARG_SCALAR(0);
	right = PG_GETARG(1);

	ERRORIF(GrB_Scalar_extractElement(
				&left,
				scalar->scalar),
			"Error extracting Scalar element");

	result = new_scalar(GB_TYPE, CurrentMemoryContext, NULL);
	ERRORIF(GrB_Scalar_setElement(
				result->scalar,
				left - right),
			"Error setting Scalar element");

	ONESPARSE_RETURN_SCALAR(result);
}

Datum FN(minus_scalar)(PG_FUNCTION_ARGS)
{
	onesparse_Scalar *scalar;
	PG_TYPE left, right;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	left = PG_GETARG(0);
	scalar = ONESPARSE_GETARG_SCALAR(1);

	ERRORIF(GrB_Scalar_extractElement(
				&right,
				scalar->scalar),
			"Error extracting Scalar element");

	PG_RETURN(left - right);
}

Datum FN(scalar_mult)(PG_FUNCTION_ARGS)
{
	onesparse_Scalar *scalar, *result;
	PG_TYPE left, right;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	scalar = ONESPARSE_GETARG_SCALAR(0);
	right = PG_GETARG(1);

	ERRORIF(GrB_Scalar_extractElement(
				&left,
				scalar->scalar),
			"Error extracting Scalar element");

	result = new_scalar(GB_TYPE, CurrentMemoryContext, NULL);
	ERRORIF(GrB_Scalar_setElement(
				result->scalar,
				left * right),
			"Error setting Scalar element");

	ONESPARSE_RETURN_SCALAR(result);
}

Datum FN(mult_scalar)(PG_FUNCTION_ARGS)
{
	onesparse_Scalar *scalar;
	PG_TYPE left, right;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	left = PG_GETARG(0);
	scalar = ONESPARSE_GETARG_SCALAR(1);

	ERRORIF(GrB_Scalar_extractElement(
				&right,
				scalar->scalar),
			"Error extracting Scalar element");

	PG_RETURN(left * right);
}

Datum FN(scalar_div)(PG_FUNCTION_ARGS)
{
	onesparse_Scalar *scalar, *result;
	PG_TYPE left, right;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	scalar = ONESPARSE_GETARG_SCALAR(0);
	right = PG_GETARG(1);

	ERRORIF(GrB_Scalar_extractElement(
				&left,
				scalar->scalar),
			"Error extracting Scalar element");

	result = new_scalar(GB_TYPE, CurrentMemoryContext, NULL);
	ERRORIF(GrB_Scalar_setElement(
				result->scalar,
				left / right),
			"Error setting Scalar element");

	ONESPARSE_RETURN_SCALAR(result);
}

Datum FN(div_scalar)(PG_FUNCTION_ARGS)
{
	onesparse_Scalar *scalar;
	PG_TYPE left, right;

	LOGF();
	ERRORNULL(0);
	ERRORNULL(1);

	left = PG_GETARG(0);
	scalar = ONESPARSE_GETARG_SCALAR(1);

	ERRORIF(GrB_Scalar_extractElement(
				&right,
				scalar->scalar),
			"Error extracting Scalar element");

	PG_RETURN(left / right);
}

#undef SUFFIX
#undef PG_TYPE
#undef GB_TYPE
#undef PG_GETARG
#undef PG_RETURN
