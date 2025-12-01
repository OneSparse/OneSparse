\pset linestyle unicode
\pset border 2
-- # Matrix
--
-- This documentation is also tests for the code, the examples below
-- show the literal output of these statements from Postgres.
--
-- Some setup to make sure the extension is installed.

set client_min_messages = 'WARNING'; -- pragma:hide
create extension if not exists onesparse;

\o /dev/null
\set ECHO none
\i sql/fixtures.sql
\set ECHO all
\o

-- OneSparse wraps the
-- [SuiteSparse:GraphBLAS](http://faculty.cse.tamu.edu/davis/GraphBLAS.html)
-- library and extends Postgres by adding new types and functions that
-- allow you to do sparse and dense linear algebra in Postgres.  This
-- is similar to functionality packages like `numpy` and
-- `scipy.sparse` bring to Python.
--
-- While a full grasp of the GraphBLAS API is not necessary to follow
-- along with this guide there are many details that are not spelled
-- out in great details here.  For complete details on the API and
-- expected behaviors, see the [SuiteSparse:GraphBLAS User
-- Guide](https://github.com/DrTimothyAldenDavis/SuiteSparse/blob/dev/GraphBLAS/Doc/GraphBLAS_UserGuide.pdf).
--
-- The most fundamenteal object in OneSparse is a Matrix.  A Matrix is
-- a two dimensional array of data with a certain number of rows *m*
-- and columns *n*.  Typically matrices are very memory hungry data
-- structures, requiring `m * n` memory to hold all of the elements.
--
-- This limits traditional matrix libraries, because many problems in
-- linear algebra are *sparse*.  Not every element is used in the
-- problem or even definable.  Traditional linear algebra libraries
-- usually encode sparse matrices into dense matrices by using the
-- number zero to indicate "nothing", but this approach does not solve
-- the memory problem.  For matrices with a large number of rows and
-- columns this means vast areas of memories filled with zeros that
-- end up being multiplied away, which also wastes time and energy.
--
-- OneSparse matrices however are smart, and can adapt to the number
-- of actually useful elements in a Matrix.  They can be dense or
-- sparse, the SuiteSparse library will adapt to choose the right
-- backend format.
--
-- ## Matrices and Graphs
--
-- Every matrix is a graph, whether you think of it that way or not.
-- And every graph has a corresponding matrix.  A lot of data that you
-- put into postgres tables can also describe a graph, and thus a
-- matrix.  These three different ways of thinking about tables,
-- graphs, and matrices is one of the core concepts of OneSparse:
--
-- ![Tables, Graphs, and Matrices](./table_graph_matrix.png)
--
-- While SuiteSparse is optimized for processing sparse matrices and
-- vectors, it also supports optimized kernels for dense objects.  A
-- dense matrix is just a sparse matrix with all its elements.  In
-- this case SuiteSparse will automatically store it in a dense
-- optimal format and use CPUs or GPUs appropriately to process them.
--
-- ## Getting Started
--
-- The examples below are all what you would see typing the exact
-- queries out in `psql`.  The GraphBLAS API is large, so onesparse is
-- always contained in the `onesparse` postgres schema.  For the sake
-- of brevity, let's set the `search_path` so that we can just type
-- `matrix` instead of `onesparse.matrix` everywhere.

SET search_path TO public,onesparse;

-- If the matrix has bounds, it can be printed to the console if they
-- are reasonable size, this is useful for debugging and
-- experimentation:
--

select print('int32(4:4)'::matrix);

-- Note that this matrix is empty, it's not filled with "zeros", it
-- contains *no elements*.  The memory needed to hold the matrix
-- contains only stored elements, if there isn't a value stored at a
-- given row or column position, no memory is consumed.  This is the
-- "sparse" in sparse matrix.  This is how it's possible to create an
-- unbounded row by unbounded column matrix without exhausting memory
-- trying to allocate `2^120` entries.
--

-- ## Drawing Matrices and Vectors
--
-- The `draw()` function turns a matrix into the Graphviz DOT language
-- that is used to draw graph diagrams:

select draw('int32(4:4)[1:2:1 2:3:2 3:1:3]'::matrix) as draw;

-- Will generate the following diagram:

select draw('int32(4:4)[1:2:1 2:3:2 3:1:3]'::matrix) as draw_source \gset
\i sql/draw.sql

-- Let's look at our cast of test objects for the remaining examples.
-- These objects from the `onesparse.test_fixture` table.

\x
select * from test_fixture;
\x
--
-- Here are a couple of sparse matrices from the test_fixture table.
-- We'll call them `a` and `b` in these docs:
--

select print(a) as a, print(b) as b from test_fixture;

select draw(a) as twocol_a_source, draw(b) as twocol_b_source from test_fixture \gset
\i sql/twocol.sql

-- Here are some sparse test vectors, they will be used for some of
-- the examples below:
--

select print(u) as u, print(v) as v from test_fixture;

select draw(u) as twocol_a_source, draw(v) as twocol_b_source from test_fixture \gset
\i sql/twocol.sql

-- There is also an example "dense" matrix named 'd':
--

select print(d) from test_fixture;

-- One way of thinking about a "dense" matrix is a fully connected
-- graph, these can be constructed with the `dense_matrix()` function:
--

select draw(dense_matrix(4, 4, 1)) as draw_source from test_fixture \gset
\i sql/draw.sql

-- And another matrix named 's' which is a Sierpinsky Graph, which
-- we'll show off a bit later.
--

select print(s) from test_fixture;

-- ## Random Matrices
--
-- `random_matrix` will generate a random matrix provided the type,
-- number of rows, number of columns, and the number of (approximate)
-- values, an optional max value, and an optional random seed for
-- deterministic generation:

select print(random_matrix('int8', 8, 8, 0.5, 42) % 42) as random_matrix;

-- This random matrix is also a random *graph*:
--

select draw(random_matrix('int8', 8, 8, 0.5, 42) % 42) as draw_source \gset
\i sql/draw.sql

-- ## Empty Matrices
--
-- The `matrix` data type wraps a SuiteSparse GrB_Matrix handle and
-- delegates functions from SQL to the library through instances of
-- this type.
--
-- An empty matrix can be constructed many ways, but one of the
-- simplest is casting a type code to the matrix type.  In this case
-- `int32` means the SuiteSparse type `GrB_INT32`.

select 'int32'::matrix;

-- Another way to create an empty matrix is to use the `matrix()`
-- constructor function:

select matrix('int32');

-- ## Matrix dimensions
--
-- The above matrices are "unbounded", they do not have a fixed number
-- of rows and/or columns.  The default possible number of rows and
-- columns is defined by the SuiteSparse library to be `GrB_INDEX_MAX`
-- which is `2^60` power indexes.  For the purposes of this
-- documentation this will be referred to as `INDEX_MAX` and matrices
-- and vector dimensions that are `INDEX_MAX` in size are reffered to
-- as "unbounded".
--
-- For matrices with known dimensions, the dimensions can be provided
-- in parentesis after the type code.  Here a 4 row by 4 column
-- matrix is created:

select 'int32(4:4)'::matrix;

-- Another way to make a new matrix is with the `matrix` constructor
-- function.

select matrix('int32', 4, 4);

-- Either dimension can be ommited, this creates a 4 row by unbounded
-- column matrix.

select 'int32(4:)'::matrix;

-- This creates a unbounded row by 4 column matrix.

select 'int32(:4)'::matrix;

-- All graphblas operations are exposed by a series of functions and
-- operators.  Here we see three very common operations, returning the
-- number of rows, the number of columns, and the number of store
-- values.

select nrows('int32'::matrix),
       ncols('int32'::matrix),
       nvals('int32'::matrix);

-- Above you can see the matrix has unbounded rows and columns (the
-- very large number is the number of *possible* entries).  And the
-- number of stored values is zero.  These matrices are empty, they
-- contain no elements.
--
-- Values can be specified after the `type(dimension)` prefix as an
-- array of elements between square brackets.  Empty brackets imply no
-- elements, so empty square brackets are the same as no square
-- brackets as above:

select nrows('int32[]'::matrix),
       ncols('int32[]'::matrix),
       nvals('int32[]'::matrix);

-- Elements are specified between square brackets are coordinates of
-- 'row_id:column_id:value' separated by spaces:

select 'int32[1:2:1 2:3:2 3:1:3]'::matrix,
       'int32(4:)[1:2:1 2:3:2 3:1:3]'::matrix,
       'int32(:4)[1:2:1 2:3:2 3:3:1]'::matrix;

-- ## Elements
--
-- All the elements in a matrix can be iterated with the `elements()`
-- function:

select * from elements((select a from test_fixture));

-- The inverse operation of constructing matrices from rows can be
-- done with `matrix_agg()`:

select matrix_agg(i, i, i) as unbound_matrix from generate_series(0, 10) as i;

-- Aggregate matrices are always unbounded so use `resize()` to bound
-- the matrix:

select print(resize(matrix_agg(i, i, i), 10, 10)) as bound_matrix from generate_series(0, 10) as i;

-- ## Equality
--
-- Two matrices can be compared for equality with the '=' and '!=' operators:

select a != b as "a != b", a = b as "a = b", b = a as "b = a", b = b as "b = b" from test_fixture;

-- ## Setting and Getting individual Elements

-- Elements can be set individually with `set_element`, the modified
-- input is returned:

select print(set_element(a, 1, 1, 1)) as set_element from test_fixture;

-- Scalar elements can be extracted individually with `get_element`

select get_element(a, 3, 2) as get_element from test_fixture;

-- If an element does exist `get_element` will return an "empty"
-- scalar:

select get_element(a, 3, 3) as get_element from test_fixture;

-- ## Elementwise Addition
--
-- The GraphBLAS API has elementwise operations on matrices that
-- operate pairs of matrices.  `eadd` computes the element-wise
-- “addition” of two matrices a and b, element-wise using any binary
-- operator.  The "add" in the name means that the union of both
-- graphs is taken; elements present on both sides of the operation
-- are included in the result.

select print(a) as a, binaryop, print(b) as b, print(eadd(a, b, binaryop)) as "eadd(a, b, binaryop)" from test_fixture;

-- Eadd can also be accomplished with binary operators specific to
-- OneSparse.  Different binaryops are passed to eadd to do different
-- elementwise operations:

select print(a |+ b) as "a |+ b", print(a |- b) as "a |- b", print(a |* b) as "a |* b", print(a |/ b) as "a |/ b" from test_fixture;

-- From a graph standpoint, elementwise addition can be seen as the
-- merging ("union") of two graphs, such that the result has edges
-- from both graphs.  Any edges that occur in both graphs are merged
-- with the provided binary operator.

select draw(a) as binop_a_source, draw(b) as binop_b_source, draw(eadd(a, b, binaryop)) as binop_c_source from test_fixture \gset

\i sql/binop.sql

-- ## Elementwise Multiplication
--
-- `emult` multiplies elements of two matrices, taking only the
-- intersection of common elements in both matrices, if an element is
-- missing from either the left or right side, it is ommited from the
-- result:

select print(a) as a, binaryop, print(b) as b, print(emult(a, b, binaryop)) as "emult(a, b, binaryop)" from test_fixture;

-- Emult can also be accomplished with binary operators specific to
-- OneSparse.  Different binaryops are passed to emult to do different
-- elementwise operations:

select print(a &+ b) as "a &+ b", print(a &- b) as "a &- b", print(a &* b) as "a &* b", print(a &/ b) as "a &/ b" from test_fixture;

-- From a graph standpoint, elementwise multiplication can be seen as
-- the intersection of two graphs, such that the result has edges that
-- are only present in both graphs.  The edges are combined with the
-- provided binary operator.

select draw(a) as binop_a_source, draw(b) as binop_b_source, draw(emult(a, b, binaryop)) as binop_c_source from test_fixture \gset

\i sql/binop.sql

-- ## Elementwise Union
--
-- `eunion` is like `eadd` but differs in how the binary op is
-- applied. A pair of scalars, `alpha` and `beta` define the inputs to
-- the operator when entries are present in one matrix but not the
-- other.

select print(a) as a, binaryop, print(b) as b, print(eunion(a, 3::scalar, b, 4::scalar, binaryop)) as "eunion(a, 3::scalar, b, 4::scalar, binaryop)" from test_fixture;

-- From a graph standpoint, elementwise union is very similar to
-- `eadd()`, and can be seen as the merging ("union") of two graphs,
-- such that the result has edges from both graphs.  Any edges that
-- occur in both graphs are merged with the provided binary operator.
-- If an edge occurs in a but not in b, it is combined with the
-- scalar `alpha`, if the edge occurs in the b but not in a, then
-- the edge is combined with scalar `beta`.

select draw(a) as binop_a_source, draw(b) as binop_b_source, draw(eunion(a, 3::scalar, b, 4::scalar, binaryop)) as binop_c_source from test_fixture \gset

\i sql/binop.sql

-- ## Reduction
--
-- The entire matrix can be reduced to a scalar value:

select print(b) as b, 'plus_monoid_int32' as monoid, reduce_scalar(b) from test_fixture;

-- The entire matrix can be reduced to a scalar value with a provided
-- monoid that changes the reduction operation:

select print(b) as b, 'min_monoid_int32' as monoid, reduce_scalar(b, 'min_monoid_int32') from test_fixture;

-- The matrix can also be reduced to a column vector:

select print(b) as b, 'plus_monoid_int32' as monoid, print(reduce_cols(b)) as reduce_cols from test_fixture;

-- To reduce a row vector:

select print(b) as b, 'plus_monoid_int32' as monoid, print(reduce_rows(b)) as reduce_rows from test_fixture;

-- ## Matrix Matrix Multiplication
--
-- Matrix Multiplication is the heart of linear algebra.  All matrix
-- multiplication happens over a semiring.  For the most common form
-- of matrix multiplication, the outer opperation is to multiply
-- coresponding elements with the "times" operator and then reduce
-- those products with the "plus" operator.  This is called the
-- `plus_times` semiring:

select print(a) as a, semiring, print(b) as b, print(mxm(a, b)) as "mxm(a, b)" from test_fixture;

select draw(a) as binop_a_source, draw(b) as binop_b_source, draw(mxm(a, b)) as binop_c_source from test_fixture \gset
\i sql/binop.sql

-- AxB can also be done with the `@` operator, mimicking the Python
-- syntax.  The default semiring for numeric types is `plus_times`.

select print(a) as a, '@' as "@", print(b) as b, print(a @ b) as "a @ b" from test_fixture;

-- ## Matrix Vector Multiplication
--
-- Matrices can be multipled by vectors on the right taking the linear
-- combination of the matrices columns using the vectors elements as
-- coefficients:

select print(a) as a, '@' as "@", semiring, print(u) as u, print(mxv(a, u)) as "mxv(a, u)" from test_fixture;

-- From a graph standpoint, matrix vector multiplication is used to
-- "pull" back to adjacent nodes from their incoming edges.  When
-- iterated, it forms the basis for working back along incoming links.

select draw(a) as binop_a_source, draw(u) as binop_b_source, draw(mxv(a, u)) as binop_c_source from test_fixture \gset
\i sql/binop.sql

-- 'mxv' is also supported by the `@` operator:

select print(a) as a, '@' as "@", print(u) as u, print(a @ u) as "a @ u" from test_fixture;

-- ## Vector Matrix Multiplication
--
-- Matrices can be multipled by vectors on the right taking the linear
-- combination of the matrices rows using the vectors elements as
-- coefficients:

select print(v) as v, semiring, print(b) as b, print(vxm(v, b, semiring)) as "vxm(v, b, semiring)" from test_fixture;

-- From a graph standpoint, vector matrix multiplication is used to
-- "push" forward to adjacent nodes from their outgoing edges.  When
-- iterated, it forms the basis for working forward along outgoing
-- edges.

select draw(v) as binop_a_source, draw(b) as binop_b_source, draw(vxm(v, b)) as binop_c_source from test_fixture \gset
\i sql/binop.sql

-- 'vxm' is also supported by the `@` operator:

select print(v) as v, '@' as "@", print(b) as b, print(v @ b) as "v @ b" from test_fixture;

-- ## Choosing Elements
--
-- The `choose` method calls the `GrB_select()` API function.  The
-- name `choose` was chosen not to conflict with the SQL keyword
-- `select`.  Selection provides a conditional operator called an
-- `indexunaryop` and a parameter for the operator to use to compare
-- elements in the matrix.  Below, all elements with values greater
-- than 1 are returned:

select print(a) as a, indexunaryop, print(choose(a, indexunaryop, 1)) as selected from test_fixture;

select draw(a) as uop_a_source, draw(choose(a, indexunaryop, 1)) as uop_b_source from test_fixture \gset
\i sql/uop.sql

-- ## Choosing Operators
-- Selection can also be done with scalars and operators:p

select print(a > 1) as "a > 1", print(a >= 1) as "a >= 1", print(a < 1) as "a < 1", print(a <= 1) as "a <= 1" from test_fixture;

-- A useful select operator is `triu`, it select only upper triangular
-- values from a given offset, where 0 is the digonal, this turns your
-- graph into a direct acyclic graph (DAG) by removing all the links
-- "back" from higher number nodes to lower.

select print(random_matrix('int8', 8, 8, 0.5, 42) % 42) as matrix,
       print(choose(random_matrix('uint8', 8, 8, 1, 42), 'triu', 1) % 42) as triu from test_fixture;

select draw(random_matrix('int8', 8, 8, 0.5, 42) % 42) as uop_a_source,
       draw(choose(random_matrix('int8', 8, 8, 0.5, 42) % 42, 'triu', 1)) as uop_b_source
       from test_fixture \gset
\i sql/uop.sql

-- ## Kronecker
--
-- The `kronecker()` function takes two input matrices, and replaces
-- every element in the second matrix with a new submatrix of the
-- first.  This "expands" the matrix exponentially.  This is useful
-- for constructing synthetic graphs with specific power law
-- distributions.
--

select print(s) as s, semiring, print(s) as s, print(kronecker(s, s, semiring)) as kronecker from test_fixture;

select draw(s) as binop_a_source, draw(s) as binop_b_source, draw(kronecker(s, s, semiring)) as binop_c_source from test_fixture \gset
\i sql/binop.sql

-- ## Kronecker Power
--
-- There's a special function for exponentiating a matrix to itself a
-- certain number of times, `kronpower`:

select print(kronpower(s, 2)) from test_fixture;

select draw(kronpower(s, 2)) as draw_source from test_fixture \gset
\i sql/draw.sql

-- Kronecker products can very quickly make huge graphs with power law
-- distributions.  These are handy synthetic graphs to mimic certain
-- statistical edge distributions common in sparse graph problems:

select nvals(kronpower(s, 4)) from test_fixture;

-- ## Transpose
--
-- A matrix can be transposed with the `transpose()` function:

select print(transpose(a)) from test_fixture;

-- ## Apply
--
-- `apply` takes an operator of type `unaryop` and applies it to every
-- element of the matrix.  The 'ainv_int32' returned the additive
-- inverse (the negative value for integers) of every element:

select print(a) as a, unaryop, print(apply(a, unaryop)) as applied from test_fixture;

-- ## SuiteSparse Info
--
-- The `info` function returns a descripton of the matrix from
-- SuiteSparse.

select info(a) from test_fixture;

-- ## Matrix Duplication
--
-- The `dup` function duplicates a matrix returning a new matrix
-- object with the same values:

select dup(a) from test_fixture;

-- ## Work Completion
--
-- The `wait` method is used to "complete" a matrix, which may have
-- pending operations waiting to be performed when using the default
-- SuiteSparse non-blocking mode.  As a side effect, wait will sort
-- the elements of the input:

select wait('int32[2:2:2 3:3:3 1:1:1]'::matrix);

-- The `clear` function clears the matrix of all elements and returns
-- the same object, but empty.  The dimensions do not change:

-- ## Clearing Matrices
--
select clear('int32[1:1:1 2:2:2 3:3:3]'::matrix);

-- ## Extra tests
--
-- This documentation also forms the basis for the onesparse tests,
-- These tests run the documentation against a live server, all the
-- above results are automatically generated.

-- Get number of rows from a matrix with row-only dimension specified.
select nrows('int32(10)[1:1:1 2:2:2 3:3:3]'::matrix);

-- Get number of columns from a matrix with row-only dimension specified.
select ncols('int32(10)[1:1:1 2:2:2 3:3:3]'::matrix);

-- Get number of stored values from a matrix with row-only dimension specified.
select nvals('int32(10)[1:1:1 2:2:2 3:3:3]'::matrix);

-- Get number of rows from a matrix with bounded rows and unbounded columns.
select nrows('int32(10:)[1:1:1 2:2:2 3:3:3]'::matrix);

-- Get number of columns from a matrix with bounded rows and unbounded columns.
select ncols('int32(10:)[1:1:1 2:2:2 3:3:3]'::matrix);

-- Get number of stored values from a matrix with bounded rows and unbounded columns.
select nvals('int32(10:)[1:1:1 2:2:2 3:3:3]'::matrix);

-- Get number of rows from a matrix with unbounded rows and bounded columns.
select nrows('int32(:10)[1:1:1 2:2:2 3:3:3]'::matrix);

-- Get number of columns from a matrix with unbounded rows and bounded columns.
select ncols('int32(:10)[1:1:1 2:2:2 3:3:3]'::matrix);

-- Get number of stored values from a matrix with unbounded rows and bounded columns.
select nvals('int32(:10)[1:1:1 2:2:2 3:3:3]'::matrix);

-- Get number of rows from a fully bounded matrix.
select nrows('int32(10:10)[1:1:1 2:2:2 3:3:3]'::matrix);

-- Get number of columns from a fully bounded matrix.
select ncols('int32(10:10)[1:1:1 2:2:2 3:3:3]'::matrix);

-- Get number of stored values from a fully bounded matrix.
select nvals('int32(10:10)[1:1:1 2:2:2 3:3:3]'::matrix);

-- ## Matrix Assignment
--
-- The assign operation allows setting a submatrix within a matrix.

-- Basic assign: replace elements at specific indices
select print(assign('int32(10:10)[0:0:1 2:2:3 4:4:5]'::matrix, 'int32(3:3)[0:0:10 1:1:20 2:2:30]'::matrix, array[5,6,7]::bigint[], array[5,6,7]::bigint[]));

-- Assign with mask
select print(assign('int32(10:10)[0:0:1 2:2:3 4:4:5]'::matrix, 'int32(3:3)[0:0:10 1:1:20 2:2:30]'::matrix, array[1,3,5]::bigint[], array[1,3,5]::bigint[], mask=>'int32(10:10)[1:1:1 3:3:1 5:5:1]'::matrix, descr=>'r'));

-- Assign scalar to indices
select print(assign('int32(10:10)[0:0:1 2:2:3 4:4:5]'::matrix, 99::int, array[2,4,6]::bigint[], array[2,4,6]::bigint[]));

-- Assign with accumulator
select print(assign('int32(10:10)[0:0:1 2:2:3 4:4:5 6:6:7]'::matrix, 'int32(3:3)[0:0:10 1:1:20 2:2:30]'::matrix, array[0,2,4]::bigint[], array[0,2,4]::bigint[], accum=>'plus_int32'::binaryop));

-- Assign empty submatrix
select print(assign('int32(10:10)[0:0:1 2:2:3 4:4:5]'::matrix, 'int32(0:0)[]'::matrix, array[]::bigint[], array[]::bigint[]));

-- Assign to empty matrix
select print(assign('int32(10:10)[]'::matrix, 'int32(3:3)[0:0:10 1:1:20 2:2:30]'::matrix, array[1,3,5]::bigint[], array[1,3,5]::bigint[]));

-- ## Matrix Extraction
--
-- The extract operation extracts a submatrix from a matrix.

-- Basic extract: get specific row/column indices
select print(extract_matrix('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix, array[1,3]::bigint[], array[1,3]::bigint[]));

-- Extract with indices array
select print(extract_matrix('int32(5:5)[0:0:1 1:1:2 2:2:3 3:3:4 4:4:5]'::matrix, array[0,2,4]::bigint[], array[0,2,4]::bigint[]));

-- Extract single element as matrix
select print(extract_matrix('int32(5:5)[0:0:10 1:1:20 2:2:30]'::matrix, array[1]::bigint[], array[1]::bigint[]));

-- Extract all diagonal elements
select print(extract_matrix('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix, array[0,1,2,3,4]::bigint[], array[0,1,2,3,4]::bigint[]));

-- Extract with mask
select print(extract_matrix('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix, array[0,1,2,3,4]::bigint[], array[0,1,2,3,4]::bigint[], mask=>'int32(5:5)[0:0:1 2:2:1 4:4:1]'::matrix));

-- Extract from sparse matrix
select print(extract_matrix('int32(20:20)[5:5:100 10:10:200 15:15:300]'::matrix, array[5,10,15]::bigint[], array[5,10,15]::bigint[]));

-- ## Element Manipulation
--
-- Operations for checking and removing individual elements.

-- Check if matrix contains element at position
select contains('int32(5:5)[0:0:1 2:2:3 4:4:5]'::matrix, 0, 0);
select contains('int32(5:5)[0:0:1 2:2:3 4:4:5]'::matrix, 1, 1);
select contains('int32(5:5)[0:0:1 2:2:3 4:4:5]'::matrix, 2, 2);

-- Check multiple positions
select i, j, contains('int32(5:5)[0:0:1 2:2:3 4:4:5]'::matrix, i, j) as present
from generate_series(0, 4) as i, generate_series(0, 4) as j
where i = j;

-- Remove element from matrix
select print(remove_element('int32(5:5)[0:0:1 1:1:2 2:2:3 3:3:4]'::matrix, 1, 1));

-- Remove multiple elements
select print(remove_element(
    remove_element('int32(5:5)[0:0:1 1:1:2 2:2:3 3:3:4]'::matrix, 1, 1),
    3, 3
));

-- Remove non-existent element (no-op)
select print(remove_element('int32(5:5)[0:0:1 2:2:3 4:4:5]'::matrix, 1, 1));

-- ## Matrix Norms
--
-- Calculate various norms of matrices.

select norm('fp64(3:3)[0:0:3.0 1:1:-4.0 2:2:5.0]'::matrix);

-- Norm of sparse matrix
select norm('fp64(100:100)[10:10:3.0 50:50:4.0 90:90:5.0]'::matrix);

-- Norm of empty matrix
select norm('fp64(5:5)[]'::matrix);

-- Norm of single element
select norm('fp64(3:3)[1:1:5.0]'::matrix);

-- ## Matrix Comparison Select
--
-- Use comparison operators with choose/select operations.

-- Select elements greater than threshold
select print(choose('int32(5:5)[0:0:10 1:1:20 2:2:5 3:3:30 4:4:15]'::matrix, 'valuegt_int32'::indexunaryop, 10::int));

-- Select elements less than threshold
select print(choose('int32(5:5)[0:0:10 1:1:20 2:2:5 3:3:30 4:4:15]'::matrix, 'valuelt_int32'::indexunaryop, 10::int));

-- Select elements greater than or equal to threshold
select print(choose('int32(5:5)[0:0:10 1:1:20 2:2:15 3:3:30 4:4:15]'::matrix, 'valuege_int32'::indexunaryop, 20::int));

-- Select elements less than or equal to threshold
select print(choose('int32(5:5)[0:0:10 1:1:20 2:2:15 3:3:30 4:4:15]'::matrix, 'valuele_int32'::indexunaryop, 20::int));

-- Select non-zero elements
select print(choose('int32(5:5)[0:0:10 1:1:0 2:2:5 3:3:0 4:4:15]'::matrix, 'valuene_int32'::indexunaryop, 0::int));

-- Select equal elements
select print(choose('int32(5:5)[0:0:10 1:1:20 2:2:15 3:3:30 4:4:15]'::matrix, 'valueeq_int32'::indexunaryop, 15::int));

-- Comparison with float matrices
select print(choose('fp64(4:4)[0:0:1.5 1:1:2.5 2:2:1.0 3:3:3.5]'::matrix, 'valuegt_fp64'::indexunaryop, 2.0::double precision));

-- ## Matrix Cast
--
-- Type conversion between matrix types.

-- Cast int32 to int64
select cast_to('int32(3:3)[0:0:1 1:1:2 2:2:3]'::matrix, 'int64');

-- Cast int32 to float
select cast_to('int32(3:3)[0:0:1 1:1:2 2:2:3]'::matrix, 'fp64');

-- Cast float to int (truncates)
select cast_to('fp64(3:3)[0:0:1.7 1:1:2.3 2:2:3.9]'::matrix, 'int32');

-- Cast between integer sizes
select cast_to('int64(3:3)[0:0:100 1:1:200 2:2:300]'::matrix, 'int32');
select cast_to('int32(3:3)[0:0:1 1:1:2 2:2:3]'::matrix, 'int16');

-- Cast bool to int
select cast_to('bool(3:3)[0:0:true 1:1:false 2:2:true]'::matrix, 'int32');

-- Cast int to bool (0=false, non-zero=true)
select cast_to('int32(4:4)[0:0:0 1:1:1 2:2:5 3:3:0]'::matrix, 'bool');

-- Cast preserves sparsity
select nrows(cast_to('int32(100:100)[10:10:5 50:50:10 90:90:15]'::matrix, 'fp64'));

-- ## Matrix Resize
--
-- Change the dimension bounds of a matrix.

-- Resize to larger size
select nrows(resize('int32(10:10)[0:0:1 2:2:3 4:4:5]'::matrix, 20, 20));

-- Resize to smaller size (keeps elements that fit)
select print(resize('int32(10:10)[0:0:1 2:2:3 4:4:5 8:8:9]'::matrix, 5, 5));

-- Resize unbounded matrix
select nrows(resize('int32[0:0:1 2:2:3 4:4:5]'::matrix, 10, 10));

-- Resize to same size (no-op)
select print(resize('int32(10:10)[0:0:1 2:2:3 4:4:5]'::matrix, 10, 10));

-- Resize empty matrix
select nrows(resize('int32(10:10)[]'::matrix, 20, 20));

-- ## Matrix Info
--
-- Get internal SuiteSparse information about matrices.

-- Basic info
select info('int32(5:5)[0:0:1 1:1:2 2:2:3]'::matrix);

-- Info on sparse matrix
select info('int32(1000:1000)[10:10:5 500:500:10 990:990:15]'::matrix);

-- Info on empty matrix
select info('int32(10:10)[]'::matrix);

-- ## Matrix Aggregation
--
-- Aggregate table data into matrices.

-- Create test table for aggregation
create temporary table mat_agg_test (row_idx bigint, col_idx bigint, val integer);
insert into mat_agg_test values (0, 0, 10), (1, 1, 20), (2, 2, 30), (0, 1, 15);

-- Aggregate into matrix
select matrix_agg(row_idx, col_idx, val) from mat_agg_test;

-- Aggregate empty table
delete from mat_agg_test;
select matrix_agg(row_idx, col_idx, val) from mat_agg_test;

-- Aggregate with different types
create temporary table mat_agg_float (row_idx bigint, col_idx bigint, val double precision);
insert into mat_agg_float values (0, 0, 1.5), (1, 1, 2.5), (2, 2, 3.5);
select matrix_agg(row_idx, col_idx, val) from mat_agg_float;

-- Aggregate bool values
create temporary table mat_agg_bool (row_idx bigint, col_idx bigint, val boolean);
insert into mat_agg_bool values (0, 0, true), (1, 1, false), (2, 2, true), (0, 1, false);
select matrix_agg(row_idx, col_idx, val) from mat_agg_bool;

drop table mat_agg_test;
drop table mat_agg_float;
drop table mat_agg_bool;

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
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'ainv_int32'::unaryop,
    mask=>'bool(5:5)[0:0:true 1:1:false 2:2:true 3:3:false 4:4:true]'::matrix));

-- Structural mask (s): all existing positions allow updates, regardless of value
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'ainv_int32'::unaryop,
    mask=>'bool(5:5)[0:0:true 1:1:false 2:2:true 3:3:false 4:4:true]'::matrix,
    descr=>'s'::descriptor));

-- Structural mask with integer values (all non-empty positions)
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'abs_int32'::unaryop,
    mask=>'int32(5:5)[0:0:0 1:1:100 2:2:0 3:3:200 4:4:0]'::matrix,
    descr=>'s'::descriptor));

-- Compare: without structural, zeros might block
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'abs_int32'::unaryop,
    mask=>'int32(5:5)[0:0:0 1:1:100 2:2:0 3:3:200 4:4:0]'::matrix));

