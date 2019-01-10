#include "pggraphblas.h"
PG_MODULE_MAGIC;

#include "matrix.c"
#include "vector.c"

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
