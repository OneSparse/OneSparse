#include "onesparse.h"

void *calloc_function(size_t num, size_t size) {
  MemoryContext oldcxt;
  void *p;
  size_t total;
  total = num * size;
  oldcxt = MemoryContextSwitchTo(TopMemoryContext);
  p = palloc_extended(total, MCXT_ALLOC_HUGE | MCXT_ALLOC_ZERO);
  MemoryContextSwitchTo(oldcxt);
  return p;
}

void *malloc_function(size_t size) {
  MemoryContext oldcxt;
  void *p;
  oldcxt = MemoryContextSwitchTo(TopMemoryContext);

  p = palloc_extended(size, MCXT_ALLOC_HUGE);
  MemoryContextSwitchTo(oldcxt);
  return p;
}

void *realloc_function(void *p, size_t size) {
  MemoryContext oldcxt;
  oldcxt = MemoryContextSwitchTo(TopMemoryContext);
  p = repalloc_huge(p, size);
  MemoryContextSwitchTo(oldcxt);
  return p;
}

void free_function(void *p) {
  //  MemoryContext oldcxt;
  //  oldcxt = MemoryContextSwitchTo(TopMemoryContext);
  pfree(p);
  //  MemoryContextSwitchTo(oldcxt);
}

void _PG_init(void)
{
	ERRORIF(GxB_init(GrB_NONBLOCKING,
					 &malloc_function,
					 &calloc_function,
					 &realloc_function,
					 &free_function) != GrB_SUCCESS,
			"Cannot initialize GraphBLAS");
	/* ERRORIF(GrB_init(GrB_NONBLOCKING) != GrB_SUCCESS, */
	/* 		"Cannot initialize GraphBLAS"); */
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
