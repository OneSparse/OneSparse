\pset linestyle unicode
\pset border 2
-- # Examples Coming Soon
--
-- OneSparse is a early alpha prototype, so these examples are likely
-- to change as bugs are fixed and new approaches are tried:

\o /dev/null
\set ECHO none
\i demo/fixtures.sql
\set ECHO all
\o

--
-- ## BFS
--
-- Min Parent BFS returns "node:min parent node" vector:

CREATE OR REPLACE FUNCTION public.bfs(graph matrix, start_node bigint)
    RETURNS vector LANGUAGE plpgsql AS
    $$
    DECLARE
    sssp_vector vector = vector('int32');
    next_vector vector = vector('int32');
    BEGIN
        sssp_vector = set_element(sssp_vector, start_node, 1);
        WHILE sssp_vector != next_vector LOOP
            next_vector = dup(sssp_vector);
            sssp_vector = vxm(sssp_vector, graph, 'any_secondi_int32', w=>sssp_vector, accum=>'min_int32');
        END LOOP;
    RETURN sssp_vector;
    end;
    $$;

--
-- ## SSSP
--
-- Single Source Shortest Path as a CTE:

WITH RECURSIVE
    vars(graph, bfs_vector, next_vector)
    AS (SELECT graph,
               'int32[1:1]'::vector,
               'int32'::vector
        FROM test_graphs WHERE name = 'mbeacxc'
        UNION ALL
        SELECT
            graph,
            bfs_vector @<+< graph,
            dup(bfs_vector)
        FROM vars
        WHERE next_vector != bfs_vector)
SELECT bfs_vector FROM vars;

--
-- As a plpgsql function:
--

CREATE OR REPLACE FUNCTION public.sssp(graph matrix, start_node bigint)
    RETURNS vector LANGUAGE plpgsql AS
    $$
    DECLARE
    sssp_vector vector = vector(type(graph));
    next_vector vector = vector(type(graph));
    BEGIN
        sssp_vector = set_element(sssp_vector, start_node, 1);
        WHILE sssp_vector != next_vector LOOP
            next_vector = dup(sssp_vector);
            sssp_vector = sssp_vector @<+< graph;
        END LOOP;
    RETURN sssp_vector;
    end;
    $$;

-- ## Triangle Centrality as a CTE
--
-- https://arxiv.org/abs/2105.00110
--

WITH
    setup AS (SELECT graph as a from test_graphs),
    triangulate AS (SELECT mxm(a, a, mask=>a, descriptor=>'st1') AS t FROM setup),
    reduce_graph AS (SELECT reduce_vector(t) as y FROM triangulate),
    compute_thresh AS (SELECT reduce_scalar(y) as k FROM reduce_graph)
SELECT 3 * ((a @ y) |- 2 * (one(t) @ y) |+ y) / k
FROM setup, triangulate, reduce_graph, compute_thresh;

-- ## Page Rank TODO