-- Structural mask with eadd
select print(eadd('int32(5:5)[0:0:10 1:1:20 2:2:30]'::matrix,
    'int32(5:5)[1:1:5 2:2:6 3:3:7]'::matrix,
    'plus_int32'::binaryop,
    mask=>'int32(5:5)[0:0:0 1:1:1 2:2:0 3:3:1]'::matrix,
    descr=>'s'::descriptor));

-- Structural mask with emult
select print(emult('int32(5:5)[0:0:2 1:1:3 2:2:4 3:3:5 4:4:6]'::matrix,
    'int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'times_int32'::binaryop,
    mask=>'bool(5:5)[0:0:false 1:1:true 2:2:false 3:3:true 4:4:false]'::matrix,
    descr=>'s'::descriptor));

-- ## Complement Mask Descriptor (c)
--
-- Complement inverts the mask: masked positions become unmasked and vice versa.

-- Normal mask: update positions 0, 2, 4
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'ainv_int32'::unaryop,
    mask=>'int32(5:5)[0:0:1 2:2:1 4:4:1]'::matrix));

-- Complement mask: update positions 1, 3 (opposite of normal)
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'ainv_int32'::unaryop,
    mask=>'int32(5:5)[0:0:1 2:2:1 4:4:1]'::matrix,
    descr=>'c'::descriptor));

