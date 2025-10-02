#include "../onesparse.h"

PG_FUNCTION_INFO_V1(graph_new);
Datum
graph_new(PG_FUNCTION_ARGS)
{
	os_Graph   *A;
	os_Matrix  *matrix;
	text	   *kind_char;
	LAGraph_Kind kind;
	char		c;

	matrix = OS_GETARG_MATRIX(0);
	kind_char = PG_GETARG_TEXT_PP(1);

	if (VARSIZE_ANY_EXHDR(kind_char) != 1)
		ereport(ERROR, (errmsg("kind must be a single character")));

	c = *VARDATA_ANY(kind_char);

	if (c == 'u')
	{
		kind = LAGraph_ADJACENCY_UNDIRECTED;
	}
	else if (c == 'd')
	{
		kind = LAGraph_ADJACENCY_DIRECTED;
	}
	else
	{
		ereport(ERROR, (errmsg("character must be 'u' or 'd'")));
	}
	A = new_graph(matrix->matrix, kind, CurrentMemoryContext);
	OS_RETURN_GRAPH(A);
}
