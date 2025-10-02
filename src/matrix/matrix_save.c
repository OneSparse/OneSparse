#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_save);
Datum
matrix_save(PG_FUNCTION_ARGS)
{
	os_Matrix  *matrix;
	void	   *data;
	size_t		size;
	Oid			lo_oid,
				lo_func_oid;
	int			fd;
	struct timeval start,
				end;
	Oid			lo_create_argtypes[1] = {OIDOID};
	Oid			lo_open_argtypes[2] = {OIDOID, INT4OID};
	Oid			lo_close_argtypes[1] = {INT4OID};

	matrix = OS_GETARG_MATRIX(0);
	lo_oid = PG_GETARG_OID(1);

	OS_START_BENCH();

	if (lo_oid == InvalidOid)
	{
		lo_func_oid = LookupFuncName(list_make1(makeString("lo_create")), 1, lo_create_argtypes, false);
		lo_oid = DatumGetObjectId(OidFunctionCall1(lo_func_oid, ObjectIdGetDatum(InvalidOid)));
	}

	lo_func_oid = LookupFuncName(list_make1(makeString("lo_open")), 2, lo_open_argtypes, false);
	fd = DatumGetInt32(OidFunctionCall2(lo_func_oid, ObjectIdGetDatum(lo_oid), Int32GetDatum(INV_WRITE)));

	OS_CHECK(GxB_Matrix_serialize(&data, &size, matrix->matrix, NULL),
			 matrix->matrix,
			 "Error serializing matrix");

	lo_write(fd, data, size);

	lo_func_oid = LookupFuncName(list_make1(makeString("lo_close")), 1, lo_close_argtypes, false);
	OidFunctionCall1(lo_func_oid, Int32GetDatum(fd));

	OS_END_BENCH();
	PG_RETURN_OID(lo_oid);
}