-- Complement with boolean mask
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'abs_int32'::unaryop,
    mask=>'bool(5:5)[0:0:true 1:1:true 2:2:false 3:3:false 4:4:true]'::matrix,
    descr=>'c'::descriptor));

-- Complement with empty mask (all positions updated)
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30]'::matrix,
    'abs_int32'::unaryop,
    mask=>'int32(5:5)[]'::matrix,
    descr=>'c'::descriptor));

-- Complement with full mask (no positions updated)
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'abs_int32'::unaryop,
    mask=>'int32(5:5)[0:0:1 1:1:1 2:2:1 3:3:1 4:4:1]'::matrix,
    descr=>'c'::descriptor));

-- Complement mask with eadd
select print(eadd('int32(5:5)[0:0:10 1:1:20 2:2:30]'::matrix,
    'int32(5:5)[1:1:5 2:2:6 3:3:7]'::matrix,
    'plus_int32'::binaryop,
    mask=>'int32(5:5)[1:1:1 2:2:1]'::matrix,
    descr=>'c'::descriptor));

-- Complement mask with emult
select print(emult('int32(5:5)[0:0:2 1:1:3 2:2:4 3:3:5 4:4:6]'::matrix,
    'int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'times_int32'::binaryop,
    mask=>'int32(5:5)[0:0:1 2:2:1 4:4:1]'::matrix,
    descr=>'c'::descriptor));

