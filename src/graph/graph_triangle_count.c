#include "../onesparse.h"

PG_FUNCTION_INFO_V1(graph_triangle_count);
Datum graph_triangle_count(PG_FUNCTION_ARGS)
{
	os_Graph *graph;
	uint64_t result;
	char msg [LAGRAPH_MSG_LEN];
	LOGF();
	graph = OS_GETARG_GRAPH(0);

	LA_CHECK(LAGraph_TriangleCount(&result,
                                   graph->graph,
                                   msg));
    PG_RETURN_INT64(result);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
