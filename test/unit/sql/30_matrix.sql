BEGIN;

CREATE EXTENSION IF NOT EXISTS onesparse;
CREATE EXTENSION IF NOT EXISTS pgtap;
SET LOCAL search_path = public, onesparse;

SELECT plan(31);

-- Construction and sparse representation.
SELECT is((type('int32'::matrix))::text, 'int32',
          'an empty matrix retains its element type');
SELECT is(nrows('int32(3:4)'::matrix), 3::bigint,
          'a bounded matrix reports its row count');
SELECT is(ncols('int32(3:4)'::matrix), 4::bigint,
          'a bounded matrix reports its column count');
SELECT is(nvals('int32(3:4)'::matrix), 0::bigint,
          'an empty matrix has no stored values');
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(
        'int32(3:4)[0:1:10 2:3:30]'::matrix) ORDER BY i, j$$,
    $$VALUES (0::bigint, 1::bigint, 'int32:10'::text),
             (2::bigint, 3::bigint, 'int32:30'::text)$$,
    'elements returns sorted sparse coordinates');
SELECT is((type('int32(3:4)[0:1:10 2:3:30]'::matrix))::text, 'int32',
          'a sparse literal reports its element type');

-- Element access and lifecycle.
SELECT is((get_element('int32(3:4)[0:1:10 2:3:30]'::matrix, 2, 3))::text,
          'int32:30', 'get_element returns the stored scalar');
SELECT ok(contains('int32(3:4)[0:1:10 2:3:30]'::matrix, 2, 3),
          'contains finds a stored coordinate');
SELECT ok(NOT contains('int32(3:4)[0:1:10 2:3:30]'::matrix, 1, 1),
          'contains rejects an absent coordinate');
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(set_element(
        'int32(3:4)'::matrix, 1, 2, 20::scalar)) ORDER BY i, j$$,
    $$VALUES (1::bigint, 2::bigint, 'int32:20'::text)$$,
    'set_element inserts a value');
SELECT is((get_element(set_element(
        'int32(3:4)[1:2:20]'::matrix, 1, 2, 25::scalar), 1, 2))::text,
          'int32:25', 'set_element replaces an existing value');
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(remove_element(
        'int32(3:4)[0:1:10 2:3:30]'::matrix, 0, 1)) ORDER BY i, j$$,
    $$VALUES (2::bigint, 3::bigint, 'int32:30'::text)$$,
    'remove_element removes a stored value');
SELECT ok(eq(remove_element('int32(3:4)[0:1:10 2:3:30]'::matrix, 1, 1),
             'int32(3:4)[0:1:10 2:3:30]'::matrix),
          'remove_element is a no-op for an absent coordinate');
SELECT ok(eq(dup('int32(3:4)[0:1:10 2:3:30]'::matrix),
             'int32(3:4)[0:1:10 2:3:30]'::matrix),
          'dup preserves matrix contents');
SELECT ok(eq(wait('int32(3:4)[0:1:10 2:3:30]'::matrix),
             'int32(3:4)[0:1:10 2:3:30]'::matrix),
          'wait preserves matrix contents');
SELECT is(nvals(clear('int32(3:4)[0:1:10 2:3:30]'::matrix)), 0::bigint,
          'clear removes all stored values');

-- Element-wise operations, reduction, transpose, and multiplication.
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(eadd(
        'int32(3:4)[0:1:10 2:3:30]'::matrix,
        'int32(3:4)[1:2:5 2:3:7]'::matrix,
        'plus_int32'::binaryop)) ORDER BY i, j$$,
    $$VALUES (0::bigint, 1::bigint, 'int32:10'::text),
             (1::bigint, 2::bigint, 'int32:5'::text),
             (2::bigint, 3::bigint, 'int32:37'::text)$$,
    'eadd combines values at matching coordinates');
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(emult(
        'int32(3:4)[0:1:10 2:3:30]'::matrix,
        'int32(3:4)[1:2:5 2:3:7]'::matrix,
        'times_int32'::binaryop)) ORDER BY i, j$$,
    $$VALUES (2::bigint, 3::bigint, 'int32:210'::text)$$,
    'emult keeps only matching coordinates');
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(eunion(
        'int32(3:4)[0:1:10]'::matrix, 1::scalar,
        'int32(3:4)[1:2:20]'::matrix, 2::scalar,
        'plus_int32'::binaryop)) ORDER BY i, j$$,
    $$VALUES (0::bigint, 1::bigint, 'int32:12'::text),
             (1::bigint, 2::bigint, 'int32:21'::text)$$,
    'eunion applies defaults to absent coordinates');