-- ## Structural Complement Descriptor (sc)
--
-- Combine structural and complement: use pattern (not values) and invert.

-- sc descriptor: structural interpretation then complement
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'ainv_int32'::unaryop,
    mask=>'int32(5:5)[0:0:0 2:2:0 4:4:100]'::matrix,
    descr=>'sc'::descriptor));

-- Compare with just structural
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'ainv_int32'::unaryop,
    mask=>'int32(5:5)[0:0:0 2:2:0 4:4:100]'::matrix,
    descr=>'s'::descriptor));

-- Compare with just complement
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'ainv_int32'::unaryop,
    mask=>'int32(5:5)[0:0:0 2:2:0 4:4:100]'::matrix,
    descr=>'c'::descriptor));

-- sc with eadd
select print(eadd('int32(5:5)[0:0:10 1:1:20 2:2:30]'::matrix,
    'int32(5:5)[1:1:5 2:2:6 3:3:7]'::matrix,
    'plus_int32'::binaryop,
    mask=>'int32(5:5)[0:0:0 1:1:0 2:2:1]'::matrix,
    descr=>'sc'::descriptor));

-- ## Replace Descriptor (r)
--
-- Replace clears the output matrix before the operation, removing all
-- existing values not produced by the operation.

-- Without replace: existing values at unmasked positions remain
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'ainv_int32'::unaryop,
    mask=>'int32(5:5)[0:0:1 2:2:1]'::matrix));

