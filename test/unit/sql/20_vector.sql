BEGIN;

CREATE EXTENSION IF NOT EXISTS onesparse;
CREATE EXTENSION IF NOT EXISTS pgtap;
SET LOCAL search_path = public, onesparse;

SELECT plan(32);

-- Construction and sparse representation.
SELECT is((type('int32'::vector))::text, 'int32',
          'an empty vector retains its element type');
SELECT is(size('int32(4)'::vector), 4::bigint,
          'a bounded vector reports its size');
SELECT is(nvals('int32(4)'::vector), 0::bigint,
          'an empty vector has no stored values');
SELECT results_eq(
    $$SELECT i, v::text FROM elements('int32(4)[0:10 2:30]'::vector) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:10'::text), (2::bigint, 'int32:30'::text)$$,
    'elements returns sorted sparse coordinates');
SELECT is(nvals('{10,NULL,30}'::integer[]::vector), 2::bigint,
          'NULL array members are absent vector elements');
SELECT is(('int32(3)[0:10 2:30]'::vector::integer[])::text,
          '{10,NULL,30}',
          'casting a vector to an array preserves absent positions');
SELECT is(size('{10,NULL,30}'::integer[]::vector), 3::bigint,
          'an array cast creates a bounded vector');
SELECT is((type('int32[0:10 2:30]'::vector))::text, 'int32',
          'a sparse literal reports its element type');

-- Element access and lifecycle.
SELECT is((get_element('int32(4)[0:10 2:30]'::vector, 2))::text,
          'int32:30', 'get_element returns the stored scalar');
SELECT ok(contains('int32(4)[0:10 2:30]'::vector, 2),
          'contains finds a stored coordinate');
SELECT ok(NOT contains('int32(4)[0:10 2:30]'::vector, 1),
          'contains rejects an absent coordinate');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(set_element('int32(4)'::vector, 1, 20::scalar)) ORDER BY i$$,
    $$VALUES (1::bigint, 'int32:20'::text)$$,
    'set_element inserts a value');
SELECT is((get_element(set_element('int32(4)[1:20]'::vector, 1, 25::scalar), 1))::text,
          'int32:25', 'set_element replaces an existing value');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(remove_element('int32(4)[0:10 2:30]'::vector, 0)) ORDER BY i$$,
    $$VALUES (2::bigint, 'int32:30'::text)$$,
    'remove_element removes a stored value');
SELECT ok(eq(remove_element('int32(4)[0:10 2:30]'::vector, 1),
             'int32(4)[0:10 2:30]'::vector),
          'remove_element is a no-op for an absent coordinate');
SELECT ok(eq(dup('int32(4)[0:10 2:30]'::vector),
             'int32(4)[0:10 2:30]'::vector),
          'dup preserves vector contents');
SELECT ok(eq(wait('int32(4)[0:10 2:30]'::vector),
             'int32(4)[0:10 2:30]'::vector),
          'wait preserves vector contents');
SELECT is(nvals(clear('int32(4)[0:10 2:30]'::vector)), 0::bigint,
          'clear removes all stored values');

-- Element-wise operations, reduction, selection, and apply.
SELECT results_eq(
    $$SELECT i, v::text FROM elements(eadd(
        'int32(4)[0:10 2:30]'::vector,
        'int32(4)[1:5 2:7]'::vector,
        'plus_int32'::binaryop)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:10'::text),
             (1::bigint, 'int32:5'::text),
             (2::bigint, 'int32:37'::text)$$,
    'eadd combines values at matching coordinates');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(emult(
        'int32(4)[0:10 2:30]'::vector,
        'int32(4)[1:5 2:7]'::vector,
        'times_int32'::binaryop)) ORDER BY i$$,
    $$VALUES (2::bigint, 'int32:210'::text)$$,
    'emult keeps only matching coordinates');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(eunion(
        'int32(4)[0:10]'::vector, 1::scalar,
        'int32(4)[1:20]'::vector, 2::scalar,
        'plus_int32'::binaryop)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:12'::text),
             (1::bigint, 'int32:21'::text)$$,
    'eunion applies defaults to absent coordinates');
SELECT is((reduce_scalar('int32(4)[0:10 2:30]'::vector,
                         'plus_monoid_int32'::monoid))::text,
          'int32:40', 'reduce_scalar sums stored values');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(apply(
        'int32(4)[0:-10 2:30]'::vector,
        'abs_int32'::unaryop)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:10'::text),
             (2::bigint, 'int32:30'::text)$$,
    'apply transforms stored values without densifying');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(choose(
        'int32(4)[0:10 1:5 2:30]'::vector,
        'valuegt_int32'::indexunaryop, 10::scalar)) ORDER BY i$$,
    $$VALUES (2::bigint, 'int32:30'::text)$$,
    'choose selects values matching a predicate');

-- Assignment and extraction.
SELECT results_eq(
    $$SELECT i, v::text FROM elements(assign(
        'int32(5)[0:1 2:3 4:5]'::vector,
        'int32(2)[0:10 1:20]'::vector,
        ARRAY[1, 3]::bigint[])) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:1'::text),
             (1::bigint, 'int32:10'::text),
             (2::bigint, 'int32:3'::text),
             (3::bigint, 'int32:20'::text),
             (4::bigint, 'int32:5'::text)$$,
    'assign writes a subvector at explicit indices');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(assign(
        'int32(5)[0:1 2:3]'::vector,
        9::scalar, ARRAY[1, 3]::bigint[])) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:1'::text),
             (1::bigint, 'int32:9'::text),
             (2::bigint, 'int32:3'::text),
             (3::bigint, 'int32:9'::text)$$,
    'scalar assignment writes each requested index');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(assign(
        'int32(5)[0:1 2:3]'::vector,
        'int32(2)[0:10 1:20]'::vector,
        ARRAY[0, 2]::bigint[], accum=>'plus_int32'::binaryop)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:11'::text),
             (2::bigint, 'int32:23'::text)$$,
    'assign applies an accumulator at target indices');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(extract_vector(
        'int32(5)[0:10 2:30 4:50]'::vector,
        ARRAY[4, 1, 0]::bigint[])) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:50'::text),
             (2::bigint, 'int32:10'::text)$$,
    'extract_vector maps requested source coordinates');

-- Size and type transformations.
SELECT is(size(resize('int32(5)[0:1 2:3 4:5]'::vector, 3)), 3::bigint,
          'resize changes the vector bound');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(resize(
        'int32(5)[0:1 2:3 4:5]'::vector, 3)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:1'::text),
             (2::bigint, 'int32:3'::text)$$,
    'resize drops values outside the new bound');
SELECT is((type(cast_to('int32[0:1 2:3]'::vector, 'fp64'::type)))::text,
          'fp64', 'cast_to changes the vector element type');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(cast_to(
        'int32[0:1 2:3]'::vector, 'fp64'::type)) ORDER BY i$$,
    $$VALUES (0::bigint, 'fp64:1.000000'::text),
             (2::bigint, 'fp64:3.000000'::text)$$,
    'cast_to preserves sparse coordinates and converts values');

SELECT * FROM finish();

ROLLBACK;
