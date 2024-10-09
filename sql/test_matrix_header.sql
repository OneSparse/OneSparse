-- # Matrix
--
-- This documentation is also tests for the code, the examples below
-- show the literal output of these statements from Postgres.
--
-- Some setup to make sure the extension is installed.

set client_min_messages = 'WARNING'; -- pragma:hide
create extension if not exists onesparse;

\pset linestyle unicode
\pset border 2

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

-- The above matrices are "unbounced", they do not have a fixed number
-- of rows and/or columns.  The default possible number of rows and
-- columns is defined by the SuiteSparse library to be `GrB_INDEX_MAX`
-- which is `2^60` power indexes.  For the purposes of this
-- documentation this will be referred to as `INDEX_MAX` and matrices
-- and vector dimensions that are `INDEX_MAX` in size are reffered to
-- as "unbounded".

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

-- Note that in all the above cases the matrices created are *empty*.
-- They contain no stored elements.  The memory needed to hold the
-- matrix contains only stored elements, if there isn't a value stored
-- at a given row or column position, no memory is consumed.  This is
-- the "sparse" in sparse matrix.  This is how it's possible to create
-- an unbounded row by unbounded column matrix without exhausting
-- memory trying to allocate `2^120` entries.
--
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

-- Seeing matrices in this format is pretty hard to understand, there
-- are two helpful functions for visualizing matrices, the first is
-- `print` which prints the matrix in

-- Below you see the number of rows, columns and spaces for a variety
-- of combinations:

select print('int32(4:4)[1:2:1 2:3:2 3:1:3]'::matrix) as matrix;

-- Above you can see the sparse matrix format of an 8x8 matrix.  It's
-- only possible to print matrices that have fixed dimensions of a
-- reasonable size.

-- Another useful function is `dot()` This turns a matrix into the
-- Graphviz DOT language that is used to draw graph diagrams:

select dot('int32(4:4)[1:2:1 2:3:2 3:1:3]'::matrix) as dot;

-- Will generate the following diagram:
--

select dot('int32(4:4)[1:2:1 2:3:2 3:1:3]'::matrix) as dot_source \gset
\i sql/dot.sql

-- # Adjacency Matrices
--
-- Onesparse sparse matrices are very similar to matrix objects from
-- other libraries like `scipy.sparse` and NVIDIA's `cuSparse`.  The
-- primary difference with the the GraphBLAS there also comes an
-- entire library of pre-optimized algebraic kernels.
--
-- A useful function to illustrate this concept is `random_matrix()`.
-- This will generate a random matrix provided the type, number of
-- rows, number of columns, and the number of (approximate) values and
-- an optional random seed for deterministic generation:
--

select print(random_matrix(8, 8, 16, seed=>0.42, max=>42)) as random_matrix;

-- This random matrix is also a random *graph*:
--

select dot(random_matrix(8, 8, 16, seed=>0.42, max=>42)) as dot_source \gset
\i sql/dot.sql

-- # Every Matrix is a Graph
--
-- In fact every matrix is a graph, whether you think of it that way
-- or not.  And every graph has a corresponding matrix.  The data that
-- you put into tables can also describe a graph, and thus a matrix.
-- These three different ways of thinking about tables, graphs, and
-- matrices is one of the core concepts of OneSparse:
--
-- ![Tables, Graphs, and Matrices](./table_graph_matrix.png)
--
-- One way of thinking about a "dense" matrix is a fully connected
-- graph, these can be constructed with the `dense_matrix()` function:

select print(dense_matrix('int32', 4, 4, 42));
select dot(dense_matrix('int32', 4, 4, 42)) as dot_source \gset
\i sql/dot.sql

-- # Test Fixtures
--
-- Let's get a test fixture table with a couple matrix and vector
-- columns so that we can do some operations without tediously
-- repeating the literal syntax.  These matrices and vectors are
-- construction with `random_matrix()` show above, and the
-- `random_vector()` function:

\o /dev/null
\set ECHO none
\i sql/fixtures.sql
\set ECHO all
\o

select print(a) as a, print(b) as b, print(u) as u, print(v) as v from test_fixture;

-- # Element-wise operations
--
-- The GraphBLAS API has elementwise operations on matrices that
-- operate pairs of matrices.  `eadd` computes the element-wise
-- “addition” of two matrices A and B, element-wise using any binary
-- operator.  Elements present on both sides of the operation are
-- included in the result.

select print(a) as a, binaryop, print(b) as b, print(eadd(A, B, binaryop)) as eadd from test_fixture;

select dot(a) as binop_a_source, dot(b) as binop_b_source, dot(eadd(A, B, binaryop)) as binop_c_source from test_fixture \gset

\i sql/binop.sql

-- `emult` multiplies elements of two matrices, taking only the
-- intersection of common elements in both matrices, if an element is
-- missing from either the left or right side, it is ommited from the
-- result:

select print(a) as a, binaryop, print(b) as b, print(emult(A, B, binaryop)) as emult from test_fixture;

select dot(a) as binop_a_source, dot(b) as binop_b_source, dot(emult(A, B, binaryop)) as binop_c_source from test_fixture \gset