-- With replace: only masked result positions remain
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'ainv_int32'::unaryop,
    mask=>'int32(5:5)[0:0:1 2:2:1]'::matrix,
    descr=>'r'::descriptor));

-- Replace with sparse mask
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'abs_int32'::unaryop,
    mask=>'int32(5:5)[1:1:1]'::matrix,
    descr=>'r'::descriptor));

-- Replace with full mask (all results kept)
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'ainv_int32'::unaryop,
    mask=>'int32(5:5)[0:0:1 1:1:1 2:2:1 3:3:1 4:4:1]'::matrix,
    descr=>'r'::descriptor));

-- Replace with eadd
select print(eadd('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'int32(5:5)[1:1:5 2:2:6 3:3:7]'::matrix,
    'plus_int32'::binaryop,
    mask=>'int32(5:5)[1:1:1 2:2:1]'::matrix,
    descr=>'r'::descriptor));

-- Replace with emult
select print(emult('int32(5:5)[0:0:2 1:1:3 2:2:4 3:3:5 4:4:6]'::matrix,
    'int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'times_int32'::binaryop,
    mask=>'int32(5:5)[0:0:1 2:2:1 4:4:1]'::matrix,
    descr=>'r'::descriptor));

-- ## Combined Descriptors (rc, rs, rsc)
--
-- Test combinations of replace with other descriptors.

