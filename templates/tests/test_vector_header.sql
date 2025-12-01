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
select '{{1,2,3}}'::integer[]::vector;

-- Cast from a Postgres array with NULL indicating empty value:
select '{{1,NULL,3}}'::integer[]::vector;

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

-- ## Vector-Vector Multiplication (vxv)
--
-- The vxv operation performs inner product (dot product) between two vectors.
-- It's analogous to matrix-matrix multiplication but for vectors.

-- Basic vxv with default semiring (plus_times)
select vxv('int32(4)[0:1 1:2 2:3]'::vector, 'int32(4)[0:2 1:3 2:4]'::vector);

-- vxv with different semirings
select vxv('int32(4)[0:1 1:2 2:3]'::vector, 'int32(4)[0:2 1:3 2:4]'::vector, 'plus_times_int32'::semiring);

-- vxv with min_plus semiring
select vxv('int32(4)[0:1 1:2 2:3]'::vector, 'int32(4)[0:2 1:3 2:4]'::vector, 'min_plus_int32'::semiring);

-- vxv with max_times semiring
select vxv('int32(4)[0:1 1:2 2:3]'::vector, 'int32(4)[0:2 1:3 2:4]'::vector, 'max_times_int32'::semiring);

-- vxv with sparse vectors (no overlap)
select vxv('int32(4)[0:1 1:2]'::vector, 'int32(4)[2:3 3:4]'::vector);

-- vxv with partial overlap
select vxv('int32(4)[0:1 1:2 2:3]'::vector, 'int32(4)[1:5 2:6 3:7]'::vector);

-- vxv with empty vector
select vxv('int32(4)[]'::vector, 'int32(4)[0:1 1:2]'::vector);

-- vxv from test fixture
select vxv(u, v) from test_fixture;

-- vxv with boolean vectors (logical AND)
select vxv('bool(4)[0:true 1:true 2:false]'::vector, 'bool(4)[0:true 1:false 2:true]'::vector);

-- vxv with float vectors
select vxv('fp64(4)[0:1.5 1:2.5 2:3.5]'::vector, 'fp64(4)[0:2.0 1:3.0 2:4.0]'::vector);

-- ## Vector Assignment
--
-- The assign operation allows setting a subvector within a vector.

-- Basic assign: replace elements
select assign('int32(10)[0:1 2:3 4:5]'::vector, 'int32(3)[0:10 1:20 2:30]'::vector, array[5,6,7]::bigint[]);

-- Assign with mask
select assign('int32(10)[0:1 2:3 4:5]'::vector, 'int32(3)[0:10 1:20 2:30]'::vector, array[1,3,5]::bigint[], 'int32(10)[1:1 3:1 5:1]'::vector, descr='r');

-- Assign scalar to indices
select assign('int32(10)[0:1 2:3 4:5]'::vector, 99::int, array[2,4,6]::bigint[]);

-- Assign with accumulator
select assign('int32(10)[0:1 2:3 4:5 6:7]'::vector, 'int32(3)[0:10 1:20 2:30]'::vector, array[0,2,4]::bigint[], accum=>'plus_int32'::binaryop);

-- Assign empty subvector
select assign('int32(10)[0:1 2:3 4:5]'::vector, 'int32(0)[]'::vector, array[]::bigint[]);

-- Assign to empty vector
select assign('int32(10)[]'::vector, 'int32(3)[0:10 1:20 2:30]'::vector, array[1,3,5]::bigint[]);

-- ## Vector Extraction
--
-- The extract operation extracts a subvector from a vector.

-- Basic extract: get specific indices
select extract_vector('int32[0:10 1:20 2:30 3:40 4:50]'::vector, array[1,3]::bigint[]);

-- Extract with indices array
select extract_vector('int32[0:1 1:2 2:3 3:4 4:5]'::vector, array[0,2,4]::bigint[]);

-- Extract single element as vector
select extract_vector('int32[0:10 1:20 2:30]'::vector, array[1]::bigint[]);

-- Extract all elements
select extract_vector('int32[0:10 1:20 2:30]'::vector, array[0,1,2]::bigint[]);

