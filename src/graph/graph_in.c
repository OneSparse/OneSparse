#include "../onesparse.h"

PG_FUNCTION_INFO_V1(graph_in);
Datum graph_in(PG_FUNCTION_ARGS)
{
	char *input;
	LAGraph_Kind kind;
	os_Matrix *matrix;
	os_Graph *graph;

	input = PG_GETARG_CSTRING(0);

	if (strstr(input, "d:"))
	{
		kind = LAGraph_ADJACENCY_DIRECTED;
	}
	else if (strstr(input, "u:"))
	{
		kind = LAGraph_ADJACENCY_UNDIRECTED;
	}
	else
	{
		elog(ERROR, "Graph must begin with u: or d:");
	}

	matrix = _parse_matrix(pstrdup(input+2));
	graph = new_graph(matrix->matrix, kind, CurrentMemoryContext);
	OS_RETURN_GRAPH(graph);
}

/* Local Variables: */
/* mode: c */
/* c-file-style: "postgresql" */
/* End: */
