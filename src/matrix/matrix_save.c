#include "../onesparse.h"

PG_FUNCTION_INFO_V1(matrix_save);
Datum matrix_save(PG_FUNCTION_ARGS) {
    os_Matrix *matrix;
    void *data;
    size_t size;
    Oid lo_oid, lo_func_oid;
    int fd;
    struct timeval start, end;

	OS_START_BENCH();
    matrix = OS_GETARG_MATRIX(0);
    lo_oid = PG_GETARG_OID(1);

    if (lo_oid == InvalidOid) {
        lo_func_oid = LookupFuncName(list_make1(makeString("lo_create")), 1, (Oid[]){OIDOID}, false);
        lo_oid = DatumGetObjectId(OidFunctionCall1(lo_func_oid, ObjectIdGetDatum(InvalidOid)));
    }

    lo_func_oid = LookupFuncName(list_make1(makeString("lo_open")), 2, (Oid[]){OIDOID, INT4OID}, false);
    fd = DatumGetInt32(OidFunctionCall2(lo_func_oid, ObjectIdGetDatum(lo_oid), Int32GetDatum(INV_WRITE)));

    OS_CHECK(GrB_wait(matrix->matrix, GrB_MATERIALIZE),
             matrix->matrix,
             "Error waiting to materialize matrix.");

    OS_CHECK(GxB_Matrix_serialize(&data, &size, matrix->matrix, NULL),
             matrix->matrix,
             "Error serializing matrix");

    lo_write(fd, data, size);

    lo_func_oid = LookupFuncName(list_make1(makeString("lo_close")), 1, (Oid[]){INT4OID}, false);
    OidFunctionCall1(lo_func_oid, Int32GetDatum(fd));

	OS_END_BENCH();
    PG_RETURN_OID(lo_oid);
}