-- Extract with mask
select extract_vector('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector, array[0,1,2,3,4]::bigint[], mask=>'int32(5)[0:1 2:1 4:1]'::vector);

-- Extract from sparse vector
select extract_vector('int32(20)[5:100 10:200 15:300]'::vector, array[5,10,15]::bigint[]);

-- Extract non-existent indices returns empty positions
select extract_vector('int32(10)[2:20 5:50]'::vector, array[0,1,2,3,4,5]::bigint[]);

-- ## Element Manipulation
--
-- Operations for adding and removing individual elements.

-- Check if vector contains element at index
select contains('int32[0:1 2:3 4:5]'::vector, 0);
select contains('int32[0:1 2:3 4:5]'::vector, 1);
select contains('int32[0:1 2:3 4:5]'::vector, 2);

-- Check multiple indices
select i, contains('int32[0:1 2:3 4:5]'::vector, i) as present
from generate_series(0, 5) as i;

-- Remove element from vector
select remove_element('int32[0:1 1:2 2:3 3:4]'::vector, 1);

-- Remove multiple elements
select remove_element(
    remove_element('int32[0:1 1:2 2:3 3:4]'::vector, 1),
    3
);

-- Remove non-existent element (no-op)
select remove_element('int32[0:1 2:3 4:5]'::vector, 1);

-- Remove from sparse vector
select remove_element('int32(100)[10:5 50:10 90:15]'::vector, 50);

-- Remove all elements one by one
select remove_element(
    remove_element(
        remove_element('int32[0:1 1:2 2:3]'::vector, 0),
        1
    ),
    2
);

-- ## Random Vector Generation
--
-- Generate random vectors with specified density and type.

-- Generate random int32 vector with 50% density
select nvals(random_vector('int32', 10, 0.5, 42)) as stored_elements,
       size(random_vector('int32', 10, 0.5, 42)) as vector_size;

-- Random vector with specific seed gives consistent results
select random_vector('int32', 5, 1.0, 42);
select random_vector('int32', 5, 1.0, 42);

-- Random vector with different seed gives different results
select random_vector('int32', 5, 1.0, 42);
select random_vector('int32', 5, 1.0, 43);

-- Random vector with low density
select print(random_vector('int32', 20, 0.2, 42));

-- Random vector with high density
select print(random_vector('int32', 20, 0.9, 42));

-- Random vector with 100% density
select nvals(random_vector('int32', 10, 'inf', 42));

-- Random vector with 0% density (empty)
select nvals(random_vector('int32', 10, 0.0, 42));

-- Random vectors of different types
select nvals(random_vector('int8', 10, 0.5, 42)) as int8_nvals,
       nvals(random_vector('int16', 10, 0.5, 42)) as int16_nvals,
       nvals(random_vector('int64', 10, 0.5, 42)) as int64_nvals;

-- Random float vectors
select nvals(random_vector('fp32', 10, 0.5, 42)) as fp32_nvals,
       nvals(random_vector('fp64', 10, 0.5, 42)) as fp64_nvals;

-- Random boolean vector
select print(random_vector('bool', 10, 0.5, 42));

-- Large random vector
select size(random_vector('int32', 1000, 0.1, 42)) as size,
       nvals(random_vector('int32', 1000, 0.1, 42)) as nvals;

-- ## Vector Norms
--
-- Calculate various norms of vectors.

select norm('fp64[0:3.0 1:-4.0 2:5.0]'::vector);

-- Norm of integer vector (casts to float)
-- Norm of sparse vector
select norm('fp64(100)[10:3.0 50:4.0 90:5.0]'::vector);

-- Norm of empty vector
select norm('fp64[]'::vector);

-- Norm of single element
select norm('fp64[0:5.0]'::vector);

-- ## Vector Comparison Select
--
-- Use comparison operators with choose/select operations.

-- Select elements greater than threshold
select choose('int32[0:10 1:20 2:5 3:30 4:15]'::vector, 'valuegt_int32'::indexunaryop, 10::int);

-- Select elements less than threshold
select choose('int32[0:10 1:20 2:5 3:30 4:15]'::vector, 'valuelt_int32'::indexunaryop, 10::int);

-- Select elements greater than or equal to threshold
select choose('int32[0:10 1:20 2:15 3:30 4:15]'::vector, 'valuege_int32'::indexunaryop, 20::int);

-- Select elements less than or equal to threshold
select choose('int32[0:10 1:20 2:15 3:30 4:15]'::vector, 'valuele_int32'::indexunaryop, 20::int);

-- Select non-zero elements (valuene with threshold 0)
select choose('int32[0:10 1:0 2:5 3:0 4:15]'::vector, 'valuene_int32'::indexunaryop, 0::int);

-- Select equal elements
select choose('int32[0:10 1:20 2:15 3:30 4:15]'::vector, 'valueeq_int32'::indexunaryop, 15::int);

-- Comparison with float vectors
select choose('fp64[0:1.5 1:2.5 2:1.0 3:3.5]'::vector, 'valuegt_fp64'::indexunaryop, 2.0::double precision);

-- ## Vector Cast
--
-- Type conversion between vector types.

-- Cast int32 to int64
select cast_to('int32[0:1 1:2 2:3]'::vector, 'int64');

-- Cast int32 to float
select cast_to('int32[0:1 1:2 2:3]'::vector, 'fp64');

-- Cast float to int (truncates)
select cast_to('fp64[0:1.7 1:2.3 2:3.9]'::vector, 'int32');

-- Cast between integer sizes
select cast_to('int64[0:100 1:200 2:300]'::vector, 'int32');
select cast_to('int32[0:1 1:2 2:3]'::vector, 'int16');

-- Cast bool to int
select cast_to('bool[0:true 1:false 2:true]'::vector, 'int32');

-- Cast int to bool (0=false, non-zero=true)
select cast_to('int32[0:0 1:1 2:5 3:0]'::vector, 'bool');

-- Cast preserves sparsity
select size(cast_to('int32(100)[10:5 50:10 90:15]'::vector, 'fp64'));

-- ## Vector Resize
--
-- Change the size bound of a vector.

-- Resize to larger size
select size(resize('int32(10)[0:1 2:3 4:5]'::vector, 20));

-- Resize to smaller size (keeps elements that fit)
select print(resize('int32(10)[0:1 2:3 4:5 8:9]'::vector, 5));

-- Resize unbounded vector
select size(resize('int32[0:1 2:3 4:5]'::vector, 10));

-- Resize to same size (no-op)
select print(resize('int32(10)[0:1 2:3 4:5]'::vector, 10));

-- Resize to zero
select resize('int32(10)[0:1 2:3]'::vector, 0);

-- Resize empty vector
select size(resize('int32(10)[]'::vector, 20));

-- ## Vector Info
--
-- Get internal SuiteSparse information about vectors.

-- Basic info
select info('int32[0:1 1:2 2:3]'::vector);

-- Info on sparse vector
select info('int32(1000)[10:5 500:10 990:15]'::vector);

-- Info on empty vector
select info('int32(10)[]'::vector);

-- Info on dense-ish vector
select info(random_vector('int32', 100, 0.9, 42));

-- ## Vector Aggregation
--
-- Aggregate table data into vectors.

-- Create test table for aggregation
create temporary table vec_agg_test (idx bigint, val integer);
insert into vec_agg_test values (0, 10), (2, 20), (4, 30), (1, 15);

-- Aggregate into vector
select vector_agg(idx, val) from vec_agg_test;

-- Aggregate empty table
delete from vec_agg_test;
select vector_agg(idx, val) from vec_agg_test;

-- Aggregate with different types
create temporary table vec_agg_float (idx bigint, val double precision);
insert into vec_agg_float values (0, 1.5), (1, 2.5), (3, 3.5);
select vector_agg(idx, val) from vec_agg_float;

-- Aggregate bool values
create temporary table vec_agg_bool (idx bigint, val boolean);
insert into vec_agg_bool values (0, true), (1, false), (2, true), (3, false);
select vector_agg(idx, val) from vec_agg_bool;

drop table vec_agg_test;
drop table vec_agg_float;
drop table vec_agg_bool;

-- ## Descriptor Basics
--
-- Test basic descriptor construction and properties.

-- Get descriptor name
select name('s'::descriptor);
select name('c'::descriptor);
select name('r'::descriptor);

-- ## Structural Mask Descriptor (s)
--
-- Structural masks use only the pattern (which indices exist),
-- ignoring actual values.

-- Value mask (default): false values block updates
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'ainv_int32'::unaryop,
    mask=>'bool(5)[0:true 1:false 2:true 3:false 4:true]'::vector);

