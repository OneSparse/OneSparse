-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION onesparse" to load this file. \quit

CREATE TYPE graph;

CREATE FUNCTION graph_in(cstring)
RETURNS graph
AS '$libdir/onesparse', 'graph_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION graph_out(graph)
RETURNS cstring
AS '$libdir/onesparse', 'graph_out'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE graph (
    input = graph_in,
    output = graph_out,
    storage = 'external',
    internallength = -1
    );

COMMENT ON TYPE graph IS 'Graphs wrap the LAGraph library functions.';

CREATE FUNCTION graph(matrix, text)
RETURNS graph
AS '$libdir/onesparse', 'graph_new'
LANGUAGE C STABLE;

CREATE OR REPLACE FUNCTION graph(matrix)
RETURNS graph
LANGUAGE sql STABLE
BEGIN ATOMIC
    select graph($1, 'u');
END;

CREATE FUNCTION sssp(graph, bigint, scalar)
RETURNS vector
AS '$libdir/onesparse', 'graph_sssp'
LANGUAGE C STABLE;

CREATE OR REPLACE FUNCTION sssp(matrix, bigint, scalar)
RETURNS vector
LANGUAGE sql STABLE
BEGIN ATOMIC
    select sssp(graph($1), $2, $3);
END;

CREATE FUNCTION pagerank(graph, float, float, integer)
RETURNS vector
AS '$libdir/onesparse', 'graph_pagerank'
LANGUAGE C STABLE;

CREATE FUNCTION pagerank(graph)
RETURNS vector
LANGUAGE sql STABLE
BEGIN ATOMIC
  SELECT pagerank($1, 0.85::float, 1e-6::float, 100);
END;

CREATE OR REPLACE FUNCTION pagerank(matrix, float, float, integer, text)
RETURNS vector
LANGUAGE sql STABLE
BEGIN ATOMIC
    select pagerank(graph($1, $5), $2, $3, $4);
END;

CREATE OR REPLACE FUNCTION pagerank(matrix, float, float, integer)
RETURNS vector
LANGUAGE sql STABLE
BEGIN ATOMIC
    select pagerank(graph($1, 'u'), $2, $3, $4);
END;

CREATE FUNCTION pagerank(matrix)
RETURNS vector
LANGUAGE sql STABLE
BEGIN ATOMIC
  SELECT pagerank($1, 0.85::float, 1e-6::float, 100);
END;

CREATE FUNCTION triangle_count(graph)
RETURNS bigint
AS '$libdir/onesparse', 'graph_triangle_count'
LANGUAGE C STABLE;

CREATE FUNCTION triangle_count(matrix)
RETURNS bigint
LANGUAGE sql STABLE
BEGIN ATOMIC
  SELECT triangle_count(graph($1));
END;

CREATE FUNCTION triangle_centrality(graph)
RETURNS vector
AS '$libdir/onesparse', 'graph_triangle_centrality'
LANGUAGE C STABLE;

CREATE FUNCTION betweenness(graph, bigint[])
RETURNS vector
AS '$libdir/onesparse', 'graph_betweenness'
LANGUAGE C STABLE;

CREATE FUNCTION connected_components(graph)
RETURNS vector
AS '$libdir/onesparse', 'graph_connected_components'
LANGUAGE C STABLE;

CREATE TYPE bfs_level_parent AS (level vector, parent vector);

CREATE FUNCTION bfs(graph, bigint)
RETURNS bfs_level_parent
AS '$libdir/onesparse', 'graph_bfs'
LANGUAGE C STABLE;
