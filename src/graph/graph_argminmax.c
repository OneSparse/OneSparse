#include "../onesparse.h"

PG_FUNCTION_INFO_V1(graph_argminmax);
Datum graph_argminmax(PG_FUNCTION_ARGS)
{
	os_Graph *graph;
	GrB_Vector p_result;
	GrB_Vector x_result;
	os_Vector *p_resultv;
	os_Vector *x_resultv;
	int dim;
	bool is_min;
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
	dim = PG_GETARG_INT32(1);
	is_min = PG_GETARG_BOOL(2);

	OS_START_BENCH();
	LA_CHECK(LAGraph_argminmax(
				 &x_result,
				 &p_result,
				 graph->graph->A,
				 dim,
				 is_min,
				 msg
				 ));
	OS_END_BENCH();

	x_resultv = new_vector(NULL, 0, CurrentMemoryContext, x_result);
	p_resultv = new_vector(NULL, 0, CurrentMemoryContext, p_result);

    values[0] = OS_VECTOR_DATUM(x_resultv);
    values[1] = OS_VECTOR_DATUM(p_resultv);

    tuple = heap_form_tuple(tupdesc, values, nulls);
    PG_RETURN_DATUM(HeapTupleGetDatum(tuple));
}

