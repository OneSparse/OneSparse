-- # Vectors
--
-- This documentation is also tests for the code, the examples below
-- show the literal output of these statements from Postgres.
--
-- Some setup to make sure warnings are shown, and that the extension
-- is installed.

set client_min_messages = 'WARNING';
create extension if not exists onesparse;

-- Describe the vector type:

\dT+ vector

-- An empty vector can be constructed many ways, but one of the
-- simplest is casting a type code to the matrix type.  In this case
-- `i4` means GrB_INT32.  The type codes are intentionally compressed
-- to be as short as possible for smaller pg_dumps.
--
-- Much of these functions are basically vector versions of the same
-- functions for matrix.  See those docs for details:

select 'i4'::vector;

select nvals('i4'::vector);

select size('i4'::vector);

select 'i4[]'::vector;

select nvals('i4[]'::vector);

select size('i4[]'::vector);

select 'i4(10)'::vector;

select nvals('i4(10)'::vector);

select size('i4(10)'::vector);

select 'i4(10)[]'::vector;

select nvals('i4(10)[]'::vector);

select size('i4(10)[]'::vector);

select 'i4[0:1 1:2 2:3]'::vector;

select nvals('i4[0:1 1:2 2:3]'::vector);

select 'i4(10)[0:1 1:2 2:3]'::vector;

select size('i4(10)[0:1 1:2 2:3]'::vector);

select size('i4(2)[0:1 1:2 2:3]'::vector);

select ewise_add('i4[0:1 1:2 2:3]'::vector, 'i4[0:1 1:2 2:3]'::vector, 'plus_int32');

select ewise_mult('i4[0:1 1:2 2:3]'::vector, 'i4[0:1 1:2 2:3]'::vector, 'times_int32');

select ewise_union('i4[0:1 1:2 2:3]'::vector, 42, 'i4[0:1 1:2 2:3]'::vector, 84, 'plus_int32');

select reduce_scalar('i4[0:1 1:2 2:3]'::vector, 'plus_monoid_int32');

select selection('i4[0:1 1:2 2:3]'::vector, 'valuegt_int32', 1);

select apply('i4[1:1 2:2 3:3]'::vector, 'ainv_int32'::unaryop);

select wait('i4[0:1 1:2 2:3]'::vector);

select dup('i4[0:1 1:2 2:3]'::vector);

select clear('i4[0:1 1:2 2:3]'::vector);