-- Structural mask (s): all existing positions allow updates, regardless of value
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'ainv_int32'::unaryop,
    mask=>'bool(5)[0:true 1:false 2:true 3:false 4:true]'::vector,
    descr=>'s'::descriptor);

-- Structural mask with integer values (all non-empty positions)
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'abs_int32'::unaryop,
    mask=>'int32(5)[0:0 1:100 2:0 3:200 4:0]'::vector,
    descr=>'s'::descriptor);

-- Compare: without structural, zeros might block
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'abs_int32'::unaryop,
    mask=>'int32(5)[0:0 1:100 2:0 3:200 4:0]'::vector);

-- Structural mask with eadd
select eadd('int32(5)[0:10 1:20 2:30]'::vector,
    'int32(5)[1:5 2:6 3:7]'::vector,
    'plus_int32'::binaryop,
    mask=>'int32(5)[0:0 1:1 2:0 3:1]'::vector,
    descr=>'s'::descriptor);

-- Structural mask with emult
select emult('int32(5)[0:2 1:3 2:4 3:5 4:6]'::vector,
    'int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'times_int32'::binaryop,
    mask=>'bool(5)[0:false 1:true 2:false 3:true 4:false]'::vector,
    descr=>'s'::descriptor);

-- Structural mask with assign
select assign('int32(10)[0:1 2:3 4:5]'::vector,
    'int32(3)[0:100 1:200 2:300]'::vector,
    array[1,3,5]::bigint[],
    mask=>'int32(10)[1:0 3:0 5:1]'::vector,
    descr=>'s'::descriptor);

-- ## Complement Mask Descriptor (c)
--
-- Complement inverts the mask: masked positions become unmasked and vice versa.

-- Normal mask: update positions 0, 2, 4
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'ainv_int32'::unaryop,
    mask=>'int32(5)[0:1 2:1 4:1]'::vector);

-- Complement mask: update positions 1, 3 (opposite of normal)
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'ainv_int32'::unaryop,
    mask=>'int32(5)[0:1 2:1 4:1]'::vector,
    descr=>'c'::descriptor);

-- Complement with boolean mask
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'abs_int32'::unaryop,
    mask=>'bool(5)[0:true 1:true 2:false 3:false 4:true]'::vector,
    descr=>'c'::descriptor);

-- Complement with empty mask (all positions updated)
select apply('int32(5)[0:10 1:20 2:30]'::vector,
    'abs_int32'::unaryop,
    mask=>'int32(5)[]'::vector,
    descr=>'c'::descriptor);

-- Complement with full mask (no positions updated)
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'abs_int32'::unaryop,
    mask=>'int32(5)[0:1 1:1 2:1 3:1 4:1]'::vector,
    descr=>'c'::descriptor);

-- Complement mask with eadd
select eadd('int32(5)[0:10 1:20 2:30]'::vector,
    'int32(5)[1:5 2:6 3:7]'::vector,
    'plus_int32'::binaryop,
    mask=>'int32(5)[1:1 2:1]'::vector,
    descr=>'c'::descriptor);

-- Complement mask with emult
select emult('int32(5)[0:2 1:3 2:4 3:5 4:6]'::vector,
    'int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'times_int32'::binaryop,
    mask=>'int32(5)[0:1 2:1 4:1]'::vector,
    descr=>'c'::descriptor);

-- Complement mask with assign
select assign('int32(10)[0:1 2:3 4:5]'::vector,
    'int32(3)[0:100 1:200 2:300]'::vector,
    array[1,3,5]::bigint[],
    mask=>'int32(10)[1:1 3:1]'::vector,
    descr=>'c'::descriptor);

