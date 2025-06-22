#include "../onesparse.h"

PG_FUNCTION_INFO_V1(graph_triangle_count);
Datum graph_triangle_count(PG_FUNCTION_ARGS)
{
	os_Graph *graph;
	uint64_t result;
	char msg [LAGRAPH_MSG_LEN];
    struct timeval start, end;

	LOGF();
	graph = OS_GETARG_GRAPH(0);

	OS_START_BENCH();
	LA_CHECK(LAGraph_TriangleCount(&result,
                                   graph->graph,
                                   msg));
	OS_END_BENCH();
    PG_RETURN_INT64(result);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
