BEGIN;

CREATE EXTENSION IF NOT EXISTS onesparse;
CREATE EXTENSION IF NOT EXISTS pgtap;
SET LOCAL search_path = public, onesparse;

SELECT plan(11);

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

SELECT * FROM finish();

ROLLBACK;
