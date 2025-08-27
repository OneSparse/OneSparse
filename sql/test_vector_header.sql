-- # Vector
--
-- This documentation is also tests for the code, the examples below
-- show the literal output of these statements from Postgres.
--
-- Some setup to make sure warnings are shown, and that the extension
-- is installed.

set client_min_messages = 'WARNING';
create extension if not exists onesparse;

\pset linestyle unicode
\pset border 2

-- ## Construction
--
-- Examples of creating vectors and inspecting their properties.

-- An empty vector can be constructed many ways, but one of the
-- simplest is casting a type code to the matrix type.  In this case
-- `int32` means GrB_INT32.  The type codes are intentionally compressed
-- to be as short as possible for smaller pg_dumps.
--
-- Much of these functions are basically vector versions of the same
-- functions for matrix.  See those docs for details:

-- Cast a type name to make an empty vector
select 'int32'::vector;

-- Use the constructor function
select vector('int32');

-- Count the stored elements
select nvals('int32'::vector);

-- Inspect the possible size of the vector
select size('int32'::vector);

-- Cast from an empty array
select 'int32[]'::vector;

-- Stored elements in the empty array
select nvals('int32[]'::vector);

-- Size of the unbounded empty array
select size('int32[]'::vector);

-- Create a bounded but empty vector
select 'int32(10)'::vector;

-- Bounded vectors start with zero elements
select nvals('int32(10)'::vector);

-- Its size reflects the bound
select size('int32(10)'::vector);

-- Alternate syntax for a bounded empty vector
select 'int32(10)[]'::vector;

-- It also has zero stored elements
select nvals('int32(10)[]'::vector);

-- And a fixed size
select size('int32(10)[]'::vector);

-- Create a vector with some values
select 'int32[0:1 1:2 2:3]'::vector;

-- Cast a vector from a Postgres array
select '{1,2,3}'::integer[]::vector;

-- Cast from a Postgres array with NULL indicating empty value:
select '{1,NULL,3}'::integer[]::vector;

-- Cast a Postgres array from a vector
select 'int32(3)[0:1 1:2 2:3]'::vector::integer[];

-- Cast a Postgres array from a vector with  NULL indicating empty value:
select 'int32(3)[0:1 2:3]'::vector::integer[];

-- Count those values
select nvals('int32[0:1 1:2 2:3]'::vector);

-- Iterate over the index/value pairs
select * from elements('int32[0:1 1:2 2:3]'::vector);

-- Combine bounds and elements
select 'int32(10)[0:1 1:2 2:3]'::vector;

-- Bounded size is retained
select size('int32(10)[0:1 1:2 2:3]'::vector);

-- Attempting to store outside the bounds raises an error
select size('int32(2)[0:1 1:2 2:3]'::vector);

-- ## Equality
--
-- Two matrices can be compared for equality with the '=' and '!=' operators:

select u != v as "u != v", u = v as "u = v", v = u as "v = u", v = u as "v = u" from test_fixture;

-- ## Elementwise Addition
--
-- Add two vectors element by element using a binary operator
select eadd('int32[0:1 1:2 2:3]'::vector, 'int32[0:1 1:2 2:3]'::vector, 'plus_int32');

-- Eadd can also be accomplished with binary operators specific to
-- OneSparse.  Different binaryops are passed to eadd to do different
-- elementwise operations:

select print(u |+ v) as "u |+ v", print(u |- v) as "u |- v", print(u |* v) as "u |* v", print(u |/ v) as "u |/ v" from test_fixture;
-- ## Elementwise Multiplication
--
-- Multiply corresponding elements of two vectors

select emult('int32[0:1 1:2 2:3]'::vector, 'int32[0:1 1:2 2:3]'::vector, 'times_int32');

-- Emult can also be accomplished with binary operators specific to
-- OneSparse.  Different binaryops are passed to emult to do different
-- elementwise operations:

select print(u &+ v) as "u &+ v", print(u &- v) as "u &- v", print(u &* v) as "u &* v", print(u &/ v) as "u &/ v" from test_fixture;

-- ## Elementwise Union
--
-- Union of two vectors with scalars for missing values
select eunion('int32[0:1 1:2 2:3]'::vector, 42, 'int32[0:1 1:2 2:3]'::vector, 84, 'plus_int32');

-- ## Reduction
--
-- Reduce a vector to a scalar value using a monoid
select reduce_scalar('int32[0:1 1:2 2:3]'::vector, 'plus_monoid_int32');

-- ## Selection and Apply
--
-- Filter a vector and apply unary operators
select choose('int32[0:1 1:2 2:3]'::vector, 'valuegt_int32', 1);

select apply('int32[1:1 2:2 3:3]'::vector, 'ainv_int32'::unaryop);
-- ## Setting and Getting Elements

-- Elements can be set individually with `set_element`, the modified
-- input is returned:

select set_element('int32[1:1 2:2 3:3]'::vector, 4, 4);

-- Scalar elements can be extracted individually with `get_element`

select get_element('int32[1:1 2:2 3:3]'::vector, 3);

-- ## Utility Operations
--
-- Miscellaneous helpers for vectors
select print('int32(4)[1:1 2:2 3:3]'::vector);

select wait('int32[0:1 1:2 2:3]'::vector);

select dup('int32[0:1 1:2 2:3]'::vector);

select clear('int32[0:1 1:2 2:3]'::vector);