-- rc: replace + complement
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'ainv_int32'::unaryop,
    mask=>'int32(5:5)[0:0:1 2:2:1]'::matrix,
    descr=>'rc'::descriptor));

-- rs: replace + structural
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'abs_int32'::unaryop,
    mask=>'int32(5:5)[0:0:0 2:2:100 4:4:0]'::matrix,
    descr=>'rs'::descriptor));

-- rsc: replace + structural + complement
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'ainv_int32'::unaryop,
    mask=>'int32(5:5)[1:1:0 3:3:0]'::matrix,
    descr=>'rsc'::descriptor));

-- ## Descriptors with Accumulators
--
-- Test how descriptors interact with accumulator operations.

-- Normal accumulation with mask
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'ainv_int32'::unaryop,
    mask=>'int32(5:5)[0:0:1 2:2:1 4:4:1]'::matrix,
    accum=>'plus_int32'::binaryop));

-- Accumulation with complement mask
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'ainv_int32'::unaryop,
    mask=>'int32(5:5)[0:0:1 2:2:1 4:4:1]'::matrix,
    accum=>'plus_int32'::binaryop,
    descr=>'c'::descriptor));

-- Accumulation with structural mask
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'abs_int32'::unaryop,
    mask=>'int32(5:5)[0:0:0 2:2:1 4:4:0]'::matrix,
    accum=>'times_int32'::binaryop,
    descr=>'s'::descriptor));

-- Accumulation with replace
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'ainv_int32'::unaryop,
    mask=>'int32(5:5)[0:0:1 2:2:1]'::matrix,
    accum=>'plus_int32'::binaryop,
    descr=>'r'::descriptor));

-- ## Basic Masking with apply()
--
-- The mask parameter controls which elements of the output are written.

-- Apply with mask: only masked positions are updated
select print(apply('int32(5:5)[0:0:1 1:1:2 2:2:3 3:3:4 4:4:5]'::matrix, 'abs_int32'::unaryop,
    mask=>'int32(5:5)[0:0:1 2:2:1 4:4:1]'::matrix));

-- Apply with dense mask
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix, 'ainv_int32'::unaryop,
    mask=>'int32(5:5)[0:0:1 1:1:1 2:2:1 3:3:1 4:4:1]'::matrix));

-- Apply with empty mask (nothing updated)
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30]'::matrix, 'abs_int32'::unaryop,
    mask=>'int32(5:5)[]'::matrix));

-- Apply with sparse mask
select print(apply('int32(5:5)[0:0:1 1:1:2 2:2:3 3:3:4 4:4:5]'::matrix, 'minv_int32'::unaryop,
    mask=>'int32(5:5)[1:1:1 3:3:1]'::matrix));

