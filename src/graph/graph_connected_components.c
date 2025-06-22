#include "../onesparse.h"

PG_FUNCTION_INFO_V1(graph_connected_components);
Datum graph_connected_components(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Graph *graph;
	GrB_Vector output;
	GrB_Index vsize;
    struct timeval start, end;

	char msg [LAGRAPH_MSG_LEN];

	LOGF();
	graph = OS_GETARG_GRAPH(0);

	OS_START_BENCH();
	LA_CHECK(LAGr_ConnectedComponents(&output,
									  graph->graph,
									  msg));
	OS_END_BENCH();

	OS_CHECK(GxB_Vector_type(&type, output),
			 output,
			 "Cannot get return vector type");

	OS_CHECK(GrB_Vector_size(&vsize, output),
			 output,
			 "Cannot get return vector size.");

	OS_RETURN_VECTOR(new_vector(type, vsize, CurrentMemoryContext, output));
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
