BEGIN;

CREATE EXTENSION IF NOT EXISTS onesparse;
CREATE EXTENSION IF NOT EXISTS pgtap;
SET LOCAL search_path = public, onesparse;

SELECT plan(8);

-- Query rows are loaded as sparse matrix elements in cursor-sized batches.
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(matrix_query(
        'SELECT i::bigint, j::bigint, v::integer
         FROM (VALUES (0, 1, 10), (2, 3, 40), (1, 0, 20)) AS entries(i, j, v)',
        3, 4, 1)) ORDER BY i, j$$,
    $$VALUES (0::bigint, 1::bigint, 'int32:10'::text),
             (1::bigint, 0::bigint, 'int32:20'::text),
             (2::bigint, 3::bigint, 'int32:40'::text)$$,
    'matrix_query loads integer entries across multiple batches');
SELECT is((type(matrix_query(
    'SELECT 0::bigint, 0::bigint, 1.5::double precision', 1, 1)))::text,
    'fp64', 'matrix_query derives the matrix type from the value column');
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(matrix_query(
        'SELECT i::bigint, j::bigint, v::integer
         FROM (VALUES (0, 0, 10), (0, 0, 20)) AS entries(i, j, v)',
        1, 1, 1))$$,
    $$VALUES (0::bigint, 0::bigint, 'int32:20'::text)$$,
    'matrix_query keeps the last value for duplicate coordinates');
SELECT throws_ok(
    $$SELECT matrix_query('SELECT 0::bigint, 0::bigint')$$,
    'XX000', 'query must return exactly three columns, got 2',
    'matrix_query requires exactly three query columns');
SELECT is((type(matrix_query(
    'SELECT 0::bigint, 1::bigint, true', 1, 2)))::text,
    'bool', 'matrix_query supports boolean value columns');
SELECT is((type(matrix_query(
    'SELECT 0::bigint, 1::bigint, 1::bigint', 1, 2)))::text,
    'int64', 'matrix_query supports bigint value columns');
SELECT throws_ok(
    $$SELECT matrix_query('SELECT 0::integer, 0::bigint, 1::integer')$$,
    'XX000', 'first two columns must be bigint',
    'matrix_query requires bigint coordinate columns');
SELECT throws_ok(
    $$SELECT matrix_query('SELECT 0::bigint, 0::bigint, ''text''::text')$$,
    'XX000', 'third column must be bool, integer, bigint, float4 or float8',
    'matrix_query rejects unsupported value types');

SELECT * FROM finish();

ROLLBACK;
