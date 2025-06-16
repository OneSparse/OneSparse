#include "../onesparse.h"

PG_FUNCTION_INFO_V1(graph_pagerank);
Datum graph_pagerank(PG_FUNCTION_ARGS)
{
	GrB_Type type;
	os_Graph *graph;
	GrB_Vector output;
	GrB_Index vsize;
    float damping;
    float tol;
    int itermax;
	int iters;

	char msg [LAGRAPH_MSG_LEN];

	LOGF();
	graph = OS_GETARG_GRAPH(0);
	damping = PG_GETARG_FLOAT8(1);
	tol = PG_GETARG_FLOAT8(2);
    itermax = PG_GETARG_INT32(3);

	LA_CHECK(LAGraph_Cached_OutDegree(graph->graph, msg));
	LA_CHECK(LAGraph_Cached_AT(graph->graph, msg));

	LA_CHECK(LAGr_PageRank(&output,
						   &iters,
						   graph->graph,
						   damping,
						   tol,
						   itermax,
						   msg));

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
