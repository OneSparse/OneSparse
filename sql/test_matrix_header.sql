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

-- OneSparse wraps the SuiteSparse:GraphBLAS library and extends
-- Postgres by adding new types and functions that allow you to do
-- sparse and dense linear algebra in Postgres.  This is similar to
-- functionality packages like `numpy` and `scipy.sparse` bring to
-- Python.
--
-- The most powerful object in OneSparse is a Matrix.  A Matrix is a
-- two dimensional array of data with a certain number of rows *m* and
-- columns *n*.  Typically matrices are very memory hungry data
-- structures, requiring `m * n` memory to hold all of the elements.
--
-- This limits traditional matrix libraries, because many problems in
-- linear algebra are *sparse*.  Not every element is used in the
-- problem or even definable.  Traditional linear algebra libraries
-- usually encode sparse matrices into dense matrices by using the
-- number zero to indicate "nothing", but this does not aleviate the
-- memory problem.  For matrices with a large number of rows and
-- columns this means vast areas of memories filled with zeros that
-- end up being multiplied away, wasting energy.
--
-- OneSparse matrices however are smart, and can adapt to the number
-- of actually useful elements in a Matrix.  They can be dense or
-- sparse, the SuiteSparse library will adapt to choose the right
-- backend format.
--
-- ## Matrices and Graphs
--
-- Every matrix is a graph, whether you think of it that way or not.
-- And every graph has a corresponding matrix.  The data that you put
-- into tables can also describe a graph, and thus a matrix.  These
-- three different ways of thinking about tables, graphs, and matrices
-- is one of the core concepts of OneSparse:
--
-- ![Tables, Graphs, and Matrices](./table_graph_matrix.png)
--
-- While SuiteSparse is optimized for processing sparse matrices and
-- vectors, it also supports optimized kernels for dense objects.  A
-- dense matrix is just a sparse matrix with all its elements.  In
-- this case SuiteSparse will automatically store it in a dense
-- optimal format and use CPUs or GPUs appropriately to process them.
--
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

select draw(d) as draw_source from test_fixture \gset
\i sql/draw.sql

-- And another matrix named 's' which is a Sierpinsky Graph, which
-- we'll show off a bit later.
--

select print(s) from test_fixture;

-- The `matrix` data type wraps a SuiteSparse GrB_Matrix handle and
-- delegates functions from SQL to the library through instances of
-- this type.
--
-- ## Empty Matrices
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

-- Elements can be set individually with `set_element`, the modified
-- input is returned:

select print(set_element(a, 1, 1, 1)) as set_element from test_fixture;

-- Scalar elements can be extracted individually with `get_element`

select get_element(a, 3, 2) as get_element from test_fixture;

-- If an element does exist `get_element` will return an "empty"
-- scalar:

select get_element(a, 3, 3) as get_element from test_fixture;

-- ## Random Matrices
--
-- `random_matrix` will generate a random matrix provided the type,
-- number of rows, number of columns, and the number of (approximate)
-- values, an optional max value, and an optional random seed for
-- deterministic generation:

select print(random_matrix(8, 8, 16, seed=>0.42, max=>42)) as random_matrix;

-- This random matrix is also a random *graph*:
--

select draw(random_matrix(8, 8, 16, seed=>0.42, max=>42)) as draw_source \gset
\i sql/draw.sql

-- ## Elementwise Addition
--
-- The GraphBLAS API has elementwise operations on matrices that
-- operate pairs of matrices.  `eadd` computes the element-wise
-- “addition” of two matrices a and b, element-wise using any binary
-- operator.  Elements present on both sides of the operation are
-- included in the result.

select print(a) as a, binaryop, print(b) as b, print(eadd(a, b, binaryop)) as eadd from test_fixture;

-- Eadd can also be accomplished with the '+' operator:

select print(a) as a, binaryop, print(b) as b, print(a + b) as eadd from test_fixture;

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

select print(a) as a, binaryop, print(b) as b, print(emult(a, b, binaryop)) as emult from test_fixture;

-- Emult can also be accomplished with the '*' operator:

select print(a) as a, binaryop, print(b) as b, print(a * b) as emult from test_fixture;

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

select print(a) as a, binaryop, print(b) as b, print(eunion(a, 3::scalar, b, 4::scalar, binaryop)) as eunion from test_fixture;

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

select print(a) as a, 'plus_monoid_int32' as monoid, reduce_scalar(a) from test_fixture;

-- The entire matrix can be reduced to a scalar value with a provided
-- monoid that changes the reduction operation:

select print(a) as a, 'min_monoid_int32' as monoid, reduce_scalar(a, 'min_monoid_int32') from test_fixture;

-- The matrix can also be reduced to a column vector:

select print(a) as a, 'plus_monoid_int32' as monoid, print(reduce_vector(a)) as reduce_vector from test_fixture;

-- To reduce a row vector, specify that the input should be transposed
-- with the descriptor `t0`:

select print(a) as a, 'plus_monoid_int32' as monoid, print(reduce_vector(a, descriptor=>'t0')) as transpose_reduce_vector from test_fixture;

