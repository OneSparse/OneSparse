-- # Scalar Functions
--
-- This documentation is also tests for the code, the examples below
-- show the literal output of these statements from Postgres.
--
-- Some setup to make sure warnings are shown, and that the extension
-- is installed.
set client_min_messages = 'WARNING';
create extension if not exists onesparse;

-- Describe the scalar type
\dT+ scalar

-- Duplicate a scalar
select dup('i4:42'::scalar);

-- Wait for a scalar to complete in non-blocking mode
select wait('i4:42'::scalar);

-- Clear a scalar, deleting its stored element.
select clear('i4:42'::scalar);
