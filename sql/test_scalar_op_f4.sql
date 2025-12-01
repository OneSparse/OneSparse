--
-- ## Scalar `float4`
--
-- Test various scalar math operations with native Postgres types

-- Scalar-native arithmetic (scalar op native)
select (1::float4)::scalar + -1::float4;
select (1::float4)::scalar - 1::float4;
select (1::float4)::scalar * -1::float4;
select (1::float4)::scalar / 1::float4;

-- Native-scalar arithmetic (native op scalar)
select 1::float4 + (-1::float4)::scalar;
select 1::float4 - (1::float4)::scalar;
select 1::float4 * (-1::float4)::scalar;
select 1::float4 / (1::float4)::scalar;

-- Scalar-scalar arithmetic (via native extraction)
select ((1::float4)::scalar)::float4 + ((-1::float4)::scalar)::float4;
select ((1::float4)::scalar)::float4 - ((0::float4)::scalar)::float4;
select ((1::float4)::scalar)::float4 * ((-1::float4)::scalar)::float4;
select ((1::float4)::scalar)::float4 / ((1::float4)::scalar)::float4;

-- Test construction of min, zero and max values:
select '-1'::float4::scalar;
select '0'::float4::scalar;
select '1'::float4::scalar;

-- Test setting a scalar value from max to 2
select set('1'::float4::scalar, 2);

-- Test setting to min, zero, max
select set('0'::float4::scalar, -1::float4);
select set('1'::float4::scalar, 0::float4);
select set('-1'::float4::scalar, 1::float4);

-- Comparison operations (via extraction)
select ((1::float4)::scalar)::float4 = 1::float4;
select ((-1::float4)::scalar)::float4 = -1::float4;
select ((0::float4)::scalar)::float4 = 0::float4;
select ((1::float4)::scalar)::float4 > -1::float4;
select ((-1::float4)::scalar)::float4 < 1::float4;
select ((1::float4)::scalar)::float4 >= 1::float4;
select ((-1::float4)::scalar)::float4 <= -1::float4;
select ((1::float4)::scalar)::float4 != -1::float4;

-- Compare two scalar values (via extraction)
select ((1::float4)::scalar)::float4 = ((1::float4)::scalar)::float4;
select ((1::float4)::scalar)::float4 > ((-1::float4)::scalar)::float4;
select ((-1::float4)::scalar)::float4 < ((1::float4)::scalar)::float4;

-- Edge case: operations with zero
select (0::float4)::scalar + 1::float4;
select (1::float4)::scalar - 0::float4;
select (0::float4)::scalar * 1::float4;
select (1::float4)::scalar * 0::float4;

-- Edge case: operations at boundaries
select (1::float4)::scalar + 0::float4;
select (-1::float4)::scalar - 0::float4;
select (1::float4)::scalar / 1::float4;
select (-1::float4)::scalar * 1::float4;

-- Test various casting functions used by the CREATE CAST machinery:
select scalar_float4((-1)::float4);
select scalar_float4((0)::float4);
select scalar_float4((1)::float4);
select float4_scalar((-1)::float4::scalar);
select float4_scalar((0)::float4::scalar);
select float4_scalar((1)::float4::scalar);

-- These casting functions cast the Postgres type `float4` to the
-- GraphBLAS scalar type `GrB_INT32`.

select cast(-1::float4 as scalar);
select cast(0::float4 as scalar);
select cast(1::float4 as scalar);

-- These tests cast back from the scalar type `GrB_INT32` to the
-- Postgres type `float4`

select cast((-1::float4)::scalar as float4);
select cast((0::float4)::scalar as  float4);
select cast((1::float4)::scalar as scalar);

-- Round-trip conversion tests (value should be preserved)
select (-1::float4)::scalar::float4 = -1::float4;
select (0::float4)::scalar::float4 = 0::float4;
select (1::float4)::scalar::float4 = 1::float4;

-- Multiple round-trips should preserve value
select ((-1::float4)::scalar::float4)::scalar::float4 = -1::float4;
select ((1::float4)::scalar::float4)::scalar::float4 = 1::float4;

-- NULL handling
select NULL::float4::scalar;
select (1::float4)::scalar + NULL::float4;
select NULL::float4 + (1::float4)::scalar;
select NULL::float4 * (0::float4)::scalar;

-- Utility functions
select nvals((1::float4)::scalar) as has_value;
select nvals(('fp32')::scalar) as empty_scalar;
select type((1::float4)::scalar);
select print((1::float4)::scalar);
select dup((1::float4)::scalar)::float4;
select wait((1::float4)::scalar)::float4;
select nvals(clear((1::float4)::scalar)) as cleared;