-- Complement mask with choose/select
select choose('int32(10)[0:5 1:15 2:10 3:20 4:8 5:25]'::vector,
    15::int,
    '>_int32'::indexunaryop,
    mask=>'int32(10)[0:1 2:1 4:1]'::vector,
    descr=>'c'::descriptor);

-- ## Structural Complement Descriptor (sc)
--
-- Combine structural and complement: use pattern (not values) and invert.

-- sc descriptor: structural interpretation then complement
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'ainv_int32'::unaryop,
    mask=>'int32(5)[0:0 2:0 4:100]'::vector,
    descr=>'sc'::descriptor);

-- Compare with just structural
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'ainv_int32'::unaryop,
    mask=>'int32(5)[0:0 2:0 4:100]'::vector,
    descr=>'s'::descriptor);

-- Compare with just complement
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'ainv_int32'::unaryop,
    mask=>'int32(5)[0:0 2:0 4:100]'::vector,
    descr=>'c'::descriptor);

-- sc with boolean mask (positions exist → true for structural, then complement)
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'abs_int32'::unaryop,
    mask=>'bool(5)[0:false 2:false 4:false]'::vector,
    descr=>'sc'::descriptor);

-- sc with eadd
select eadd('int32(5)[0:10 1:20 2:30]'::vector,
    'int32(5)[1:5 2:6 3:7]'::vector,
    'plus_int32'::binaryop,
    mask=>'int32(5)[0:0 1:0 2:1]'::vector,
    descr=>'sc'::descriptor);

-- sc with emult
select emult('int32(5)[0:2 1:3 2:4 3:5 4:6]'::vector,
    'int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'times_int32'::binaryop,
    mask=>'int32(5)[1:0 3:0]'::vector,
    descr=>'sc'::descriptor);

-- ## Replace Descriptor (r)
--
-- Replace clears the output vector before the operation, removing all
-- existing values not produced by the operation.

-- Without replace: existing values at unmasked positions remain
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'ainv_int32'::unaryop,
    mask=>'int32(5)[0:1 2:1]'::vector);

-- With replace: only masked result positions remain
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'ainv_int32'::unaryop,
    mask=>'int32(5)[0:1 2:1]'::vector,
    descr=>'r'::descriptor);

-- Replace with sparse mask
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'abs_int32'::unaryop,
    mask=>'int32(5)[1:1]'::vector,
    descr=>'r'::descriptor);

-- Replace with full mask (all results kept)
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'ainv_int32'::unaryop,
    mask=>'int32(5)[0:1 1:1 2:1 3:1 4:1]'::vector,
    descr=>'r'::descriptor);

-- Replace with eadd
select eadd('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'int32(5)[1:5 2:6 3:7]'::vector,
    'plus_int32'::binaryop,
    mask=>'int32(5)[1:1 2:1]'::vector,
    descr=>'r'::descriptor);

-- Replace with emult
select emult('int32(5)[0:2 1:3 2:4 3:5 4:6]'::vector,
    'int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'times_int32'::binaryop,
    mask=>'int32(5)[0:1 2:1 4:1]'::vector,
    descr=>'r'::descriptor);

-- ## Combined Descriptors (rc, rs, rsc)
--
-- Test combinations of replace with other descriptors.

-- rc: replace + complement
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'ainv_int32'::unaryop,
    mask=>'int32(5)[0:1 2:1]'::vector,
    descr=>'rc'::descriptor);

-- rs: replace + structural
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'abs_int32'::unaryop,
    mask=>'int32(5)[0:0 2:100 4:0]'::vector,
    descr=>'rs'::descriptor);

-- rsc: replace + structural + complement
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'ainv_int32'::unaryop,
    mask=>'int32(5)[1:0 3:0]'::vector,
    descr=>'rsc'::descriptor);

-- ## Descriptors with Accumulators
--
-- Test how descriptors interact with accumulator operations.

-- Normal accumulation with mask
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'ainv_int32'::unaryop,
    mask=>'int32(5)[0:1 2:1 4:1]'::vector,
    accum=>'plus_int32'::binaryop);

-- Accumulation with complement mask
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'ainv_int32'::unaryop,
    mask=>'int32(5)[0:1 2:1 4:1]'::vector,
    accum=>'plus_int32'::binaryop,
    descr=>'c'::descriptor);

-- Accumulation with structural mask
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'abs_int32'::unaryop,
    mask=>'int32(5)[0:0 2:1 4:0]'::vector,
    accum=>'times_int32'::binaryop,
    descr=>'s'::descriptor);

-- Accumulation with replace (replace then accumulate)
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'ainv_int32'::unaryop,
    mask=>'int32(5)[0:1 2:1]'::vector,
    accum=>'plus_int32'::binaryop,
    descr=>'r'::descriptor);

-- Accumulation with rsc
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'abs_int32'::unaryop,
    mask=>'int32(5)[1:0 3:0]'::vector,
    accum=>'times_int32'::binaryop,
    descr=>'rsc'::descriptor);

-- ## Descriptors with Different Operation Types
--
-- Test descriptors across various vector operations.

-- Descriptor with apply
select apply('int32(5)[0:10 1:20 2:30]'::vector,
    'abs_int32'::unaryop,
    mask=>'int32(5)[0:1 2:1]'::vector,
    descr=>'c'::descriptor);

