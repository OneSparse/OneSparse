--
-- ## Scalar `bigint`
--
-- Test various scalar math operations with native Postgres types

-- Scalar-native arithmetic (scalar op native)
select (1::bigint)::scalar + -1::bigint;
select (1::bigint)::scalar - 1::bigint;
select (1::bigint)::scalar * -1::bigint;
select (1::bigint)::scalar / 1::bigint;

-- Native-scalar arithmetic (native op scalar)
select 1::bigint + (-1::bigint)::scalar;
select 1::bigint - (1::bigint)::scalar;
select 1::bigint * (-1::bigint)::scalar;
select 1::bigint / (1::bigint)::scalar;

-- Scalar-scalar arithmetic (via native extraction)
select ((1::bigint)::scalar)::bigint + ((-1::bigint)::scalar)::bigint;
select ((1::bigint)::scalar)::bigint - ((0::bigint)::scalar)::bigint;
select ((1::bigint)::scalar)::bigint * ((-1::bigint)::scalar)::bigint;
select ((1::bigint)::scalar)::bigint / ((1::bigint)::scalar)::bigint;

-- Test construction of min, zero and max values:
select '-1'::bigint::scalar;
select '0'::bigint::scalar;
select '1'::bigint::scalar;

-- Test setting a scalar value from max to 2
select set('1'::bigint::scalar, 2);

-- Test setting to min, zero, max
select set('0'::bigint::scalar, -1::bigint);
select set('1'::bigint::scalar, 0::bigint);
select set('-1'::bigint::scalar, 1::bigint);

-- Comparison operations (via extraction)
select ((1::bigint)::scalar)::bigint = 1::bigint;
select ((-1::bigint)::scalar)::bigint = -1::bigint;
select ((0::bigint)::scalar)::bigint = 0::bigint;
select ((1::bigint)::scalar)::bigint > -1::bigint;
select ((-1::bigint)::scalar)::bigint < 1::bigint;
select ((1::bigint)::scalar)::bigint >= 1::bigint;
select ((-1::bigint)::scalar)::bigint <= -1::bigint;
select ((1::bigint)::scalar)::bigint != -1::bigint;

-- Compare two scalar values (via extraction)
select ((1::bigint)::scalar)::bigint = ((1::bigint)::scalar)::bigint;
select ((1::bigint)::scalar)::bigint > ((-1::bigint)::scalar)::bigint;
select ((-1::bigint)::scalar)::bigint < ((1::bigint)::scalar)::bigint;

-- Edge case: operations with zero
select (0::bigint)::scalar + 1::bigint;
select (1::bigint)::scalar - 0::bigint;
select (0::bigint)::scalar * 1::bigint;
select (1::bigint)::scalar * 0::bigint;

-- Edge case: operations at boundaries
select (1::bigint)::scalar + 0::bigint;
select (-1::bigint)::scalar - 0::bigint;
select (1::bigint)::scalar / 1::bigint;
select (-1::bigint)::scalar * 1::bigint;

-- Test various casting functions used by the CREATE CAST machinery:
select scalar_bigint((-1)::bigint);
select scalar_bigint((0)::bigint);
select scalar_bigint((1)::bigint);
select bigint_scalar((-1)::bigint::scalar);
select bigint_scalar((0)::bigint::scalar);
select bigint_scalar((1)::bigint::scalar);

-- These casting functions cast the Postgres type `bigint` to the
-- GraphBLAS scalar type `GrB_INT64`.

select cast(-1::bigint as scalar);
select cast(0::bigint as scalar);
select cast(1::bigint as scalar);

-- These tests cast back from the scalar type `GrB_INT64` to the
-- Postgres type `bigint`

select cast((-1::bigint)::scalar as bigint);
select cast((0::bigint)::scalar as  bigint);
select cast((1::bigint)::scalar as scalar);

-- Round-trip conversion tests (value should be preserved)
select (-1::bigint)::scalar::bigint = -1::bigint;
select (0::bigint)::scalar::bigint = 0::bigint;
select (1::bigint)::scalar::bigint = 1::bigint;

-- Multiple round-trips should preserve value
select ((-1::bigint)::scalar::bigint)::scalar::bigint = -1::bigint;
select ((1::bigint)::scalar::bigint)::scalar::bigint = 1::bigint;

-- NULL handling
select NULL::bigint::scalar;
select (1::bigint)::scalar + NULL::bigint;
select NULL::bigint + (1::bigint)::scalar;
select NULL::bigint * (0::bigint)::scalar;

-- Utility functions
select nvals((1::bigint)::scalar) as has_value;
select nvals(('int64')::scalar) as empty_scalar;
select type((1::bigint)::scalar);
select print((1::bigint)::scalar);
select dup((1::bigint)::scalar)::bigint;
select wait((1::bigint)::scalar)::bigint;
select nvals(clear((1::bigint)::scalar)) as cleared;
