#include "pggraphblas.h"
PG_MODULE_MAGIC;

#define HUGE_ALLOC

pgGrB_Semiring semirings[960];
pgGrB_BinaryOp binops[256];
pgGrB_UnaryOp uops[67];
MemoryContext gb_context;

#include "type.c"
#include "vector.c"
#include "matrix.c"

void *calloc_function(size_t num, size_t size) {
  MemoryContext oldcxt;
  void *p;
  size_t total;
  total = num * size;
  oldcxt = MemoryContextSwitchTo(TopMemoryContext);
#ifdef HUGE_ALLOC
  p = palloc_extended(total, MCXT_ALLOC_HUGE | MCXT_ALLOC_ZERO);
#else
  p = palloc0(total);
#endif
  MemoryContextSwitchTo(oldcxt);
  return p;
}

void *malloc_function(size_t size) {
  MemoryContext oldcxt;
  void *p;
  oldcxt = MemoryContextSwitchTo(TopMemoryContext);

#ifdef HUGE_ALLOC
  p = palloc_extended(size, MCXT_ALLOC_HUGE);
#else
  p = palloc(size);
#endif
  MemoryContextSwitchTo(oldcxt);
  return p;
}

void *realloc_function(void *p, size_t size) {
  MemoryContext oldcxt;
  oldcxt = MemoryContextSwitchTo(TopMemoryContext);
#ifdef HUGE_ALLOC
  p = repalloc_huge(p, size);
#else
  p = repalloc(p, size);
#endif
  MemoryContextSwitchTo(oldcxt);
  return p;
}

void free_function(void *p) {
  //  MemoryContext oldcxt;
  //  oldcxt = MemoryContextSwitchTo(TopMemoryContext);
  pfree(p);
  //  MemoryContextSwitchTo(oldcxt);
}

void
_PG_init(void)
{
  GrB_Info info;

#ifdef IMPORT_EXPORT
  info = LAGraph_xinit (malloc_function,
                        calloc_function,
                        realloc_function,
                        free_function,
                        false);
#else
  info = LAGraph_init (GrB_BLOCKING);
#endif

  if (! (info == GrB_SUCCESS || info == GrB_NO_VALUE))  {
    elog(ERROR, "%s", GrB_error());
    return;
  }
  
  SEMIRING_GROUP(0, FIRST_);
  SEMIRING_GROUP(40, SECOND_);
  SEMIRING_GROUP(80, MIN_);
  SEMIRING_GROUP(120, MAX_);
  SEMIRING_GROUP(160, PLUS_);
  SEMIRING_GROUP(200, MINUS_);
  SEMIRING_GROUP(240, TIMES_);
  SEMIRING_GROUP(280, DIV_);
  SEMIRING_GROUP(320, ISEQ_);
  SEMIRING_GROUP(360, ISNE_);
  SEMIRING_GROUP(400, ISGT_);
  SEMIRING_GROUP(440, ISLT_);
  SEMIRING_GROUP(480, ISGE_);
  SEMIRING_GROUP(520, ISLE_);
  SEMIRING_GROUP(560, LOR_);
  SEMIRING_GROUP(600, LAND_);
  SEMIRING_GROUP(640, LXOR_);
  SEMIRING_BOOL_GROUP(680, EQ_);
  SEMIRING_BOOL_GROUP(720, NE_);
  SEMIRING_BOOL_GROUP(760, GT_);
  SEMIRING_BOOL_GROUP(800, LT_);
  SEMIRING_BOOL_GROUP(840, GE_);
  SEMIRING_BOOL_GROUP(880, LE_);
  SEMIRING_PURE_BOOL_GROUP(920);

  BINOP_TYPES(0, GrB_, FIRST_);
  BINOP_TYPES(11, GrB_, SECOND_);
  BINOP_TYPES(22, GrB_, MIN_);
  BINOP_TYPES(33, GrB_, MAX_);
  BINOP_TYPES(44, GrB_, PLUS_);
  BINOP_TYPES(55, GrB_, MINUS_);
  BINOP_TYPES(66, GrB_, TIMES_);
  BINOP_TYPES(77, GrB_, DIV_);

  BINOP_TYPES(88, GxB_, ISEQ_);
  BINOP_TYPES(99, GxB_, ISNE_);
  BINOP_TYPES(110, GxB_, ISGT_);
  BINOP_TYPES(121, GxB_, ISLT_);
  BINOP_TYPES(132, GxB_, ISGE_);
  BINOP_TYPES(143, GxB_, ISLE_);

  BINOP_TYPES(154, GrB_, EQ_);
  BINOP_TYPES(165, GrB_, NE_);
  BINOP_TYPES(176, GrB_, GT_);
  BINOP_TYPES(187, GrB_, LT_);
  BINOP_TYPES(198, GrB_, GE_);
  BINOP_TYPES(209, GrB_, LE_);

  BINOP_TYPES(220, GxB_, LOR_);
  BINOP_TYPES(231, GxB_, LAND_);
  BINOP_TYPES(242, GxB_, LXOR_);

  BINOP(254, GrB_, LOR,);
  BINOP(255, GrB_, LAND,);
  BINOP(256, GrB_, LXOR,);

  UOP_TYPES(0, GrB_, IDENTITY_);
  UOP_TYPES(11, GrB_, AINV_);
  UOP_TYPES(22, GrB_, MINV_);
  UOP_TYPES(33, GxB_, LNOT_);
  UOP_TYPES(44, GxB_, ONE_);
  UOP_TYPES(55, GxB_, ABS_);
  UOP(67, GrB_, LNOT,);
}
