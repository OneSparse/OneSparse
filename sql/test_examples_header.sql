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

-- The examples below will use the `karate` graph the demo test
-- fixtures:

select draw(triu(graph), directed=>false, weights=>false) as draw_source from test_graphs where name = 'karate' \gset
\i sql/draw_sfdp.sql

--
-- ## BFS
--
-- Min Parent BFS returns "node:min parent node" vector:

CREATE OR REPLACE FUNCTION bfs(graph matrix, start_node bigint)
    RETURNS vector LANGUAGE plpgsql AS
    $$
    DECLARE
    bfs_vector vector = vector('int32');
    next_vector vector = vector('int32');
    BEGIN
        bfs_vector = set_element(bfs_vector, start_node, 1);
        WHILE bfs_vector != next_vector LOOP
            next_vector = dup(bfs_vector);
            bfs_vector = vxm(bfs_vector, graph, 'any_secondi_int32',
                             w=>bfs_vector, accum=>'min_int32');
        END LOOP;
    RETURN bfs_vector;
    end;
    $$;

SELECT bfs(graph, 1) as node_labels from test_graphs where name = 'karate' \gset
select draw(triu(graph), :'node_labels', directed=>false, weights=>false) as draw_source from test_graphs where name = 'karate' \gset
\i sql/draw_sfdp.sql

--
-- ## SSSP
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
-- https://arxiv.org/abs/2105.00110
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
