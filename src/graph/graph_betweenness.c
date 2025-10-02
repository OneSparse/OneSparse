#include "../onesparse.h"

PG_FUNCTION_INFO_V1(graph_betweenness);
Datum
graph_betweenness(PG_FUNCTION_ARGS)
{
	os_Graph   *graph;
	ArrayType  *arr;
	Datum	   *datums;
	bool	   *nulls;
	int			nelems;
	int64_t    *values;
	GrB_Vector	output;
	GrB_Index	vsize;
	char		msg[LAGRAPH_MSG_LEN];
	struct timeval start,
				end;

	LOGF();
	graph = OS_GETARG_GRAPH(0);
	arr = PG_GETARG_ARRAYTYPE_P(1);

	deconstruct_array(
					  arr,
					  INT8OID,
					  8,
					  true,
					  'd',
					  &datums,
					  &nulls,
					  &nelems
		);

	values = palloc(sizeof(int64_t) * nelems);

	for (int i = 0; i < nelems; i++)
	{
		if (nulls[i])
			ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
							errmsg("sources array must not contain nulls")));
		values[i] = DatumGetInt64(datums[i]);
	}

	LAGraph_Cached_AT(graph->graph, msg);

	OS_START_BENCH();
	LA_CHECK(LAGr_Betweenness(
							  &output,
							  graph->graph,
							  (GrB_Index *) values,
							  nelems,
							  msg
							  ));
	OS_END_BENCH();

	OS_CHECK(GrB_Matrix_nrows(&vsize, graph->graph->A),
			 graph->graph->A,
			 "Error extracting matrix nrows.");

	OS_RETURN_VECTOR(new_vector(GrB_FP64, vsize, CurrentMemoryContext, output));
}
