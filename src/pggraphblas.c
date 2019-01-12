#include "pggraphblas.h"
PG_MODULE_MAGIC;

/* From contrib/intarry */
/* Create a new int array with room for "num" elements */
ArrayType *
new_int64ArrayType(int num)
{
	ArrayType  *r;
	int			nbytes;

	/* if no elements, return a zero-dimensional array */
	if (num <= 0)
	{
		Assert(num == 0);
		r = construct_empty_array(INT8OID);
		return r;
	}

	nbytes = ARR_OVERHEAD_NONULLS(1) + sizeof(int64) * num;

	r = (ArrayType *) palloc0(nbytes);

	SET_VARSIZE(r, nbytes);
	ARR_NDIM(r) = 1;
	r->dataoffset = 0;			/* marker for no null bitmap */
	ARR_ELEMTYPE(r) = INT8OID;
	ARR_DIMS(r)[0] = num;
	ARR_LBOUND(r)[0] = 1;

	return r;
}

#include "vector.c"
#include "matrix.c"

void
_PG_init(void)
{
  GrB_Info info;
  info = GrB_init (GrB_BLOCKING);
  if (! (info == GrB_SUCCESS || info == GrB_NO_VALUE))
    {
      elog(ERROR, "%s", GrB_error());
      return;
    }
}