-- Descriptor with apply_first (scalar, vector)
select apply(100::int,
    'int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'minus_int32'::binaryop,
    mask=>'int32(5)[1:1 3:1]'::vector,
    descr=>'c'::descriptor);

-- Descriptor with apply_second (vector, scalar)
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    5::int,
    'plus_int32'::binaryop,
    mask=>'int32(5)[0:1 2:1 4:1]'::vector,
    descr=>'s'::descriptor);

-- Descriptor with eadd
select eadd('int32(5)[0:10 1:20 2:30]'::vector,
    'int32(5)[1:5 2:6 3:7]'::vector,
    'plus_int32'::binaryop,
    mask=>'int32(5)[1:1 2:1]'::vector,
    descr=>'r'::descriptor);

-- Descriptor with emult
select emult('int32(5)[0:2 1:3 2:4 3:5 4:6]'::vector,
    'int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'times_int32'::binaryop,
    mask=>'int32(5)[0:1 2:1 4:1]'::vector,
    descr=>'rc'::descriptor);

-- Descriptor with eunion
select eunion('int32(5)[0:10 2:30]'::vector,
    'int32(5)[1:20 3:40]'::vector,
    'plus_int32'::binaryop,
    0::int,
    0::int,
    mask=>'int32(5)[0:1 2:1]'::vector,
    descr=>'c'::descriptor);

-- Descriptor with assign
select assign('int32(10)[0:1 2:3 4:5]'::vector,
    'int32(3)[0:100 1:200 2:300]'::vector,
    array[1,3,5]::bigint[],
    mask=>'int32(10)[1:1 3:1 5:1]'::vector,
    descr=>'r'::descriptor);

-- Descriptor with extract
select extract_vector('int32[0:10 1:20 2:30 3:40 4:50]'::vector,
    array[0,1,2,3,4]::bigint[],
    mask=>'int32[0:1 2:1 4:1]'::vector,
    descr=>'c'::descriptor);

-- Descriptor with choose/select
select choose('int32(10)[0:5 1:15 2:10 3:20 4:8]'::vector,
    10::int,
    '>_int32'::indexunaryop,
    mask=>'int32(10)[0:1 1:1 2:1 3:1]'::vector,
    descr=>'s'::descriptor);

-- ## Descriptor Edge Cases
--
-- Test descriptors with edge cases and boundary conditions.

-- Descriptor with empty vector
select apply('int32(5)[]'::vector,
    'abs_int32'::unaryop,
    mask=>'int32(5)[0:1 2:1]'::vector,
    descr=>'c'::descriptor);

-- Descriptor with empty mask
select apply('int32(5)[0:10 1:20 2:30]'::vector,
    'abs_int32'::unaryop,
    mask=>'int32(5)[]'::vector,
    descr=>'r'::descriptor);

-- Descriptor with single-element mask
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'ainv_int32'::unaryop,
    mask=>'int32(5)[2:1]'::vector,
    descr=>'c'::descriptor);

-- Complement of empty mask (all positions)
select apply('int32(5)[0:10 1:20 2:30]'::vector,
    'abs_int32'::unaryop,
    mask=>'int32(5)[]'::vector,
    descr=>'c'::descriptor);

-- Replace with no mask (clears everything)
select apply('int32(5)[0:10 1:20 2:30]'::vector,
    'abs_int32'::unaryop,
    descr=>'r'::descriptor);

-- ## Practical Descriptor Usage
--
-- Real-world patterns combining descriptors effectively.

-- Conditional update: update only positions NOT in a set
select assign('int32(10)[0:1 1:2 2:3 3:4 4:5 5:6 6:7 7:8 8:9 9:10]'::vector,
    'int32(3)[0:0 1:0 2:0]'::vector,
    array[1,3,5]::bigint[],
    mask=>'int32(10)[1:1 3:1 5:1]'::vector,
    descr=>'c'::descriptor);

-- Filter and replace: keep only filtered results
select apply('int32(10)[0:5 1:-10 2:15 3:-20 4:8 5:30]'::vector,
    'abs_int32'::unaryop,
    mask=>choose('int32(10)[0:5 1:-10 2:15 3:-20 4:8 5:30]'::vector,
        'valuelt_int32'::indexunaryop, 0::int),
    descr=>'r'::descriptor);

-- Structural masking with mixed-value mask
select eadd('int32(10)[0:10 2:20 4:30 6:40]'::vector,
    'int32(10)[1:15 3:25 5:35 7:45]'::vector,
    'plus_int32'::binaryop,
    mask=>'int32(10)[0:1 1:0 2:999 3:-1 4:0 5:42]'::vector,
    descr=>'s'::descriptor);

-- Complex filtering with sc descriptor
select choose('int32(10)[0:100 1:50 2:150 3:75 4:200 5:25]'::vector,
    'valuelt_int32'::indexunaryop,
    100::int,
    mask=>'int32(10)[1:1 3:1 5:1]'::vector,
    descr=>'sc'::descriptor);

-- ## Basic Masking with apply()
--
-- The mask parameter controls which elements of the output are written.

-- Apply with mask: only masked positions are updated
select apply('int32(10)[0:1 1:2 2:3 3:4 4:5]'::vector, 'abs_int32'::unaryop,
    mask=>'int32(10)[0:1 2:1 4:1]'::vector);

-- Apply with dense mask
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector, 'ainv_int32'::unaryop,
    mask=>'int32(5)[0:1 1:1 2:1 3:1 4:1]'::vector);

