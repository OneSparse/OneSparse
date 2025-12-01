--
-- ## Scalar `integer`
--
-- Test various scalar math operations with native Postgres types

-- Scalar-native arithmetic (scalar op native)
select (1::integer)::scalar + -1::integer;
select (1::integer)::scalar - 1::integer;
select (1::integer)::scalar * -1::integer;
select (1::integer)::scalar / 1::integer;

-- Native-scalar arithmetic (native op scalar)
select 1::integer + (-1::integer)::scalar;
select 1::integer - (1::integer)::scalar;
select 1::integer * (-1::integer)::scalar;
select 1::integer / (1::integer)::scalar;

-- Scalar-scalar arithmetic (via native extraction)
select ((1::integer)::scalar)::integer + ((-1::integer)::scalar)::integer;
select ((1::integer)::scalar)::integer - ((0::integer)::scalar)::integer;
select ((1::integer)::scalar)::integer * ((-1::integer)::scalar)::integer;
select ((1::integer)::scalar)::integer / ((1::integer)::scalar)::integer;

-- Test construction of min, zero and max values:
select '-1'::integer::scalar;
select '0'::integer::scalar;
select '1'::integer::scalar;

-- Test setting a scalar value from max to 2
select set('1'::integer::scalar, 2);

-- Test setting to min, zero, max
select set('0'::integer::scalar, -1::integer);
select set('1'::integer::scalar, 0::integer);
select set('-1'::integer::scalar, 1::integer);

-- Comparison operations (via extraction)
select ((1::integer)::scalar)::integer = 1::integer;
select ((-1::integer)::scalar)::integer = -1::integer;
select ((0::integer)::scalar)::integer = 0::integer;
select ((1::integer)::scalar)::integer > -1::integer;
select ((-1::integer)::scalar)::integer < 1::integer;
select ((1::integer)::scalar)::integer >= 1::integer;
select ((-1::integer)::scalar)::integer <= -1::integer;
select ((1::integer)::scalar)::integer != -1::integer;

-- Compare two scalar values (via extraction)
select ((1::integer)::scalar)::integer = ((1::integer)::scalar)::integer;
select ((1::integer)::scalar)::integer > ((-1::integer)::scalar)::integer;
select ((-1::integer)::scalar)::integer < ((1::integer)::scalar)::integer;

-- Edge case: operations with zero
select (0::integer)::scalar + 1::integer;
select (1::integer)::scalar - 0::integer;
select (0::integer)::scalar * 1::integer;
select (1::integer)::scalar * 0::integer;

-- Edge case: operations at boundaries
select (1::integer)::scalar + 0::integer;
select (-1::integer)::scalar - 0::integer;
select (1::integer)::scalar / 1::integer;
select (-1::integer)::scalar * 1::integer;

-- Test various casting functions used by the CREATE CAST machinery:
select scalar_integer((-1)::integer);
select scalar_integer((0)::integer);
select scalar_integer((1)::integer);
select integer_scalar((-1)::integer::scalar);
select integer_scalar((0)::integer::scalar);
select integer_scalar((1)::integer::scalar);

-- These casting functions cast the Postgres type `integer` to the
-- GraphBLAS scalar type `GrB_INT32`.

select cast(-1::integer as scalar);
select cast(0::integer as scalar);
select cast(1::integer as scalar);

-- These tests cast back from the scalar type `GrB_INT32` to the
-- Postgres type `integer`

select cast((-1::integer)::scalar as integer);
select cast((0::integer)::scalar as  integer);
select cast((1::integer)::scalar as scalar);

-- Round-trip conversion tests (value should be preserved)
select (-1::integer)::scalar::integer = -1::integer;
select (0::integer)::scalar::integer = 0::integer;
select (1::integer)::scalar::integer = 1::integer;

-- Multiple round-trips should preserve value
select ((-1::integer)::scalar::integer)::scalar::integer = -1::integer;
select ((1::integer)::scalar::integer)::scalar::integer = 1::integer;

-- NULL handling
select NULL::integer::scalar;
select (1::integer)::scalar + NULL::integer;
select NULL::integer + (1::integer)::scalar;
select NULL::integer * (0::integer)::scalar;

-- Utility functions
select nvals((1::integer)::scalar) as has_value;
select nvals(('int32')::scalar) as empty_scalar;
select type((1::integer)::scalar);
select print((1::integer)::scalar);
select dup((1::integer)::scalar)::integer;
select wait((1::integer)::scalar)::integer;
select nvals(clear((1::integer)::scalar)) as cleared;
