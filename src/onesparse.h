#ifndef OS_H
#define OS_H

#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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

#undef OS_DEBUG

#define CCAT2(x, y) x ## y
#define CCAT(x, y) CCAT2(x, y)
#define FN(x) CCAT(x, SUFFIX)

#define ERRORNULL(_arg) \
	do { \
	if (PG_ARGISNULL(_arg)) elog(ERROR, "Cannot pass NULL to %s", __func__); \
	} while (0) \

#define OS_CHECK(method, obj, msg)                               \
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

#define OS_MTYPE(_mtype, _m)      \
	OS_CHECK(GxB_Matrix_type(&_mtype, _m->matrix),\
				  _m->matrix,\
				  "Cannot get matrix type");

#define OS_MNROWS(_nrows, _m) \
	OS_CHECK(GrB_Matrix_nrows(&_nrows, _m->matrix), \
		  _m->matrix, \
		  "Error extracting matrix nrows.");

#define OS_MNCOLS(_ncols, _m) \
	OS_CHECK(GrB_Matrix_ncols(&_ncols, _m->matrix),\
		  _m->matrix,\
		  "Error extracting matrix ncols.");

#define OS_MNVALS(_nvals, _m) \
	OS_CHECK(GrB_Matrix_nvals(&_nvals, _m->matrix),\
		  _m->matrix,\
		  "Error extracting matrix nvals.");

#define OS_VTYPE(_vtype, _v)      \
	OS_CHECK(GxB_Vector_type(&_vtype, _v->vector),\
				  _v->vector,\
				  "Cannot get vector type");

#define OS_VNVALS(_nvals, _v)\
	OS_CHECK(GrB_Vector_nvals(&_nvals, _v->vector),\
			 _v->vector,\
			 "Error extracting vector nvals.");

#define OS_VSIZE(_size, _v)\
	OS_CHECK(GrB_Vector_size(&_size, _v->vector), \
		  _v->vector, \
		  "Cannot get matrix type");

#define OS_STYPE(_stype, _s)      \
	OS_CHECK(GxB_Scalar_type(&_stype, _s->scalar),\
				  _s->scalar,\
				  "Cannot get scalar type");

#define OS_SNVALS(_nvals, _s)\
	OS_CHECK(GrB_Scalar_nvals(&_nvals, _s->scalar),\
			 _s->scalar,\
			 "Error extracting scalar nvals.");


#ifdef OS_DEBUG
#define LOGF() elog(INFO, __func__)
#else
#define LOGF()
#endif

char* short_code(GrB_Type_Code code);
GrB_Type short_type(char *name);
const char* error_name(GrB_Info info);
GrB_Type type_promote(GrB_Type left, GrB_Type right);

GrB_BinaryOp default_binaryop(GrB_Type type);
GrB_Monoid default_monoid(GrB_Type type);
GrB_Semiring default_semiring(GrB_Type type);

void *malloc_function(size_t);
void *calloc_function(size_t, size_t);
void *realloc_function(void*, size_t);
void free_function(void*);

void initialize_types(void);
void initialize_descriptors();
void initialize_unaryops();
void initialize_indexunaryops();
void initialize_binaryops();
void initialize_monoids();
void initialize_semirings();

GrB_Type lookup_type(char *name);

void _PG_init(void);

#include "type.h"
#include "descriptor.h"
#include "unaryop.h"
#include "indexunaryop.h"
#include "binaryop.h"
#include "monoid.h"
#include "semiring.h"
#include "scalar.h"
#include "vector.h"
#include "matrix.h"

#endif /* OS_H */

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