-- Apply with boolean mask
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix, 'abs_int32'::unaryop,
    mask=>'bool(5:5)[0:0:true 2:2:true 4:4:true]'::matrix));

-- ## Masking with eadd()
--
-- Mask controls which elements participate in the element-wise operation.

-- Element-wise add with mask
select print(eadd('int32(5:5)[0:0:10 1:1:20 2:2:30]'::matrix,
    'int32(5:5)[1:1:5 2:2:6 3:3:7]'::matrix,
    'plus_int32'::binaryop,
    mask=>'int32(5:5)[1:1:1 2:2:1]'::matrix));

-- Element-wise add with full mask
select print(eadd('int32(5:5)[0:0:1 1:1:2 2:2:3 3:3:4 4:4:5]'::matrix,
    'int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'times_int32'::binaryop,
    mask=>'int32(5:5)[0:0:1 1:1:1 2:2:1 3:3:1 4:4:1]'::matrix));

-- ## Masking with emult()
--
-- Mask filters the element-wise multiplication result.

-- Element-wise multiply with mask
select print(emult('int32(5:5)[0:0:2 1:1:3 2:2:4 3:3:5 4:4:6]'::matrix,
    'int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'times_int32'::binaryop,
    mask=>'int32(5:5)[0:0:1 2:2:1 4:4:1]'::matrix));

-- Element-wise multiply with boolean mask
select print(emult('fp64(5:5)[0:0:1.5 1:1:2.5 2:2:3.5 3:3:4.5 4:4:5.5]'::matrix,
    'fp64(5:5)[0:0:2.0 1:1:3.0 2:2:4.0 3:3:5.0 4:4:6.0]'::matrix,
    'times_fp64'::binaryop,
    mask=>'bool(5:5)[0:0:true 1:1:true 2:2:false 3:3:true 4:4:false]'::matrix));

-- Element-wise multiply with min operator and mask
select print(emult('int32(5:5)[0:0:100 1:1:200 2:2:50 3:3:300 4:4:25]'::matrix,
    'int32(5:5)[0:0:150 1:1:100 2:2:75 3:3:250 4:4:50]'::matrix,
    'min_int32'::binaryop,
    mask=>'int32(5:5)[1:1:1 3:3:1]'::matrix));

-- ## Masking with eunion()
--
-- eunion uses default values for missing elements.

-- Element-wise union with mask
select print(eunion('int32(5:5)[0:0:10 2:2:30]'::matrix,
    0::int,
    'int32(5:5)[1:1:20 3:3:40]'::matrix,
    0::int,
    'plus_int32'::binaryop,
    mask=>'int32(5:5)[0:0:1 1:1:1 2:2:1]'::matrix));

-- ## Masking with select/choose()
--
-- Combine selection with masking for complex filtering.

-- Select with mask
select print(choose('int32(5:5)[0:0:10 1:1:20 2:2:5 3:3:30 4:4:15]'::matrix,
    'valuegt_int32'::indexunaryop,
    15::int,
    mask=>'int32(5:5)[0:0:1 1:1:1 2:2:1 3:3:1 4:4:1]'::matrix));

-- Select with boolean mask
select print(choose('fp64(5:5)[0:0:1.5 1:1:2.5 2:2:1.0 3:3:3.5 4:4:2.0]'::matrix,
    'valuege_fp64'::indexunaryop,
    2.0::double precision,
    mask=>'bool(5:5)[0:0:true 2:2:true 4:4:true]'::matrix));

-- ## Structural vs Value Masks
--
-- Structural masks consider only the pattern (which indices exist),
-- not the actual values.

-- Structural mask: only considers which positions exist
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'ainv_int32'::unaryop,
    mask=>'bool(5:5)[0:0:true 2:2:true 4:4:true]'::matrix,
    descr=>'s'::descriptor));

-- Structural mask with integer matrix
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'abs_int32'::unaryop,
    mask=>'int32(5:5)[1:1:100 3:3:200 4:4:0]'::matrix,
    descr=>'s'::descriptor));

-- ## Mask Type Compatibility
--
-- Test masks of different types with matrices.

-- int32 matrix with int64 mask
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30 3:3:40 4:4:50]'::matrix,
    'abs_int32'::unaryop,
    mask=>'int64(5:5)[0:0:1 2:2:1 4:4:1]'::matrix));

-- fp64 matrix with bool mask
select print(apply('fp64(5:5)[0:0:1.5 1:1:2.5 2:2:3.5 3:3:4.5 4:4:5.5]'::matrix,
    'abs_fp64'::unaryop,
    mask=>'bool(5:5)[0:0:true 2:2:true 4:4:true]'::matrix));

-- fp64 matrix with int32 mask
select print(apply('fp64(5:5)[0:0:1.5 1:1:2.5 2:2:3.5 3:3:4.5 4:4:5.5]'::matrix,
    'ainv_fp64'::unaryop,
    mask=>'int32(5:5)[1:1:1 3:3:1]'::matrix));

-- ## Edge Cases
--
-- Test boundary conditions for masks.

-- Empty matrix with non-empty mask
select print(apply('int32(5:5)[]'::matrix,
    'abs_int32'::unaryop,
    mask=>'int32(5:5)[0:0:1 2:2:1 4:4:1]'::matrix));

-- Non-empty matrix with empty mask
select print(apply('int32(5:5)[0:0:10 1:1:20 2:2:30]'::matrix,
    'abs_int32'::unaryop,
    mask=>'int32(5:5)[]'::matrix));

-- ## Boolean Matrix Operations
--
-- Test boolean-specific operations and logic.

-- Boolean construction
select 'bool(5:5)[0:0:true 1:1:false 2:2:true 3:3:false 4:4:true]'::matrix;
select 'bool(5:5)[0:0:t 1:1:f 2:2:t 3:3:f 4:4:t]'::matrix;

-- Boolean element-wise OR
select print(eadd('bool(5:5)[0:0:true 1:1:false 2:2:true]'::matrix,
    'bool(5:5)[0:0:false 1:1:true 2:2:true]'::matrix,
    'lor'::binaryop));

-- Boolean element-wise AND
select print(emult('bool(5:5)[0:0:true 1:1:false 2:2:true 3:3:true 4:4:false]'::matrix,
    'bool(5:5)[0:0:true 1:1:true 2:2:false 3:3:true 4:4:false]'::matrix,
    'land'::binaryop));

-- Boolean element-wise XOR
select print(eadd('bool(5:5)[0:0:true 1:1:false 2:2:true]'::matrix,
    'bool(5:5)[0:0:false 1:1:false 2:2:true]'::matrix,
    'lxor'::binaryop));

-- Boolean element-wise XNOR
select print(eadd('bool(5:5)[0:0:true 1:1:false 2:2:true]'::matrix,
    'bool(5:5)[0:0:false 1:1:false 2:2:true]'::matrix,
    'eq_bool'::binaryop));

