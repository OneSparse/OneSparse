BEGIN;

CREATE EXTENSION IF NOT EXISTS onesparse;
CREATE EXTENSION IF NOT EXISTS pgtap;
SET LOCAL search_path = public, onesparse;

SELECT plan(69);

-- Construction and sparse representation.
SELECT is((type('int32'::vector))::text, 'int32',
           'an empty vector retains its element type');
SELECT is(size(vector('int16'::type, 3)), 3::bigint,
          'vector constructor creates a bounded vector');
SELECT is((type(vector('int16'::type, 3)))::text, 'int16',
          'vector constructor retains its requested type');
SELECT is(nvals(vector('int16'::type, 3)), 0::bigint,
          'vector constructor creates an empty vector');
SELECT is(vector_out('int32(4)[0:10 2:30]'::vector)::text,
          'int32(4)[0:10 2:30]',
          'vector output round-trips a sparse literal');
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

-- Every built-in vector type has a canonical text I/O round trip.
SELECT is(vector_out(vector_in(vector_out('bool(2)[1:t]'::vector)))::text, 'bool(2)[1:t]',
          'vector text I/O round-trips true');
SELECT is(vector_out(vector_in(vector_out('bool(2)[1:f]'::vector)))::text, 'bool(2)[1:f]',
          'vector text I/O round-trips false');
SELECT is(vector_out(vector_in(vector_out('int8(2)[1:-42]'::vector)))::text, 'int8(2)[1:-42]',
          'vector text I/O round-trips int8');
SELECT is(vector_out(vector_in(vector_out('uint8(2)[1:200]'::vector)))::text, 'uint8(2)[1:200]',
          'vector text I/O round-trips uint8');
SELECT is(vector_out(vector_in(vector_out('int16(2)[1:-42]'::vector)))::text, 'int16(2)[1:-42]',
          'vector text I/O round-trips int16');
SELECT is(vector_out(vector_in(vector_out('uint16(2)[1:60000]'::vector)))::text, 'uint16(2)[1:60000]',
          'vector text I/O round-trips uint16');
SELECT is(vector_out(vector_in(vector_out('int32(2)[1:-42]'::vector)))::text, 'int32(2)[1:-42]',
          'vector text I/O round-trips int32');
SELECT is(vector_out(vector_in(vector_out('int64(2)[1:-42]'::vector)))::text, 'int64(2)[1:-42]',
          'vector text I/O round-trips int64');
SELECT is(vector_out(vector_in(vector_out('uint64(2)[1:9223372036854775808]'::vector)))::text,
          'uint64(2)[1:9223372036854775808]', 'vector text I/O round-trips uint64');
SELECT is(vector_out(vector_in(vector_out('uint32(2)[1:4000000000]'::vector)))::text,
          'uint32(2)[1:4000000000]', 'vector text I/O round-trips uint32');
SELECT is(vector_out(vector_in(vector_out('fp32(2)[1:1.25]'::vector)))::text, 'fp32(2)[1:1.250000]',
          'vector text I/O round-trips fp32');
SELECT is(vector_out(vector_in(vector_out('fp64(2)[1:-1.25]'::vector)))::text, 'fp64(2)[1:-1.250000]',
          'vector text I/O round-trips fp64');

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
SELECT ok(info('int32(4)[0:10 2:30]'::vector, 0) IS NOT NULL,
          'info accepts the lowest GraphBLAS print level');
SELECT throws_ok(
    $$SELECT info('int32(4)[0:10]'::vector, 6)$$,
    'XX000',
    'Print level is from 0 to 5',
    'info rejects an unsupported print level');

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
SELECT is((reduce_scalar('int32(4)[0:10 2:30]'::vector))::text,
          'int32:40', 'reduce_scalar resolves the default monoid');
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
SELECT results_eq(
    $$SELECT i, v::text FROM elements(extract_vector(
        'int32(3)[0:10 2:30]'::vector)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:10'::text),
             (2::bigint, 'int32:30'::text)$$,
    'extract_vector accepts default all-index selection');

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
-- Exercise every remaining built-in GraphBLAS target type.
SELECT is((type(cast_to('int32(2)[0:1]'::vector, 'bool'::type)))::text, 'bool',
          'cast_to supports boolean vectors');
SELECT is((type(cast_to('int32(2)[0:1]'::vector, 'int8'::type)))::text, 'int8',
          'cast_to supports int8 vectors');
SELECT is((type(cast_to('int32(2)[0:1]'::vector, 'int16'::type)))::text, 'int16',
          'cast_to supports int16 vectors');
SELECT is((type(cast_to('int32(2)[0:1]'::vector, 'int32'::type)))::text, 'int32',
          'cast_to supports int32 vectors');
SELECT is((type(cast_to('int32(2)[0:1]'::vector, 'int64'::type)))::text, 'int64',
          'cast_to supports int64 vectors');
SELECT is((type(cast_to('int32(2)[0:1]'::vector, 'uint8'::type)))::text, 'uint8',
          'cast_to supports uint8 vectors');
SELECT is((type(cast_to('int32(2)[0:1]'::vector, 'uint16'::type)))::text, 'uint16',
          'cast_to supports uint16 vectors');
SELECT is((type(cast_to('int32(2)[0:1]'::vector, 'uint32'::type)))::text, 'uint32',
          'cast_to supports uint32 vectors');
SELECT is((type(cast_to('int32(2)[0:1]'::vector, 'uint64'::type)))::text, 'uint64',
          'cast_to supports uint64 vectors');
SELECT is((type(cast_to('int32(2)[0:1]'::vector, 'fp32'::type)))::text, 'fp32',
           'cast_to supports fp32 vectors');
SELECT ok(eq(cast_to('int32(2)[0:1]'::vector, 'int32'::type),
             'int32(2)[0:1]'::vector),
          'cast_to preserves an identity conversion');
SELECT is(nvals(cast_to('int32(3)'::vector, 'bool'::type)), 0::bigint,
          'cast_to preserves an empty vector');

-- Vector products, normalization, and dense hashing use small exact inputs.
SELECT results_eq(
    $$SELECT i, v::text FROM elements(norm('fp32(2)[0:3 1:4]'::vector)) ORDER BY i$$,
    $$VALUES (0::bigint, 'fp32:0.600000'::text),
             (1::bigint, 'fp32:0.800000'::text)$$,
    'norm scales a vector to unit length');
SELECT is(lsh('fp64(3)[0:-1 1:0 2:2]'::vector), 6::bigint,
          'lsh sets bits for non-negative dense vector values');
SELECT throws_ok(
    $$SELECT lsh('fp64(3)[0:1 2:2]'::vector)$$,
    'XX000',
    'Vector must be dense.',
    'lsh rejects sparse vectors');
SELECT throws_ok(
    $$SELECT lsh(vector('fp64'::type, 61))$$,
    'XX000',
    'Vector cannot be larger than 60.',
    'lsh rejects vectors larger than its hash width');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(vxv(
        'int32(2)[0:2 1:3]'::vector,
        'int32(2)[0:4 1:5]'::vector,
        'plus_times_int32'::semiring)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:8'::text),
             (1::bigint, 'int32:12'::text)$$,
    'vxv multiplies vectors using the requested semiring');

SELECT * FROM finish();

ROLLBACK;
