# Matrix

This documentation is also tests for the code, the examples below
show the literal output of these statements from Postgres.

Some setup to make sure the extension is installed.
``` postgres-console
create extension if not exists onesparse;
psql:sql/fixtures.sql:32: ERROR:  function random_matrix(integer, integer, integer, integer, integer, boolean, numeric) does not exist
LINE 3:         random_matrix(4, 4, 10, 1, 4, true, 0.0 + 0.3),
                ^
HINT:  No function matches the given name and argument types. You might need to add explicit type casts.
```
OneSparse wraps the
[SuiteSparse:GraphBLAS](http://faculty.cse.tamu.edu/davis/GraphBLAS.html)
library and extends Postgres by adding new types and functions that
allow you to do sparse and dense linear algebra in Postgres.  This
is similar to functionality packages like `numpy` and
`scipy.sparse` bring to Python.

While a full grasp of the GraphBLAS API is not necessary to follow
along with this guide there are many details that are not spelled
out in great details here.  For complete details on the API and
expected behaviors, see the [SuiteSparse:GraphBLAS User
Guide](https://github.com/DrTimothyAldenDavis/SuiteSparse/blob/dev/GraphBLAS/Doc/GraphBLAS_UserGuide.pdf).

The most fundamenteal object in OneSparse is a Matrix.  A Matrix is
a two dimensional array of data with a certain number of rows *m*
and columns *n*.  Typically matrices are very memory hungry data
structures, requiring `m * n` memory to hold all of the elements.

This limits traditional matrix libraries, because many problems in
linear algebra are *sparse*.  Not every element is used in the
problem or even definable.  Traditional linear algebra libraries
usually encode sparse matrices into dense matrices by using the
number zero to indicate "nothing", but this approach does not solve
the memory problem.  For matrices with a large number of rows and
columns this means vast areas of memories filled with zeros that
end up being multiplied away, which also wastes time and energy.

OneSparse matrices however are smart, and can adapt to the number
of actually useful elements in a Matrix.  They can be dense or
sparse, the SuiteSparse library will adapt to choose the right
backend format.

## Matrices and Graphs

Every matrix is a graph, whether you think of it that way or not.
And every graph has a corresponding matrix.  A lot of data that you
put into postgres tables can also describe a graph, and thus a
matrix.  These three different ways of thinking about tables,
graphs, and matrices is one of the core concepts of OneSparse:

![Tables, Graphs, and Matrices](./table_graph_matrix.png)

While SuiteSparse is optimized for processing sparse matrices and
vectors, it also supports optimized kernels for dense objects.  A
dense matrix is just a sparse matrix with all its elements.  In
this case SuiteSparse will automatically store it in a dense
optimal format and use CPUs or GPUs appropriately to process them.

## Getting Started

The examples below are all what you would see typing the exact
queries out in `psql`.  The GraphBLAS API is large, so onesparse is
always contained in the `onesparse` postgres schema.  For the sake
of brevity, let's set the `search_path` so that we can just type
`matrix` instead of `onesparse.matrix` everywhere.
``` postgres-console
SET search_path TO public,onesparse;
```
If the matrix has bounds, it can be printed to the console if they
are reasonable size, this is useful for debugging and
experimentation:

``` postgres-console
select print('int32(4:4)'::matrix);
┌────────────────────┐
│       print        │
├────────────────────┤
│      0  1  2  3    │
│    ────────────    │
│  0│                │
│  1│                │
│  2│                │
│  3│                │
│                    │
└────────────────────┘
(1 row)

```
Note that this matrix is empty, it's not filled with "zeros", it
contains *no elements*.  The memory needed to hold the matrix
contains only stored elements, if there isn't a value stored at a
given row or column position, no memory is consumed.  This is the
"sparse" in sparse matrix.  This is how it's possible to create an
unbounded row by unbounded column matrix without exhausting memory
trying to allocate `2^120` entries.

## Drawing Matrices and Vectors

The `draw()` function turns a matrix into the Graphviz DOT language
that is used to draw graph diagrams:
``` postgres-console
select draw('int32(4:4)[1:2:1 2:3:2 3:1:3]'::matrix) as draw;
┌───────────────────────┐
│         draw          │
├───────────────────────┤
│ digraph {             │
│  node [shape=circle]; │
│  rankdir=LR;          │
│ 1 -> 2 [label="1" ];  │
│ 2 -> 3 [label="2" ];  │
│ 3 -> 1 [label="3" ];  │
│ }                     │
│                       │
└───────────────────────┘
(1 row)

```
Will generate the following diagram:
``` postgres-console
select draw('int32(4:4)[1:2:1 2:3:2 3:1:3]'::matrix) as draw_source \gset
```
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="185pt" height="66pt"
 viewBox="0.00 0.00 185.34 66.11" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 62.11)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-62.11 181.34,-62.11 181.34,4 -4,4"/>
<!-- 1 -->
<g id="node1" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-13.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-42.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-40.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge1" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M30.27,-20.76C39.91,-24.42 53.05,-29.41 64.32,-33.69"/>
<polygon fill="black" stroke="black" points="63.41,-37.09 74,-37.37 65.89,-30.54 63.41,-37.09"/>
<text text-anchor="middle" x="52.11" y="-33.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="161.78" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="161.78" y="-13.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge2" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M103.38,-37.35C113.02,-33.69 126.16,-28.7 137.44,-24.42"/>
<polygon fill="black" stroke="black" points="139.01,-27.57 147.11,-20.75 136.52,-21.02 139.01,-27.57"/>
<text text-anchor="middle" x="125.23" y="-33.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge3" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M145.94,-13.21C134.56,-11.54 118.45,-9.43 104.23,-8.56 90.42,-7.71 86.91,-7.71 73.11,-8.56 62.67,-9.2 51.2,-10.51 41.34,-11.82"/>
<polygon fill="black" stroke="black" points="40.81,-8.36 31.39,-13.21 41.78,-15.29 40.81,-8.36"/>
<text text-anchor="middle" x="88.67" y="-11.16" font-family="Times,serif" font-size="8.00">3</text>
</g>
</g>
</svg>
</div>

Let's look at our cast of test objects for the remaining examples.
These objects from the `onesparse.test_fixture` table.
``` postgres-console
select * from test_fixture;
(0 rows)

```

Here are a couple of sparse matrices from the test_fixture table.
We'll call them `a` and `b` in these docs:

``` postgres-console
select print(a) as a, print(b) as b from test_fixture;
┌───┬───┐
│ a │ b │
├───┼───┤
└───┴───┘
(0 rows)

select draw(a) as twocol_a_source, draw(b) as twocol_b_source from test_fixture \gset
no rows returned for \gset
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
    <td style="width: 30%;">
        <!-- Diagram B -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
  </tr>
</table>
Here are some sparse test vectors, they will be used for some of
the examples below:

``` postgres-console
select print(u) as u, print(v) as v from test_fixture;
┌───┬───┐
│ u │ v │
├───┼───┤
└───┴───┘
(0 rows)

select draw(u) as twocol_a_source, draw(v) as twocol_b_source from test_fixture \gset
no rows returned for \gset
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
    <td style="width: 30%;">
        <!-- Diagram B -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
  </tr>
</table>
There is also an example "dense" matrix named 'd':

``` postgres-console
select print(d) from test_fixture;
┌───────┐
│ print │
├───────┤
└───────┘
(0 rows)

```
One way of thinking about a "dense" matrix is a fully connected
graph, these can be constructed with the `dense_matrix()` function:

``` postgres-console
select draw(dense_matrix(4, 4, 1)) as draw_source from test_fixture \gset
no rows returned for \gset
Error: <stdin>: syntax error in line 1 near ':'
```
And another matrix named 's' which is a Sierpinsky Graph, which
we'll show off a bit later.

``` postgres-console
select print(s) from test_fixture;
┌───────┐
│ print │
├───────┤
└───────┘
(0 rows)

```
## Random Matrices

`random_matrix` will generate a random matrix provided the type,
number of rows, number of columns, and the number of (approximate)
values, an optional max value, and an optional random seed for
deterministic generation:
``` postgres-console
select print(random_matrix(8, 8, 16, seed=>0.42, max=>42)) as random_matrix;
ERROR:  function random_matrix(integer, integer, integer, seed => numeric, max => integer) does not exist
LINE 1: select print(random_matrix(8, 8, 16, seed=>0.42, max=>42)) a...
                     ^
HINT:  No function matches the given name and argument types. You might need to add explicit type casts.
```
This random matrix is also a random *graph*:

``` postgres-console
select draw(random_matrix(8, 8, 16, seed=>0.42, max=>42)) as draw_source \gset
ERROR:  function random_matrix(integer, integer, integer, seed => numeric, max => integer) does not exist
LINE 1: select draw(random_matrix(8, 8, 16, seed=>0.42, max=>42)) as...
                    ^
HINT:  No function matches the given name and argument types. You might need to add explicit type casts.
Error: <stdin>: syntax error in line 1 near ':'
```
## Empty Matrices

The `matrix` data type wraps a SuiteSparse GrB_Matrix handle and
delegates functions from SQL to the library through instances of
this type.

An empty matrix can be constructed many ways, but one of the
simplest is casting a type code to the matrix type.  In this case
`int32` means the SuiteSparse type `GrB_INT32`.
``` postgres-console
select 'int32'::matrix;
┌────────┐
│ matrix │
├────────┤
│ int32  │
└────────┘
(1 row)

```
Another way to create an empty matrix is to use the `matrix()`
constructor function:
``` postgres-console
select matrix('int32');
┌────────┐
│ matrix │
├────────┤
│ int32  │
└────────┘
(1 row)

```
## Matrix dimensions

The above matrices are "unbounded", they do not have a fixed number
of rows and/or columns.  The default possible number of rows and
columns is defined by the SuiteSparse library to be `GrB_INDEX_MAX`
which is `2^60` power indexes.  For the purposes of this
documentation this will be referred to as `INDEX_MAX` and matrices
and vector dimensions that are `INDEX_MAX` in size are reffered to
as "unbounded".

For matrices with known dimensions, the dimensions can be provided
in parentesis after the type code.  Here a 4 row by 4 column
matrix is created:
``` postgres-console
select 'int32(4:4)'::matrix;
┌────────────┐
│   matrix   │
├────────────┤
│ int32(4:4) │
└────────────┘
(1 row)

```
Another way to make a new matrix is with the `matrix` constructor
function.
``` postgres-console
select matrix('int32', 4, 4);
┌────────────┐
│   matrix   │
├────────────┤
│ int32(4:4) │
└────────────┘
(1 row)

```
Either dimension can be ommited, this creates a 4 row by unbounded
column matrix.
``` postgres-console
select 'int32(4:)'::matrix;
┌───────────┐
│  matrix   │
├───────────┤
│ int32(4:) │
└───────────┘
(1 row)

```
This creates a unbounded row by 4 column matrix.
``` postgres-console
select 'int32(:4)'::matrix;
┌───────────┐
│  matrix   │
├───────────┤
│ int32(:4) │
└───────────┘
(1 row)

```
All graphblas operations are exposed by a series of functions and
operators.  Here we see three very common operations, returning the
number of rows, the number of columns, and the number of store
values.
``` postgres-console
select nrows('int32'::matrix),
       ncols('int32'::matrix),
       nvals('int32'::matrix);
┌─────────────────────┬─────────────────────┬───────┐
│        nrows        │        ncols        │ nvals │
├─────────────────────┼─────────────────────┼───────┤
│ 1152921504606846976 │ 1152921504606846976 │     0 │
└─────────────────────┴─────────────────────┴───────┘
(1 row)

```
Above you can see the matrix has unbounded rows and columns (the
very large number is the number of *possible* entries).  And the
number of stored values is zero.  These matrices are empty, they
contain no elements.

Values can be specified after the `type(dimension)` prefix as an
array of elements between square brackets.  Empty brackets imply no
elements, so empty square brackets are the same as no square
brackets as above:
``` postgres-console
select nrows('int32[]'::matrix),
       ncols('int32[]'::matrix),
       nvals('int32[]'::matrix);
┌─────────────────────┬─────────────────────┬───────┐
│        nrows        │        ncols        │ nvals │
├─────────────────────┼─────────────────────┼───────┤
│ 1152921504606846976 │ 1152921504606846976 │     0 │
└─────────────────────┴─────────────────────┴───────┘
(1 row)

```
Elements are specified between square brackets are coordinates of
'row_id:column_id:value' separated by spaces:
``` postgres-console
select 'int32[1:2:1 2:3:2 3:1:3]'::matrix,
       'int32(4:)[1:2:1 2:3:2 3:1:3]'::matrix,
       'int32(:4)[1:2:1 2:3:2 3:3:1]'::matrix;
┌──────────────────────────┬──────────────────────────────┬──────────────────────────────┐
│          matrix          │            matrix            │            matrix            │
├──────────────────────────┼──────────────────────────────┼──────────────────────────────┤
│ int32[1:2:1 2:3:2 3:1:3] │ int32(4:)[1:2:1 2:3:2 3:1:3] │ int32(:4)[1:2:1 2:3:2 3:3:1] │
└──────────────────────────┴──────────────────────────────┴──────────────────────────────┘
(1 row)

```
## Elements

All the elements in a matrix can be iterated with the `elements()`
function:
``` postgres-console
select * from elements((select a from test_fixture));
┌───┬───┬───┐
│ i │ j │ v │
├───┼───┼───┤
└───┴───┴───┘
(0 rows)

```
The inverse operation of constructing matrices from rows can be
done with `matrix_agg()`:
``` postgres-console
select matrix_agg(i, i, i) as unbound_matrix from generate_series(0, 10) as i;
┌─────────────────────────────────────────────────────────────────────────────┐
│                               unbound_matrix                                │
├─────────────────────────────────────────────────────────────────────────────┤
│ int32[0:0:0 1:1:1 2:2:2 3:3:3 4:4:4 5:5:5 6:6:6 7:7:7 8:8:8 9:9:9 10:10:10] │
└─────────────────────────────────────────────────────────────────────────────┘
(1 row)

```
Aggregate matrices are always unbounded so use `resize()` to bound
the matrix:
``` postgres-console
select print(resize(matrix_agg(i, i, i), 10, 10)) as bound_matrix from generate_series(0, 10) as i;
┌──────────────────────────────────────┐
│             bound_matrix             │
├──────────────────────────────────────┤
│      0  1  2  3  4  5  6  7  8  9    │
│    ──────────────────────────────    │
│  0│  0                               │
│  1│     1                            │
│  2│        2                         │
│  3│           3                      │
│  4│              4                   │
│  5│                 5                │
│  6│                    6             │
│  7│                       7          │
│  8│                          8       │
│  9│                             9    │
│                                      │
└──────────────────────────────────────┘
(1 row)

```
## Equality

Two matrices can be compared for equality with the '=' and '!=' operators:
``` postgres-console
select a != b as "a != b", a = b as "a = b", b = a as "b = a", b = b as "b = b" from test_fixture;
┌────────┬───────┬───────┬───────┐
│ a != b │ a = b │ b = a │ b = b │
├────────┼───────┼───────┼───────┤
└────────┴───────┴───────┴───────┘
(0 rows)

```
## Setting and Getting individual Elements
Elements can be set individually with `set_element`, the modified
input is returned:
``` postgres-console
select print(set_element(a, 1, 1, 1)) as set_element from test_fixture;
┌─────────────┐
│ set_element │
├─────────────┤
└─────────────┘
(0 rows)

```
Scalar elements can be extracted individually with `get_element`
``` postgres-console
select get_element(a, 3, 2) as get_element from test_fixture;
┌─────────────┐
│ get_element │
├─────────────┤
└─────────────┘
(0 rows)

```
If an element does exist `get_element` will return an "empty"
scalar:
``` postgres-console
select get_element(a, 3, 3) as get_element from test_fixture;
┌─────────────┐
│ get_element │
├─────────────┤
└─────────────┘
(0 rows)

```
## Elementwise Addition

The GraphBLAS API has elementwise operations on matrices that
operate pairs of matrices.  `eadd` computes the element-wise
“addition” of two matrices a and b, element-wise using any binary
operator.  The "add" in the name means that the union of both
graphs is taken; elements present on both sides of the operation
are included in the result.
``` postgres-console
select print(a) as a, binaryop, print(b) as b, print(eadd(a, b, binaryop)) as "eadd(a, b, binaryop)" from test_fixture;
┌───┬──────────┬───┬──────────────────────┐
│ a │ binaryop │ b │ eadd(a, b, binaryop) │
├───┼──────────┼───┼──────────────────────┤
└───┴──────────┴───┴──────────────────────┘
(0 rows)

```
Eadd can also be accomplished with binary operators specific to
OneSparse.  Different binaryops are passed to eadd to do different
elementwise operations:
``` postgres-console
select print(a |+ b) as "a |+ b", print(a |- b) as "a |- b", print(a |* b) as "a |* b", print(a |/ b) as "a |/ b" from test_fixture;
┌────────┬────────┬────────┬────────┐
│ a |+ b │ a |- b │ a |* b │ a |/ b │
├────────┼────────┼────────┼────────┤
└────────┴────────┴────────┴────────┘
(0 rows)

```
From a graph standpoint, elementwise addition can be seen as the
merging ("union") of two graphs, such that the result has edges
from both graphs.  Any edges that occur in both graphs are merged
with the provided binary operator.
``` postgres-console
select draw(a) as binop_a_source, draw(b) as binop_b_source, draw(eadd(a, b, binaryop)) as binop_c_source from test_fixture \gset
no rows returned for \gset
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
    <td style="font-size: 30px; vertical-align: middle;">op</td>
    <td style="width: 30%;">
        <!-- Diagram B -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
    <td style="font-size: 30px; vertical-align: middle;">=</td>
    <td style="width: 30%;">
        <!-- Diagram C -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
  </tr>
</table>
## Elementwise Multiplication

`emult` multiplies elements of two matrices, taking only the
intersection of common elements in both matrices, if an element is
missing from either the left or right side, it is ommited from the
result:
``` postgres-console
select print(a) as a, binaryop, print(b) as b, print(emult(a, b, binaryop)) as "emult(a, b, binaryop)" from test_fixture;
┌───┬──────────┬───┬───────────────────────┐
│ a │ binaryop │ b │ emult(a, b, binaryop) │
├───┼──────────┼───┼───────────────────────┤
└───┴──────────┴───┴───────────────────────┘
(0 rows)

```
Emult can also be accomplished with binary operators specific to
OneSparse.  Different binaryops are passed to emult to do different
elementwise operations:
``` postgres-console
select print(a &+ b) as "a &+ b", print(a &- b) as "a &- b", print(a &* b) as "a &* b", print(a &/ b) as "a &/ b" from test_fixture;
┌────────┬────────┬────────┬────────┐
│ a &+ b │ a &- b │ a &* b │ a &/ b │
├────────┼────────┼────────┼────────┤
└────────┴────────┴────────┴────────┘
(0 rows)

```
From a graph standpoint, elementwise multiplication can be seen as
the intersection of two graphs, such that the result has edges that
are only present in both graphs.  The edges are combined with the
provided binary operator.
``` postgres-console
select draw(a) as binop_a_source, draw(b) as binop_b_source, draw(emult(a, b, binaryop)) as binop_c_source from test_fixture \gset
no rows returned for \gset
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
    <td style="font-size: 30px; vertical-align: middle;">op</td>
    <td style="width: 30%;">
        <!-- Diagram B -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
    <td style="font-size: 30px; vertical-align: middle;">=</td>
    <td style="width: 30%;">
        <!-- Diagram C -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
  </tr>
</table>
## Elementwise Union

`eunion` is like `eadd` but differs in how the binary op is
applied. A pair of scalars, `alpha` and `beta` define the inputs to
the operator when entries are present in one matrix but not the
other.
``` postgres-console
select print(a) as a, binaryop, print(b) as b, print(eunion(a, 3::scalar, b, 4::scalar, binaryop)) as "eunion(a, 3::scalar, b, 4::scalar, binaryop)" from test_fixture;
┌───┬──────────┬───┬──────────────────────────────────────────────┐
│ a │ binaryop │ b │ eunion(a, 3::scalar, b, 4::scalar, binaryop) │
├───┼──────────┼───┼──────────────────────────────────────────────┤
└───┴──────────┴───┴──────────────────────────────────────────────┘
(0 rows)

```
From a graph standpoint, elementwise union is very similar to
`eadd()`, and can be seen as the merging ("union") of two graphs,
such that the result has edges from both graphs.  Any edges that
occur in both graphs are merged with the provided binary operator.
If an edge occurs in a but not in b, it is combined with the
scalar `alpha`, if the edge occurs in the b but not in a, then
the edge is combined with scalar `beta`.
``` postgres-console
select draw(a) as binop_a_source, draw(b) as binop_b_source, draw(eunion(a, 3::scalar, b, 4::scalar, binaryop)) as binop_c_source from test_fixture \gset
no rows returned for \gset
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
    <td style="font-size: 30px; vertical-align: middle;">op</td>
    <td style="width: 30%;">
        <!-- Diagram B -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
    <td style="font-size: 30px; vertical-align: middle;">=</td>
    <td style="width: 30%;">
        <!-- Diagram C -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
  </tr>
</table>
## Reduction

The entire matrix can be reduced to a scalar value:
``` postgres-console
select print(b) as b, 'plus_monoid_int32' as monoid, reduce_scalar(b) from test_fixture;
┌───┬────────┬───────────────┐
│ b │ monoid │ reduce_scalar │
├───┼────────┼───────────────┤
└───┴────────┴───────────────┘
(0 rows)

```
The entire matrix can be reduced to a scalar value with a provided
monoid that changes the reduction operation:
``` postgres-console
select print(b) as b, 'min_monoid_int32' as monoid, reduce_scalar(b, 'min_monoid_int32') from test_fixture;
┌───┬────────┬───────────────┐
│ b │ monoid │ reduce_scalar │
├───┼────────┼───────────────┤
└───┴────────┴───────────────┘
(0 rows)

```
The matrix can also be reduced to a column vector:
``` postgres-console
select print(b) as b, 'plus_monoid_int32' as monoid, print(reduce_cols(b)) as reduce_cols from test_fixture;
┌───┬────────┬─────────────┐
│ b │ monoid │ reduce_cols │
├───┼────────┼─────────────┤
└───┴────────┴─────────────┘
(0 rows)

```
To reduce a row vector:
``` postgres-console
select print(b) as b, 'plus_monoid_int32' as monoid, print(reduce_rows(b)) as reduce_rows from test_fixture;
┌───┬────────┬─────────────┐
│ b │ monoid │ reduce_rows │
├───┼────────┼─────────────┤
└───┴────────┴─────────────┘
(0 rows)

```
## Matrix Matrix Multiplication

Matrix Multiplication is the heart of linear algebra.  All matrix
multiplication happens over a semiring.  For the most common form
of matrix multiplication, the outer opperation is to multiply
coresponding elements with the "times" operator and then reduce
those products with the "plus" operator.  This is called the
`plus_times` semiring:
``` postgres-console
select print(a) as a, semiring, print(b) as b, print(mxm(a, b)) as "mxm(a, b)" from test_fixture;
┌───┬──────────┬───┬───────────┐
│ a │ semiring │ b │ mxm(a, b) │
├───┼──────────┼───┼───────────┤
└───┴──────────┴───┴───────────┘
(0 rows)

select draw(a) as binop_a_source, draw(b) as binop_b_source, draw(mxm(a, b)) as binop_c_source from test_fixture \gset
no rows returned for \gset
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
    <td style="font-size: 30px; vertical-align: middle;">op</td>
    <td style="width: 30%;">
        <!-- Diagram B -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
    <td style="font-size: 30px; vertical-align: middle;">=</td>
    <td style="width: 30%;">
        <!-- Diagram C -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
  </tr>
</table>
AxB can also be done with the `@` operator, mimicking the Python
syntax.  The default semiring for numeric types is `plus_times`.
``` postgres-console
select print(a) as a, '@' as "@", print(b) as b, print(a @ b) as "a @ b" from test_fixture;
┌───┬───┬───┬───────┐
│ a │ @ │ b │ a @ b │
├───┼───┼───┼───────┤
└───┴───┴───┴───────┘
(0 rows)

```
## Matrix Vector Multiplication

Matrices can be multipled by vectors on the right taking the linear
combination of the matrices columns using the vectors elements as
coefficients:
``` postgres-console
select print(a) as a, '@' as "@", semiring, print(u) as u, print(mxv(a, u)) as "mxv(a, u)" from test_fixture;
┌───┬───┬──────────┬───┬───────────┐
│ a │ @ │ semiring │ u │ mxv(a, u) │
├───┼───┼──────────┼───┼───────────┤
└───┴───┴──────────┴───┴───────────┘
(0 rows)

```
From a graph standpoint, matrix vector multiplication is used to
"pull" back to adjacent nodes from their incoming edges.  When
iterated, it forms the basis for working back along incoming links.
``` postgres-console
select draw(a) as binop_a_source, draw(u) as binop_b_source, draw(mxv(a, u)) as binop_c_source from test_fixture \gset
no rows returned for \gset
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
    <td style="font-size: 30px; vertical-align: middle;">op</td>
    <td style="width: 30%;">
        <!-- Diagram B -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
    <td style="font-size: 30px; vertical-align: middle;">=</td>
    <td style="width: 30%;">
        <!-- Diagram C -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
  </tr>
</table>
'mxv' is also supported by the `@` operator:
``` postgres-console
select print(a) as a, '@' as "@", print(u) as u, print(a @ u) as "a @ u" from test_fixture;
┌───┬───┬───┬───────┐
│ a │ @ │ u │ a @ u │
├───┼───┼───┼───────┤
└───┴───┴───┴───────┘
(0 rows)

```
## Vector Matrix Multiplication

Matrices can be multipled by vectors on the right taking the linear
combination of the matrices rows using the vectors elements as
coefficients:
``` postgres-console
select print(v) as v, semiring, print(b) as b, print(vxm(v, b, semiring)) as "vxm(v, b, semiring)" from test_fixture;
┌───┬──────────┬───┬─────────────────────┐
│ v │ semiring │ b │ vxm(v, b, semiring) │
├───┼──────────┼───┼─────────────────────┤
└───┴──────────┴───┴─────────────────────┘
(0 rows)

```
From a graph standpoint, vector matrix multiplication is used to
"push" forward to adjacent nodes from their outgoing edges.  When
iterated, it forms the basis for working forward along outgoing
edges.
``` postgres-console
select draw(v) as binop_a_source, draw(b) as binop_b_source, draw(vxm(v, b)) as binop_c_source from test_fixture \gset
no rows returned for \gset
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
    <td style="font-size: 30px; vertical-align: middle;">op</td>
    <td style="width: 30%;">
        <!-- Diagram B -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
    <td style="font-size: 30px; vertical-align: middle;">=</td>
    <td style="width: 30%;">
        <!-- Diagram C -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
  </tr>
</table>
'vxm' is also supported by the `@` operator:
``` postgres-console
select print(v) as v, '@' as "@", print(b) as b, print(v @ b) as "v @ b" from test_fixture;
┌───┬───┬───┬───────┐
│ v │ @ │ b │ v @ b │
├───┼───┼───┼───────┤
└───┴───┴───┴───────┘
(0 rows)

```
## Choosing Elements

The `choose` method calls the `GrB_select()` API function.  The
name `choose` was chosen not to conflict with the SQL keyword
`select`.  Selection provides a conditional operator called an
`indexunaryop` and a parameter for the operator to use to compare
elements in the matrix.  Below, all elements with values greater
than 1 are returned:
``` postgres-console
select print(a) as a, indexunaryop, print(choose(a, indexunaryop, 1)) as selected from test_fixture;
┌───┬──────────────┬──────────┐
│ a │ indexunaryop │ selected │
├───┼──────────────┼──────────┤
└───┴──────────────┴──────────┘
(0 rows)

select draw(a) as uop_a_source, draw(choose(a, indexunaryop, 1)) as uop_b_source from test_fixture \gset
no rows returned for \gset
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 45%;">
        <!-- Diagram A -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
    <td style="font-size: 30px; vertical-align: middle;">op</td>
    <td style="width: 45%;">
        <!-- Diagram B -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
  </tr>
</table>
## Choosing Operators
Selection can also be done with scalars and operators:p
``` postgres-console
select print(a > 1) as "a > 1", print(a >= 1) as "a >= 1", print(a < 1) as "a < 1", print(a <= 1) as "a <= 1" from test_fixture;
┌───────┬────────┬───────┬────────┐
│ a > 1 │ a >= 1 │ a < 1 │ a <= 1 │
├───────┼────────┼───────┼────────┤
└───────┴────────┴───────┴────────┘
(0 rows)

```
A useful select operator is `triu`, it select only upper triangular
values, this turns your graph into a direct acyclic graph (DAG) by
removing all the links "back" from higher number nodes to lower.
``` postgres-console
select print(random_matrix(8, 8, 16, seed=>0.42, max=>42)) as matrix,
       print(choose(random_matrix(8, 8, 16, seed=>0.42, max=>42), 'triu', 0)) as triu from test_fixture;
ERROR:  function random_matrix(integer, integer, integer, seed => numeric, max => integer) does not exist
LINE 1: select print(random_matrix(8, 8, 16, seed=>0.42, max=>42)) a...
                     ^
HINT:  No function matches the given name and argument types. You might need to add explicit type casts.
select draw(random_matrix(8, 8, 16, seed=>0.42, max=>42)) as uop_a_source,
       draw(choose(random_matrix(8, 8, 16, seed=>0.42, max=>42), 'triu', 0)) as uop_b_source
       from test_fixture \gset
ERROR:  function random_matrix(integer, integer, integer, seed => numeric, max => integer) does not exist
LINE 1: select draw(random_matrix(8, 8, 16, seed=>0.42, max=>42)) as...
                    ^
HINT:  No function matches the given name and argument types. You might need to add explicit type casts.
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 45%;">
        <!-- Diagram A -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
    <td style="font-size: 30px; vertical-align: middle;">op</td>
    <td style="width: 45%;">
        <!-- Diagram B -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
  </tr>
</table>
## Kronecker

The `kronecker()` function takes two input matrices, and replaces
every element in the second matrix with a new submatrix of the
first.  This "expands" the matrix exponentially.  This is useful
for constructing synthetic graphs with specific power law
distributions.

``` postgres-console
select print(s) as s, semiring, print(s) as s, print(kronecker(s, s, semiring)) as kronecker from test_fixture;
┌───┬──────────┬───┬───────────┐
│ s │ semiring │ s │ kronecker │
├───┼──────────┼───┼───────────┤
└───┴──────────┴───┴───────────┘
(0 rows)

select draw(s) as binop_a_source, draw(s) as binop_b_source, draw(kronecker(s, s, semiring)) as binop_c_source from test_fixture \gset
no rows returned for \gset
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
    <td style="font-size: 30px; vertical-align: middle;">op</td>
    <td style="width: 30%;">
        <!-- Diagram B -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
    <td style="font-size: 30px; vertical-align: middle;">=</td>
    <td style="width: 30%;">
        <!-- Diagram C -->
``` postgres-console
Error: <stdin>: syntax error in line 1 near ':'
```
    </td>
  </tr>
</table>
## Kronecker Power

There's a special function for exponentiating a matrix to itself a
certain number of times, `kronpower`:
``` postgres-console
select print(kronpower(s, 2)) from test_fixture;
┌───────┐
│ print │
├───────┤
└───────┘
(0 rows)

```
Kronecker products can very quickly make huge graphs with power law
distributions.  These are handy synthetic graphs to mimic certain
statistical edge distributions common in sparse graph problems:
``` postgres-console
select nvals(kronpower(s, 4)) from test_fixture;
┌───────┐
│ nvals │
├───────┤
└───────┘
(0 rows)

```
## Transpose

A matrix can be transposed with the `transpose()` function:
``` postgres-console
select print(transpose(a)) from test_fixture;
┌───────┐
│ print │
├───────┤
└───────┘
(0 rows)

```
## Apply

`apply` takes an operator of type `unaryop` and applies it to every
element of the matrix.  The 'ainv_int32' returned the additive
inverse (the negative value for integers) of every element:
``` postgres-console
select print(a) as a, unaryop, print(apply(a, unaryop)) as applied from test_fixture;
┌───┬─────────┬─────────┐
│ a │ unaryop │ applied │
├───┼─────────┼─────────┤
└───┴─────────┴─────────┘
(0 rows)

```
## SuiteSparse Info

The `info` function returns a descripton of the matrix from
SuiteSparse.
``` postgres-console
select info(a) from test_fixture;
┌──────┐
│ info │
├──────┤
└──────┘
(0 rows)

```
## Matrix Duplication

The `dup` function duplicates a matrix returning a new matrix
object with the same values:
``` postgres-console
select dup(a) from test_fixture;
┌─────┐
│ dup │
├─────┤
└─────┘
(0 rows)

```
## Work Completion

The `wait` method is used to "complete" a matrix, which may have
pending operations waiting to be performed when using the default
SuiteSparse non-blocking mode.  As a side effect, wait will sort
the elements of the input:
``` postgres-console
select wait('int32[2:2:2 3:3:3 1:1:1]'::matrix);
┌──────────────────────────┐
│           wait           │
├──────────────────────────┤
│ int32[1:1:1 2:2:2 3:3:3] │
└──────────────────────────┘
(1 row)

```
The `clear` function clears the matrix of all elements and returns
the same object, but empty.  The dimensions do not change:
## Clearing Matrices

``` postgres-console
select clear('int32[1:1:1 2:2:2 3:3:3]'::matrix);
┌───────┐
│ clear │
├───────┤
│ int32 │
└───────┘
(1 row)

```
## Extra tests

This documentation also forms the basis for the onesparse tests,
These tests run the documentation against a live server, all the
above results are automatically generated.
``` postgres-console
select nrows('int32(10)[1:1:1 2:2:2 3:3:3]'::matrix);
┌───────┐
│ nrows │
├───────┤
│    10 │
└───────┘
(1 row)

select ncols('int32(10)[1:1:1 2:2:2 3:3:3]'::matrix);
┌─────────────────────┐
│        ncols        │
├─────────────────────┤
│ 1152921504606846976 │
└─────────────────────┘
(1 row)

select nvals('int32(10)[1:1:1 2:2:2 3:3:3]'::matrix);
┌───────┐
│ nvals │
├───────┤
│     3 │
└───────┘
(1 row)

select nrows('int32(10:)[1:1:1 2:2:2 3:3:3]'::matrix);
┌───────┐
│ nrows │
├───────┤
│    10 │
└───────┘
(1 row)

select ncols('int32(10:)[1:1:1 2:2:2 3:3:3]'::matrix);
┌─────────────────────┐
│        ncols        │
├─────────────────────┤
│ 1152921504606846976 │
└─────────────────────┘
(1 row)

select nvals('int32(10:)[1:1:1 2:2:2 3:3:3]'::matrix);
┌───────┐
│ nvals │
├───────┤
│     3 │
└───────┘
(1 row)

select nrows('int32(:10)[1:1:1 2:2:2 3:3:3]'::matrix);
┌─────────────────────┐
│        nrows        │
├─────────────────────┤
│ 1152921504606846976 │
└─────────────────────┘
(1 row)

select ncols('int32(:10)[1:1:1 2:2:2 3:3:3]'::matrix);
┌───────┐
│ ncols │
├───────┤
│    10 │
└───────┘
(1 row)

select nvals('int32(:10)[1:1:1 2:2:2 3:3:3]'::matrix);
┌───────┐
│ nvals │
├───────┤
│     3 │
└───────┘
(1 row)

select nrows('int32(10:10)[1:1:1 2:2:2 3:3:3]'::matrix);
┌───────┐
│ nrows │
├───────┤
│    10 │
└───────┘
(1 row)

select ncols('int32(10:10)[1:1:1 2:2:2 3:3:3]'::matrix);
┌───────┐
│ ncols │
├───────┤
│    10 │
└───────┘
(1 row)

select nvals('int32(10:10)[1:1:1 2:2:2 3:3:3]'::matrix);
┌───────┐
│ nvals │
├───────┤
│     3 │
└───────┘
(1 row)

```