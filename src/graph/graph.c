#include "../onesparse.h"

static void context_callback_graph_free(void*);
static Size graph_get_flat_size(ExpandedObjectHeader *eohptr);

static void flatten_graph(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size);

static const ExpandedObjectMethods graph_methods = {
	graph_get_flat_size,
	flatten_graph
};

static Size graph_get_flat_size(ExpandedObjectHeader *eohptr) {
	os_Graph *graph;
	void *serialized_data;
	GrB_Index serialized_size;

	graph = (os_Graph*) eohptr;
	Assert(graph->em_magic == graph_MAGIC);

	if (graph->flat_size)
	{
		Assert(graph->serialized_data != NULL);
		Assert(OS_GRAPH_FLATSIZE() + graph->serialized_size == graph->flat_size);
		return graph->flat_size;
	}

    OS_CHECK(GrB_wait(graph->graph->A, GrB_MATERIALIZE),
			 graph->graph->A,
			 "Error waiting to materialize graph.");

	OS_CHECK(GxB_Matrix_serialize(
				 &serialized_data,
				 &serialized_size,
				 graph->graph->A, NULL),
			 graph->graph->A,
			 "Error serializing graph");

	graph->serialized_data = serialized_data;
	graph->serialized_size = serialized_size;
	graph->flat_size = OS_GRAPH_FLATSIZE() + serialized_size;
	return graph->flat_size;
}

/* Flatten graph into a pre-allocated result buffer that is
   allocated_size in bytes.	 */
static void flatten_graph(
	ExpandedObjectHeader *eohptr,
	void *result,
	Size allocated_size)
{
	os_Graph *graph;
	os_FlatGraph *flat;
	void* data;

	LOGF();

	graph = (os_Graph *) eohptr;
	flat = (os_FlatGraph *) result;

	Assert(graph->em_magic == graph_MAGIC);
	Assert(allocated_size == graph->flat_size);
	Assert(graph->serialized_data != NULL);
	Assert(graph->serialized_size > 0);

	memset(flat, 0, allocated_size);

	OS_CHECK(GrB_get(
				 graph->graph->A,
				 &flat->type_code,
				 GrB_EL_TYPE_CODE),
			 graph->graph->A,
			 "Cannot get Graph Type code.");

	OS_CHECK(GrB_Matrix_nrows(&flat->nrows, graph->graph->A),
			 graph->graph->A,
			 "Error extracting matrix nrows.");

	OS_CHECK(GrB_Matrix_nrows(&flat->ncols, graph->graph->A),
			 graph->graph->A,
			 "Error extracting matrix ncols.");

	data = OS_GRAPH_DATA(flat);

	memcpy(data, graph->serialized_data, graph->serialized_size);
	flat->serialized_size = graph->serialized_size;

	/* free_function = NULL; */
	/* ERRORIF(GrB_get(GrB_Global, (void*)free_function, GxB_FREE_FUNCTION) != GrB_SUCCESS, */
	/*		"Cannot get SuiteSparse free function."); */

	/* free_function(graph->serialized_data); */
	SET_VARSIZE(flat, allocated_size);
}

/* Construct an empty expanded graph. */
os_Graph* new_graph(
	GrB_Matrix matrix,
	LAGraph_Kind kind,
	MemoryContext parentcontext)
{
	os_Graph *graph;
	char msg [LAGRAPH_MSG_LEN] ;

	MemoryContext objcxt, oldcxt;
	MemoryContextCallback *ctxcb;

	LOGF();
	objcxt = AllocSetContextCreate(parentcontext,
								   "expanded graph",
								   ALLOCSET_DEFAULT_SIZES);

	graph = MemoryContextAllocZero(objcxt,	sizeof(os_Graph));

	EOH_init_header(&graph->hdr, &graph_methods, objcxt);

	oldcxt = MemoryContextSwitchTo(objcxt);

	graph->em_magic = graph_MAGIC;
	graph->flat_size = 0;
	graph->serialized_data = NULL;
	graph->serialized_size = 0;

	LAGraph_New(&graph->graph, &matrix, kind, msg);

	ctxcb = MemoryContextAlloc(objcxt, sizeof(MemoryContextCallback));

	ctxcb->func = context_callback_graph_free;
	ctxcb->arg = graph;

	MemoryContextRegisterResetCallback(objcxt, ctxcb);

	MemoryContextSwitchTo(oldcxt);
	return graph;
}

/* Expand a flat graph in to an Expanded one, return as Postgres Datum. */
Datum expand_graph(os_FlatGraph *flat, MemoryContext parentcontext)
{
	GrB_Type type;
	GrB_Matrix matrix;
	os_Graph *graph;
	void* data;

	LOGF();

	type = code_type(flat->type_code);
	data = OS_GRAPH_DATA(flat);

	OS_CHECK(GxB_Matrix_deserialize(
				 &matrix,
				 type,
				 data,
				 flat->serialized_size, NULL),
			 matrix,
			 "Error deserializing graph");

	graph = new_graph(matrix, flat->kind, parentcontext);
	OS_RETURN_GRAPH(graph);
}

static void
context_callback_graph_free(void* ptr)
{
	char msg [LAGRAPH_MSG_LEN] ;
	os_Graph *graph = (os_Graph *) ptr;
	LAGraph_Delete(&graph->graph, msg);
}

/* Helper function to always expand datum

   This is used by PG_GETARG_GRAPH */
os_Graph* DatumGetGraph(Datum datum)
{
	os_Graph *graph;
	os_FlatGraph *flat;
	Pointer flat_datum_pointer;

	flat_datum_pointer = DatumGetPointer(datum);

	if (VARATT_IS_EXTERNAL_EXPANDED(flat_datum_pointer)) {
		graph = GraphGetEOHP(datum);
		Assert(graph->em_magic == graph_MAGIC);
		return graph;
	}
	flat = OS_DETOAST_GRAPH(datum);
	datum = expand_graph(flat, CurrentMemoryContext);
	graph = GraphGetEOHP(datum);
	graph->flat_datum_pointer = flat_datum_pointer;
	return graph;
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
