BEGIN;

CREATE EXTENSION IF NOT EXISTS onesparse;
CREATE EXTENSION IF NOT EXISTS pgtap;
SET LOCAL search_path = public, onesparse;

SELECT plan(10);

-- Identities match the neutral element for each operation and element type.
SELECT is((ident('plus_monoid_int32'::monoid))::text, 'int32:0',
          'plus monoid identity is zero');
SELECT is((ident('times_monoid_int32'::monoid))::text, 'int32:1',
          'times monoid identity is one');
SELECT is((ident('min_monoid_int32'::monoid))::text, 'int32:2147483647',
          'min monoid identity is the largest int32 value');
SELECT is((ident('max_monoid_int32'::monoid))::text, 'int32:-2147483648',
          'max monoid identity is the smallest int32 value');

-- Monoids expose their defining GraphBLAS binary operator.
SELECT is(name(oper('plus_monoid_int32'::monoid)), 'plus_int32',
          'plus monoid exposes its plus binary operator');
SELECT is(name(oper('land_monoid_bool'::monoid)), 'land',
          'boolean land monoid exposes its binary operator');

-- Terminals are populated only for operations with a short-circuit value.
SELECT is(nvals(terminal('plus_monoid_int32'::monoid)), 0::int2,
          'plus monoid has no terminal value');
SELECT is((terminal('min_monoid_int32'::monoid))::text, 'int32:-2147483648',
          'min monoid terminal is the smallest int32 value');
SELECT is((terminal('land_monoid_bool'::monoid))::text, 'bool:f',
          'boolean land monoid terminal is false');
SELECT is((terminal('lor_monoid_bool'::monoid))::text, 'bool:t',
          'boolean lor monoid terminal is true');

SELECT * FROM finish();

ROLLBACK;
