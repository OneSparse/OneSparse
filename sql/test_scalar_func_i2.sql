-- # Scalar `smallint` Functions
--
-- This documentation is also tests for the code, the examples below
-- show the literal output of these statements from Postgres.
--
-- Some setup to make sure warnings are shown, and that the extension
-- is installed.
set client_min_messages = 'WARNING';
create extension if not exists onesparse;

-- Test construction of min, zero and max values:
select '-1'::smallint::scalar;
select '0'::smallint::scalar;
select '1'::smallint::scalar;

-- Test setting a scalar value from max to 2
select set('1'::smallint::scalar, 2);

-- Test various casting functions used by the CREATE CAST machinery:
select scalar_smallint((-1)::smallint);
select scalar_smallint((0)::smallint);
select scalar_smallint((1)::smallint);
select smallint_scalar((-1)::smallint::scalar);
select smallint_scalar((0)::smallint::scalar);
select smallint_scalar((1)::smallint::scalar);
