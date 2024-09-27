#ifndef ONESPARSE_H
#define ONESPARSE_H

#include <string.h>
#include <errno.h>
#include "postgres.h"
#include "common/fe_memutils.h"
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
#include "common/hashfn.h"
#include "suitesparse/GraphBLAS.h"

#undef ONESPARSE_DEBUG

#define CCAT2(x, y) x ## y
#define CCAT(x, y) CCAT2(x, y)
#define FN(x) CCAT(x, SUFFIX)

#define ERRORNULL(_arg) \
	do { \
	if (PG_ARGISNULL(_arg)) elog(ERROR, "Cannot pass NULL to %s", __func__); \
	} while (0) \

#define CHECK(method, obj, msg)                               \
    do {                                                      \
        GrB_Info __info = method ;                            \
        if (! (__info == GrB_SUCCESS || __info == GrB_NO_VALUE))  \
            {                                                 \
				const char *ename = error_name(__info);         \
                const char *emsg;                             \
                GrB_error(&emsg, obj);                        \
                elog(ERROR, "%s %s: %s", ename, emsg, msg);   \
            }                                                 \
    } while (0)                                               \


#ifdef ONESPARSE_DEBUG
#define LOGF() elog(INFO, __func__)
#else
#define LOGF()
#endif

char* short_code(GrB_Type_Code code);
GrB_Type short_type(char *name);
const char* error_name(GrB_Info info);

void *malloc_function(size_t);
void *calloc_function(size_t, size_t);
void *realloc_function(void*, size_t);
void free_function(void*);

void _PG_init(void);

#include "descriptor.h"
#include "unaryop.h"
#include "indexunaryop.h"
#include "binaryop.h"
#include "monoid.h"
#include "semiring.h"
#include "scalar.h"
#include "vector.h"
#include "matrix.h"

#endif /* ONESPARSE_H */

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
