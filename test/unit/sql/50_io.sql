BEGIN;

CREATE EXTENSION IF NOT EXISTS onesparse;
CREATE EXTENSION IF NOT EXISTS pgtap;
SET LOCAL search_path = public, onesparse;

SELECT plan(13);

-- In-memory SuiteSparse serialization.
SELECT ok(octet_length(serialize(
    'int32(3:4)[0:1:10 1:2:20 2:3:30]'::matrix)) > 0,
          'serialize returns a non-empty bytea');
SELECT ok(eq(
    deserialize(serialize(
        'int32(3:4)[0:1:10 1:2:20 2:3:30]'::matrix)),
    'int32(3:4)[0:1:10 1:2:20 2:3:30]'::matrix),
          'deserialize restores matrix contents');
SELECT is(nrows(deserialize(serialize(
    'int32(3:4)[0:1:10 1:2:20 2:3:30]'::matrix))), 3::bigint,
          'in-memory serialization preserves row count');
SELECT is(ncols(deserialize(serialize(
    'int32(3:4)[0:1:10 1:2:20 2:3:30]'::matrix))), 4::bigint,
          'in-memory serialization preserves column count');

-- Filesystem round trip. The test container permits writes to /tmp.
SELECT is(serialize_file(
    'int32(3:4)[0:1:10 1:2:20 2:3:30]'::matrix,
    '/tmp/onesparse-unit.grb'), true,
          'serialize_file reports a successful write');
SELECT ok(eq(
    deserialize_file('/tmp/onesparse-unit.grb'),
    'int32(3:4)[0:1:10 1:2:20 2:3:30]'::matrix),
          'deserialize_file restores matrix contents');
SELECT results_eq(
    $$SELECT i, j, v::text
        FROM elements(deserialize_file('/tmp/onesparse-unit.grb'))
        ORDER BY i, j$$,
    $$VALUES (0::bigint, 1::bigint, 'int32:10'::text),
             (1::bigint, 2::bigint, 'int32:20'::text),
             (2::bigint, 3::bigint, 'int32:30'::text)$$,
    'deserialize_file preserves sparse coordinates');

-- PostgreSQL large-object round trip.
CREATE TEMP TABLE onesparse_unit_lo (loid oid);
INSERT INTO onesparse_unit_lo
SELECT save('int32(3:4)[0:1:10 1:2:20 2:3:30]'::matrix);
SELECT ok((SELECT loid FROM onesparse_unit_lo) > 0::oid,
          'save creates a PostgreSQL large object');
SELECT ok(eq(
    load((SELECT loid FROM onesparse_unit_lo)),
    'int32(3:4)[0:1:10 1:2:20 2:3:30]'::matrix),
          'load restores a matrix from a large object');

-- Missing files are deliberate error contracts.
SELECT throws_ok(
    $$SELECT deserialize_file('/tmp/onesparse-unit-missing.grb')$$,
    'XX000', 'Failed to open file',
    'deserialize_file rejects a missing file');
SELECT throws_ok(
    $$SELECT mmread('/tmp/onesparse-unit-missing.mtx')$$,
    'XX000', 'Failed to open file',
    'mmread rejects a missing file');
SELECT throws_ok(
    $$SELECT deserialize(decode('not-a-graphblas-matrix', 'escape'))$$,
    'XX000', 'INVALID_OBJECT : Error deserializing matrix',
    'deserialize rejects corrupt input');
SELECT throws_ok(
    $$SELECT load(999999999::oid)$$,
    '42704', 'large object 999999999 does not exist',
    'load rejects a missing large object');

SELECT * FROM finish();

ROLLBACK;
