BEGIN;

CREATE EXTENSION IF NOT EXISTS onesparse;
CREATE EXTENSION IF NOT EXISTS pgtap;
SET LOCAL search_path = public, onesparse;

SELECT plan(57);

-- Constructors, casts, lifecycle, floating-point behavior, and errors.

-- Construction and representation. These cases mirror the scalar doctest
-- template, but assert values rather than formatted terminal output.
SELECT is(('int32:42'::scalar)::text, 'int32:42', 'construct an int32 scalar from text');
SELECT is((42::int4::scalar)::text, 'int32:42', 'construct an int32 scalar from a native value');
SELECT is((3.14::float8::scalar)::text, 'fp64:3.140000', 'construct an fp64 scalar');
SELECT is((true::bool::scalar)::text, 'bool:t', 'construct a boolean scalar');
SELECT is((127::int2::scalar)::text, 'int16:127', 'construct an int16 scalar');
SELECT is((2147483647::int8::scalar)::text, 'int64:2147483647', 'construct an int64 scalar');
SELECT is((3.14::float4::scalar)::text, 'fp32:3.140000', 'construct an fp32 scalar');
SELECT is(('uint32:42'::scalar)::text, 'uint32:42', 'construct an unsigned scalar');
SELECT is((scalar_integer(42))::text, 'int32:42', 'construct with the explicit constructor');
SELECT is((integer_scalar('int32:42'::scalar)), 42, 'extract with the explicit cast function');

-- Empty scalars retain their type and do not count as having an element.
SELECT is(('int32'::scalar)::text, 'int32', 'construct an empty scalar');
SELECT is(nvals('int32'::scalar), 0::int2, 'empty scalar has no value');
SELECT is(nvals('int32:0'::scalar), 1::int2, 'zero is still a stored value');
SELECT is(('int32:42'::scalar)::int4, 42, 'extract an int32 value');
SELECT is(('fp64:3.14'::scalar)::float8, 3.14::float8, 'extract an fp64 value');
SELECT is(('bool:true'::scalar)::bool, true, 'extract a boolean value');

-- Scalar arithmetic with native PostgreSQL values.
SELECT is((('int32:10'::scalar + 5)::text), 'int32:15', 'scalar plus native');
SELECT is((5 + 'int32:10'::scalar), 15, 'native plus scalar');
SELECT is((('int32:20'::scalar - 5)::text), 'int32:15', 'scalar minus native');
SELECT is((30 - 'int32:10'::scalar), 20, 'native minus scalar');
SELECT is((('int32:7'::scalar * 6)::text), 'int32:42', 'scalar times native');
SELECT is((6 * 'int32:7'::scalar), 42, 'native times scalar');
SELECT is((('int32:20'::scalar / 4)::text), 'int32:5', 'scalar divided by native');
SELECT is((100 / 'int32:5'::scalar), 20, 'native divided by scalar');
SELECT is((('fp64:10.0'::scalar / 2.5)::float8), 4.0::float8, 'floating-point scalar division');
SELECT is((('int32:7'::scalar / 2)::int4), 3, 'integer scalar division truncates');

-- Type conversion and promotion.
SELECT is(('fp64:3.9'::scalar)::int4, 3, 'float to integer conversion truncates');
SELECT is(('fp64:-3.9'::scalar)::int4, -3, 'negative float to integer conversion truncates');
SELECT is(('bool:true'::scalar)::int4, 1, 'true converts to one');
SELECT is(('bool:false'::scalar)::int4, 0, 'false converts to zero');
SELECT is((0::int4::scalar)::bool, false, 'zero converts to false');
SELECT is(((-1)::int4::scalar)::bool, true, 'nonzero converts to true');
SELECT is(('int32:42'::scalar::int4::float8::scalar)::text,
          'fp64:42.000000', 'conversion through a native type changes scalar type');
SELECT is((('int32:100'::scalar + 1::int2)::text), 'int16:101', 'promotion to int16');
SELECT is((('int32:100'::scalar + 1::int8)::text), 'int64:101', 'promotion to int64');
SELECT is((('int32:10'::scalar + 3.14::float8)::text), 'fp64:13.140000', 'promotion to fp64');

-- Mutation and lifecycle operations.
SELECT is((set('int32:42'::scalar, 100)::text), 'int32:100', 'set replaces a scalar value');
SELECT is((set('int32'::scalar, 42)::text), 'int32:42', 'set populates an empty scalar');
SELECT is((set('int32:42'::scalar, 0)::text), 'int32:0', 'set stores zero');
SELECT is((dup('int32:42'::scalar)::text), 'int32:42', 'dup preserves the value');
SELECT is((wait('int32:42'::scalar)::text), 'int32:42', 'wait preserves the value');
SELECT is(nvals(clear('int32:42'::scalar)), 0::int2, 'clear removes the stored value');
SELECT is((clear('int32'::scalar)::text), 'int32', 'clear is safe on an empty scalar');
SELECT is(print('int32:42'::scalar), '42', 'print returns the scalar value');
SELECT is(print('int32'::scalar), '', 'print returns an empty string for no value');
SELECT is((type('int32:42'::scalar))::text, 'int32', 'type reports the scalar type');
SELECT is((type('fp64:3.14'::scalar))::text, 'fp64', 'type reports floating-point type');

-- Boolean and special floating-point values.
SELECT is((false::bool::scalar)::text, 'bool:f', 'construct false');
SELECT is(('Infinity'::float8::scalar)::text, 'fp64:Infinity', 'construct positive infinity');
SELECT is(('-Infinity'::float8::scalar)::text, 'fp64:-Infinity', 'construct negative infinity');
SELECT is(('NaN'::float8::scalar)::text, 'fp64:NaN', 'construct NaN');
SELECT ok(('fp64:1.0'::scalar)::float8 < 'Infinity'::float8,
          'finite scalar remains below infinity');
SELECT is((('fp64:1.0'::scalar + 'Infinity'::float8)::text), 'fp64:Infinity', 'finite plus infinity');
SELECT is((('fp64:0.0'::scalar / 0.0)::text), 'fp64:NaN', 'zero divided by zero produces NaN');

-- SQL NULL behavior is intentionally an error for scalar arithmetic. A NULL
-- cast creates a typed empty scalar, matching the documented behavior.
SELECT is(nvals(NULL::int4::scalar), 0::int2, 'NULL cast creates an empty typed scalar');
SELECT throws_ok(
    $$SELECT 'int32:42'::scalar + NULL::int4$$,
    'XX000',
    'Cannot pass NULL to scalar_plus_int32',
    'scalar plus NULL is rejected');
SELECT throws_ok(
    $$SELECT NULL::int4 + 'int32:42'::scalar$$,
    'XX000',
    'Cannot pass NULL to plus_scalar_int32',
    'NULL plus scalar is rejected');

SELECT * FROM finish();

ROLLBACK;
