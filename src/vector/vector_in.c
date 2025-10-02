#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_in);
Datum
vector_in(PG_FUNCTION_ARGS)
{
	os_Vector  *vector;
	char	   *input;
	GrB_Type	typ;
	char	   *short_name;
	char	   *input_copy;
	char	   *token;
	char	   *saveptr;
	char	   *endptr;
	char	   *number_token;
	char	   *number_saveptr;
	int			index = 0;
	GrB_Index	usize;
	int			matched;

	input = PG_GETARG_CSTRING(0);
	input_copy = strdup(input);
	token = strtok_r(input_copy, "[", &saveptr);

	usize = GrB_INDEX_MAX + 1;
	if (token != NULL)
	{
		short_name = palloc(strlen(token) + 1);
		matched = sscanf(token, "%99[^()](%lu)", short_name, &usize);
		if (matched == 1)
		{
			usize = GrB_INDEX_MAX + 1;
		}
		else if (matched != 2)
		{
			elog(ERROR, "Invalid prefix %s", token);
		}
		typ = short_type(short_name);
	}
	else
		elog(ERROR, "Vector parse error, no short type code prefix.");

	vector = new_vector(typ, usize, CurrentMemoryContext, NULL);

	strcpy(input_copy, input);
	strtok_r(input_copy, "[", &saveptr);
	token = strtok_r(NULL, "]", &saveptr);
	if (token == NULL)
	{
		OS_RETURN_VECTOR(vector);
	}

	for (number_token = strtok_r(token, " ", &number_saveptr); number_token != NULL;
		 number_token = strtok_r(NULL, " ", &number_saveptr))
	{
		errno = 0;

		if (sscanf(number_token, "%d:%s", &index, number_token) != 2)
			elog(ERROR, "Invalid vector entry %s", number_token);

		if (typ == GrB_INT64)
		{
			int64_t		num;

			num = strtoll(number_token, &endptr, 10);
			if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid INT64 %s", number_token);
			OS_CHECK(GrB_Vector_setElement(vector->vector, num, index),
					 vector->vector,
					 "Error setting Vector Element");
		}
		else if (typ == GrB_UINT64)
		{
			uint64_t	num;

			num = strtoull(number_token, &endptr, 10);
			if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid UINT64 %s", number_token);
			OS_CHECK(GrB_Vector_setElement(vector->vector, num, index),
					 vector->vector,
					 "Error setting Vector Element");
		}
		else if (typ == GrB_INT32)
		{
			long		num = strtol(number_token, &endptr, 10);

			if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid INT32 %s", number_token);
			OS_CHECK(GrB_Vector_setElement(vector->vector, num, index),
					 vector->vector,
					 "Error setting Vector Element");
		}
		else if (typ == GrB_UINT32)
		{
			unsigned long num = strtoul(number_token, &endptr, 10);

			if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid UINT32 %s", number_token);
			OS_CHECK(GrB_Vector_setElement(vector->vector, num, index),
					 vector->vector,
					 "Error setting Vector Element");
		}
		else if (typ == GrB_INT16)
		{
			int			num = (int) strtol(number_token, &endptr, 10);

			if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid INT16 %s", number_token);
			OS_CHECK(GrB_Vector_setElement(vector->vector, num, index),
					 vector->vector,
					 "Error setting Vector Element");
		}
		else if (typ == GrB_UINT16)
		{
			unsigned int num = (int) strtoul(number_token, &endptr, 10);

			if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid UINT16 %s", number_token);
			OS_CHECK(GrB_Vector_setElement(vector->vector, num, index),
					 vector->vector,
					 "Error setting Vector Element");
		}
		else if (typ == GrB_INT8)
		{
			long		num = strtol(number_token, &endptr, 10);

			if (num < INT8_MIN || num > INT8_MAX)
				elog(ERROR, "Invalid INT8 range %s", number_token);
			if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid INT8 %s", number_token);
			OS_CHECK(GrB_Vector_setElement(vector->vector, num, index),
					 vector->vector,
					 "Error setting Vector Element");
		}
		else if (typ == GrB_UINT8)
		{
			unsigned long num = strtoul(number_token, &endptr, 10);

			if (num < INT8_MIN || num > INT8_MAX)
				elog(ERROR, "Invalid UINT8 range %s", number_token);
			if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid UINT8 %s", number_token);
			OS_CHECK(GrB_Vector_setElement(vector->vector, num, index),
					 vector->vector,
					 "Error setting Vector Element");
		}
		else if (typ == GrB_FP64)
		{
			double		num = strtod(number_token, &endptr);

			if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid FP64 %s", number_token);
			OS_CHECK(GrB_Vector_setElement(vector->vector, num, index),
					 vector->vector,
					 "Error setting Vector Element");
		}
		else if (typ == GrB_FP32)
		{
			float		num = strtof(number_token, &endptr);

			if (errno != 0 || *endptr != '\0')
				elog(ERROR, "Invalid FP32 %s", number_token);
			OS_CHECK(GrB_Vector_setElement(vector->vector, num, index),
					 vector->vector,
					 "Error setting Vector Element");
		}
		else if (typ == GrB_BOOL)
		{
			bool		num;

			if (strcmp(number_token, "t") == 0)
				num = true;
			else if (strcmp(number_token, "f") == 0)
				num = false;
			else
				elog(ERROR, "Invalid BOOL %s", number_token);
			OS_CHECK(GrB_Vector_setElement(vector->vector, num, index),
					 vector->vector,
					 "Error setting Vector Element");
		}
	}
	OS_RETURN_VECTOR(vector);
}
