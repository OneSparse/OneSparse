-- # Scalar '{type.pgtype}' Casting
--
-- This documentation is also tests for the code, the examples below
-- show the literal output of these statements from Postgres.
--
-- Some setup to make sure warnings are shown, and that the extension
-- is installed.
set client_min_messages = 'WARNING';
create extension if not exists onesparse;

-- These casting functions cast the Postgres type {type.pgtype} to the
-- GraphBLAS scalar type {type.gbtype}.

select cast({type.min}::{type.pgtype} as scalar);
select cast({type.zero}::{type.pgtype} as scalar);
select cast({type.max}::{type.pgtype} as scalar);

-- These tests cast back from the scalar type {type.gbtype} to the
-- Postgres type {type.pgtype}

select cast({type.min}::{type.pgtype}::scalar as {type.pgtype});
select cast({type.zero}::{type.pgtype}::scalar as  {type.pgtype});
select cast({type.max}::{type.pgtype}::scalar as scalar);