\i sql/binop.sql

-- `eunion` is like `eadd` but differs in how the binary op is
-- applied. A pair of scalars, `alpha` and `beta` define the inputs to
-- the operator when entries are present in one matrix but not the
-- other.

select print(a) as a, binaryop, print(b) as b, print(eunion(A, 3::scalar, B, 4::scalar, binaryop)) as eunion from test_fixture;

select dot(a) as binop_a_source, dot(b) as binop_b_source, dot(eunion(A, 3::scalar, B, 4::scalar, binaryop)) as binop_c_source from test_fixture \gset

\i sql/binop.sql

-- The entire matrix can be reduced to a scalar value:

select print(a) as a, monoid, reduce_scalar(a, monoid) from test_fixture;

-- The matrix can also be reduced to a column vector:

select print(a) as a, monoid, print(reduce_vector(a, monoid)) as reduce_vector from test_fixture;

-- To reduce a row vector, specify that the input should be transposed
-- with the descriptor `t0`:

select print(a) as a, monoid, print(reduce_vector(a, monoid, descriptor=>'t0')) as transpose_reduce_vector from test_fixture;

-- Matrix Multiplication (referred to here as A @ B) is the heart of
-- linear algebra.  All matrix multiplication happens over a semiring.
-- For the most common form of matrix multiplication, the outer
-- opperation is to multiply coresponding elements with the "times"
-- operator and then reduce those products with the "plus" operator.
-- This is called the `plus_times` semiring:

select print(a) as a, semiring, print(b) as b, print(mxm(a, b)) as mxm from test_fixture;

select dot(a) as binop_a_source, dot(b) as binop_b_source, dot(mxm(a, b)) as binop_c_source from test_fixture \gset
\i sql/binop.sql

-- AxB can also be done with the `@` operator, mimicking the Python
-- syntax.  The default semiring for numeric types is `plus_times`.

select print(a) as a, '@' as "@", print(b) as b, print(a @ b) as mxm from test_fixture;

-- Matrices can be multipled by vectors on the right taking the linear
-- combination of the matrices columns using the vectors elements as
-- coefficients:

select print(a) as a, '@' as "@", semiring, print(u) as u, print(mxv(a, u)) as mxv from test_fixture;

select dot(a) as binop_a_source, dot(u) as binop_b_source, dot(mxv(a, u)) as binop_c_source from test_fixture \gset
\i sql/binop.sql

-- 'mxv' is also supported by the `@` operator:

select print(a) as a, '@' as "@", print(u) as u, print(a @ u) as mxv from test_fixture;

-- Matrices can be multipled by vectors on the right taking the linear
-- combination of the matrices rows using the vectors elements as
-- coefficients:

select print(v) as v, semiring, print(b) as b, print(vxm(v, b, semiring)) as vxm from test_fixture;

select dot(v) as binop_a_source, dot(b) as binop_b_source, dot(vxm(v, b)) as binop_c_source from test_fixture \gset
\i sql/binop.sql

-- 'vxm' is also supported by the `@` operator:

select print(v) as v, '@' as "@", print(b) as b, print(v @ b) as vxm from test_fixture;

-- The `selection` method calls the `GrB_select()` API function.  The
-- name `selection` was chosen not to conflict with the SQL keyword
-- `select`.  Selection provides a conditional operator called an
-- `indexunaryop` and a parameter for the operator to use to compare
-- elements in the matrix.  Below, all elements with values greater
-- than 50 are returned:

select print(a) as a, indexunaryop, print(selection(a, indexunaryop, 1)) as selected from test_fixture;

select dot(a) as uop_a_source, dot(selection(a, indexunaryop, 1)) as uop_b_source from test_fixture \gset
\i sql/uop.sql

-- `apply` takes an operator of type `unaryop` and applies it to every
-- element of the matrix.  The 'ainv_int32' returned the additive
-- inverse (the negative value for integers) of every element:

select print(a) as a, unaryop, print(apply(a, unaryop)) as applied from test_fixture;

-- Elements can be set individually with `set_element`, the modified
-- input is returned:

select print(set_element(a, 1, 1, 1)) as set_element from test_fixture;

-- Scalar elements can be extracted individually with `get_element`

select get_element(a, 3, 3) as get_element from test_fixture;

-- The `print` function returns a descripton of the matrix from
-- SuiteSparse.

select info(a) from test_fixture;

-- The `print` function takes an optional "level" argument that
-- defaults to `1` which is a short summary.

select info(a, 5) from test_fixture;

-- The `dup` function duplicates a matrix returning a new matrix
-- object with the same values:

select dup(a) from test_fixture;

-- The `wait` method is used to "complete" a matrix, which may have
-- pending operations waiting to be performed when using the default
-- SuiteSparse non-blocking mode.  As a side effect, wait will sort
-- the elements of the input:

select wait('int32[2:2:2 3:3:3 1:1:1]'::matrix);

-- The `clear` function clears the matrix of all elements and returns
-- the same object, but empty.  The dimensions do not change:

select clear('int32[1:1:1 2:2:2 3:3:3]'::matrix);

-- # Extra tests
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
