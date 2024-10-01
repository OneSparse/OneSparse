-- # Matrix Objects
--
-- This documentation is also tests for the code, the examples below
-- show the literal output of these statements from Postgres.
--
-- Some setup to make sure warnings are shown, and that the extension
-- is installed.

set client_min_messages = 'WARNING';
create extension if not exists onesparse;

-- # Matrix
--
-- The 'matrix' data type wraps a SuiteSparse GrB_Matrix handle and
-- delegates functions from SQL to the library through instances of
-- this type.

\dT+ matrix

-- An empty matrix can be constructed many ways, but one of the
-- simplest is casting a type code to the matrix type.  In this case
-- 'i4' means GrB_INT32.  The type codes are intentionally compressed
-- to be as short as possible for smaller pg_dumps.

select 'i4'::matrix;

-- A matrix can have a fixed number of rows and/or columns.  The
-- default possible number of rows and columns is defined by the
-- SuiteSparse library to be `GrB_INDEX_MAX` which is '2^60' power
-- indexes.  For the purposes of this documentation this will be
-- referred to as "INDEX_MAX" and matrices and vector dimensions that
-- are INDEX_MAX in size are reffered to as "unbounded".  For matrices
-- with known dimensions, the dimensions can be provided in parentesis
-- after the type code.  Here a 10 row by 10 column matrix is created:

select 'i4(10:10)'::matrix;

-- Either dimension can be ommited, this creates a 10 row by unbounded
-- column matrix.

select 'i4(10:)'::matrix;

-- This creates a unbounded row by 10 column matrix.

select 'i4(10:)'::matrix;

-- Note that in all the above cases the matrices created are *empty*.
-- They contain no stored elements.  The memory needed to hold the
-- matrix contains only stored elements, if there isn't a value stored
-- at a given row or column position, no memory is consumed.  This is
-- the "sparse" in sparse matrix.  This is how it's possible to create
-- an unbounded row by unbounded column matrix without exhausting
-- memory trying to allocate 2^120 entries.
--
-- All graphblas operations are exposed by a series of functions and
-- operators.  Here we see three very common operations, returning the
-- number of rows, the number of columns, and the number of store
-- values.

select nrows('i4'::matrix);

select ncols('i4'::matrix);

select nvals('i4'::matrix);

-- Above you can see the matrix has unbounded rows and columns (the
-- very large number is the number of *possible* entries).  And the
-- number of stored values is zero.

-- Values can be specified after the 'type(dimension)' prefix as an
-- array of elements between square brackets.  Empty brackets imply no
-- elements:

select nrows('i4[]'::matrix);

select ncols('i4[]'::matrix);

select nvals('i4[]'::matrix);

-- Elements are specified between square brackets are coordinates of
-- 'row_id:column_id:value' separated by spaces:

select 'i4[1:1:1 2:2:2 3:3:3]'::matrix;

select 'i4(10:)[1:1:1 2:2:2 3:3:3]'::matrix;

select 'i4(:10)[1:1:1 2:2:2 3:3:3]'::matrix;

-- Below you see the number of rows, columns and spaces for a variety
-- of combinations:

select nrows('i4(10)[1:1:1 2:2:2 3:3:3]'::matrix);

select ncols('i4(10)[1:1:1 2:2:2 3:3:3]'::matrix);

select nvals('i4(10)[1:1:1 2:2:2 3:3:3]'::matrix);

select nrows('i4(10:)[1:1:1 2:2:2 3:3:3]'::matrix);

select ncols('i4(10:)[1:1:1 2:2:2 3:3:3]'::matrix);

select nvals('i4(10:)[1:1:1 2:2:2 3:3:3]'::matrix);

select nrows('i4(:10)[1:1:1 2:2:2 3:3:3]'::matrix);

select ncols('i4(:10)[1:1:1 2:2:2 3:3:3]'::matrix);

select nvals('i4(:10)[1:1:1 2:2:2 3:3:3]'::matrix);

select nrows('i4(10:10)[1:1:1 2:2:2 3:3:3]'::matrix);

select ncols('i4(10:10)[1:1:1 2:2:2 3:3:3]'::matrix);

select nvals('i4(10:10)[1:1:1 2:2:2 3:3:3]'::matrix);

-- # Element-wise operations
--
-- The GraphBLAS API has elementwise operations on matrices that
-- operate pairs of matrices.  'ewise_add' computes the element-wise
-- “addition” of two matrices A and B, element-wise using any binary
-- operator.  Elements present on both sides of the operation are
-- included in the result.

