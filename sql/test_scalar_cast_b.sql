-- # Scalar 'bool' Casting
--
-- This documentation is also tests for the code, the examples below
-- show the literal output of these statements from Postgres.
--
-- Some setup to make sure warnings are shown, and that the extension
-- is installed.
set client_min_messages = 'WARNING';
create extension if not exists onesparse;

-- These casting functions cast the Postgres type bool to the
-- GraphBLAS scalar type GrB_BOOL.

select cast(0::bool as scalar);
select cast(0::bool as scalar);
select cast(1::bool as scalar);

-- These tests cast back from the scalar type GrB_BOOL to the
-- Postgres type bool

select cast(0::bool::scalar as bool);
select cast(0::bool::scalar as  bool);
select cast(1::bool::scalar as scalar);
