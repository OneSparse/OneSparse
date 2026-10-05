BEGIN;

CREATE EXTENSION IF NOT EXISTS onesparse;
CREATE EXTENSION IF NOT EXISTS pgtap;
SET LOCAL search_path = public, onesparse;

SELECT plan(14);

-- Bounds, literals, incompatible dimensions, and unsupported operations.
SELECT throws_ok(
    $$SELECT 'not-a-type'::matrix$$,
    'XX000', 'Unknown short name not-a-type',
    'matrix input rejects an invalid type prefix');
SELECT throws_ok(
    $$SELECT 'int32(2:2)[0:1:not-an-integer]'::matrix$$,
    'XX000', 'Invalid INT32 not-an-integer',
    'matrix input rejects an invalid element');
SELECT throws_ok(
    $$SELECT 'int32(2:2)[0:1:1]'::vector$$,
    'XX000', 'Invalid INT32 1:1',
    'vector input rejects matrix-shaped literals');

SELECT throws_ok(
    $$SELECT get_element('int32(2)'::vector, 2)$$,
    'XX000', NULL,
    'vector access rejects an index at the size boundary');
SELECT throws_ok(
    $$SELECT get_element('int32(2:2)'::matrix, 2, 0)$$,
    'XX000', NULL,
    'matrix access rejects an out-of-range row');
SELECT throws_ok(
    $$SELECT set_element('int32(2)'::vector, -1, 1::scalar)$$,
    'XX000', NULL,
    'vector mutation rejects a negative index');

SELECT throws_ok(
    $$SELECT get_element(NULL::vector, 0)$$,
    'XX000', 'Cannot pass NULL to vector_get_element',
    'vector access rejects a NULL vector');
SELECT throws_ok(
    $$SELECT get_element('int32(2)'::vector, NULL::bigint)$$,
    'XX000', 'Cannot pass NULL to vector_get_element',
    'vector access rejects a NULL index');
SELECT throws_ok(
    $$SELECT get_element('int32(2:2)'::matrix, 0, NULL::bigint)$$,
    'XX000', 'Cannot pass NULL to matrix_get_element',
    'matrix access rejects a NULL column');

SELECT throws_ok(
    $$SELECT eadd('int32(2)'::vector, 'int32(3)'::vector,
                  'plus_int32'::binaryop)$$,
    'XX000', NULL,
    'vector eadd rejects incompatible sizes');
SELECT throws_ok(
    $$SELECT mxm('int32(2:3)'::matrix, 'int32(2:2)'::matrix)$$,
    'XX000', NULL,
    'matrix multiplication rejects incompatible shapes');
SELECT throws_ok(
    $$SELECT cast_to('int32(2)'::vector, 'graph')$$,
    'XX000', 'Unknown type graph',
    'vector casts reject unsupported target types');

SELECT throws_ok(
    $$SELECT lsh('int32(3)[0:1 2:1]'::vector)$$,
    'XX000', 'Vector must be dense.',
    'lsh rejects sparse vectors');
SELECT throws_ok(
    $$SELECT lsh('int32(61)'::vector)$$,
    'XX000', 'Vector cannot be larger than 60.',
    'lsh rejects vectors larger than 60 positions');

SELECT * FROM finish();

ROLLBACK;
