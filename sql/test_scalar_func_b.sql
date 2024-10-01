-- # Scalar `bool` Functions
--
-- This documentation is also tests for the code, the examples below
-- show the literal output of these statements from Postgres.
--
-- Some setup to make sure warnings are shown, and that the extension
-- is installed.
set client_min_messages = 'WARNING';
create extension if not exists onesparse;

-- Test construction of min, zero and max values:
select '0'::bool::scalar;
select '0'::bool::scalar;
select '1'::bool::scalar;

-- Test setting a scalar value from max to 2
select set('1'::bool::scalar, 2);

-- Test various casting functions used by the CREATE CAST machinery:
select scalar_bool((0)::bool);
select scalar_bool((0)::bool);
select scalar_bool((1)::bool);
select bool_scalar((0)::bool::scalar);
select bool_scalar((0)::bool::scalar);
select bool_scalar((1)::bool::scalar);
