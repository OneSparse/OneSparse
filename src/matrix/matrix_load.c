#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_load);
Datum
matrix_load(PG_FUNCTION_ARGS)
{
	Oid			lo_oid = PG_GETARG_OID(0);
	Oid			lo_func_oid;
	int			fd;
	int64		size;
	char	   *buffer;
	GrB_Matrix	matrix;
	os_Matrix  *result;
	struct timeval start,
				end;
	Oid			lo_open_argtypes[2] = {OIDOID, INT4OID};
	Oid			lo_seek_argtypes[3] = {INT4OID, INT4OID, INT4OID};
	Oid			lo_close_argtypes[1] = {INT4OID};


	OS_START_BENCH();
	lo_func_oid = LookupFuncName(list_make1(makeString("lo_open")), 2, lo_open_argtypes, false);
	fd = DatumGetInt32(OidFunctionCall2(lo_func_oid, ObjectIdGetDatum(lo_oid), Int32GetDatum(INV_READ)));

	lo_func_oid = LookupFuncName(list_make1(makeString("lo_lseek")), 3, lo_seek_argtypes, false);
	size = DatumGetInt32(OidFunctionCall3(lo_func_oid, Int32GetDatum(fd), Int32GetDatum(0), Int32GetDatum(SEEK_END)));

	(void) OidFunctionCall3(lo_func_oid, Int32GetDatum(fd), Int32GetDatum(0), Int32GetDatum(SEEK_SET));

	buffer = palloc(1);
	buffer = repalloc_huge(buffer, size);
	if (lo_read(fd, buffer, size) != size)
		elog(ERROR, "lo_read: read size mismatch");

	OS_CHECK(GxB_Matrix_deserialize(
									&matrix,
									NULL,
									buffer,
									size,
									NULL),
			 matrix,
			 "Error deserializing matrix");

	lo_func_oid = LookupFuncName(list_make1(makeString("lo_close")), 1, lo_close_argtypes, false);
	(void) OidFunctionCall1(lo_func_oid, Int32GetDatum(fd));
	OS_END_BENCH();

	result = new_matrix(NULL, 0, 0, CurrentMemoryContext, matrix);
	OS_RETURN_MATRIX(result);
}
