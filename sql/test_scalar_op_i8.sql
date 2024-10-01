-- # Scalar Ops
--
-- This documentation is also tests for the code, the examples below
-- show the literal output of these statements from Postgres.
--
-- Some setup to make sure warnings are shown, and that the extension
-- is installed.
set client_min_messages = 'WARNING';
create extension if not exists onesparse;

-- Test various scalar math operations with native Postgres types
select (1::bigint)::scalar + -1::bigint;
select 1::bigint + (-1::bigint)::scalar;
select (1::bigint)::scalar - 1::bigint;
select 1::bigint - (1::bigint)::scalar;
select (1::bigint)::scalar * -1::bigint;
select 1::bigint * (-1::bigint)::scalar;
select (1::bigint)::scalar / 1::bigint;
select 1::bigint / (1::bigint)::scalar;
