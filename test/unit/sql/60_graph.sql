BEGIN;

CREATE EXTENSION IF NOT EXISTS onesparse;
CREATE EXTENSION IF NOT EXISTS pgtap;
SET LOCAL search_path = public, onesparse;

SELECT plan(17);

-- A four-vertex path makes BFS levels, parents, and weighted distances
-- independently verifiable.
SELECT results_eq(
    $$SELECT i, v::text FROM elements((bfs(
        'int32(4:4)[0:1:1 1:0:1 1:2:1 2:1:1 2:3:1 3:2:1]'::matrix,
        0)).level) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:0'::text),
             (1::bigint, 'int32:1'::text),
             (2::bigint, 'int32:2'::text),
             (3::bigint, 'int32:3'::text)$$,
    'bfs returns the expected levels');
SELECT results_eq(
    $$SELECT i, v::text FROM elements((bfs(
        'int32(4:4)[0:1:1 1:0:1 1:2:1 2:1:1 2:3:1 3:2:1]'::matrix,
        0)).parent) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:0'::text),
             (1::bigint, 'int32:0'::text),
             (2::bigint, 'int32:1'::text),
             (3::bigint, 'int32:2'::text)$$,
    'bfs returns a valid path parent for every vertex');
SELECT is(size((bfs(
    'int32(4:4)[0:1:1 1:0:1 1:2:1 2:1:1 2:3:1 3:2:1]'::matrix,
    0)).level), 4::bigint,
    'bfs returns one level for every vertex');

SELECT results_eq(
    $$SELECT i, v::text FROM elements(sssp(
        'int32(4:4)[0:1:1 1:0:1 1:2:2 2:1:2 2:3:3 3:2:3]'::matrix,
        0, 1::scalar)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:0'::text),
             (1::bigint, 'int32:1'::text),
             (2::bigint, 'int32:3'::text),
             (3::bigint, 'int32:6'::text)$$,
    'sssp returns exact weighted distances');

-- PageRank is checked through stable invariants rather than implementation-
-- dependent floating-point formatting.
SELECT is(size(pagerank(
    'int32(4:4)[0:1:1 1:0:1 1:2:1 2:1:1 2:3:1 3:2:1]'::matrix)), 4::bigint,
    'pagerank returns one score per vertex');
SELECT ok((SELECT bool_and((v::float8) >= 0)
           FROM elements(pagerank(
               'int32(4:4)[0:1:1 1:0:1 1:2:1 2:1:1 2:3:1 3:2:1]'::matrix))),
    'pagerank scores are non-negative');
SELECT ok(abs((SELECT sum(v::float8)
               FROM elements(pagerank(
                   'int32(4:4)[0:1:1 1:0:1 1:2:1 2:1:1 2:3:1 3:2:1]'::matrix))) - 1.0) < 1e-5,
    'pagerank scores sum to one');
SELECT ok((SELECT v::float8 FROM elements(pagerank(
               'int32(4:4)[0:1:1 1:0:1 1:2:1 2:1:1 2:3:1 3:2:1]'::matrix))
           WHERE i = 1)
          > (SELECT v::float8 FROM elements(pagerank(
               'int32(4:4)[0:1:1 1:0:1 1:2:1 2:1:1 2:3:1 3:2:1]'::matrix))
             WHERE i = 0),
    'pagerank distinguishes vertices by graph structure');

-- Component labels are intentionally compared as a partition, not by their
-- arbitrary numeric representative values.
SELECT ok((SELECT count(*) = 5
           FROM elements(connected_components(
               'int32(5:5)[0:1:1 1:0:1 1:2:1 2:1:1 3:4:1 4:3:1]'::matrix))),
    'connected_components returns one label per vertex');
SELECT ok((WITH labels AS (
               SELECT i, v::text AS label
               FROM elements(connected_components(
                   'int32(5:5)[0:1:1 1:0:1 1:2:1 2:1:1 3:4:1 4:3:1]'::matrix)))
           SELECT (SELECT label FROM labels WHERE i = 0) =
                  (SELECT label FROM labels WHERE i = 2)
              AND (SELECT label FROM labels WHERE i = 3) =
                  (SELECT label FROM labels WHERE i = 4)
              AND (SELECT label FROM labels WHERE i = 0) <>
                  (SELECT label FROM labels WHERE i = 3)),
    'connected_components preserves the component partition');

-- A triangle gives exact, small centrality results.
SELECT is(triangle_count(
    'int32(3:3)[0:1:1 0:2:1 1:0:1 1:2:1 2:0:1 2:1:1]'::matrix),
    1::bigint, 'triangle_count finds one triangle');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(triangle_centrality(
        'int32(3:3)[0:1:1 0:2:1 1:0:1 1:2:1 2:0:1 2:1:1]'::matrix)) ORDER BY i$$,
    $$VALUES (0::bigint, 'fp64:1.000000'::text),
             (1::bigint, 'fp64:1.000000'::text),
             (2::bigint, 'fp64:1.000000'::text)$$,
    'triangle_centrality counts the incident triangle');
SELECT is(size(betweenness(
    'int32(4:4)[0:1:1 1:0:1 1:2:1 2:1:1 2:3:1 3:2:1]'::matrix,
    ARRAY[0, 3]::bigint[])), 4::bigint,
    'betweenness returns one score per vertex');
SELECT ok((SELECT (v::float8) > 0 FROM elements(betweenness(
               'int32(4:4)[0:1:1 1:0:1 1:2:1 2:1:1 2:3:1 3:2:1]'::matrix,
               ARRAY[0, 3]::bigint[])) WHERE i = 1),
    'betweenness identifies an interior path vertex');
SELECT ok((SELECT (v::float8) > 0 FROM elements(betweenness(
               'int32(4:4)[0:1:1 1:0:1 1:2:1 2:1:1 2:3:1 3:2:1]'::matrix,
               ARRAY[0, 3]::bigint[])) WHERE i = 2),
    'betweenness identifies both interior path vertices');

-- Invalid graph inputs have deliberate, stable error contracts.
SELECT throws_ok(
    $$SELECT graph('int32(2:2)'::matrix, 'x')$$,
    'XX000', 'character must be ''u'' or ''d''',
    'graph rejects an unknown graph kind');
SELECT throws_ok(
    $$SELECT betweenness('int32(2:2)[0:1:1 1:0:1]'::matrix,
                         ARRAY[NULL]::bigint[])$$,
    '22004', 'sources array must not contain nulls',
    'betweenness rejects null source vertices');

SELECT * FROM finish();

ROLLBACK;
