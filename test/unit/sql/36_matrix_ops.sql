BEGIN;

CREATE EXTENSION IF NOT EXISTS onesparse;
CREATE EXTENSION IF NOT EXISTS pgtap;
SET LOCAL search_path = public, onesparse;

SELECT plan(25);

-- GraphBLAS info text is diagnostic output, so only assert stable structure.
SELECT ok(length(info('int32(2:3)[0:1:4 1:2:5]'::matrix)) > 0,
          'matrix info returns diagnostic text');
SELECT throws_ok(
    $$SELECT info('int32(2:3)'::matrix, 6)$$,
    'XX000', 'Print level is from 0 to 5',
    'matrix info rejects print levels above five');

-- Non-square factors verify both Kronecker dimensions and coordinate mapping.
SELECT is(nrows(kronecker(
    'int32(2:2)[0:0:1 1:1:2]'::matrix,
    'int32(2:3)[0:1:3 1:2:4]'::matrix,
    'plus_times_int32'::semiring)), 4::bigint,
    'kronecker uses the product of input row counts');
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(kronecker(
        'int32(2:2)[0:0:1 1:1:2]'::matrix,
        'int32(2:3)[0:1:3 1:2:4]'::matrix,
        'plus_times_int32'::semiring)) ORDER BY i, j$$,
    $$VALUES (0::bigint, 1::bigint, 'int32:3'::text),
             (1::bigint, 2::bigint, 'int32:4'::text),
             (2::bigint, 4::bigint, 'int32:6'::text),
             (3::bigint, 5::bigint, 'int32:8'::text)$$,
    'kronecker maps non-square factor coordinates and values');

-- Row and column vector operations preserve the non-target matrix entries.
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(assign_row(
        'int32(3:4)[0:0:1 2:3:9]'::matrix,
        'int32(4)[0:10 2:30]'::vector, 1)) ORDER BY i, j$$,
    $$VALUES (0::bigint, 0::bigint, 'int32:1'::text),
             (1::bigint, 0::bigint, 'int32:10'::text),
             (1::bigint, 2::bigint, 'int32:30'::text),
             (2::bigint, 3::bigint, 'int32:9'::text)$$,
    'assign_row writes a vector across every matrix column');
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(assign_row(
        'int32(3:4)'::matrix, 'int32(2)[0:10 1:20]'::vector,
        2, ARRAY[3, 1]::bigint[])) ORDER BY i, j$$,
    $$VALUES (2::bigint, 1::bigint, 'int32:20'::text),
             (2::bigint, 3::bigint, 'int32:10'::text)$$,
    'assign_row maps vector positions through explicit columns');
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(assign_col(
        'int32(3:4)[0:0:1 2:3:9]'::matrix,
        'int32(3)[0:10 2:30]'::vector, 1)) ORDER BY i, j$$,
    $$VALUES (0::bigint, 0::bigint, 'int32:1'::text),
             (0::bigint, 1::bigint, 'int32:10'::text),
             (2::bigint, 1::bigint, 'int32:30'::text),
             (2::bigint, 3::bigint, 'int32:9'::text)$$,
    'assign_col writes a vector across every matrix row');
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(assign_col(
        'int32(3:4)'::matrix, 'int32(2)[0:10 1:20]'::vector,
        2, ARRAY[2, 0]::bigint[])) ORDER BY i, j$$,
    $$VALUES (0::bigint, 2::bigint, 'int32:20'::text),
             (2::bigint, 2::bigint, 'int32:10'::text)$$,
    'assign_col maps vector positions through explicit rows');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(extract_col(
        'int32(3:4)[0:1:10 1:2:20 2:1:30]'::matrix,
        1, ARRAY[2, 0]::bigint[])) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:30'::text),
             (1::bigint, 'int32:10'::text)$$,
    'extract_col maps selected rows into vector positions');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(extract_row(
        'int32(3:4)[1:0:10 1:2:20 2:1:30]'::matrix,
        1, ARRAY[2, 0]::bigint[], 'int32(2)'::vector)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:20'::text),
             (1::bigint, 'int32:10'::text)$$,
    'extract_row maps selected columns into a supplied vector');

