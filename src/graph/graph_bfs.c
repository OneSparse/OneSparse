#include "../onesparse.h"

PG_FUNCTION_INFO_V1(graph_bfs);
Datum graph_bfs(PG_FUNCTION_ARGS)
{
	os_Graph *graph;
	GrB_Index source;
	GrB_Vector level;
	GrB_Vector parent;
	os_Vector *levelv;
	os_Vector *parentv;
	char msg [LAGRAPH_MSG_LEN];
    struct timeval start, end;
    TupleDesc tupdesc;
    Datum values[2];
    bool nulls[2] = {false, false};
    HeapTuple tuple;

    if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
	{
        elog(ERROR, "return type must be a row type");
	}

	LOGF();
	graph = OS_GETARG_GRAPH(0);
	source = PG_GETARG_INT64(1);

	LA_CHECK(LAGraph_Cached_AT(graph->graph, msg));
	LA_CHECK(LAGraph_Cached_OutDegree(graph->graph, msg));

	OS_START_BENCH();
	LA_CHECK(LAGr_BreadthFirstSearch(
				 &level,
				 &parent,
				 graph->graph,
				 source,
				 msg
				 ));
	OS_END_BENCH();

	levelv = new_vector(NULL, 0, CurrentMemoryContext, level);
	parentv = new_vector(NULL, 0, CurrentMemoryContext, parent);

    values[0] = OS_VECTOR_DATUM(levelv);
    values[1] = OS_VECTOR_DATUM(parentv);

    tuple = heap_form_tuple(tupdesc, values, nulls);
    PG_RETURN_DATUM(HeapTupleGetDatum(tuple));
}

