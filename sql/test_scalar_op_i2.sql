--
-- ## Scalar `smallint`
--
-- Test various scalar math operations with native Postgres types

-- Scalar-native arithmetic (scalar op native)
select (1::smallint)::scalar + -1::smallint;
select (1::smallint)::scalar - 1::smallint;
select (1::smallint)::scalar * -1::smallint;
select (1::smallint)::scalar / 1::smallint;

-- Native-scalar arithmetic (native op scalar)
select 1::smallint + (-1::smallint)::scalar;
select 1::smallint - (1::smallint)::scalar;
select 1::smallint * (-1::smallint)::scalar;
select 1::smallint / (1::smallint)::scalar;

-- Scalar-scalar arithmetic (via native extraction)
select ((1::smallint)::scalar)::smallint + ((-1::smallint)::scalar)::smallint;
select ((1::smallint)::scalar)::smallint - ((0::smallint)::scalar)::smallint;
select ((1::smallint)::scalar)::smallint * ((-1::smallint)::scalar)::smallint;
select ((1::smallint)::scalar)::smallint / ((1::smallint)::scalar)::smallint;

-- Test construction of min, zero and max values:
select '-1'::smallint::scalar;
select '0'::smallint::scalar;
select '1'::smallint::scalar;

-- Test setting a scalar value from max to 2
select set('1'::smallint::scalar, 2);

-- Test setting to min, zero, max
select set('0'::smallint::scalar, -1::smallint);
select set('1'::smallint::scalar, 0::smallint);
select set('-1'::smallint::scalar, 1::smallint);

-- Comparison operations (via extraction)
select ((1::smallint)::scalar)::smallint = 1::smallint;
select ((-1::smallint)::scalar)::smallint = -1::smallint;
select ((0::smallint)::scalar)::smallint = 0::smallint;
select ((1::smallint)::scalar)::smallint > -1::smallint;
select ((-1::smallint)::scalar)::smallint < 1::smallint;
select ((1::smallint)::scalar)::smallint >= 1::smallint;
select ((-1::smallint)::scalar)::smallint <= -1::smallint;
select ((1::smallint)::scalar)::smallint != -1::smallint;

-- Compare two scalar values (via extraction)
select ((1::smallint)::scalar)::smallint = ((1::smallint)::scalar)::smallint;
select ((1::smallint)::scalar)::smallint > ((-1::smallint)::scalar)::smallint;
select ((-1::smallint)::scalar)::smallint < ((1::smallint)::scalar)::smallint;

-- Edge case: operations with zero
select (0::smallint)::scalar + 1::smallint;
select (1::smallint)::scalar - 0::smallint;
select (0::smallint)::scalar * 1::smallint;
select (1::smallint)::scalar * 0::smallint;

-- Edge case: operations at boundaries
select (1::smallint)::scalar + 0::smallint;
select (-1::smallint)::scalar - 0::smallint;
select (1::smallint)::scalar / 1::smallint;
select (-1::smallint)::scalar * 1::smallint;

-- Test various casting functions used by the CREATE CAST machinery:
select scalar_smallint((-1)::smallint);
select scalar_smallint((0)::smallint);
select scalar_smallint((1)::smallint);
select smallint_scalar((-1)::smallint::scalar);
select smallint_scalar((0)::smallint::scalar);
select smallint_scalar((1)::smallint::scalar);

-- These casting functions cast the Postgres type `smallint` to the
-- GraphBLAS scalar type `GrB_INT16`.

select cast(-1::smallint as scalar);
select cast(0::smallint as scalar);
select cast(1::smallint as scalar);

-- These tests cast back from the scalar type `GrB_INT16` to the
-- Postgres type `smallint`

select cast((-1::smallint)::scalar as smallint);
select cast((0::smallint)::scalar as  smallint);
select cast((1::smallint)::scalar as scalar);

-- Round-trip conversion tests (value should be preserved)
select (-1::smallint)::scalar::smallint = -1::smallint;
select (0::smallint)::scalar::smallint = 0::smallint;
select (1::smallint)::scalar::smallint = 1::smallint;

-- Multiple round-trips should preserve value
select ((-1::smallint)::scalar::smallint)::scalar::smallint = -1::smallint;
select ((1::smallint)::scalar::smallint)::scalar::smallint = 1::smallint;

-- NULL handling
select NULL::smallint::scalar;
select (1::smallint)::scalar + NULL::smallint;
select NULL::smallint + (1::smallint)::scalar;
select NULL::smallint * (0::smallint)::scalar;

-- Utility functions
select nvals((1::smallint)::scalar) as has_value;
select nvals(('int16')::scalar) as empty_scalar;
select type((1::smallint)::scalar);
select print((1::smallint)::scalar);
select dup((1::smallint)::scalar)::smallint;
select wait((1::smallint)::scalar)::smallint;
select nvals(clear((1::smallint)::scalar)) as cleared;
