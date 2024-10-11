-- # Scalar `float4`
--
-- Test various scalar math operations with native Postgres types
select (1::float4)::scalar + -1::float4;
select 1::float4 + (-1::float4)::scalar;
select (1::float4)::scalar - 1::float4;
select 1::float4 - (1::float4)::scalar;
select (1::float4)::scalar * -1::float4;
select 1::float4 * (-1::float4)::scalar;
select (1::float4)::scalar / 1::float4;
select 1::float4 / (1::float4)::scalar;

-- Test construction of min, zero and max values:
select '-1'::float4::scalar;
select '0'::float4::scalar;
select '1'::float4::scalar;

-- Test setting a scalar value from max to 2
select set('1'::float4::scalar, 2);

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
