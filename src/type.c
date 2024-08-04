
GrB_Semiring lookup_semiring(char *name) {
  for (int i = 0; i < 960; i++) {
    if (strcasecmp(semirings[i].name, name) == 0)
        return semirings[i].R;
  }
  return NULL;
}

GrB_BinaryOp lookup_binop(char *name) {
  for (int i = 0; i < 256; i++) {
    if (strcasecmp(binops[i].name, name) == 0)
        return binops[i].B;
  }
  return NULL;
}

GrB_Type mxm_type(pgGrB_Matrix *left, pgGrB_Matrix *right) {
  GrB_Info info;
  GrB_Type atype, btype;
  CHECKD(GxB_Matrix_type(&atype, left->M), left->M);
  CHECKD(GxB_Matrix_type(&btype, right->M), right->M);
  RETURN_TYPECHCK2(atype, btype, FP64);
  RETURN_TYPECHCK2(atype, btype, FP32);
  RETURN_TYPECHCK2(atype, btype, INT64);
  RETURN_TYPECHCK2(atype, btype, INT32);
  RETURN_TYPECHCK2(atype, btype, INT16);
  RETURN_TYPECHCK2(atype, btype, BOOL);
  return NULL;
}

GrB_Type mxv_type(pgGrB_Matrix *left, pgGrB_Vector *right) {
  GrB_Info info;
  GrB_Type atype, btype;
  CHECKD(GxB_Matrix_type(&atype, left->M), left->M);
  CHECKD(GxB_Vector_type(&btype, right->V), right->V);
  RETURN_TYPECHCK2(atype, btype, FP64);
  RETURN_TYPECHCK2(atype, btype, FP32);
  RETURN_TYPECHCK2(atype, btype, INT64);
  RETURN_TYPECHCK2(atype, btype, INT32);
  RETURN_TYPECHCK2(atype, btype, INT16);
  RETURN_TYPECHCK2(atype, btype, BOOL);
  return NULL;
}

GrB_Type vxm_type(pgGrB_Vector *left, pgGrB_Matrix *right) {
  GrB_Info info;
  GrB_Type atype, btype;
  CHECKD(GxB_Vector_type(&atype, left->V), left->V);
  CHECKD(GxB_Matrix_type(&btype, right->M), right->M);
  RETURN_TYPECHCK2(atype, btype, FP64);
  RETURN_TYPECHCK2(atype, btype, FP32);
  RETURN_TYPECHCK2(atype, btype, INT64);
  RETURN_TYPECHCK2(atype, btype, INT32);
  RETURN_TYPECHCK2(atype, btype, INT16);
  RETURN_TYPECHCK2(atype, btype, BOOL);
  return NULL;
}

char* mxm_semiring(pgGrB_Matrix *left, pgGrB_Matrix *right) {
  GrB_Info info;
  GrB_Type atype, btype;
  CHECKD(GxB_Matrix_type(&atype, left->M), left->M);
  CHECKD(GxB_Matrix_type(&btype, right->M), right->M);
  RETURN_TYPECHCK1(atype, btype, PLUS_TIMES, FP64);
  RETURN_TYPECHCK1(atype, btype, PLUS_TIMES, FP32);
  RETURN_TYPECHCK1(atype, btype, PLUS_TIMES, INT64);
  RETURN_TYPECHCK1(atype, btype, PLUS_TIMES, INT32);
  RETURN_TYPECHCK1(atype, btype, PLUS_TIMES, INT16);
  RETURN_TYPECHCK1(atype, btype, LOR_LAND, BOOL);
  return "";
}

