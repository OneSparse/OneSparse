-- # Scalar `{type.pgtype}` Operators
--
-- This documentation is also tests for the code, the examples below
-- show the literal output of these statements from Postgres.
--
-- Some setup to make sure warnings are shown, and that the extension
-- is installed.
set client_min_messages = 'WARNING';
create extension if not exists onesparse;

-- Test various scalar math operations with native Postgres types
select ({type.max}::{type.pgtype})::scalar + {type.min}::{type.pgtype};
select {type.max}::{type.pgtype} + ({type.min}::{type.pgtype})::scalar;
select ({type.max}::{type.pgtype})::scalar - {type.max}::{type.pgtype};
select {type.max}::{type.pgtype} - ({type.max}::{type.pgtype})::scalar;
select ({type.max}::{type.pgtype})::scalar * {type.min}::{type.pgtype};
select {type.max}::{type.pgtype} * ({type.min}::{type.pgtype})::scalar;
select ({type.max}::{type.pgtype})::scalar / {type.max}::{type.pgtype};
select {type.max}::{type.pgtype} / ({type.max}::{type.pgtype})::scalar;
