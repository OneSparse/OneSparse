\pset linestyle unicode
\pset border 2
-- # Getting Started (Work In Progress)
--
-- The GraphBLAS is is sparse linear algebra library with three core
-- objects, Matrices, Vectors and Scalars.  OneSparse provides
-- Postgres types that let you do sparse algebraic and graph
-- operations on these objects.  OneSparse is a binding to the
-- powerful
-- [SuiteSparse:GraphBLAS](http://faculty.cse.tamu.edu/davis/GraphBLAS.html)
-- library, which implements a Just-In-Time Compiler to target CPUs
-- and CUDA GPUs without changing any of the algebraic code.  The same
-- code that can be written and run on laptops with small datasets,
-- can be run on powerful multi-core GPU systems with no changes.
--
-- First we need some data.  Run the command `\i demo/fixtures.sql`
-- from a Docker Demo container to include the data used in this
-- guide:

\o /dev/null
\set ECHO none
\i demo/fixtures.sql
\set ECHO all
\o

-- The examples below will use the
-- [Newman/Karate](https://sparse.tamu.edu/Newman/karate) graph the
-- demo test fixtures:

select draw(triu(graph), directed=>false, weights=>false) as draw_source from test_graphs where name = 'karate' \gset
\i sql/draw_sfdp.sql

--
-- ## BFS
--
-- The core operation of Graph Algorithms is Breadth First Search, or
-- BFS.  In the GraphBLAS, this pattern usually consists of vector
-- matrix multiplication in a loop until some terminal condition is
-- reached.
--
-- Min Parent BFS returns a sparse vector of nodes containing the
-- minimum parent id:

CREATE OR REPLACE FUNCTION bfs(
        graph matrix,          -- This is the input graph
        start_node bigint      -- This is the node id to start the BFS
    )
    RETURNS vector             -- will return a vector of (node:minparent)
    LANGUAGE plpgsql AS
    $$
    DECLARE
    bfs_vector vector = vector('int64');  -- The result vector to accumulate ids
    prev_vector vector = vector('int64'); -- temp vector to detect termination
    BEGIN
        bfs_vector = set_element(bfs_vector, start_node, 1); -- set the start node value to 1
        WHILE bfs_vector != prev_vector LOOP  -- keep looping when the result changes
            prev_vector = dup(bfs_vector);    -- dup bfs and assign to previous
            bfs_vector = vxm(
                bfs_vector,          -- Starting from the current frontier
                graph,               -- traverse this graph.
                'any_secondi_int64', -- At any edge value, store the parent node id
                c=>bfs_vector,       -- assign back into bfs_vector
                accum=>'min_int64'   -- accumulate the minimum parent id for collisions
            );
        END LOOP;
    RETURN bfs_vector;
    end;
    $$;

-- Now run the function passing a graph and a starting point:
-- ```
-- SELECT bfs(graph, 1) as node_labels from test_graphs where name = 'karate' \gset
-- ```
--
SELECT bfs(graph, 1) as node_labels from test_graphs where name = 'karate' \gset
select draw(triu(graph), :'node_labels', directed=>false, weights=>false) as draw_source from test_graphs where name = 'karate' \gset
\i sql/draw_sfdp.sql

--
-- ## SSSP
--
-- The Single Source [Shortest
-- Path](https://en.wikipedia.org/wiki/Shortest_path_problem)
-- algorithm is a varation on BFS with a different semiring called
-- `min_plus`.  Instead of taking the minium parent id, the semiring
-- takes the minimum path length from a single source to every node.
-- This implies adding path lengths instead of multiplying them as
-- would happen in the common `plus_times` semiring.  In this example
-- we use one of OneSparse experimental operators `@<+<` to specify an
-- in place matrix multiplication and accumulation with the `min_plus`
-- semiring (`@<+`) and the minimum accumulator `<` combine to form
-- `@<+<`.
--

CREATE OR REPLACE FUNCTION sssp(graph matrix, start_node bigint)
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

SELECT sssp(graph, 1) as node_labels from test_graphs where name = 'karate' \gset
select draw(triu(graph), :'node_labels', directed=>false, weights=>false) as draw_source from test_graphs where name = 'karate' \gset
\i sql/draw_sfdp.sql

-- ## Triangle Centrality
--
-- Based on the Algorithm from https://arxiv.org/abs/2105.00110
--
-- Triangle Centrality is a node ranking metric for finding nodes in
-- the center of the most triangles.  Triangles represent strongly
-- connected nodes and their centrality can convey more importance
-- than node centrality.
--

CREATE OR REPLACE FUNCTION tc(a matrix)
    RETURNS vector LANGUAGE plpgsql AS
    $$
    DECLARE
    m matrix;
    t matrix;
    y vector;
    k scalar;
    BEGIN
        m = tril(a, -1);
        t = mxm(a, a, 'plus_pair_int32', mask=>m, descr=>'st1');
        y = reduce_cols(t) |+ reduce_rows(t);
        k = reduce_scalar(y);
        RETURN 3 * ((a @ y) |- 2 * (one(t) @ y) |+ y) / k;
    END;
    $$;

SELECT tc(cast_to(graph, 'fp64')) as node_labels from test_graphs where name = 'karate' \gset
select draw(triu(graph), :'node_labels', directed=>false, weights=>false) as draw_source from test_graphs where name = 'karate' \gset
\i sql/draw_sfdp.sql

-- ## Page Rank TODO
