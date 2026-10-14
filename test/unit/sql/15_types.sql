BEGIN;

CREATE EXTENSION IF NOT EXISTS onesparse;
CREATE EXTENSION IF NOT EXISTS pgtap;
SET LOCAL search_path = public, onesparse;

SELECT plan(22);

-- Type names expose the GraphBLAS type registry through the SQL interface.
SELECT is(name('int32'::type), 'int32', 'int32 type reports its name');
SELECT is(name('fp64'::type), 'fp64', 'fp64 type reports its name');
SELECT is(name('bool'::type), 'bool', 'bool type reports its name');

-- jit_type emits a C layout for a composite PostgreSQL type.
CREATE TYPE jit_fixture AS (
    id integer,
    score double precision,
    code character(3),
    flags bit(9)
);

SELECT is(
    jit_type('jit_fixture'::regtype),
    E'typedef struct {\n    int32_t id;\n    double score;\n    char code[3];\n    unsigned char flags[2];\n} jit_fixture;',
    'jit_type emits C fields for supported composite attributes');
SELECT throws_ok(
    $$SELECT jit_type('integer'::regtype)$$,
    'XX000',
    'OID 23 is not a composite type',
    'jit_type rejects a non-composite type');

-- Row/scalar conversion uses the same fixed C layout emitted by jit_type.
SELECT is(
    scalar_row(
        row_scalar(ROW(7, 2.5::float8, 'xy'::character(3), B'101010101'::bit(9))::jit_fixture),
        NULL::jit_fixture)::text,
    '(7,2.5,"xy ",101010101)',
    'row_scalar and scalar_row round-trip a fixed-layout composite');
SELECT is(
    row_scalar(ROW(7, 2.5::float8, 'xy'::character(3), B'101010101'::bit(9))::jit_fixture),
    row_scalar(ROW(7, 2.5::float8, 'xy'::character(3), B'101010101'::bit(9))::jit_fixture),
    'row_scalar has a deterministic representation for equal records');
SELECT throws_ok(
    $$SELECT row_scalar(ROW(NULL::integer, 2.5::float8, 'xy'::character(3), B'101010101'::bit(9))::jit_fixture)$$,
    'XX000',
    'attribute "id" cannot be NULL',
    'row_scalar rejects a null fixed-layout field');

-- Output functions must be invoked directly; normal pgTAP comparisons do not
-- necessarily coerce an extension value through PostgreSQL type output.
SELECT is(descriptor_out('sc'::descriptor)::text, 'sc',
          'descriptor output round-trips its name');
SELECT is(unaryop_out('abs_int32'::unaryop)::text, 'abs_int32',
          'unaryop output round-trips its name');
SELECT is(name('abs_int32'::unaryop), 'abs_int32',
          'unaryop reports its name');
SELECT is(indexunaryop_out('valuegt_int32'::indexunaryop)::text, 'valuegt_int32',
          'indexunaryop output round-trips its name');
SELECT is(name('valuegt_int32'::indexunaryop), 'valuegt_int32',
          'indexunaryop reports its name');
SELECT is(binaryop_out('plus_int32'::binaryop)::text, 'plus_int32',
          'binaryop output round-trips its name');
SELECT is(monoid_out('plus_monoid_int32'::monoid)::text, 'plus_monoid_int32',
          'monoid output round-trips its name');
SELECT is(name('plus_monoid_int32'::monoid), 'plus_monoid_int32',
          'monoid reports its name');
SELECT is(semiring_out('plus_times_int32'::semiring)::text, 'plus_times_int32',
          'semiring output round-trips its name');
SELECT is(name('plus_times_int32'::semiring), 'plus_times_int32',
          'semiring reports its name');

-- Scalar lifecycle functions preserve their type for empty and populated values.
SELECT is((type('uint64'::scalar))::text, 'uint64',
          'type reports an empty scalar type');
SELECT is(nvals(clear('int32'::scalar)), 0::int2,
          'clear preserves an empty scalar state');
SELECT is((dup('int16'::scalar))::text, 'int16',
          'dup preserves an empty scalar type');
SELECT is((wait('int32:42'::scalar, 1))::text, 'int32:42',
          'wait accepts an explicit completion mode');

SELECT * FROM finish();

ROLLBACK;