-- Non-square products distinguish matrix-times-vector from vector-times-matrix.
SELECT results_eq(
    $$SELECT i, v::text FROM elements(mxv(
        'int32(2:3)[0:0:1 0:2:2 1:1:3]'::matrix,
        'int32(3)[0:4 1:5 2:6]'::vector,
        'plus_times_int32'::semiring)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:16'::text),
             (1::bigint, 'int32:15'::text)$$,
    'mxv allocates a vector sized to the matrix row count');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(mxv(
        'int32(2:3)[0:0:1 0:2:2 1:1:3]'::matrix,
        'int32(3)[0:4 1:5 2:6]'::vector,
        'plus_times_int32'::semiring, 'int32(2)'::vector)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:16'::text),
             (1::bigint, 'int32:15'::text)$$,
    'mxv accepts a caller-provided result vector');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(vxm(
        'int32(2)[0:4 1:5]'::vector,
        'int32(2:3)[0:0:1 0:2:2 1:1:3]'::matrix,
        'plus_times_int32'::semiring)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:4'::text),
             (1::bigint, 'int32:15'::text),
             (2::bigint, 'int32:8'::text)$$,
    'vxm allocates a vector sized to the matrix column count');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(vxm(
        'int32(2)[0:4 1:5]'::vector,
        'int32(2:3)[0:0:1 0:2:2 1:1:3]'::matrix,
        'plus_times_int32'::semiring, 'int32(3)'::vector)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:4'::text),
             (1::bigint, 'int32:15'::text),
             (2::bigint, 'int32:8'::text)$$,
    'vxm accepts a caller-provided result vector');

-- Aggregates construct sparse objects and finalize empty input as NULL.
SELECT results_eq(
    $$SELECT i, v::text FROM
      (SELECT vector_agg(i, v) AS vector
       FROM (VALUES (0::bigint, 10::integer),
                    (3::bigint, 40::integer)) AS entries(i, v)) AS aggregated
      CROSS JOIN LATERAL elements(aggregated.vector)
      ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:10'::text),
             (3::bigint, 'int32:40'::text)$$,
    'vector_agg builds an unbounded sparse vector from coordinates');
SELECT is((SELECT vector_agg(i, v) IS NULL
           FROM (SELECT NULL::bigint AS i, NULL::integer AS v WHERE false) AS entries), true,
          'vector_agg returns NULL for empty input');
SELECT results_eq(
    $$SELECT i, j, v::text FROM
      (SELECT matrix_agg(i, j, v) AS matrix
       FROM (VALUES (0::bigint, 1::bigint, 10::integer),
                    (2::bigint, 3::bigint, 40::integer)) AS entries(i, j, v)) AS aggregated
      CROSS JOIN LATERAL elements(aggregated.matrix)
      ORDER BY i, j$$,
    $$VALUES (0::bigint, 1::bigint, 'int32:10'::text),
             (2::bigint, 3::bigint, 'int32:40'::text)$$,
    'matrix_agg builds an unbounded sparse matrix from coordinates');
SELECT results_eq(
    $$SELECT i, j, v::text FROM
      (SELECT matrix_agg(a) AS matrix
       FROM (VALUES ('int32(2:2)[0:0:2]'::matrix),
                    ('int32(2:2)[0:0:3 1:1:4]'::matrix)) AS entries(a)) AS aggregated
      CROSS JOIN LATERAL elements(aggregated.matrix)
      ORDER BY i, j$$,
    $$VALUES (0::bigint, 0::bigint, 'int32:6'::text),
             (1::bigint, 1::bigint, 'int32:4'::text)$$,
    'matrix_agg merges matrices with the default binary operator');

-- Iterators return each occupied coordinate exactly once.
SELECT results_eq(
    $$SELECT irows FROM irows('int32(4:5)[0:3:1 2:1:2 2:4:3]'::matrix) AS irows ORDER BY irows$$,
    $$VALUES (0::bigint), (2::bigint)$$,
    'irows returns distinct stored row coordinates');
SELECT results_eq(
    $$SELECT icols FROM icols('int32(4:5)[0:3:1 2:1:2 2:4:3]'::matrix) AS icols ORDER BY icols$$,
    $$VALUES (1::bigint), (3::bigint), (4::bigint)$$,
    'icols returns distinct stored column coordinates');
SELECT is((SELECT count(*) FROM irows('int32(4:5)'::matrix)), 0::bigint,
          'irows returns no coordinates for an empty matrix');
SELECT is((SELECT count(*) FROM icols('int32(4:5)'::matrix)), 0::bigint,
          'icols returns no coordinates for an empty matrix');

-- Selection keeps sparse entries that satisfy value or index predicates.
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(choose(
        'int32(3:4)[0:0:5 0:3:20 1:2:10 2:1:30]'::matrix,
        'valuegt_int32'::indexunaryop, 10::scalar)) ORDER BY i, j$$,
    $$VALUES (0::bigint, 3::bigint, 'int32:20'::text),
             (2::bigint, 1::bigint, 'int32:30'::text)$$,
    'choose filters matrix entries greater than its scalar thunk');
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(choose(
        'int32(3:4)[0:0:5 0:3:20 1:2:10 2:1:30]'::matrix,
        'triu'::indexunaryop, 0::scalar)) ORDER BY i, j$$,
    $$VALUES (0::bigint, 0::bigint, 'int32:5'::text),
             (0::bigint, 3::bigint, 'int32:20'::text),
             (1::bigint, 2::bigint, 'int32:10'::text)$$,
    'choose applies index-based upper-triangular selection');
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(choose(
        'int32(3:4)[0:0:5 0:3:20 1:2:10 2:1:30]'::matrix,
        'valuele_int32'::indexunaryop, 10::scalar, 'int32(3:4)'::matrix)) ORDER BY i, j$$,
    $$VALUES (0::bigint, 0::bigint, 'int32:5'::text),
             (1::bigint, 2::bigint, 'int32:10'::text)$$,
    'choose writes value selection into a caller-provided matrix');

SELECT * FROM finish();

ROLLBACK;