char* mxv_semiring(pgGrB_Matrix *left, pgGrB_Vector *right) {
  GrB_Info info;
  GrB_Type atype, btype;
  CHECKD(GxB_Matrix_type(&atype, left->M), left->M);
  CHECKD(GxB_Vector_type(&btype, right->V), right->V);
  RETURN_TYPECHCK1(atype, btype, PLUS_TIMES, FP64);
  RETURN_TYPECHCK1(atype, btype, PLUS_TIMES, FP32);
  RETURN_TYPECHCK1(atype, btype, PLUS_TIMES, INT64);
  RETURN_TYPECHCK1(atype, btype, PLUS_TIMES, INT32);
  RETURN_TYPECHCK1(atype, btype, PLUS_TIMES, INT16);
  RETURN_TYPECHCK1(atype, btype, LOR_LAND, BOOL);
  return "";
}

char* vxm_semiring(pgGrB_Vector *left, pgGrB_Matrix *right) {
  GrB_Info info;
  GrB_Type atype, btype;
  CHECKD(GxB_Vector_type(&atype, left->V), left->V);
  CHECKD(GxB_Matrix_type(&btype, right->M), right->M);
  RETURN_TYPECHCK1(atype, btype, PLUS_TIMES, FP64);
  RETURN_TYPECHCK1(atype, btype, PLUS_TIMES, FP32);
  RETURN_TYPECHCK1(atype, btype, PLUS_TIMES, INT64);
  RETURN_TYPECHCK1(atype, btype, PLUS_TIMES, INT32);
  RETURN_TYPECHCK1(atype, btype, PLUS_TIMES, INT16);
  RETURN_TYPECHCK1(atype, btype, LOR_LAND, BOOL);
  return "";

}

char* matrix_times_binop(pgGrB_Matrix *left, pgGrB_Matrix *right) {
  GrB_Info info;
  GrB_Type type;
  CHECKD(GxB_Matrix_type(&type, left->M), left->M);
  return DEFAULT_TIMES_BINOP(type);
}

char* matrix_plus_binop(pgGrB_Matrix *left, pgGrB_Matrix *right) {
  GrB_Info info;
  GrB_Type type;
  CHECKD(GxB_Matrix_type(&type, left->M), left->M);
  return DEFAULT_PLUS_BINOP(type);
}


char* vector_times_binop(pgGrB_Vector *left, pgGrB_Vector *right) {
  GrB_Info info;
  GrB_Type type;
  CHECKD(GxB_Vector_type(&type, left->V), left->V);
  return DEFAULT_TIMES_BINOP(type);
}

char* vector_plus_binop(pgGrB_Vector *left, pgGrB_Vector *right) {
  GrB_Info info;
  GrB_Type type;
  CHECKD(GxB_Vector_type(&type, left->V), left->V);
  return DEFAULT_PLUS_BINOP(type);
}

char* grb_type_to_name(GrB_Type t) {
  if ( t == GrB_BOOL )
    return "bool";
  if ( t == GrB_INT8 )
    return "char";
  if ( t == GrB_UINT8 )
    return "uchar";
  if ( t == GrB_INT16 )
    return "smallint";
  if ( t == GrB_UINT16 )
    return "usmallint";
  if ( t == GrB_INT32 )
    return "integer";
  if ( t == GrB_UINT32 )
    return "uinteger";
  if ( t == GrB_INT64 )
    return "bigint";
  if ( t == GrB_UINT64 )
    return "ubigint";
  if ( t == GrB_FP32 )
    return "real";
  if ( t == GrB_FP64 )
    return "float";
  return "unknown";
}

/* #define SEMIRING(add, mul) \ */
/*   {"GxB_" #add #mul "INT8", GxB_ ## add ## mul ## INT8},                \ */
/*   {"GxB_" #add #mul "INT16", GxB_ ## add ## mul ## INT16},              \ */
/*   {"GxB_" #add #mul "INT32", GxB_ ## add ## mul ## INT32},              \ */
/*   {"GxB_" #add #mul "INT64", GxB_ ## add ## mul ## INT64},              \ */

/* struct pgGrB_Semiring */
/* { */
/*     const char* name; */
/*     GrB_Semiring semiring; */
/* } */
/* pgGrB_Semiring[] =  */
/* { */
/* SEMIRING(PLUS_, TIMES_); */
/* {"setinel", NULL} */
/* }; */