select ewise_add('i4[1:1:1 2:2:2 3:3:3]'::matrix, 'i4[1:1:1 2:2:2 3:3:3]'::matrix, 'plus_int32');

-- 'ewise_mult' multiplies elements of two matrices, taking only the
-- intersection of common elements in both matrices, if an element is
-- missing from either the left or right side, it is ommited from the
-- result:

select ewise_mult('i4[1:1:1 2:2:2 3:3:3]'::matrix, 'i4[1:1:1 2:2:2 3:3:3]'::matrix, 'times_int32');


-- 'ewise_union' is like 'ewise_add' but differs in how the binary op
-- is applied. A pair of scalars, alpha and beta define the inputs to
-- the operator when entries are present in one matrix but not the
-- other.

select ewise_union('i4[1:1:1 2:2:2 3:3:3]'::matrix, 42, 'i4[1:1:1 2:2:2 3:3:3]'::matrix, 84, 'plus_int32');

-- The entire matrix can be reduced to a scalar value:

select reduce_scalar('i4[1:1:1 2:2:2 3:3:3]'::matrix, 'plus_monoid_int32');

-- The matrix can also be reduced to a column vector:

select reduce_vector('i4[1:1:1 1:2:3 2:2:2]'::matrix, 'plus_monoid_int32');

-- To reduce a row vector, specify that the input should be transposed
-- with the descriptor 't0':

select reduce_vector('i4[1:1:1 1:2:3 2:2:2]'::matrix, 'plus_monoid_int32', descriptor='t0');

-- Matrix Multiplication (referred to here as A @ B) is the heart of
-- linear algebra.  All matrix multiplication happens over a semiring.
-- For the most common form of matrix multiplication, the outer
-- opperation is to multiply coresponding elements with the "times"
-- operator and then reduce those products with the "plus" operator.
-- This is called the "plus_times" semiring:

select mxm('i4[0:0:1 0:1:2]'::matrix, 'i4[0:0:1 0:1:3]'::matrix, 'plus_times_int32');

-- AxB can also be done with the '@' operator, mimicking the Python
-- syntax:

select 'i4[0:0:1 0:1:2]'::matrix @ 'i4[0:0:1 0:1:3]'::matrix;

-- Matrices can be multipled by vectors on the right taking the linear
-- combination of the matrices columns using the vectors elements as
-- coefficients:

select mxv('i4[0:0:1 0:1:2]'::matrix, 'i4[0:0 1:1]'::vector, 'plus_times_int32');

-- 'mxv' is also supported by the '@' operator:

select 'i4[0:0:1 0:1:2]'::matrix @ 'i4[0:0 1:1]'::vector;

-- Matrices can be multipled by vectors on the right taking the linear
-- combination of the matrices rows using the vectors elements as
-- coefficients:

select vxm('i4[0:0 1:1]'::vector, 'i4[0:0:1 0:1:2]'::matrix, 'plus_times_int32');

-- 'vxm' is also supported by the '@' operator:

select 'i4[0:0 1:1]'::vector @ 'i4[0:0:1 0:1:2]'::matrix;

-- The 'selection' method calls the 'GrB_select()' API function.  The
-- name 'selection' was chosen not to conflict with the SQL keyword
-- 'select'.  Selection provides a conditional operator called an
-- 'indexunaryop' and a parameter for the operator to use to compare
-- elements in the matrix.  Below, all elements with values greater
-- than 1 are returned:

select selection('i4[1:1:1 2:2:2 3:3:3]'::matrix, 'valuegt_int32'::indexunaryop, 1::scalar);

-- Here are all values equal to 2:

select selection('i4[1:1:1 2:2:2 3:3:3]'::matrix, 'valueeq_int32'::indexunaryop, 2::scalar);

-- Apply takes an operator called a 'unaryop' and applies it to every
-- element of the matrix.  The 'ainv_int32' returned the additive
-- inverse (the negative value for integers) of every element:

select apply('i4[1:1:1 2:2:2 3:3:3]'::matrix, 'ainv_int32'::unaryop);

-- The 'dup' function duplicates a matrix returning a new matrix
-- object with the same values:

select dup('i4[1:1:1 2:2:2 3:3:3]'::matrix);

-- The 'wait' method is used to "complete" a matrix, which may have
-- pending operations waiting to be performed when using the default
-- SuiteSparse non-blocking mode.  As a side effect, wait will sort
-- the elements of the input:

select wait('i4[2:2:2 3:3:3 1:1:1]'::matrix);

-- The 'clear' function clears the matrix of all elements and returns
-- the same object, but empty.  The dimensions do not change:

select clear('i4[1:1:1 2:2:2 3:3:3]'::matrix);
