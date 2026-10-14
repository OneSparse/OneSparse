BEGIN;

CREATE EXTENSION IF NOT EXISTS onesparse;
CREATE EXTENSION IF NOT EXISTS pgtap;
SET LOCAL search_path = public, onesparse;

SELECT plan(16);

-- Explicit seeds make generated sparse objects reproducible.
SELECT ok(eq(random_matrix('int32', 3, 5, 0.5, 42),
             random_matrix('int32', 3, 5, 0.5, 42)),
          'random_matrix produces the same matrix for the same seed');
SELECT is(nrows(random_matrix('int32', 3, 5, 0.5, 42)), 3::bigint,
          'random_matrix retains its requested row count');
SELECT is(ncols(random_matrix('int32', 3, 5, 0.5, 42)), 5::bigint,
          'random_matrix retains its requested column count');
SELECT is((type(random_matrix('fp64', 3, 5, 0.5, 42)))::text, 'fp64',
          'random_matrix retains its requested element type');
SELECT is(nvals(random_matrix('int32', 3, 5, 0, 42)), 0::bigint,
          'random_matrix with zero density is empty');
SELECT ok(nvals(random_matrix('int32', 3, 5, 1, 42)) > 0
           AND nvals(random_matrix('int32', 3, 5, 1, 42)) <= 15,
           'random_matrix bounds generated entries by its shape');
SELECT throws_ok(
    $$SELECT random_matrix('int32', 3, 5, -0.1, 42)$$,
    'XX000',
    '-3: LAGraph failure (file /home/postgres/LAGraph-1.2.2/experimental/utility/LAGraph_Random_Matrix.c, line 106): invalid density',
    'random_matrix rejects a negative density');
SELECT throws_ok(
    $$SELECT random_matrix('int32', -2, 5, 0.5, 42)$$,
    'XX000',
    '-3: GraphBLAS failure (file /home/postgres/LAGraph-1.2.2/experimental/utility/LAGraph_Random_Matrix.c, line 115): info: -3',
    'random_matrix rejects an invalid row count');

SELECT ok(eq(random_vector('int32', 7, 0.5, 42),
             random_vector('int32', 7, 0.5, 42)),
          'random_vector produces the same vector for the same seed');
SELECT is(size(random_vector('int32', 7, 0.5, 42)), 7::bigint,
          'random_vector retains its requested size');
SELECT is((type(random_vector('bool', 7, 0.5, 42)))::text, 'bool',
          'random_vector retains its requested element type');
SELECT is(nvals(random_vector('int32', 7, 0, 42)), 0::bigint,
          'random_vector with zero density is empty');
SELECT ok(nvals(random_vector('int32', 7, 1, 42)) > 0
           AND nvals(random_vector('int32', 7, 1, 42)) <= 7,
           'random_vector bounds generated entries by its size');
SELECT ok(nvals(random_vector('int32', 7, 1, 42)) > 0
          AND nvals(random_vector('int32', 7, 1, 42)) <= 7,
          'random_vector accepts full density');
SELECT throws_ok(
    $$SELECT random_vector('int32', 7, -0.1, 42)$$,
    'XX000',
    '-3: LAGraph failure (file /home/postgres/LAGraph-1.2.2/experimental/utility/LAGraph_Random_Matrix.c, line 106): invalid density',
    'random_vector rejects a negative density');
SELECT throws_ok(
    $$SELECT random_vector('int32', -2, 0.5, 42)$$,
    'XX000',
    'INVALID_VALUE : Cannot create new Vector.',
    'random_vector rejects an invalid size');

SELECT * FROM finish();

ROLLBACK;
