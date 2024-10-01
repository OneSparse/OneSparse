-- # Scalar `float4` Functions
--
-- This documentation is also tests for the code, the examples below
-- show the literal output of these statements from Postgres.
--
-- Some setup to make sure warnings are shown, and that the extension
-- is installed.
set client_min_messages = 'WARNING';
create extension if not exists onesparse;

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
