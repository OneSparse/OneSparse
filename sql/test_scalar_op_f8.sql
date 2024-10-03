-- # Scalar `float8` Operations
--
-- Test various scalar math operations with native Postgres types
select (1::float8)::scalar + -1::float8;
select 1::float8 + (-1::float8)::scalar;
select (1::float8)::scalar - 1::float8;
select 1::float8 - (1::float8)::scalar;
select (1::float8)::scalar * -1::float8;
select 1::float8 * (-1::float8)::scalar;
select (1::float8)::scalar / 1::float8;
select 1::float8 / (1::float8)::scalar;

-- Test construction of min, zero and max values:
select '-1'::float8::scalar;
select '0'::float8::scalar;
select '1'::float8::scalar;

-- Test setting a scalar value from max to 2
select set('1'::float8::scalar, 2);

-- Test various casting functions used by the CREATE CAST machinery:
select scalar_float8((-1)::float8);
select scalar_float8((0)::float8);
select scalar_float8((1)::float8);
select float8_scalar((-1)::float8::scalar);
select float8_scalar((0)::float8::scalar);
select float8_scalar((1)::float8::scalar);

-- These casting functions cast the Postgres type `float8` to the
-- GraphBLAS scalar type `GrB_INT64`.

select cast(-1::float8 as scalar);
select cast(0::float8 as scalar);
select cast(1::float8 as scalar);

-- These tests cast back from the scalar type `GrB_INT64` to the
-- Postgres type `float8`

select cast((-1::float8)::scalar as float8);
select cast((0::float8)::scalar as  float8);
select cast((1::float8)::scalar as scalar);
