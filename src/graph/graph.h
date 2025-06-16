#ifndef OS_GRAPH_H
#define OS_GRAPH_H

#define graph_MAGIC 689222813

typedef struct os_FlatGraph {
    int32 vl_len_;
	int32_t type_code;
	GrB_Index nrows;
	GrB_Index ncols;
	GrB_Index serialized_size;
	LAGraph_Kind kind;
	Oid loid;
} os_FlatGraph;

typedef struct os_Graph  {
    ExpandedObjectHeader hdr;
    int em_magic;
    LAGraph_Graph graph;
	Pointer flat_datum_pointer;
    Size flat_size;
    void* serialized_data;
    GrB_Index serialized_size;
	Oid loid;
} os_Graph;

Datum expand_graph(os_FlatGraph *flat,	MemoryContext parentcontext);

os_Graph* new_graph(
	GrB_Matrix matrix,
	LAGraph_Kind kind,
	MemoryContext parentcontext);

os_Graph* DatumGetGraph(Datum d);

#define OS_DETOAST_GRAPH(_datum) (os_FlatGraph*)PG_DETOAST_DATUM(datum)

#define OS_GETARG_GRAPH(_arg_num)  DatumGetGraph(PG_GETARG_DATUM(_arg_num))
#define OS_GETARG_GRAPH_A(_arg_num, _A)  DatumGetGraphMaybeA(PG_GETARG_DATUM(_arg_num), _A)
#define OS_GETARG_GRAPH_AB(_arg_num, _A, _B)  DatumGetGraphMaybeAB(PG_GETARG_DATUM(_arg_num), _A, _B)
#define OS_GETARG_GRAPH_ABC(_arg_num, _A, _B, _C)  DatumGetGraphMaybeABC(PG_GETARG_DATUM(_arg_num), _A, _B, _C)

#define OS_GETARG_GRAPH_HANDLE_OR_NULL(_nargs, _arg_num) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_GRAPH(_arg_num)->graph : NULL;
#define OS_GETARG_GRAPH_HANDLE_OR_NULL_A(_nargs, _arg_num, _A) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_GRAPH_A(_arg_num, _A)->graph : NULL;
#define OS_GETARG_GRAPH_HANDLE_OR_NULL_AB(_nargs, _arg_num, _A, _B) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_GRAPH_AB(_arg_num, _A, _B)->graph : NULL;
#define OS_GETARG_GRAPH_HANDLE_OR_NULL_ABC(_nargs, _arg_num, _A, _B, _C) \
	_nargs > _arg_num ? PG_ARGISNULL(_arg_num) ? NULL : OS_GETARG_GRAPH_ABC(_arg_num, _A, _B, _C)->graph : NULL;

#define OS_RETURN_GRAPH(_graph) return EOHPGetRWDatum(&(_graph)->hdr)
#define OS_GRAPH_FLATSIZE() MAXALIGN(sizeof(os_FlatGraph))

#define OS_GRAPH_DATA(_flat) ((char*)(_flat) + OS_GRAPH_FLATSIZE())
#define GraphGetEOHP(_datum) (os_Graph *) DatumGetEOHP(_datum)

#endif /* OS_GRAPH_H */

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
