#ifndef ONESPARSE_H
#define ONESPARSE_H

#include "postgres.h"
#include "utils/builtins.h"
#include "libpq/pqformat.h"
#include "funcapi.h"
#include "access/htup_details.h"
#include "utils/array.h"
#include "utils/arrayaccess.h"
#include "catalog/pg_type_d.h"
#include "catalog/pg_type.h"
#include "utils/lsyscache.h"
#include "nodes/pg_list.h"
#include "utils/varlena.h"
#include "suitesparse/GraphBLAS.h"

#define ERRORIF(cond, msg) if (cond) ereport(ERROR, (errmsg(msg)))
#define LOGF() elog(INFO, __func__)

void *malloc_function(size_t);
void *calloc_function(size_t, size_t);
void *realloc_function(void*, size_t);
void free_function(void*);

void _PG_init(void);

#endif /* ONESPARSE_H */

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
