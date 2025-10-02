#include "../onesparse.h"

PG_FUNCTION_INFO_V1(graph_sssp);
Datum
graph_sssp(PG_FUNCTION_ARGS)
{
	GrB_Type	type;
	os_Graph   *graph;
	GrB_Index	source;
	os_Scalar  *delta;
	GrB_Vector	output;
	GrB_Index	vsize;
	char		msg[LAGRAPH_MSG_LEN];
	struct timeval start,
				end;

	graph = OS_GETARG_GRAPH(0);
	source = PG_GETARG_INT64(1);
	delta = OS_GETARG_SCALAR(2);

	OS_CHECK(GxB_Matrix_type(&type, graph->graph->A),
			 graph->graph->A,
			 "Cannot get matrix type");

	OS_CHECK(GrB_Matrix_nrows(&vsize, graph->graph->A),
			 graph->graph->A,
			 "Error extracting matrix nrows.");

	LA_CHECK(LAGraph_Cached_EMin(graph->graph, msg));

	OS_START_BENCH();
	LA_CHECK(LAGr_SingleSourceShortestPath(
										   &output,
										   graph->graph,
										   source,
										   delta->scalar,
										   msg
										   ));
	OS_END_BENCH();

	OS_RETURN_VECTOR(new_vector(type, vsize, CurrentMemoryContext, output));
}