-- Apply with empty mask (nothing updated)
select apply('int32(5)[0:10 1:20 2:30]'::vector, 'abs_int32'::unaryop,
    mask=>'int32(5)[]'::vector);

-- Apply with sparse mask
select apply('int32(10)[0:1 1:2 2:3 3:4 4:5]'::vector, 'minv_int32'::unaryop,
    mask=>'int32(10)[1:1 3:1 5:1]'::vector);

-- Apply with boolean mask
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector, 'abs_int32'::unaryop,
    mask=>'bool(5)[0:true 2:true 4:true]'::vector);

-- ## Masking with assign()
--
-- Mask controls which indices are assigned to.

-- Basic assign with mask
select assign('int32(10)[0:1 2:3 4:5]'::vector,
    'int32(3)[0:100 1:200 2:300]'::vector,
    array[1,3,5]::bigint[],
    mask=>'int32(10)[1:1 3:1]'::vector);

-- Assign with full mask
select assign('int32(10)[0:1 1:2 2:3]'::vector,
    'int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    array[0,1,2,3,4]::bigint[],
    mask=>'int32(10)[0:1 1:1 2:1 3:1 4:1]'::vector);

-- Assign scalar with mask
select assign('int32(10)[0:1 2:3 4:5]'::vector,
    99::int,
    array[0,2,4,6,8]::bigint[],
    mask=>'int32(10)[0:1 4:1 6:1]'::vector);

-- Assign with empty mask (nothing assigned)
select assign('int32(10)[0:1 2:3 4:5]'::vector,
    'int32(3)[0:100 1:200 2:300]'::vector,
    array[5,6,7]::bigint[],
    mask=>'int32(10)[]'::vector);

-- ## Masking with extract_vector()
--
-- Mask selects which extracted elements to keep.

-- Extract with mask
select extract_vector('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    array[0,1,2,3,4]::bigint[],
    mask=>'int32(5)[0:1 2:1 4:1]'::vector);

-- Extract with boolean mask
select extract_vector('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    array[0,1,2,3,4]::bigint[],
    mask=>'bool(5)[0:true 1:false 2:true 3:false 4:true]'::vector);

-- ## Masking with eadd() (Element-wise Add)
--
-- Mask controls which elements participate in the element-wise operation.

-- Element-wise add with mask
select eadd('int32(5)[0:10 1:20 2:30]'::vector,
    'int32(5)[1:5 2:6 3:7]'::vector,
    'plus_int32'::binaryop,
    mask=>'int32(5)[1:1 2:1]'::vector);

-- Element-wise add with full mask
select eadd('int32(5)[0:1 1:2 2:3 3:4 4:5]'::vector,
    'int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'times_int32'::binaryop,
    mask=>'int32(5)[0:1 1:1 2:1 3:1 4:1]'::vector);

-- ## Masking with emult() (Element-wise Multiply)
--
-- Mask filters the element-wise multiplication result.

-- Element-wise multiply with mask
select emult('int32(5)[0:2 1:3 2:4 3:5 4:6]'::vector,
    'int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'times_int32'::binaryop,
    mask=>'int32(5)[0:1 2:1 4:1]'::vector);

-- Element-wise multiply with boolean mask
select emult('fp64(5)[0:1.5 1:2.5 2:3.5 3:4.5 4:5.5]'::vector,
    'fp64(5)[0:2.0 1:3.0 2:4.0 3:5.0 4:6.0]'::vector,
    'times_fp64'::binaryop,
    mask=>'bool(5)[0:true 1:true 2:false 3:true 4:false]'::vector);

-- Element-wise multiply with min operator and mask
select emult('int32(5)[0:100 1:200 2:50 3:300 4:25]'::vector,
    'int32(5)[0:150 1:100 2:75 3:250 4:50]'::vector,
    'min_int32'::binaryop,
    mask=>'int32(5)[1:1 3:1]'::vector);

-- ## Masking with eunion() (Element-wise Union)
--
-- eunion uses default values for missing elements.

-- Element-wise union with mask
select eunion('int32(5)[0:10 2:30]'::vector,
    0::int,
    'int32(5)[1:20 3:40]'::vector,
    0::int,
    'plus_int32'::binaryop,
    mask=>'int32(5)[0:1 1:1 2:1]'::vector);

-- Element-wise union with sparse mask
select eunion('int32(10)[2:5 4:10 6:15]'::vector,
    1::int,
    'int32(10)[3:7 5:12 7:18]'::vector,
    1::int,
    'times_int32'::binaryop,
    mask=>'int32(10)[2:1 5:1 7:1]'::vector);

-- ## Masking with select/choose()
--
-- Combine selection with masking for complex filtering.

-- Select with mask
select choose('int32(10)[0:10 1:20 2:5 3:30 4:15 5:25]'::vector,
    'valuegt_int32'::indexunaryop,
    15::int,
    mask=>'int32(10)[0:1 1:1 2:1 3:1 4:1]'::vector);

-- Select with boolean mask
select choose('fp64(5)[0:1.5 1:2.5 2:1.0 3:3.5 4:2.0]'::vector,
    'valuege_fp64'::indexunaryop,
    2.0::double precision,
    mask=>'bool(5)[0:true 2:true 4:true]'::vector);

-- ## Structural vs Value Masks
--
-- Structural masks consider only the pattern (which indices exist),
-- not the actual values. This is controlled by the descriptor.

