
void initialize_semirings();
void initialize_binaryops();
void initialize_monoids();
void initialize_unaryops();
void initialize_indexunaryops();
void initialize_descriptors();

#include "onesparse.h"
PG_MODULE_MAGIC;

char* short_code(GrB_Type_Code code)
{
	switch(code)
	{
		case GrB_INT64_CODE:
			return "i8";
		case GrB_INT32_CODE:
			return "i4";
		case GrB_INT16_CODE:
			return "i2";
		case GrB_FP64_CODE:
			return "f8";
		case GrB_FP32_CODE:
			return "f4";
		case GrB_BOOL_CODE:
			return "b";
	}
	return "?";
}

GrB_Type short_type(char *name)
{
	if (strcmp(name, "i8") == 0)
		return GrB_INT64;
	if (strcmp(name, "i4") == 0)
		return GrB_INT32;
	if (strcmp(name, "i2") == 0)
		return GrB_INT16;
	if (strcmp(name, "f8") == 0)
		return GrB_FP64;
	if (strcmp(name, "f4") == 0)
		return GrB_FP32;
	if (strcmp(name, "b") == 0)
		return GrB_BOOL;
	elog(ERROR, "Unknown short name %s", name);
}

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
	ERRORIF(GrB_init(GrB_NONBLOCKING) != GrB_SUCCESS,
			"Cannot initialize GraphBLAS");

	/* ERRORIF(GxB_init(GrB_NONBLOCKING, */
	/* 				 &malloc_function, */
	/* 				 &calloc_function, */
	/* 				 &realloc_function, */
	/* 				 &free_function) != GrB_SUCCESS, */
	/* 		"Cannot initialize GraphBLAS"); */

	initialize_semirings();
	initialize_binaryops();
	initialize_monoids();
	initialize_unaryops();
	initialize_indexunaryops();
	initialize_descriptors();
}


/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
