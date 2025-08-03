#include "../onesparse.h"

PG_FUNCTION_INFO_V1(graph_fglt);
Datum graph_fglt(PG_FUNCTION_ARGS)
{
	os_Graph *graph;
	GrB_Matrix output;
	char msg [LAGRAPH_MSG_LEN];
    struct timeval start, end;

	graph = OS_GETARG_GRAPH(0);

	LA_CHECK(LAGraph_Cached_NSelfEdges(graph->graph, msg));

	OS_START_BENCH();
	LA_CHECK(LAGraph_FastGraphletTransform(
				 &output,
				 graph->graph,
                 true,
				 msg
				 ));
	OS_END_BENCH();

	OS_RETURN_MATRIX(new_matrix(NULL, 0, 0, CurrentMemoryContext, output));
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