-- Structural mask: only considers which positions exist
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'ainv_int32'::unaryop,
    mask=>'bool(5)[0:true 2:true 4:true]'::vector,
    descr=>'s'::descriptor);

-- Structural mask with integer vector
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'abs_int32'::unaryop,
    mask=>'int32(5)[1:100 3:200 4:0]'::vector,
    descr=>'s'::descriptor);

-- ## Mask Complement
--
-- The descriptor can invert the mask (complement).

-- Normal mask
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'ainv_int32'::unaryop,
    mask=>'int32(5)[0:1 2:1 4:1]'::vector);

-- Complemented mask (opposite positions)
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'ainv_int32'::unaryop,
    mask=>'int32(5)[0:1 2:1 4:1]'::vector,
    descr=>'c'::descriptor);

-- Complement with boolean mask
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'abs_int32'::unaryop,
    mask=>'bool(5)[0:true 1:true 2:false 3:false 4:true]'::vector,
    descr=>'c'::descriptor);

-- ## Combined Descriptors
--
-- Multiple descriptor flags can be combined.

-- Structural complement mask
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'ainv_int32'::unaryop,
    mask=>'int32(5)[0:5 2:0 4:100]'::vector,
    descr=>'sc'::descriptor);

-- ## Masking with Accumulators
--
-- Combine mask with accumulator for selective accumulation.

-- Masked accumulation
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'ainv_int32'::unaryop,
    'int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    mask=>'int32(5)[0:1 2:1 4:1]'::vector,
    accum=>'plus_int32'::binaryop);

-- Masked accumulation with complement
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'minv_int32'::unaryop,
    'int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    mask=>'int32(5)[1:1 3:1]'::vector,
    accum=>'times_int32'::binaryop,
    descr=>'c'::descriptor);

-- ## Mask Type Compatibility
--
-- Test masks of different types with vectors.

-- int32 vector with int64 mask
select apply('int32(5)[0:10 1:20 2:30 3:40 4:50]'::vector,
    'abs_int32'::unaryop,
    mask=>'int64(5)[0:1 2:1 4:1]'::vector);

-- fp64 vector with bool mask
select apply('fp64(5)[0:1.5 1:2.5 2:3.5 3:4.5 4:5.5]'::vector,
    'abs_fp64'::unaryop,
    mask=>'bool(5)[0:true 2:true 4:true]'::vector);

-- fp64 vector with int32 mask
select apply('fp64(5)[0:1.5 1:2.5 2:3.5 3:4.5 4:5.5]'::vector,
    'ainv_fp64'::unaryop,
    mask=>'int32(5)[1:1 3:1]'::vector);

-- ## Edge Cases
--
-- Test boundary conditions for masks.

-- Empty vector with non-empty mask
select apply('int32(5)[]'::vector,
    'abs_int32'::unaryop,
    mask=>'int32(5)[0:1 2:1 4:1]'::vector);

-- Non-empty vector with empty mask
select apply('int32(5)[0:10 1:20 2:30]'::vector,
    'abs_int32'::unaryop,
    mask=>'int32(5)[]'::vector);

-- Unsized mask
select apply('int32[0:10 1:20]'::vector,
    'abs_int32'::unaryop,
    mask=>'int32[]'::vector);

-- ## Boolean Vector Operations
--
-- Test boolean-specific operations and logic.

-- Boolean construction
select 'bool(5)[0:true 1:false 2:true 3:false 4:true]'::vector;
select 'bool(5)[0:t 1:f 2:t 3:f 4:t]'::vector;

-- Boolean element-wise OR
select eadd('bool(5)[0:true 1:false 2:true]'::vector,
    'bool(5)[0:false 1:true 2:true]'::vector,
    'lor'::binaryop);

-- Boolean element-wise AND
select emult('bool(5)[0:true 1:false 2:true 3:true 4:false]'::vector,
    'bool(5)[0:true 1:true 2:false 3:true 4:false]'::vector,
    'land'::binaryop);

-- Boolean element-wise XOR
select eadd('bool(5)[0:true 1:false 2:true]'::vector,
    'bool(5)[0:false 1:false 2:true]'::vector,
    'lxor'::binaryop);

-- Boolean element-wise XNOR
select eadd('bool(5)[0:true 1:false 2:true]'::vector,
    'bool(5)[0:false 1:false 2:true]'::vector,
    'eq_bool'::binaryop);

-- Boolean NOT (unary complement)
select apply('bool(5)[0:true 1:false 2:true 3:false 4:true]'::vector,
    'lnot'::unaryop);

-- Boolean reduction with OR (any true?)
select reduce_scalar('bool[0:false 1:false 2:true 3:false]'::vector,
    'lor_monoid_bool'::monoid);
select reduce_scalar('bool[0:false 1:false 2:false]'::vector,
    'lor_monoid_bool'::monoid);

-- Boolean reduction with AND (all true?)
select reduce_scalar('bool[0:true 1:true 2:true]'::vector,
    'land_monoid_bool'::monoid);
select reduce_scalar('bool[0:true 1:false 2:true]'::vector,
    'land_monoid_bool'::monoid);

-- Boolean reduction with XOR (parity)
select reduce_scalar('bool[0:true 1:true 2:true]'::vector,
    'lxor_monoid_bool'::monoid);
select reduce_scalar('bool[0:true 1:false 2:true]'::vector,
    'lxor_monoid_bool'::monoid);

-- Boolean to integer conversion
select cast_to('bool[0:true 1:false 2:true 3:false]'::vector, 'int32');

