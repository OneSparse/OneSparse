#include "../onesparse.h"

PG_FUNCTION_INFO_V1(graph_triangle_centrality);
Datum graph_triangle_centrality(PG_FUNCTION_ARGS)
{
	os_Graph *graph;
	GrB_Vector output;
	GrB_Index vsize;
	uint64_t ntriangles;
	char msg [LAGRAPH_MSG_LEN];
    struct timeval start, end;

	graph = OS_GETARG_GRAPH(0);

	OS_CHECK(GrB_Matrix_nrows(&vsize, graph->graph->A),
			 graph->graph->A,
			 "Error extracting matrix nrows.");

	LA_CHECK(LAGraph_Cached_NSelfEdges(graph->graph, msg));

	OS_START_BENCH();
	LA_CHECK(LAGraph_VertexCentrality_Triangle(
				 &output,
				 &ntriangles,
				 3,
				 graph->graph,
				 msg
				 ));
	OS_END_BENCH();

	OS_RETURN_VECTOR(new_vector(GrB_FP64, vsize, CurrentMemoryContext, output));
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
