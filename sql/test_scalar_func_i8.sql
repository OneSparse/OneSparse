-- # Scalar `bigint` Functions
--
-- This documentation is also tests for the code, the examples below
-- show the literal output of these statements from Postgres.
--
-- Some setup to make sure warnings are shown, and that the extension
-- is installed.
set client_min_messages = 'WARNING';
create extension if not exists onesparse;

-- Test construction of min, zero and max values:
select '-1'::bigint::scalar;
select '0'::bigint::scalar;
select '1'::bigint::scalar;

-- Test setting a scalar value from max to 2
select set('1'::bigint::scalar, 2);

-- Test various casting functions used by the CREATE CAST machinery:
select scalar_bigint((-1)::bigint);
select scalar_bigint((0)::bigint);
select scalar_bigint((1)::bigint);
select bigint_scalar((-1)::bigint::scalar);
select bigint_scalar((0)::bigint::scalar);
select bigint_scalar((1)::bigint::scalar);