-- Boolean NOT (unary complement)
select print(apply('bool(5:5)[0:0:true 1:1:false 2:2:true 3:3:false 4:4:true]'::matrix,
    'lnot'::unaryop));

-- Boolean reduction with OR (any true?)
select reduce_scalar('bool(3:3)[0:0:false 1:1:false 2:2:true]'::matrix,
    'lor_monoid_bool'::monoid);
select reduce_scalar('bool(3:3)[0:0:false 1:1:false 2:2:false]'::matrix,
    'lor_monoid_bool'::monoid);

-- Boolean reduction with AND (all true?)
select reduce_scalar('bool(3:3)[0:0:true 1:1:true 2:2:true]'::matrix,
    'land_monoid_bool'::monoid);
select reduce_scalar('bool(3:3)[0:0:true 1:1:false 2:2:true]'::matrix,
    'land_monoid_bool'::monoid);

-- Boolean reduction with XOR (parity)
select reduce_scalar('bool(3:3)[0:0:true 1:1:true 2:2:true]'::matrix,
    'lxor_monoid_bool'::monoid);
select reduce_scalar('bool(3:3)[0:0:true 1:1:false 2:2:true]'::matrix,
    'lxor_monoid_bool'::monoid);

-- Boolean to integer conversion
select cast_to('bool(4:4)[0:0:true 1:1:false 2:2:true 3:3:false]'::matrix, 'int32');

-- Integer to boolean conversion
select cast_to('int32(5:5)[0:0:0 1:1:1 2:2:5 3:3:0 4:4:-1]'::matrix, 'bool');

-- Boolean comparisons (equal/not equal)
select print(eadd('bool(5:5)[0:0:true 1:1:false 2:2:true]'::matrix,
    'bool(5:5)[0:0:true 1:1:true 2:2:false]'::matrix,
    'eq_bool'::binaryop));

-- ## Float32 (fp32) Special Values
--
-- Test float-specific edge cases and special values.

-- Infinity values
select 'fp32(5:5)[0:0:Infinity 1:1:-Infinity 2:2:1.5]'::matrix;

-- NaN (Not a Number)
select 'fp32(5:5)[0:0:NaN 1:1:1.5 2:2:2.5]'::matrix;

-- Signed zeros
select 'fp32(5:5)[0:0:0.0 1:1:-0.0 2:2:1.5]'::matrix;

-- Very large values (near max)
select 'fp32(3:3)[0:0:3.40282e+38 1:1:-3.40282e+38 2:2:1.0]'::matrix;

-- Operations with Infinity
select print(apply('fp32(3:3)[0:0:1.0 1:1:2.0 2:2:3.0]'::matrix,
    'Infinity'::float4::scalar::float4,
    'plus_fp32'::binaryop));
select print(apply('fp32(3:3)[0:0:1.0 1:1:2.0 2:2:3.0]'::matrix,
    'Infinity'::float4::scalar::float4,
    'times_fp32'::binaryop));

-- Operations with NaN (NaN propagates)
select print(apply('fp32(3:3)[0:0:1.0 1:1:2.0 2:2:3.0]'::matrix,
    'NaN'::float4::scalar::float4,
    'plus_fp32'::binaryop));

-- Reduction with NaN
select reduce_scalar('fp32(3:3)[0:0:1.0 1:1:NaN 2:2:3.0]'::matrix,
    'plus_monoid_fp32'::monoid);

-- Min/max with special values
select reduce_scalar('fp32(3:3)[0:0:-Infinity 1:1:1.0 2:2:Infinity]'::matrix,
    'max_monoid_fp32'::monoid);
select reduce_scalar('fp32(3:3)[0:0:-Infinity 1:1:1.0 2:2:Infinity]'::matrix,
    'min_monoid_fp32'::monoid);

-- ## Float64 (fp64) Special Values
--
-- Test double-precision float edge cases.

-- Infinity values
select 'fp64(5:5)[0:0:Infinity 1:1:-Infinity 2:2:1.5 3:3:2.5 4:4:3.5]'::matrix;

-- NaN values
select 'fp64(5:5)[0:0:NaN 1:1:1.5 2:2:2.5 3:3:3.5 4:4:4.5]'::matrix;

-- Signed zeros
select 'fp64(5:5)[0:0:0.0 1:1:-0.0 2:2:1.5]'::matrix;

-- Very large values (near max)
select 'fp64(3:3)[0:0:1.7976931348623157e+308 1:1:-1.7976931348623157e+308 2:2:1.0]'::matrix;

-- Very small values (near min positive)
select 'fp64(3:3)[0:0:2.2250738585072014e-308 1:1:1.0 2:2:2.0]'::matrix;

-- High precision values
select 'fp64(3:3)[0:0:3.141592653589793 1:1:2.718281828459045 2:2:1.414213562373095]'::matrix;

-- Operations with Infinity
select print(eadd('fp64(3:3)[0:0:1.0 1:1:Infinity 2:2:3.0]'::matrix,
    'fp64(3:3)[0:0:2.0 1:1:4.0 2:2:-Infinity]'::matrix,
    'plus_fp64'::binaryop));

-- Very large number operations
select reduce_scalar('fp64(3:3)[0:0:1e100 1:1:1e100 2:2:1e100]'::matrix,
    'plus_fp64'::monoid);

-- ## Type Casting and Promotion
--
-- Test type conversions between matrix types.

-- Int to float (exact for small ints)
select cast_to('int32(5:5)[0:0:1 1:1:2 2:2:3 3:3:4 4:4:5]'::matrix, 'fp64');
select cast_to('int32(3:3)[0:0:100 1:1:200 2:2:300]'::matrix, 'fp32');

-- Float to int (truncation)
select cast_to('fp64(5:5)[0:0:1.9 1:1:2.1 2:2:3.5 3:3:-1.9 4:4:-2.1]'::matrix, 'int32');
select cast_to('fp32(3:3)[0:0:3.7 1:1:-3.7 2:2:0.5]'::matrix, 'int32');

-- Large int to float (may lose precision)
select cast_to('int64(2:2)[0:0:9007199254740992 1:1:9007199254740993]'::matrix, 'fp64');

-- Int size conversions
select cast_to('int32(3:3)[0:0:100 1:1:200 2:2:300]'::matrix, 'int64');
select cast_to('int32(3:3)[0:0:100 1:1:200 2:2:300]'::matrix, 'int16');
select cast_to('int64(3:3)[0:0:1000 1:1:2000 2:2:3000]'::matrix, 'int32');

-- Float precision conversions
select cast_to('fp64(2:2)[0:0:3.141592653589793 1:1:2.718281828459045]'::matrix, 'fp32');
select cast_to('fp32(2:2)[0:0:3.14159 1:1:2.71828]'::matrix, 'fp64');
