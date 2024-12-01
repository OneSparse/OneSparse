#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_in);
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
		else if (typ == GrB_UINT64) {
			uint64_t num = strtoull(number_token, &endptr, 10);
			if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid UINT64 %s", number_token);
			OS_CHECK(GrB_Matrix_setElement(matrix->matrix, num, row, col),
				  matrix->matrix,
				  "Error setting Matrix Element");
		}
		else if (typ == GrB_INT32) {
			int32_t num = strtol(number_token, &endptr, 10);
			if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid INT32 %s", number_token);
			OS_CHECK(GrB_Matrix_setElement(matrix->matrix, num, row, col),
				  matrix->matrix,
				  "Error setting Matrix Element");
		}
		else if (typ == GrB_UINT32) {
			uint32_t num = strtoul(number_token, &endptr, 10);
			if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid UINT32 %s", number_token);
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
		else if (typ == GrB_UINT16) {
			unsigned int num = (unsigned int)strtol(number_token, &endptr, 10);
			if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid UINT16 %s", number_token);
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

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