-- ## Matrix Matrix Multiplication
--
-- Matrix Multiplication is the heart of linear algebra.  All matrix
-- multiplication happens over a semiring.  For the most common form
-- of matrix multiplication, the outer opperation is to multiply
-- coresponding elements with the "times" operator and then reduce
-- those products with the "plus" operator.  This is called the
-- `plus_times` semiring:

select print(a) as a, semiring, print(b) as b, print(mxm(a, b)) as mxm from test_fixture;

select draw(a) as binop_a_source, draw(b) as binop_b_source, draw(mxm(a, b)) as binop_c_source from test_fixture \gset
\i sql/binop.sql

-- AxB can also be done with the `@` operator, mimicking the Python
-- syntax.  The default semiring for numeric types is `plus_times`.

select print(a) as a, '@' as "@", print(b) as b, print(a @ b) as mxm from test_fixture;

-- ## Matrix Vector Multiplication
--
-- Matrices can be multipled by vectors on the right taking the linear
-- combination of the matrices columns using the vectors elements as
-- coefficients:

select print(a) as a, '@' as "@", semiring, print(u) as u, print(mxv(a, u)) as mxv from test_fixture;

-- From a graph standpoint, matrix vector multiplication is used to
-- "pull" back to adjacent nodes from their incoming edges.  When
-- iterated, it forms the basis for working back along incoming links.

select draw(a) as binop_a_source, draw(u) as binop_b_source, draw(mxv(a, u)) as binop_c_source from test_fixture \gset
\i sql/binop.sql

-- 'mxv' is also supported by the `@` operator:

select print(a) as a, '@' as "@", print(u) as u, print(a @ u) as mxv from test_fixture;

-- ## Vector Matrix Multiplication
--
-- Matrices can be multipled by vectors on the right taking the linear
-- combination of the matrices rows using the vectors elements as
-- coefficients:

select print(v) as v, semiring, print(b) as b, print(vxm(v, b, semiring)) as vxm from test_fixture;

-- From a graph standpoint, vector matrix multiplication is used to
-- "push" forward to adjacent nodes from their outgoing edges.  When
-- iterated, it forms the basis for working forward along outgoing
-- edges.

select draw(v) as binop_a_source, draw(b) as binop_b_source, draw(vxm(v, b)) as binop_c_source from test_fixture \gset
\i sql/binop.sql

-- 'vxm' is also supported by the `@` operator:

select print(v) as v, '@' as "@", print(b) as b, print(v @ b) as vxm from test_fixture;

-- ## Element Selection
--
-- The `selection` method calls the `GrB_select()` API function.  The
-- name `selection` was chosen not to conflict with the SQL keyword
-- `select`.  Selection provides a conditional operator called an
-- `indexunaryop` and a parameter for the operator to use to compare
-- elements in the matrix.  Below, all elements with values greater
-- than 50 are returned:

select print(a) as a, indexunaryop, print(selection(a, indexunaryop, 1)) as selected from test_fixture;

select draw(a) as uop_a_source, draw(selection(a, indexunaryop, 1)) as uop_b_source from test_fixture \gset
\i sql/uop.sql

-- A useful select operator is `triu`, it select only upper triangular
-- values, this turns your graph into a direct acyclic graph (DAG) by
-- removing all the links "back" from higher number nodes to lower.

select print(random_matrix(8, 8, 16, seed=>0.42, max=>42)) as matrix,
       print(selection(random_matrix(8, 8, 16, seed=>0.42, max=>42), 'triu', 0)) as triu from test_fixture;

select draw(random_matrix(8, 8, 16, seed=>0.42, max=>42)) as uop_a_source,
       draw(selection(random_matrix(8, 8, 16, seed=>0.42, max=>42), 'triu', 0)) as uop_b_source
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
--There's a special function for exponentiating a matrix to itself
-- a certain number of times, `kronpower`.:

select print(kronpower(s, 2)) from test_fixture;

-- Kronecker products can very quickly make huge graphs with power law
-- distributions.  These are handy synthetic graphs to mimic certain
-- statistical edge distributions common in sparse graph problems:

select nvals(kronpower(s, 3)) from test_fixture;

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

select nrows('int32(10)[1:1:1 2:2:2 3:3:3]'::matrix);

select ncols('int32(10)[1:1:1 2:2:2 3:3:3]'::matrix);

select nvals('int32(10)[1:1:1 2:2:2 3:3:3]'::matrix);

select nrows('int32(10:)[1:1:1 2:2:2 3:3:3]'::matrix);

select ncols('int32(10:)[1:1:1 2:2:2 3:3:3]'::matrix);

select nvals('int32(10:)[1:1:1 2:2:2 3:3:3]'::matrix);

select nrows('int32(:10)[1:1:1 2:2:2 3:3:3]'::matrix);

select ncols('int32(:10)[1:1:1 2:2:2 3:3:3]'::matrix);

select nvals('int32(:10)[1:1:1 2:2:2 3:3:3]'::matrix);

select nrows('int32(10:10)[1:1:1 2:2:2 3:3:3]'::matrix);

select ncols('int32(10:10)[1:1:1 2:2:2 3:3:3]'::matrix);

select nvals('int32(10:10)[1:1:1 2:2:2 3:3:3]'::matrix);