-- Integer to boolean conversion
select cast_to('int32[0:0 1:1 2:5 3:0 4:-1]'::vector, 'bool');

-- Boolean comparisons (equal/not equal)
select eadd('bool(5)[0:true 1:false 2:true]'::vector,
    'bool(5)[0:true 1:true 2:false]'::vector,
    'eq_bool'::binaryop);

-- ## Float32 (fp32) Special Values
--
-- Test float-specific edge cases and special values.

-- Infinity values
select 'fp32(5)[0:Infinity 1:-Infinity 2:1.5]'::vector;

-- NaN (Not a Number)
select 'fp32(5)[0:NaN 1:1.5 2:2.5]'::vector;

-- Signed zeros
select 'fp32(5)[0:0.0 1:-0.0 2:1.5]'::vector;

-- Very large values (near max)
select 'fp32(3)[0:3.40282e+38 1:-3.40282e+38 2:1.0]'::vector;

-- Operations with Infinity
select apply('fp32(3)[0:1.0 1:2.0 2:3.0]'::vector,
    'Infinity'::float4::scalar::float4,
    'plus_fp32'::binaryop);
select apply('fp32(3)[0:1.0 1:2.0 2:3.0]'::vector,
    'Infinity'::float4::scalar::float4,
    'times_fp32'::binaryop);
select apply('fp32(3)[0:1.0 1:2.0 2:3.0]'::vector,
    'Infinity'::float4::scalar::float4,
    'div_fp32'::binaryop);

-- Operations producing NaN
select apply('fp32(3)[0:0.0 1:1.0 2:2.0]'::vector,
    0.0::float4,
    'div_fp32'::binaryop);  -- 0/0 = NaN

-- Operations with NaN (NaN propagates)
select apply('fp32(3)[0:1.0 1:2.0 2:3.0]'::vector,
    'NaN'::float4::scalar::float4,
    'plus_fp32'::binaryop);
select apply('fp32(3)[0:1.0 1:NaN 2:3.0]'::vector,
    'abs_fp32'::unaryop);

-- Reduction with NaN
select reduce_scalar('fp32[0:1.0 1:NaN 2:3.0]'::vector,
    'plus_monoid_fp32'::monoid);

-- Min/max with special values
select reduce_scalar('fp32[0:-Infinity 1:1.0 2:Infinity]'::vector,
    'max_monoid_fp32'::monoid);
select reduce_scalar('fp32[0:-Infinity 1:1.0 2:Infinity]'::vector,
    'min_monoid_fp32'::monoid);

-- ## Float64 (fp64) Special Values
--
-- Test double-precision float edge cases.

-- Infinity values
select 'fp64(5)[0:Infinity 1:-Infinity 2:1.5 3:2.5 4:3.5]'::vector;

-- NaN values
select 'fp64(5)[0:NaN 1:1.5 2:2.5 3:3.5 4:4.5]'::vector;

-- Signed zeros
select 'fp64(5)[0:0.0 1:-0.0 2:1.5]'::vector;

-- Very large values (near max)
select 'fp64(3)[0:1.7976931348623157e+308 1:-1.7976931348623157e+308 2:1.0]'::vector;

-- Very small values (near min positive)
select 'fp64(3)[0:2.2250738585072014e-308 1:1.0 2:2.0]'::vector;

-- High precision values
select 'fp64(3)[0:3.141592653589793 1:2.718281828459045 2:1.414213562373095]'::vector;

-- Operations with Infinity
select eadd('fp64(3)[0:1.0 1:Infinity 2:3.0]'::vector,
    'fp64(3)[0:2.0 1:4.0 2:-Infinity]'::vector,
    'plus_fp64'::binaryop);

-- Infinity - Infinity = NaN
select emult('fp64(3)[0:Infinity 1:1.0 2:2.0]'::vector,
    'fp64(3)[0:-1.0 1:2.0 2:3.0]'::vector,
    'plus_fp64'::binaryop);

-- Float precision limits
select apply('fp64(3)[0:1e-100 1:1e-200 2:1e-300]'::vector,
    1e-100::double precision,
    'plus_fp64'::binaryop);

-- Very large number operations
select reduce_scalar('fp64[0:1e100 1:1e100 2:1e100]'::vector,
    'plus_fp64'::monoid);

-- ## Type Casting and Promotion
--
-- Test type conversions between vector types.

-- Int to float (exact for small ints)
select cast_to('int32[0:1 1:2 2:3 3:4 4:5]'::vector, 'fp64');
select cast_to('int32[0:100 1:200 2:300]'::vector, 'fp32');

-- Float to int (truncation)
select cast_to('fp64[0:1.9 1:2.1 2:3.5 3:-1.9 4:-2.1]'::vector, 'int32');
select cast_to('fp32[0:3.7 1:-3.7 2:0.5]'::vector, 'int32');

-- Large int to float (may lose precision)
select cast_to('int64[0:9007199254740992 1:9007199254740993]'::vector, 'fp64');

-- Int size conversions
select cast_to('int32[0:100 1:200 2:300]'::vector, 'int64');
select cast_to('int32[0:100 1:200 2:300]'::vector, 'int16');
select cast_to('int64[0:1000 1:2000 2:3000]'::vector, 'int32');

-- Float precision conversions
select cast_to('fp64[0:3.141592653589793 1:2.718281828459045]'::vector, 'fp32');
select cast_to('fp32[0:3.14159 1:2.71828]'::vector, 'fp64');
