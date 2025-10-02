#include "../onesparse.h"

PG_FUNCTION_INFO_V1(graph_square_clustering);
Datum
graph_square_clustering(PG_FUNCTION_ARGS)
{
	os_Graph   *graph;
	GrB_Vector	output;
	GrB_Index	vsize;
	char		msg[LAGRAPH_MSG_LEN];
	struct timeval start,
				end;

	graph = OS_GETARG_GRAPH(0);

	OS_CHECK(GrB_Matrix_nrows(&vsize, graph->graph->A),
			 graph->graph->A,
			 "Error extracting matrix nrows.");

	LA_CHECK(LAGraph_Cached_OutDegree(graph->graph, msg));

	OS_START_BENCH();
	LA_CHECK(LAGraph_SquareClustering(
									  &output,
									  graph->graph,
									  msg
									  ));
	OS_END_BENCH();

	OS_RETURN_VECTOR(new_vector(GrB_FP64, vsize, CurrentMemoryContext, output));
}
