#include "../onesparse.h"

PG_FUNCTION_INFO_V1(vector_lsh);
Datum vector_lsh(PG_FUNCTION_ARGS)
{
	os_Vector *q;
    GrB_Index size, nvals;
    uint64_t h;
    GrB_Index i;

	ERRORNULL(0);

	q = OS_GETARG_VECTOR(0);

	OS_VSIZE(size, q);

    if (size > 60)
    {
        elog(ERROR, "Vector cannot be larger than 60.");
    }

	OS_VNVALS(nvals, q);

    if (size != nvals)
    {
        elog(ERROR, "Vector must be dense.");
    }

    h = 0;
    for (i = 0; i < size; i++)
    {
        double val;

        OS_CHECK(GrB_Vector_extractElement(&val, q->vector, i),
				 q->vector,
				 "Cannot extract hash element value");
        if (val >= 0.0f)
        {
            h |= (1ULL << i);
        }
    }

	PG_RETURN_INT64(h);
}