SELECT is((reduce_scalar('int32(3:4)[0:1:10 2:3:30]'::matrix,
                         'plus_monoid_int32'::monoid))::text,
          'int32:40', 'reduce_scalar sums stored values');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(reduce_cols(
        'int32(3:4)[0:1:10 2:3:30]'::matrix,
        'plus_monoid_int32'::monoid)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:10'::text),
             (2::bigint, 'int32:30'::text)$$,
    'reduce_cols sums each matrix column');
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(transpose(
        'int32(3:4)[0:1:10 2:3:30]'::matrix)) ORDER BY i, j$$,
    $$VALUES (1::bigint, 0::bigint, 'int32:10'::text),
             (3::bigint, 2::bigint, 'int32:30'::text)$$,
    'transpose exchanges row and column coordinates');
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(mxm(
        'int32(2:3)[0:0:1 0:2:2 1:1:3]'::matrix,
        'int32(3:2)[0:1:4 1:0:5 2:1:6]'::matrix,
        'plus_times_int32'::semiring)) ORDER BY i, j$$,
    $$VALUES (0::bigint, 1::bigint, 'int32:16'::text),
             (1::bigint, 0::bigint, 'int32:15'::text)$$,
    'mxm computes sparse matrix multiplication');

-- Assignment and extraction.
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(assign(
        'int32(5:5)[0:0:1 2:2:3 4:4:5]'::matrix,
        'int32(2:2)[0:0:10 1:1:20]'::matrix,
        ARRAY[1, 3]::bigint[], ARRAY[1, 3]::bigint[])) ORDER BY i, j$$,
    $$VALUES (0::bigint, 0::bigint, 'int32:1'::text),
             (1::bigint, 1::bigint, 'int32:10'::text),
             (2::bigint, 2::bigint, 'int32:3'::text),
             (3::bigint, 3::bigint, 'int32:20'::text),
             (4::bigint, 4::bigint, 'int32:5'::text)$$,
    'assign writes a submatrix at explicit indices');
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(assign(
        'int32(4:4)[0:0:1 2:2:3]'::matrix,
        9::scalar, ARRAY[1, 3]::bigint[], ARRAY[1, 3]::bigint[])) ORDER BY i, j$$,
    $$VALUES (0::bigint, 0::bigint, 'int32:1'::text),
             (1::bigint, 1::bigint, 'int32:9'::text),
             (1::bigint, 3::bigint, 'int32:9'::text),
             (2::bigint, 2::bigint, 'int32:3'::text),
             (3::bigint, 1::bigint, 'int32:9'::text),
             (3::bigint, 3::bigint, 'int32:9'::text)$$,
    'scalar assignment writes each requested coordinate');
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(extract_matrix(
        'int32(5:5)[0:0:10 2:2:30 4:4:50]'::matrix,
        ARRAY[4, 1, 0]::bigint[], ARRAY[4, 1, 0]::bigint[])) ORDER BY i, j$$,
    $$VALUES (0::bigint, 0::bigint, 'int32:50'::text),
             (2::bigint, 2::bigint, 'int32:10'::text)$$,
    'extract_matrix maps requested source coordinates');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(extract_col(
        'int32(3:4)[0:1:10 2:3:30]'::matrix, 3)) ORDER BY i$$,
    $$VALUES (2::bigint, 'int32:30'::text)$$,
    'extract_col returns a selected matrix column');

-- Size and type transformations.
SELECT is(nrows(resize('int32(5:5)[0:0:1 2:2:3 4:4:5]'::matrix, 3, 3)),
          3::bigint, 'resize changes the matrix row bound');
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(resize(
        'int32(5:5)[0:0:1 2:2:3 4:4:5]'::matrix, 3, 3)) ORDER BY i, j$$,
    $$VALUES (0::bigint, 0::bigint, 'int32:1'::text),
             (2::bigint, 2::bigint, 'int32:3'::text)$$,
    'resize drops values outside the new bounds');
SELECT is((type(cast_to('int32(3:3)[0:0:1 2:2:3]'::matrix,
                       'fp64'::type)))::text,
          'fp64', 'cast_to changes the matrix element type');
SELECT results_eq(
    $$SELECT i, j, v::text FROM elements(cast_to(
        'int32(3:3)[0:0:1 2:2:3]'::matrix, 'fp64'::type)) ORDER BY i, j$$,
    $$VALUES (0::bigint, 0::bigint, 'fp64:1.000000'::text),
             (2::bigint, 2::bigint, 'fp64:3.000000'::text)$$,
    'cast_to preserves sparse coordinates and converts values');

SELECT * FROM finish();

ROLLBACK;
