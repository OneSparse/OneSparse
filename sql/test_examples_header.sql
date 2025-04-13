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

CREATE OR REPLACE FUNCTION tcentrality(a matrix)
    RETURNS vector LANGUAGE plpgsql AS
    $$
    DECLARE
        n bigint;  -- number of rows in a
        l matrix;  -- lower triangle of a
        t matrix;  -- triangle count
        c vector;
        y vector;
        w vector;
        u vector;
        k scalar;
        plus_monoid monoid = 'plus_monoid_fp32';
        plus_pair semiring = 'plus_pair_fp32';
        plus_second semiring = 'plus_second_fp32';
        times binaryop = 'times_fp32';
        div binaryop = 'div_fp32';
        plus binaryop = 'plus_fp32';
        st1 descriptor = 'st1';
        t0 descriptor = 't0';
        rtype type = 'fp32';
        start_time timestamp;
    BEGIN
        a = wait(a);
        start_time = clock_timestamp();
        n = nrows(a);
        y = vector(rtype, n);
        w = vector(rtype, n);
        u = vector(rtype, n);
        c = vector(rtype, n);
        l = tril(a, -1);
        t = matrix(rtype, n, n);
        t = mxm(a, a, plus_pair, mask=>l, c=>t, descr=>st1);
        y = assign(y, 0);
        y = reduce_cols(t, plus_monoid, accum=>plus, descr=>t0);
        y = reduce_cols(t, plus_monoid, accum=>plus);
        k = reduce_scalar(y);
        w = mxv(t, y, plus_second, c=>w);
        w = mxv(t, y, plus_second, accum=>plus, c=>w, descr=>t0);
        w = apply(w, -2, times, c=>w);
        u = mxv(a, y, plus_second, c=>u);
        c = apply(3, u, times, c=>c);
        c = eadd(w, y, plus, accum=>plus, c=>c);
        c = apply(c, k, div, c=>c);
        return c;
    END;
    $$;


SELECT tcentrality(resize(graph, 34, 34)) as node_labels from test_graphs where name = 'karate' \gset
select draw(triu(graph), :'node_labels', directed=>false, weights=>false) as draw_source from test_graphs where name = 'karate' \gset
\i sql/draw_sfdp.sql

-- ## Page Rank TODO

create or replace function pagerank(
    a matrix,
    damping real default 0.85,
    itermax integer default 100,
    itermin integer default 50,
    tol real default 1e-4
    )
    returns vector language plpgsql as
    $$
    declare
        rtype type = 'fp32';
        n bigint;
        teleport scalar;
        rdiff real = 1.0;
        b matrix;
        d_out vector;
        t vector;
        w vector;
        r vector;
        plus_second semiring = 'plus_second_fp32';
        abs unaryop = 'abs_fp32';
        plus binaryop = 'plus_fp32';
        div binaryop = 'div_fp32';
        minus binaryop = 'minus_fp32';
        t0 descriptor = 't0';
        start_time timestamp;
        end_time timestamp;
    begin
        a = wait(a);
        start_time = clock_timestamp();
        n = nrows(a);
        teleport = ((1 - damping) / n)::real;
        b = apply(a, 'one_int32'::unaryop, c=>b);
        d_out = apply(reduce_rows(a), 'identity_fp32'::unaryop, c=>d_out);
        t = vector(rtype, n);
        w = vector(rtype, n);
        r = vector(rtype, n);
        r = assign(r, 1::real / n);
        d_out = assign(d_out, damping::scalar, accum=>div);
        for i in 0..itermax loop
            t = dup(r);
            r = assign(r, teleport);
            w = eadd(t, d_out, div, c=>w);
            r = mxv(a, w, plus_second, c=>r, accum=>plus, descr=>t0);
            if i >= itermin then
                t = assign(t, r, accum=>minus);
                t = apply(t, abs, c=>t);
                rdiff = reduce_scalar(t);
                exit when rdiff < tol;
            end if;
        end loop;
        end_time = clock_timestamp();
        return r;
    end;
    $$;

-- SELECT pagerank(resize(graph, 34, 34)) as node_labels from test_graphs where name = 'karate' \gset
-- select draw(triu(graph), :'node_labels', directed=>false, weights=>false) as draw_source from test_graphs where name = 'karate' \gset
-- \i sql/draw_sfdp.sql
