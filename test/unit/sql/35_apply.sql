BEGIN;

CREATE EXTENSION IF NOT EXISTS onesparse;
CREATE EXTENSION IF NOT EXISTS pgtap;
SET LOCAL search_path = public, onesparse;

SELECT plan(10);

-- Unary apply supports allocated and caller-provided matrix outputs.
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(apply(
        'int32(2:3)[0:1:-2 1:2:3]'::matrix,
        'abs_int32'::unaryop)) ORDER BY i, j$$,
    $$VALUES (0::bigint, 1::bigint, 'int32:2'::text),
             (1::bigint, 2::bigint, 'int32:3'::text)$$,
    'matrix unary apply allocates and transforms its result');
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(apply(
        'int32(2:3)[0:1:-2 1:2:3]'::matrix,
        'abs_int32'::unaryop, 'int32(2:3)'::matrix)) ORDER BY i, j$$,
    $$VALUES (0::bigint, 1::bigint, 'int32:2'::text),
             (1::bigint, 2::bigint, 'int32:3'::text)$$,
    'matrix unary apply accepts a caller-provided result');

-- First and second scalar variants must preserve operand order.
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(apply(
        10::scalar, 'int32(2:3)[0:1:2 1:2:3]'::matrix)) ORDER BY i, j$$,
    $$VALUES (0::bigint, 1::bigint, 'int32:20'::text),
             (1::bigint, 2::bigint, 'int32:30'::text)$$,
    'matrix scalar-first apply uses the default binary operator');
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(apply(
        10::scalar, 'int32(2:3)[0:1:2 1:2:3]'::matrix,
        'minus_int32'::binaryop, 'int32(2:3)'::matrix)) ORDER BY i, j$$,
    $$VALUES (0::bigint, 1::bigint, 'int32:8'::text),
             (1::bigint, 2::bigint, 'int32:7'::text)$$,
    'matrix scalar-first apply uses the scalar as the first operand');
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(apply(
        'int32(2:3)[0:1:2 1:2:3]'::matrix, 10::scalar)) ORDER BY i, j$$,
    $$VALUES (0::bigint, 1::bigint, 'int32:20'::text),
             (1::bigint, 2::bigint, 'int32:30'::text)$$,
    'matrix scalar-second apply uses the default binary operator');
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(apply(
        'int32(2:3)[0:1:2 1:2:3]'::matrix, 10::scalar,
        'minus_int32'::binaryop, 'int32(2:3)'::matrix)) ORDER BY i, j$$,
    $$VALUES (0::bigint, 1::bigint, 'int32:-8'::text),
             (1::bigint, 2::bigint, 'int32:-7'::text)$$,
    'matrix scalar-second apply uses the scalar as the second operand');

SELECT results_eq(
    $$SELECT i, v::text FROM elements(apply(
        10::scalar, 'int32(4)[0:2 2:3]'::vector)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:20'::text),
             (2::bigint, 'int32:30'::text)$$,
    'vector scalar-first apply uses the default binary operator');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(apply(
        10::scalar, 'int32(4)[0:2 2:3]'::vector,
        'minus_int32'::binaryop, 'int32(4)'::vector)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:8'::text),
             (2::bigint, 'int32:7'::text)$$,
    'vector scalar-first apply uses the scalar as the first operand');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(apply(
        'int32(4)[0:2 2:3]'::vector, 10::scalar)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:20'::text),
             (2::bigint, 'int32:30'::text)$$,
    'vector scalar-second apply uses the default binary operator');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(apply(
        'int32(4)[0:2 2:3]'::vector, 10::scalar,
        'minus_int32'::binaryop, 'int32(4)'::vector)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:-8'::text),
             (2::bigint, 'int32:-7'::text)$$,
    'vector scalar-second apply uses the scalar as the second operand');

SELECT * FROM finish();

ROLLBACK;
