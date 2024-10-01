-- # Scalar `smallint` Casting
--
-- This documentation is also tests for the code, the examples below
-- show the literal output of these statements from Postgres.
--
-- Some setup to make sure warnings are shown, and that the extension
-- is installed.
set client_min_messages = 'WARNING';
create extension if not exists onesparse;

-- These casting functions cast the Postgres type `smallint` to the
-- GraphBLAS scalar type `GrB_INT16`.

select cast(-1::smallint as scalar);
select cast(0::smallint as scalar);
select cast(1::smallint as scalar);

-- These tests cast back from the scalar type `GrB_INT16` to the
-- Postgres type `smallint`

select cast(-1::smallint::scalar as smallint);
select cast(0::smallint::scalar as  smallint);
select cast(1::smallint::scalar as scalar);
