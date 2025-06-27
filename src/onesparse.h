#ifndef OS_H
#define OS_H

#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "postgres.h"
#include "fmgr.h"
#include "miscadmin.h"
#include "common/fe_memutils.h"
#include "libpq/pqformat.h"
#include "libpq/libpq-fs.h"
#include "libpq/be-fsstubs.h"
#include "funcapi.h"
#include "access/htup_details.h"
#include "utils/builtins.h"
#include "utils/memutils.h"
#include "utils/array.h"
#include "utils/arrayaccess.h"
#include "utils/guc.h"
#include "utils/varlena.h"
#include "catalog/pg_type_d.h"
#include "catalog/pg_type.h"
#include "parser/parse_func.h"
#include "utils/lsyscache.h"
#include "nodes/pg_list.h"
#include "nodes/supportnodes.h"
#include "common/hashfn.h"
#include <GraphBLAS.h>
#include <LAGraph.h>
#include <LAGraphX.h>
#include "access/xact.h"

#define OS_DEBUG

#define CCAT2(x, y) x ## y
#define CCAT(x, y) CCAT2(x, y)
#define FN(x) CCAT(x, SUFFIX)

#define ERRORIF(B, msg)                                                        \
    if ((B))                                                                   \
        ereport(ERROR, (errcode(ERRCODE_DATA_EXCEPTION), errmsg(msg, __func__)))

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

#define LA_CHECK(method)                            \
    do {                                                      \
		if (method != GrB_SUCCESS && (method < 1000 || method >= 2000)) \
		{                                                 \
			elog(ERROR, "%i: %s", method, msg);   \
		}                                                 \
    } while (0)                                               \

#define OK_CHECK(method, msg)                               \
    do {                                                      \
        GrB_Info __info = method ;                            \
        if (__info != GrB_SUCCESS)  \
            {                                                 \
				const char *ename = error_name(__info);         \
                elog(ERROR, "%s: %s", ename, msg);   \
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

#define SUPPORT_FN(name, getter)\
	PG_FUNCTION_INFO_V1(CCAT(name, _support));\
	Datum CCAT(name, _support)(PG_FUNCTION_ARGS)\
	{\
	Node *rawreq = (Node *) PG_GETARG_POINTER(0);\
	Node *ret = NULL;\
	if (IsA(rawreq, SupportRequestModifyInPlace))\
	{\
	SupportRequestModifyInPlace *req = (SupportRequestModifyInPlace *) rawreq;\
	Param *arg = (Param *) getter(req->args);\
	\
	if (arg && IsA(arg, Param) &&\
		arg->paramkind == PARAM_EXTERN &&\
		arg->paramid == req->paramid)\
		ret = (Node *) arg;\
	}\
	PG_RETURN_POINTER(ret);\
	}

#ifdef OS_DEBUG
#define LOGF() elog(DEBUG1, __func__)
#define OS_START_BENCH()                        \
	ereport(DEBUG1, (errmsg("%s()", __func__))); \
    gettimeofday(&start, NULL);

#define OS_END_BENCH()                                                      \
    gettimeofday(&end, NULL);                                               \
    do {                                                                    \
        double elapsed = (end.tv_sec - start.tv_sec) +                      \
                         (end.tv_usec - start.tv_usec) / 1000000.0;         \
        ereport(DEBUG1, (errmsg("%s() took %.6f seconds", __func__, elapsed))); \
    } while (0)
#else
#define LOGF()
#define OS_START_BENCH()
#define OS_END_BENCH()
#endif

uint64_t* get_c_array_from_pg_array(FunctionCallInfo fcinfo, int arg_number, uint64_t *out_nelems);
char* short_code(GrB_Type_Code code);
GrB_Type code_type(GrB_Type_Code code);
GrB_Type short_type(char *name);
size_t code_size(GrB_Type_Code code);
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
void initialize_descriptors(void);
void initialize_unaryops(void);
void initialize_indexunaryops(void);
void initialize_binaryops(void);
void initialize_monoids(void);
void initialize_semirings(void);

GrB_Type lookup_type(char *name);
GrB_Descriptor lookup_descriptor(char *name);
GrB_UnaryOp lookup_unaryop(char *name);
GrB_IndexUnaryOp lookup_indexunaryop(char *name);
GrB_BinaryOp lookup_binaryop(char *name);
GrB_Monoid lookup_monoid(char *name);
GrB_Semiring lookup_semiring(char *name);

void burble_notice_func(const char *fmt, ...);

void _PG_init(void);

#include "type/type.h"
#include "descriptor/descriptor.h"
#include "unaryop/unaryop.h"
#include "indexunaryop/indexunaryop.h"
#include "binaryop/binaryop.h"
#include "monoid/monoid.h"
#include "semiring/semiring.h"
#include "scalar/scalar.h"
#include "vector/vector.h"
#include "matrix/matrix.h"
#include "graph/graph.h"

#endif /* OS_H */

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
