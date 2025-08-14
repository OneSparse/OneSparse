#include "../onesparse.h"

PG_FUNCTION_INFO_V1(graph_out);
Datum graph_out(PG_FUNCTION_ARGS)
{
	os_Graph *graph;
	size_t len;
	char *printed;
	char *output;
    struct timeval start, end;

	graph = OS_GETARG_GRAPH(0);
	OS_START_BENCH();
	printed = _print_matrix(graph->graph->A);
	len = strlen(printed);
	output = palloc(len + 3);
	if (graph->graph->kind == LAGraph_ADJACENCY_DIRECTED)
	{
		strcpy(output, "d:");
	}
	else if (graph->graph->kind == LAGraph_ADJACENCY_UNDIRECTED)
	{
		strcpy(output, "u:");
	}
	else
	{
		elog(ERROR, "Unsupported Graph kind must be directed or undirected");
	}
	strcat(output, printed);
	OS_END_BENCH();
	PG_RETURN_CSTRING(output);
}

