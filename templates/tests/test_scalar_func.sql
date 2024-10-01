-- # Scalar Functions
--
-- This documentation is also tests for the code, the examples below
-- show the literal output of these statements from Postgres.
--
-- Some setup to make sure warnings are shown, and that the extension
-- is installed.
set client_min_messages = 'WARNING';
create extension if not exists onesparse;

-- Test construction of min, zero and max values:
select '{type.min}'::{type.pgtype}::scalar;
select '{type.zero}'::{type.pgtype}::scalar;
select '{type.max}'::{type.pgtype}::scalar;

-- Test setting a scalar value from max to 2
select set('{type.max}'::{type.pgtype}::scalar, 2);

-- Test various casting functions used by the CREATE CAST machinery:
select scalar_{type.pgtype}(({type.min})::{type.pgtype});
select scalar_{type.pgtype}(({type.zero})::{type.pgtype});
select scalar_{type.pgtype}(({type.max})::{type.pgtype});
select {type.pgtype}_scalar(({type.min})::{type.pgtype}::scalar);
select {type.pgtype}_scalar(({type.zero})::{type.pgtype}::scalar);
select {type.pgtype}_scalar(({type.max})::{type.pgtype}::scalar);
