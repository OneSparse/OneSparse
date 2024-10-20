# Matrix

This documentation is also tests for the code, the examples below
show the literal output of these statements from Postgres.

Some setup to make sure the extension is installed.
``` postgres-console
create extension if not exists onesparse;
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
┌────────────────────┐
│        draw        │
├────────────────────┤
│ digraph {          │
│ 1 -> 2 [label="1"] │
│ 2 -> 3 [label="2"] │
│ 3 -> 1 [label="3"] │
│ }                  │
│                    │
└────────────────────┘
(1 row)

```
Will generate the following diagram:
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="65pt" height="170pt"
 viewBox="0.00 0.00 65.11 170.12" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 166.12)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-166.12 61.11,-166.12 61.11,4 -4,4"/>
<!-- 1 -->
<g id="node1" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="41.56" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="41.56" y="-148.2" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="15.56" y="-79.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge1" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M37.38,-138.33C33.65,-128.71 28.08,-114.36 23.51,-102.56"/>
<polygon fill="black" stroke="black" points="26.69,-101.09 19.81,-93.03 20.16,-103.62 26.69,-101.09"/>
<text text-anchor="middle" x="33.56" y="-113.68" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="41.56" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="41.56" y="-10.12" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge2" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M19.73,-69.29C23.46,-59.67 29.03,-45.32 33.61,-33.52"/>
<polygon fill="black" stroke="black" points="36.95,-34.58 37.3,-23.99 30.42,-32.05 36.95,-34.58"/>
<text text-anchor="middle" x="33.56" y="-44.64" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge3" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M41.56,-24.31C41.56,-46.97 41.56,-98.31 41.56,-127.59"/>
<polygon fill="black" stroke="black" points="38.06,-127.97 41.56,-137.97 45.06,-127.97 38.06,-127.97"/>
<text text-anchor="middle" x="44.56" y="-79.16" font-family="Times,serif" font-size="8.00">3</text>
</g>
</g>
</svg>
</div>

Let's look at our cast of test objects for the remaining examples.
These objects from the `onesparse.test_fixture` table.
``` postgres-console
select * from test_fixture;
┌─[ RECORD 1 ]─┬─────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ t            │ int32                                                                                                       │
│ a            │ int32(4:4)[0:2:0 0:3:3 1:0:2 1:2:1 1:3:0 2:0:2 2:1:2 3:0:2 3:2:1]                                           │
│ b            │ int32(4:4)[0:3:4 1:2:3 1:3:1 2:1:2 2:3:4 3:1:0 3:2:2]                                                       │
│ d            │ int32(4:4)[0:0:1 0:1:1 0:2:1 0:3:1 1:0:1 1:1:1 1:2:1 1:3:1 2:0:1 2:1:1 2:2:1 2:3:1 3:0:1 3:1:1 3:2:1 3:3:1] │
│ s            │ int32(2:2)[0:0:1 0:1:1 1:1:1]                                                                               │
│ u            │ int32(4)[1:2]                                                                                               │
│ v            │ int32(4)[1:3 2:3]                                                                                           │
│ unaryop      │ ainv_int32                                                                                                  │
│ indexunaryop │ valuegt_int32                                                                                               │
│ binaryop     │ times_int32                                                                                                 │
│ monoid       │ plus_monoid_int32                                                                                           │
│ semiring     │ plus_times_int32                                                                                            │
└──────────────┴─────────────────────────────────────────────────────────────────────────────────────────────────────────────┘

```

Here are a couple of sparse matrices from the test_fixture table.
We'll call them `a` and `b` in these docs:

``` postgres-console
select print(a) as a, print(b) as b from test_fixture;
┌────────────────────┬────────────────────┐
│         a          │         b          │
├────────────────────┼────────────────────┤
│      0  1  2  3    │      0  1  2  3    │
│    ────────────    │    ────────────    │
│  0│        0  3    │  0│           4    │
│  1│  2     1  0    │  1│        3  1    │
│  2│  2  2          │  2│     2     4    │
│  3│  2     1       │  3│     0  2       │
│                    │                    │
└────────────────────┴────────────────────┘
(1 row)

```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="153pt" height="239pt"
 viewBox="0.00 0.00 153.31 239.17" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 235.17)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-235.17 149.31,-235.17 149.31,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="69.76" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="69.76" y="-217.25" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="98.76" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="98.76" y="-148.2" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M69.21,-207.05C69.22,-199.23 70.07,-188.65 73.76,-180.12 75.71,-175.61 78.62,-171.28 81.78,-167.4"/>
<polygon fill="black" stroke="black" points="84.61,-169.49 88.79,-159.75 79.45,-164.76 84.61,-169.49"/>
<text text-anchor="middle" x="76.76" y="-182.72" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="51.76" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="51.76" y="-10.12" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M55.27,-214.33C42.6,-210.23 24.72,-202.43 14.76,-189.12 -4.01,-164.06 3.88,-151.24 0.76,-120.08 -2.27,-89.91 10.52,-48.91 14.76,-42.04 18.79,-35.51 24.85,-29.86 30.91,-25.32"/>
<polygon fill="black" stroke="black" points="33.21,-27.99 39.55,-19.51 29.3,-22.19 33.21,-27.99"/>
<text text-anchor="middle" x="3.76" y="-113.68" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;0 -->
<g id="edge6" class="edge">
<title>2&#45;&gt;0</title>
<path fill="none" stroke="black" d="M94.2,-161.63C89.99,-171.38 83.6,-186.13 78.42,-198.12"/>
<polygon fill="black" stroke="black" points="75.17,-196.81 74.42,-207.37 81.6,-199.59 75.17,-196.81"/>
<text text-anchor="middle" x="89.76" y="-182.72" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="129.76" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="129.76" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M101.77,-138.2C104.08,-130.45 107.58,-119.91 111.76,-111.08 113.43,-107.55 115.43,-103.92 117.49,-100.47"/>
<polygon fill="black" stroke="black" points="120.48,-102.28 122.85,-91.95 114.56,-98.55 120.48,-102.28"/>
<text text-anchor="middle" x="114.76" y="-113.68" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;0 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;0</title>
<path fill="none" stroke="black" d="M52.73,-24.1C55.6,-56.87 64.1,-153.69 67.89,-196.9"/>
<polygon fill="black" stroke="black" points="64.43,-197.46 68.79,-207.12 71.4,-196.85 64.43,-197.46"/>
<text text-anchor="middle" x="64.76" y="-113.68" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge9" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M55.56,-24.03C63.45,-46.86 81.69,-99.69 91.76,-128.85"/>
<polygon fill="black" stroke="black" points="88.5,-130.12 95.07,-138.43 95.11,-127.83 88.5,-130.12"/>
<text text-anchor="middle" x="82.76" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M131.33,-93.05C133.1,-109.08 134.51,-139.45 123.76,-162.12 115.76,-178.98 100.68,-193.96 88.48,-204.18"/>
<polygon fill="black" stroke="black" points="86.25,-201.48 80.61,-210.45 90.61,-206.95 86.25,-201.48"/>
<text text-anchor="middle" x="132.76" y="-148.2" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M125.91,-92.87C123.08,-100.57 118.99,-111.09 114.76,-120.08 113.26,-123.26 111.57,-126.57 109.86,-129.77"/>
<polygon fill="black" stroke="black" points="106.65,-128.35 104.86,-138.79 112.77,-131.74 106.65,-128.35"/>
<text text-anchor="middle" x="121.76" y="-113.68" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M120.51,-71.08C112.33,-63.23 99.98,-51.62 88.76,-42.04 82.92,-37.06 76.37,-31.82 70.43,-27.18"/>
<polygon fill="black" stroke="black" points="72.37,-24.26 62.31,-20.93 68.09,-29.8 72.37,-24.26"/>
<text text-anchor="middle" x="101.76" y="-44.64" font-family="Times,serif" font-size="8.00">0</text>
</g>
</g>
</svg>
</div>

    </td>
    <td style="width: 30%;">
        <!-- Diagram B -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="105pt" height="239pt"
 viewBox="0.00 0.00 104.56 239.17" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 235.17)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-235.17 100.56,-235.17 100.56,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="56.56" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="56.56" y="-217.25" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node2" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="56.56" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="56.56" y="-148.2" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M56.56,-207.08C56.56,-197.72 56.56,-184.04 56.56,-172.55"/>
<polygon fill="black" stroke="black" points="60.06,-172.42 56.56,-162.42 53.06,-172.42 60.06,-172.42"/>
<text text-anchor="middle" x="59.56" y="-182.72" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1 -->
<g id="node3" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="15.56" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge6" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M45.16,-141.51C38.19,-136.28 29.67,-128.74 24.56,-120.08 21.61,-115.1 19.63,-109.22 18.3,-103.58"/>
<polygon fill="black" stroke="black" points="21.68,-102.59 16.45,-93.37 14.79,-103.83 21.68,-102.59"/>
<text text-anchor="middle" x="27.56" y="-113.68" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node4" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="56.56" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="56.56" y="-10.12" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M56.56,-137.97C56.56,-115.34 56.56,-63.66 56.56,-34.34"/>
<polygon fill="black" stroke="black" points="60.06,-34.31 56.56,-24.31 53.06,-34.31 60.06,-34.31"/>
<text text-anchor="middle" x="59.56" y="-79.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M21.84,-92.34C28,-102.42 37.54,-118.01 45.06,-130.31"/>
<polygon fill="black" stroke="black" points="42.13,-132.22 50.33,-138.92 48.1,-128.56 42.13,-132.22"/>
<text text-anchor="middle" x="41.56" y="-113.68" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M14.9,-68.72C14.93,-60.78 15.99,-50.17 20.56,-42.04 24.2,-35.55 29.93,-29.96 35.75,-25.46"/>
<polygon fill="black" stroke="black" points="37.88,-28.24 44.12,-19.68 33.9,-22.48 37.88,-28.24"/>
<text text-anchor="middle" x="23.56" y="-44.64" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge5" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M66.58,-21.3C72.15,-26.58 78.55,-33.94 81.56,-42.04 93.63,-74.56 93.63,-87.57 81.56,-120.08 79.77,-124.9 76.79,-129.44 73.49,-133.45"/>
<polygon fill="black" stroke="black" points="70.87,-131.14 66.58,-140.82 75.97,-135.92 70.87,-131.14"/>
<text text-anchor="middle" x="93.56" y="-79.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M50.27,-23.3C44.11,-33.38 34.57,-48.97 27.05,-61.27"/>
<polygon fill="black" stroke="black" points="24.01,-59.52 21.78,-69.88 29.99,-63.18 24.01,-59.52"/>
<text text-anchor="middle" x="41.56" y="-44.64" font-family="Times,serif" font-size="8.00">2</text>
</g>
</g>
</svg>
</div>

    </td>
  </tr>
</table>
Here are some sparse test vectors, they will be used for some of
the examples below:

``` postgres-console
select print(u) as u, print(v) as v from test_fixture;
┌───────────┬───────────┐
│     u     │     v     │
├───────────┼───────────┤
│           │           │
│    ───    │    ───    │
│  0│       │  0│       │
│  1│  2    │  1│  3    │
│  2│       │  2│  3    │
│  3│       │  3│       │
│           │           │
└───────────┴───────────┘
(1 row)

```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
<div>
<!-- Title: vector Pages: 1 -->
<svg width="70pt" height="177pt"
 viewBox="0.00 0.00 70.00 177.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 173)">
<title>vector</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-173 66,-173 66,4 -4,4"/>
<g id="clust1" class="cluster">
<title>cluster_vector</title>
<polygon fill="lightgray" stroke="black" stroke-dasharray="5,2" points="8,-8 8,-161 54,-161 54,-8 8,-8"/>
</g>
<!-- node0 -->
<g id="node1" class="node">
<title>node0</title>
<polygon fill="none" stroke="black" points="33,-153 29,-153 29,-149 33,-149 33,-153"/>
</g>
<!-- node1 -->
<g id="node2" class="node">
<title>node1</title>
<polygon fill="none" stroke="black" points="46,-113 16,-113 16,-96 46,-96 46,-113"/>
<text text-anchor="middle" x="31" y="-102.6" font-family="Times,serif" font-size="8.00">1:2</text>
</g>
<!-- node0&#45;&gt;node1 -->
<!-- node2 -->
<g id="node3" class="node">
<title>node2</title>
<polygon fill="none" stroke="black" points="33,-60 29,-60 29,-56 33,-56 33,-60"/>
</g>
<!-- node1&#45;&gt;node2 -->
<!-- node3 -->
<g id="node4" class="node">
<title>node3</title>
<polygon fill="none" stroke="black" points="33,-20 29,-20 29,-16 33,-16 33,-20"/>
</g>
<!-- node2&#45;&gt;node3 -->
</g>
</svg>
</div>

    </td>
    <td style="width: 30%;">
        <!-- Diagram B -->
<div>
<!-- Title: vector Pages: 1 -->
<svg width="70pt" height="190pt"
 viewBox="0.00 0.00 70.00 190.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 186)">
<title>vector</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-186 66,-186 66,4 -4,4"/>
<g id="clust1" class="cluster">
<title>cluster_vector</title>
<polygon fill="lightgray" stroke="black" stroke-dasharray="5,2" points="8,-8 8,-174 54,-174 54,-8 8,-8"/>
</g>
<!-- node0 -->
<g id="node1" class="node">
<title>node0</title>
<polygon fill="none" stroke="black" points="33,-166 29,-166 29,-162 33,-162 33,-166"/>
</g>
<!-- node1 -->
<g id="node2" class="node">
<title>node1</title>
<polygon fill="none" stroke="black" points="46,-126 16,-126 16,-109 46,-109 46,-126"/>
<text text-anchor="middle" x="31" y="-115.6" font-family="Times,serif" font-size="8.00">1:3</text>
</g>
<!-- node0&#45;&gt;node1 -->
<!-- node2 -->
<g id="node3" class="node">
<title>node2</title>
<polygon fill="none" stroke="black" points="46,-73 16,-73 16,-56 46,-56 46,-73"/>
<text text-anchor="middle" x="31" y="-62.6" font-family="Times,serif" font-size="8.00">2:3</text>
</g>
<!-- node1&#45;&gt;node2 -->
<!-- node3 -->
<g id="node4" class="node">
<title>node3</title>
<polygon fill="none" stroke="black" points="33,-20 29,-20 29,-16 33,-16 33,-20"/>
</g>
<!-- node2&#45;&gt;node3 -->
</g>
</svg>
</div>

    </td>
  </tr>
</table>
There is also an example "dense" matrix named 'd':

``` postgres-console
select print(d) from test_fixture;
┌────────────────────┐
│       print        │
├────────────────────┤
│      0  1  2  3    │
│    ────────────    │
│  0│  1  1  1  1    │
│  1│  1  1  1  1    │
│  2│  1  1  1  1    │
│  3│  1  1  1  1    │
│                    │
└────────────────────┘
(1 row)

```
One way of thinking about a "dense" matrix is a fully connected
graph, these can be constructed with the `dense_matrix()` function:

<div>
<!-- Title: %3 Pages: 1 -->
<svg width="197pt" height="239pt"
 viewBox="0.00 0.00 196.99 239.17" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 235.17)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-235.17 192.99,-235.17 192.99,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="133.99" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="133.99" y="-217.25" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 0&#45;&gt;0 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;0</title>
<path fill="none" stroke="black" d="M147.77,-224.83C157.68,-226.69 167.54,-224.79 167.54,-219.15 167.54,-215.53 163.49,-213.45 157.98,-212.91"/>
<polygon fill="black" stroke="black" points="157.57,-209.43 147.77,-213.46 157.95,-216.42 157.57,-209.43"/>
<text text-anchor="middle" x="170.54" y="-217.25" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="36.99" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="36.99" y="-148.2" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 0&#45;&gt;1 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;1</title>
<path fill="none" stroke="black" d="M118.97,-215.61C102.38,-212.14 75.68,-204.5 57.99,-189.12 52.53,-184.39 48.22,-177.87 44.96,-171.57"/>
<polygon fill="black" stroke="black" points="48,-169.8 40.68,-162.13 41.62,-172.68 48,-169.8"/>
<text text-anchor="middle" x="60.99" y="-182.72" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2 -->
<g id="node3" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="100.99" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="100.99" y="-10.12" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M141.37,-208.53C145.04,-203.14 149.09,-196.11 150.99,-189.12 164.96,-137.62 170,-118.91 150.99,-69.04 144.71,-52.58 131.12,-37.88 119.69,-27.67"/>
<polygon fill="black" stroke="black" points="121.78,-24.86 111.88,-21.07 117.26,-30.2 121.78,-24.86"/>
<text text-anchor="middle" x="165.99" y="-113.68" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="101.99" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="101.99" y="-79.16" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge4" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M135.5,-207C137.75,-186.95 140.38,-143.84 126.99,-111.08 125.04,-106.33 122,-101.81 118.69,-97.81"/>
<polygon fill="black" stroke="black" points="121.19,-95.35 111.82,-90.42 116.07,-100.12 121.19,-95.35"/>
<text text-anchor="middle" x="139.99" y="-148.2" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M47.98,-158.7C64.13,-169.87 94.51,-190.86 114.5,-204.67"/>
<polygon fill="black" stroke="black" points="112.64,-207.65 122.86,-210.46 116.62,-201.89 112.64,-207.65"/>
<text text-anchor="middle" x="94.99" y="-182.72" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;1 -->
<g id="edge6" class="edge">
<title>1&#45;&gt;1</title>
<path fill="none" stroke="black" d="M50.77,-155.79C60.68,-157.64 70.54,-155.75 70.54,-150.1 70.54,-146.49 66.49,-144.41 60.98,-143.87"/>
<polygon fill="black" stroke="black" points="60.57,-140.39 50.77,-144.42 60.95,-147.38 60.57,-140.39"/>
<text text-anchor="middle" x="73.54" y="-148.2" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge7" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M29.42,-139.41C25.25,-133.89 20.1,-126.75 15.99,-120.08 3.64,-100.06 -3.59,-91.9 1.99,-69.04 5.19,-55.91 5.74,-50.86 15.99,-42.04 32.88,-27.5 57.41,-20.15 75.78,-16.49"/>
<polygon fill="black" stroke="black" points="76.53,-19.91 85.78,-14.74 75.33,-13.02 76.53,-19.91"/>
<text text-anchor="middle" x="4.99" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge8" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M47.16,-140.84C53.87,-135.23 62.73,-127.5 69.99,-120.08 76.27,-113.67 82.73,-106.18 88.19,-99.56"/>
<polygon fill="black" stroke="black" points="90.91,-101.76 94.47,-91.78 85.47,-97.36 90.91,-101.76"/>
<text text-anchor="middle" x="81.99" y="-113.68" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;0 -->
<g id="edge9" class="edge">
<title>2&#45;&gt;0</title>
<path fill="none" stroke="black" d="M114.47,-18.65C125.25,-23.64 140.3,-31.73 150.99,-42.04 161.19,-51.89 163.26,-55.67 167.99,-69.04 185.79,-119.36 194.39,-143.93 165.99,-189.12 162.56,-194.58 157.83,-199.63 153.06,-203.95"/>
<polygon fill="black" stroke="black" points="150.68,-201.38 145.22,-210.45 155.15,-206.77 150.68,-201.38"/>
<text text-anchor="middle" x="185.99" y="-113.68" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge10" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M95.18,-23.2C88.92,-34.32 78.8,-52.72 70.99,-69.04 61.41,-89.04 51.63,-112.44 45.04,-128.75"/>
<polygon fill="black" stroke="black" points="41.66,-127.76 41.19,-138.35 48.16,-130.37 41.66,-127.76"/>
<text text-anchor="middle" x="73.99" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;2 -->
<g id="edge11" class="edge">
<title>2&#45;&gt;2</title>
<path fill="none" stroke="black" d="M114.77,-17.71C124.68,-19.56 134.54,-17.67 134.54,-12.02 134.54,-8.4 130.49,-6.33 124.98,-5.79"/>
<polygon fill="black" stroke="black" points="124.57,-2.3 114.77,-6.33 124.95,-9.29 124.57,-2.3"/>
<text text-anchor="middle" x="137.54" y="-10.12" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge12" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M101.15,-24.33C101.3,-33.76 101.5,-47.46 101.67,-58.91"/>
<polygon fill="black" stroke="black" points="98.17,-59.05 101.82,-68.99 105.17,-58.94 98.17,-59.05"/>
<text text-anchor="middle" x="103.99" y="-44.64" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;0 -->
<g id="edge13" class="edge">
<title>3&#45;&gt;0</title>
<path fill="none" stroke="black" d="M104.55,-92.97C109.85,-115.52 122.1,-167.58 129.01,-196.97"/>
<polygon fill="black" stroke="black" points="125.67,-198.08 131.37,-207.01 132.49,-196.47 125.67,-198.08"/>
<text text-anchor="middle" x="122.99" y="-148.2" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge14" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M91.38,-89.91C84.17,-95.46 74.59,-103.29 66.99,-111.08 61.01,-117.21 55.05,-124.47 50.03,-131"/>
<polygon fill="black" stroke="black" points="47.03,-129.16 43.86,-139.27 52.64,-133.34 47.03,-129.16"/>
<text text-anchor="middle" x="69.99" y="-113.68" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge15" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M96.96,-69.58C94.64,-64.18 92.15,-57.4 90.99,-51.04 89.91,-45.17 90.74,-38.87 92.29,-33.12"/>
<polygon fill="black" stroke="black" points="95.63,-34.17 95.63,-23.58 89.02,-31.86 95.63,-34.17"/>
<text text-anchor="middle" x="93.99" y="-44.64" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge16" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M115.77,-86.75C125.68,-88.6 135.54,-86.71 135.54,-81.06 135.54,-77.45 131.49,-75.37 125.98,-74.83"/>
<polygon fill="black" stroke="black" points="125.57,-71.35 115.77,-75.37 125.95,-78.34 125.57,-71.35"/>
<text text-anchor="middle" x="138.54" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
</g>
</svg>
</div>

And another matrix named 's' which is a Sierpinsky Graph, which
we'll show off a bit later.

``` postgres-console
select print(s) from test_fixture;
┌──────────────┐
│    print     │
├──────────────┤
│      0  1    │
│    ──────    │
│  0│  1  1    │
│  1│     1    │
│              │
└──────────────┘
(1 row)

```
## Random Matrices

`random_matrix` will generate a random matrix provided the type,
number of rows, number of columns, and the number of (approximate)
values, an optional max value, and an optional random seed for
deterministic generation:
``` postgres-console
select print(random_matrix(8, 8, 16, seed=>0.42, max=>42)) as random_matrix;
┌────────────────────────────────┐
│         random_matrix          │
├────────────────────────────────┤
│      0  1  2  3  4  5  6  7    │
│    ────────────────────────    │
│  0│              6 31          │
│  1│                   12       │
│  2│    26                      │
│  3│                40          │
│  4│    11    15           7    │
│  5│ 20 12    22        1       │
│  6│              1             │
│  7│       17                   │
│                                │
└────────────────────────────────┘
(1 row)

```
This random matrix is also a random *graph*:

<div>
<!-- Title: %3 Pages: 1 -->
<svg width="245pt" height="377pt"
 viewBox="0.00 0.00 245.24 377.25" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 373.25)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-373.25 241.24,-373.25 241.24,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="34.24" cy="-357.23" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="34.24" y="-355.33" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 4 -->
<g id="node2" class="node">
<title>4</title>
<ellipse fill="none" stroke="black" cx="135.24" cy="-288.19" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="135.24" y="-286.29" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 0&#45;&gt;4 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;4</title>
<path fill="none" stroke="black" d="M45.44,-348.8C62.35,-337.57 94.65,-316.13 115.55,-302.26"/>
<polygon fill="black" stroke="black" points="117.6,-305.1 124,-296.65 113.73,-299.27 117.6,-305.1"/>
<text text-anchor="middle" x="94.24" y="-320.81" font-family="Times,serif" font-size="8.00">6</text>
</g>
<!-- 5 -->
<g id="node3" class="node">
<title>5</title>
<ellipse fill="none" stroke="black" cx="71.24" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="71.24" y="-148.2" font-family="Times,serif" font-size="8.00">5</text>
</g>
<!-- 0&#45;&gt;5 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;5</title>
<path fill="none" stroke="black" d="M27.98,-346.15C17.09,-327.46 -3.73,-285.98 1.24,-249.17 5.5,-217.63 0.71,-205.25 20.24,-180.12 27.28,-171.08 37.98,-164.38 47.77,-159.71"/>
<polygon fill="black" stroke="black" points="49.27,-162.87 57.08,-155.71 46.51,-156.44 49.27,-162.87"/>
<text text-anchor="middle" x="6.74" y="-251.77" font-family="Times,serif" font-size="8.00">31</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="135.24" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="135.24" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 4&#45;&gt;1 -->
<g id="edge6" class="edge">
<title>4&#45;&gt;1</title>
<path fill="none" stroke="black" d="M135.24,-276.16C135.24,-243.46 135.24,-146.65 135.24,-103.38"/>
<polygon fill="black" stroke="black" points="138.74,-103.15 135.24,-93.15 131.74,-103.15 138.74,-103.15"/>
<text text-anchor="middle" x="140.74" y="-182.72" font-family="Times,serif" font-size="8.00">11</text>
</g>
<!-- 3 -->
<g id="node7" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="91.24" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="91.24" y="-217.25" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 4&#45;&gt;3 -->
<g id="edge7" class="edge">
<title>4&#45;&gt;3</title>
<path fill="none" stroke="black" d="M126.28,-277.99C121.21,-272.51 114.96,-265.25 110.24,-258.17 106.52,-252.58 103.05,-246.16 100.11,-240.21"/>
<polygon fill="black" stroke="black" points="103.15,-238.43 95.74,-230.86 96.81,-241.4 103.15,-238.43"/>
<text text-anchor="middle" x="115.74" y="-251.77" font-family="Times,serif" font-size="8.00">15</text>
</g>
<!-- 7 -->
<g id="node8" class="node">
<title>7</title>
<ellipse fill="none" stroke="black" cx="179.24" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="179.24" y="-217.25" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 4&#45;&gt;7 -->
<g id="edge8" class="edge">
<title>4&#45;&gt;7</title>
<path fill="none" stroke="black" d="M141.93,-277.01C148.52,-266.96 158.75,-251.38 166.83,-239.06"/>
<polygon fill="black" stroke="black" points="169.94,-240.7 172.5,-230.42 164.08,-236.86 169.94,-240.7"/>
<text text-anchor="middle" x="163.24" y="-251.77" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 5&#45;&gt;0 -->
<g id="edge9" class="edge">
<title>5&#45;&gt;0</title>
<path fill="none" stroke="black" d="M67.93,-162.06C66.37,-167.46 64.57,-174.1 63.24,-180.12 51.07,-235.59 41.46,-301.9 36.99,-335.1"/>
<polygon fill="black" stroke="black" points="33.52,-334.65 35.67,-345.02 40.45,-335.57 33.52,-334.65"/>
<text text-anchor="middle" x="55.74" y="-251.77" font-family="Times,serif" font-size="8.00">20</text>
</g>
<!-- 5&#45;&gt;1 -->
<g id="edge10" class="edge">
<title>5&#45;&gt;1</title>
<path fill="none" stroke="black" d="M79.87,-140.06C89.93,-129.53 106.77,-111.89 119.24,-98.83"/>
<polygon fill="black" stroke="black" points="121.96,-101.04 126.34,-91.39 116.9,-96.21 121.96,-101.04"/>
<text text-anchor="middle" x="112.74" y="-113.68" font-family="Times,serif" font-size="8.00">12</text>
</g>
<!-- 6 -->
<g id="node5" class="node">
<title>6</title>
<ellipse fill="none" stroke="black" cx="135.24" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="135.24" y="-10.12" font-family="Times,serif" font-size="8.00">6</text>
</g>
<!-- 5&#45;&gt;6 -->
<g id="edge12" class="edge">
<title>5&#45;&gt;6</title>
<path fill="none" stroke="black" d="M75.24,-138.31C81.09,-122.77 92.58,-93.29 104.24,-69.04 110.29,-56.46 117.86,-42.75 124.01,-32.03"/>
<polygon fill="black" stroke="black" points="127.17,-33.56 129.17,-23.16 121.12,-30.05 127.17,-33.56"/>
<text text-anchor="middle" x="107.24" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 5&#45;&gt;3 -->
<g id="edge11" class="edge">
<title>5&#45;&gt;3</title>
<path fill="none" stroke="black" d="M74.52,-162.07C77.37,-171.66 81.6,-185.83 85.09,-197.53"/>
<polygon fill="black" stroke="black" points="81.82,-198.79 88.03,-207.37 88.53,-196.79 81.82,-198.79"/>
<text text-anchor="middle" x="88.74" y="-182.72" font-family="Times,serif" font-size="8.00">22</text>
</g>
<!-- 1&#45;&gt;6 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;6</title>
<path fill="none" stroke="black" d="M135.24,-68.99C135.24,-59.63 135.24,-45.96 135.24,-34.47"/>
<polygon fill="black" stroke="black" points="138.74,-34.33 135.24,-24.33 131.74,-34.33 138.74,-34.33"/>
<text text-anchor="middle" x="140.74" y="-44.64" font-family="Times,serif" font-size="8.00">12</text>
</g>
<!-- 6&#45;&gt;4 -->
<g id="edge13" class="edge">
<title>6&#45;&gt;4</title>
<path fill="none" stroke="black" d="M150.24,-16.18C177.24,-22.84 231.24,-41.03 231.24,-80.06 231.24,-220.15 231.24,-220.15 231.24,-220.15 231.24,-254.91 188.41,-273.14 159.97,-281.42"/>
<polygon fill="black" stroke="black" points="158.99,-278.06 150.24,-284.03 160.81,-284.82 158.99,-278.06"/>
<text text-anchor="middle" x="234.24" y="-148.2" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2 -->
<g id="node6" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="179.24" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="179.24" y="-148.2" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M172.56,-138.92C165.97,-128.88 155.74,-113.29 147.66,-100.98"/>
<polygon fill="black" stroke="black" points="150.4,-98.78 141.99,-92.34 144.55,-102.62 150.4,-98.78"/>
<text text-anchor="middle" x="165.74" y="-113.68" font-family="Times,serif" font-size="8.00">26</text>
</g>
<!-- 3&#45;&gt;5 -->
<g id="edge5" class="edge">
<title>3&#45;&gt;5</title>
<path fill="none" stroke="black" d="M81.77,-209.46C76.7,-204.15 70.95,-196.9 68.24,-189.12 66.39,-183.79 66.1,-177.79 66.54,-172.16"/>
<polygon fill="black" stroke="black" points="70.04,-172.47 68.08,-162.06 63.12,-171.42 70.04,-172.47"/>
<text text-anchor="middle" x="73.74" y="-182.72" font-family="Times,serif" font-size="8.00">40</text>
</g>
<!-- 7&#45;&gt;2 -->
<g id="edge14" class="edge">
<title>7&#45;&gt;2</title>
<path fill="none" stroke="black" d="M179.24,-207.08C179.24,-197.72 179.24,-184.04 179.24,-172.55"/>
<polygon fill="black" stroke="black" points="182.74,-172.42 179.24,-162.42 175.74,-172.42 182.74,-172.42"/>
<text text-anchor="middle" x="184.74" y="-182.72" font-family="Times,serif" font-size="8.00">17</text>
</g>
</g>
</svg>
</div>

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
┌───┬───┬─────────┐
│ i │ j │    v    │
├───┼───┼─────────┤
│ 0 │ 2 │ int32:0 │
│ 0 │ 3 │ int32:3 │
│ 1 │ 0 │ int32:2 │
│ 1 │ 2 │ int32:1 │
│ 1 │ 3 │ int32:0 │
│ 2 │ 0 │ int32:2 │
│ 2 │ 1 │ int32:2 │
│ 3 │ 0 │ int32:2 │
│ 3 │ 2 │ int32:1 │
└───┴───┴─────────┘
(9 rows)

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
│ t      │ f     │ f     │ t     │
└────────┴───────┴───────┴───────┘
(1 row)

```
## Setting and Getting individual Elements
Elements can be set individually with `set_element`, the modified
input is returned:
``` postgres-console
select print(set_element(a, 1, 1, 1)) as set_element from test_fixture;
┌────────────────────┐
│    set_element     │
├────────────────────┤
│      0  1  2  3    │
│    ────────────    │
│  0│        0  3    │
│  1│  2  1  1  0    │
│  2│  2  2          │
│  3│  2     1       │
│                    │
└────────────────────┘
(1 row)

```
Scalar elements can be extracted individually with `get_element`
``` postgres-console
select get_element(a, 3, 2) as get_element from test_fixture;
┌─────────────┐
│ get_element │
├─────────────┤
│ int32:1     │
└─────────────┘
(1 row)

```
If an element does exist `get_element` will return an "empty"
scalar:
``` postgres-console
select get_element(a, 3, 3) as get_element from test_fixture;
┌─────────────┐
│ get_element │
├─────────────┤
│ int         │
└─────────────┘
(1 row)

```
## Elementwise Addition

The GraphBLAS API has elementwise operations on matrices that
operate pairs of matrices.  `eadd` computes the element-wise
“addition” of two matrices a and b, element-wise using any binary
operator.  The "add" in the name means that the union of both
graphs is taken; elements present on both sides of the operation
are included in the result.
``` postgres-console
select print(a) as a, binaryop, print(b) as b, print(eadd(a, b, binaryop)) as eadd from test_fixture;
┌────────────────────┬─────────────┬────────────────────┬────────────────────┐
│         a          │  binaryop   │         b          │        eadd        │
├────────────────────┼─────────────┼────────────────────┼────────────────────┤
│      0  1  2  3    │ times_int32 │      0  1  2  3    │      0  1  2  3    │
│    ────────────    │             │    ────────────    │    ────────────    │
│  0│        0  3    │             │  0│           4    │  0│        0 12    │
│  1│  2     1  0    │             │  1│        3  1    │  1│  2     3  0    │
│  2│  2  2          │             │  2│     2     4    │  2│  2  4     4    │
│  3│  2     1       │             │  3│     0  2       │  3│  2  0  2       │
│                    │             │                    │                    │
└────────────────────┴─────────────┴────────────────────┴────────────────────┘
(1 row)

```
Eadd can also be accomplished with binary operators specific to
OneSparse.  Different binaryops are passed to eadd to do different
elementwise operations:
``` postgres-console
select print(a |+ b) as "a |+ b", print(a |- b) as "a |- b", print(a |* b) as "a |* b", print(a |/ b) as "a |/ b" from test_fixture;
┌────────────────────┬────────────────────┬────────────────────┬────────────────────┐
│       a |+ b       │       a |- b       │       a |* b       │       a |/ b       │
├────────────────────┼────────────────────┼────────────────────┼────────────────────┤
│      0  1  2  3    │      0  1  2  3    │      0  1  2  3    │      0  1  2  3    │
│    ────────────    │    ────────────    │    ────────────    │    ────────────    │
│  0│        0  7    │  0│        0 -1    │  0│        0 12    │  0│        0  0    │
│  1│  2     4  1    │  1│  2    -2 -1    │  1│  2     3  0    │  1│  2     0  0    │
│  2│  2  4     4    │  2│  2  0     4    │  2│  2  4     4    │  2│  2  1     4    │
│  3│  2  0  3       │  3│  2  0 -1       │  3│  2  0  2       │  3│  2  0  0       │
│                    │                    │                    │                    │
└────────────────────┴────────────────────┴────────────────────┴────────────────────┘
(1 row)

```
From a graph standpoint, elementwise addition can be seen as the
merging ("union") of two graphs, such that the result has edges
from both graphs.  Any edges that occur in both graphs are merged
with the provided binary operator.
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="153pt" height="239pt"
 viewBox="0.00 0.00 153.31 239.17" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 235.17)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-235.17 149.31,-235.17 149.31,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="69.76" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="69.76" y="-217.25" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="98.76" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="98.76" y="-148.2" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M69.21,-207.05C69.22,-199.23 70.07,-188.65 73.76,-180.12 75.71,-175.61 78.62,-171.28 81.78,-167.4"/>
<polygon fill="black" stroke="black" points="84.61,-169.49 88.79,-159.75 79.45,-164.76 84.61,-169.49"/>
<text text-anchor="middle" x="76.76" y="-182.72" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="51.76" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="51.76" y="-10.12" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M55.27,-214.33C42.6,-210.23 24.72,-202.43 14.76,-189.12 -4.01,-164.06 3.88,-151.24 0.76,-120.08 -2.27,-89.91 10.52,-48.91 14.76,-42.04 18.79,-35.51 24.85,-29.86 30.91,-25.32"/>
<polygon fill="black" stroke="black" points="33.21,-27.99 39.55,-19.51 29.3,-22.19 33.21,-27.99"/>
<text text-anchor="middle" x="3.76" y="-113.68" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;0 -->
<g id="edge6" class="edge">
<title>2&#45;&gt;0</title>
<path fill="none" stroke="black" d="M94.2,-161.63C89.99,-171.38 83.6,-186.13 78.42,-198.12"/>
<polygon fill="black" stroke="black" points="75.17,-196.81 74.42,-207.37 81.6,-199.59 75.17,-196.81"/>
<text text-anchor="middle" x="89.76" y="-182.72" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="129.76" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="129.76" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M101.77,-138.2C104.08,-130.45 107.58,-119.91 111.76,-111.08 113.43,-107.55 115.43,-103.92 117.49,-100.47"/>
<polygon fill="black" stroke="black" points="120.48,-102.28 122.85,-91.95 114.56,-98.55 120.48,-102.28"/>
<text text-anchor="middle" x="114.76" y="-113.68" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;0 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;0</title>
<path fill="none" stroke="black" d="M52.73,-24.1C55.6,-56.87 64.1,-153.69 67.89,-196.9"/>
<polygon fill="black" stroke="black" points="64.43,-197.46 68.79,-207.12 71.4,-196.85 64.43,-197.46"/>
<text text-anchor="middle" x="64.76" y="-113.68" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge9" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M55.56,-24.03C63.45,-46.86 81.69,-99.69 91.76,-128.85"/>
<polygon fill="black" stroke="black" points="88.5,-130.12 95.07,-138.43 95.11,-127.83 88.5,-130.12"/>
<text text-anchor="middle" x="82.76" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M131.33,-93.05C133.1,-109.08 134.51,-139.45 123.76,-162.12 115.76,-178.98 100.68,-193.96 88.48,-204.18"/>
<polygon fill="black" stroke="black" points="86.25,-201.48 80.61,-210.45 90.61,-206.95 86.25,-201.48"/>
<text text-anchor="middle" x="132.76" y="-148.2" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M125.91,-92.87C123.08,-100.57 118.99,-111.09 114.76,-120.08 113.26,-123.26 111.57,-126.57 109.86,-129.77"/>
<polygon fill="black" stroke="black" points="106.65,-128.35 104.86,-138.79 112.77,-131.74 106.65,-128.35"/>
<text text-anchor="middle" x="121.76" y="-113.68" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M120.51,-71.08C112.33,-63.23 99.98,-51.62 88.76,-42.04 82.92,-37.06 76.37,-31.82 70.43,-27.18"/>
<polygon fill="black" stroke="black" points="72.37,-24.26 62.31,-20.93 68.09,-29.8 72.37,-24.26"/>
<text text-anchor="middle" x="101.76" y="-44.64" font-family="Times,serif" font-size="8.00">0</text>
</g>
</g>
</svg>
</div>

    </td>
    <td style="font-size: 30px; vertical-align: middle;">op</td>
    <td style="width: 30%;">
        <!-- Diagram B -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="105pt" height="239pt"
 viewBox="0.00 0.00 104.56 239.17" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 235.17)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-235.17 100.56,-235.17 100.56,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="56.56" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="56.56" y="-217.25" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node2" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="56.56" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="56.56" y="-148.2" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M56.56,-207.08C56.56,-197.72 56.56,-184.04 56.56,-172.55"/>
<polygon fill="black" stroke="black" points="60.06,-172.42 56.56,-162.42 53.06,-172.42 60.06,-172.42"/>
<text text-anchor="middle" x="59.56" y="-182.72" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1 -->
<g id="node3" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="15.56" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge6" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M45.16,-141.51C38.19,-136.28 29.67,-128.74 24.56,-120.08 21.61,-115.1 19.63,-109.22 18.3,-103.58"/>
<polygon fill="black" stroke="black" points="21.68,-102.59 16.45,-93.37 14.79,-103.83 21.68,-102.59"/>
<text text-anchor="middle" x="27.56" y="-113.68" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node4" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="56.56" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="56.56" y="-10.12" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M56.56,-137.97C56.56,-115.34 56.56,-63.66 56.56,-34.34"/>
<polygon fill="black" stroke="black" points="60.06,-34.31 56.56,-24.31 53.06,-34.31 60.06,-34.31"/>
<text text-anchor="middle" x="59.56" y="-79.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M21.84,-92.34C28,-102.42 37.54,-118.01 45.06,-130.31"/>
<polygon fill="black" stroke="black" points="42.13,-132.22 50.33,-138.92 48.1,-128.56 42.13,-132.22"/>
<text text-anchor="middle" x="41.56" y="-113.68" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M14.9,-68.72C14.93,-60.78 15.99,-50.17 20.56,-42.04 24.2,-35.55 29.93,-29.96 35.75,-25.46"/>
<polygon fill="black" stroke="black" points="37.88,-28.24 44.12,-19.68 33.9,-22.48 37.88,-28.24"/>
<text text-anchor="middle" x="23.56" y="-44.64" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge5" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M66.58,-21.3C72.15,-26.58 78.55,-33.94 81.56,-42.04 93.63,-74.56 93.63,-87.57 81.56,-120.08 79.77,-124.9 76.79,-129.44 73.49,-133.45"/>
<polygon fill="black" stroke="black" points="70.87,-131.14 66.58,-140.82 75.97,-135.92 70.87,-131.14"/>
<text text-anchor="middle" x="93.56" y="-79.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M50.27,-23.3C44.11,-33.38 34.57,-48.97 27.05,-61.27"/>
<polygon fill="black" stroke="black" points="24.01,-59.52 21.78,-69.88 29.99,-63.18 24.01,-59.52"/>
<text text-anchor="middle" x="41.56" y="-44.64" font-family="Times,serif" font-size="8.00">2</text>
</g>
</g>
</svg>
</div>

    </td>
    <td style="font-size: 30px; vertical-align: middle;">=</td>
    <td style="width: 30%;">
        <!-- Diagram C -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="115pt" height="239pt"
 viewBox="0.00 0.00 114.56 239.17" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 235.17)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-235.17 110.56,-235.17 110.56,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="67.56" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="67.56" y="-217.25" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="15.56" y="-148.2" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M60.11,-208.54C52.07,-198.19 39.11,-181.48 29.24,-168.74"/>
<polygon fill="black" stroke="black" points="31.89,-166.45 22.99,-160.69 26.36,-170.74 31.89,-166.45"/>
<text text-anchor="middle" x="48.56" y="-182.72" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="45.56" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="45.56" y="-10.12" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M71.77,-207.3C73.64,-201.93 75.62,-195.28 76.56,-189.12 77.16,-185.17 76.65,-184.12 76.56,-180.12 75.44,-130.72 83.29,-117.05 71.56,-69.04 68.43,-56.25 62.32,-42.85 56.81,-32.36"/>
<polygon fill="black" stroke="black" points="59.72,-30.38 51.83,-23.31 53.59,-33.76 59.72,-30.38"/>
<text text-anchor="middle" x="84.06" y="-113.68" font-family="Times,serif" font-size="8.00">12</text>
</g>
<!-- 2&#45;&gt;0 -->
<g id="edge6" class="edge">
<title>2&#45;&gt;0</title>
<path fill="none" stroke="black" d="M17.52,-162.25C19.36,-170.3 22.72,-181.12 28.56,-189.12 33.34,-195.69 40.04,-201.42 46.54,-206.01"/>
<polygon fill="black" stroke="black" points="44.79,-209.05 55.08,-211.55 48.59,-203.17 44.79,-209.05"/>
<text text-anchor="middle" x="31.56" y="-182.72" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge8" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M13.8,-138C11.71,-122.33 9.25,-92.97 15.56,-69.04 19.09,-55.65 26.37,-41.91 32.88,-31.4"/>
<polygon fill="black" stroke="black" points="35.99,-33.04 38.5,-22.75 30.12,-29.22 35.99,-33.04"/>
<text text-anchor="middle" x="18.56" y="-79.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="46.56" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="46.56" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M15.71,-137.99C16.2,-130.15 17.62,-119.57 21.56,-111.08 23.62,-106.62 26.59,-102.31 29.75,-98.44"/>
<polygon fill="black" stroke="black" points="32.59,-100.53 36.74,-90.78 27.41,-95.81 32.59,-100.53"/>
<text text-anchor="middle" x="24.56" y="-113.68" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 3&#45;&gt;0 -->
<g id="edge9" class="edge">
<title>3&#45;&gt;0</title>
<path fill="none" stroke="black" d="M56.51,-20.9C63.3,-26.24 71.79,-33.81 77.56,-42.04 84.98,-52.66 85.32,-56.5 88.56,-69.04 101.9,-120.73 110.36,-139.16 91.56,-189.12 89.85,-193.65 87.13,-197.99 84.12,-201.86"/>
<polygon fill="black" stroke="black" points="81.35,-199.7 77.35,-209.51 86.6,-204.34 81.35,-199.7"/>
<text text-anchor="middle" x="103.56" y="-113.68" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge11" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M35.34,-21.65C24.42,-31.68 7.94,-49.41 2.56,-69.04 -2.94,-89.09 2.23,-112.65 7.59,-129"/>
<polygon fill="black" stroke="black" points="4.36,-130.36 11.03,-138.59 10.95,-127.99 4.36,-130.36"/>
<text text-anchor="middle" x="5.56" y="-79.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge10" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M45.73,-24.33C45.87,-33.76 46.07,-47.46 46.24,-58.91"/>
<polygon fill="black" stroke="black" points="42.74,-59.05 46.39,-68.99 49.74,-58.94 42.74,-59.05"/>
<text text-anchor="middle" x="49.56" y="-44.64" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M48.3,-93.35C51.81,-116.11 59.79,-167.81 64.3,-197.02"/>
<polygon fill="black" stroke="black" points="60.85,-197.66 65.84,-207.01 67.77,-196.59 60.85,-197.66"/>
<text text-anchor="middle" x="62.56" y="-148.2" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M41.64,-92.69C37.1,-102.52 30.22,-117.39 24.67,-129.39"/>
<polygon fill="black" stroke="black" points="21.42,-128.08 20.4,-138.63 27.77,-131.02 21.42,-128.08"/>
<text text-anchor="middle" x="36.56" y="-113.68" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M41.53,-69.58C39.21,-64.18 36.72,-57.4 35.56,-51.04 34.48,-45.17 35.31,-38.87 36.86,-33.12"/>
<polygon fill="black" stroke="black" points="40.2,-34.17 40.2,-23.58 33.6,-31.86 40.2,-34.17"/>
<text text-anchor="middle" x="38.56" y="-44.64" font-family="Times,serif" font-size="8.00">0</text>
</g>
</g>
</svg>
</div>

    </td>
  </tr>
</table>
## Elementwise Multiplication

`emult` multiplies elements of two matrices, taking only the
intersection of common elements in both matrices, if an element is
missing from either the left or right side, it is ommited from the
result:
``` postgres-console
select print(a) as a, binaryop, print(b) as b, print(emult(a, b, binaryop)) as emult from test_fixture;
┌────────────────────┬─────────────┬────────────────────┬────────────────────┐
│         a          │  binaryop   │         b          │       emult        │
├────────────────────┼─────────────┼────────────────────┼────────────────────┤
│      0  1  2  3    │ times_int32 │      0  1  2  3    │      0  1  2  3    │
│    ────────────    │             │    ────────────    │    ────────────    │
│  0│        0  3    │             │  0│           4    │  0│          12    │
│  1│  2     1  0    │             │  1│        3  1    │  1│        3  0    │
│  2│  2  2          │             │  2│     2     4    │  2│     4          │
│  3│  2     1       │             │  3│     0  2       │  3│        2       │
│                    │             │                    │                    │
└────────────────────┴─────────────┴────────────────────┴────────────────────┘
(1 row)

```
Emult can also be accomplished with binary operators specific to
OneSparse.  Different binaryops are passed to emult to do different
elementwise operations:
``` postgres-console
select print(a &+ b) as "a &+ b", print(a &- b) as "a &- b", print(a &* b) as "a &* b", print(a &/ b) as "a &/ b" from test_fixture;
┌────────────────────┬────────────────────┬────────────────────┬────────────────────┐
│       a &+ b       │       a &- b       │       a &* b       │       a &/ b       │
├────────────────────┼────────────────────┼────────────────────┼────────────────────┤
│      0  1  2  3    │      0  1  2  3    │      0  1  2  3    │      0  1  2  3    │
│    ────────────    │    ────────────    │    ────────────    │    ────────────    │
│  0│           7    │  0│          -1    │  0│          12    │  0│           0    │
│  1│        4  1    │  1│       -2 -1    │  1│        3  0    │  1│        0  0    │
│  2│     4          │  2│     0          │  2│     4          │  2│     1          │
│  3│        3       │  3│       -1       │  3│        2       │  3│        0       │
│                    │                    │                    │                    │
└────────────────────┴────────────────────┴────────────────────┴────────────────────┘
(1 row)

```
From a graph standpoint, elementwise multiplication can be seen as
the intersection of two graphs, such that the result has edges that
are only present in both graphs.  The edges are combined with the
provided binary operator.
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="153pt" height="239pt"
 viewBox="0.00 0.00 153.31 239.17" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 235.17)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-235.17 149.31,-235.17 149.31,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="69.76" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="69.76" y="-217.25" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="98.76" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="98.76" y="-148.2" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M69.21,-207.05C69.22,-199.23 70.07,-188.65 73.76,-180.12 75.71,-175.61 78.62,-171.28 81.78,-167.4"/>
<polygon fill="black" stroke="black" points="84.61,-169.49 88.79,-159.75 79.45,-164.76 84.61,-169.49"/>
<text text-anchor="middle" x="76.76" y="-182.72" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="51.76" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="51.76" y="-10.12" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M55.27,-214.33C42.6,-210.23 24.72,-202.43 14.76,-189.12 -4.01,-164.06 3.88,-151.24 0.76,-120.08 -2.27,-89.91 10.52,-48.91 14.76,-42.04 18.79,-35.51 24.85,-29.86 30.91,-25.32"/>
<polygon fill="black" stroke="black" points="33.21,-27.99 39.55,-19.51 29.3,-22.19 33.21,-27.99"/>
<text text-anchor="middle" x="3.76" y="-113.68" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;0 -->
<g id="edge6" class="edge">
<title>2&#45;&gt;0</title>
<path fill="none" stroke="black" d="M94.2,-161.63C89.99,-171.38 83.6,-186.13 78.42,-198.12"/>
<polygon fill="black" stroke="black" points="75.17,-196.81 74.42,-207.37 81.6,-199.59 75.17,-196.81"/>
<text text-anchor="middle" x="89.76" y="-182.72" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="129.76" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="129.76" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M101.77,-138.2C104.08,-130.45 107.58,-119.91 111.76,-111.08 113.43,-107.55 115.43,-103.92 117.49,-100.47"/>
<polygon fill="black" stroke="black" points="120.48,-102.28 122.85,-91.95 114.56,-98.55 120.48,-102.28"/>
<text text-anchor="middle" x="114.76" y="-113.68" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;0 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;0</title>
<path fill="none" stroke="black" d="M52.73,-24.1C55.6,-56.87 64.1,-153.69 67.89,-196.9"/>
<polygon fill="black" stroke="black" points="64.43,-197.46 68.79,-207.12 71.4,-196.85 64.43,-197.46"/>
<text text-anchor="middle" x="64.76" y="-113.68" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge9" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M55.56,-24.03C63.45,-46.86 81.69,-99.69 91.76,-128.85"/>
<polygon fill="black" stroke="black" points="88.5,-130.12 95.07,-138.43 95.11,-127.83 88.5,-130.12"/>
<text text-anchor="middle" x="82.76" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M131.33,-93.05C133.1,-109.08 134.51,-139.45 123.76,-162.12 115.76,-178.98 100.68,-193.96 88.48,-204.18"/>
<polygon fill="black" stroke="black" points="86.25,-201.48 80.61,-210.45 90.61,-206.95 86.25,-201.48"/>
<text text-anchor="middle" x="132.76" y="-148.2" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M125.91,-92.87C123.08,-100.57 118.99,-111.09 114.76,-120.08 113.26,-123.26 111.57,-126.57 109.86,-129.77"/>
<polygon fill="black" stroke="black" points="106.65,-128.35 104.86,-138.79 112.77,-131.74 106.65,-128.35"/>
<text text-anchor="middle" x="121.76" y="-113.68" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M120.51,-71.08C112.33,-63.23 99.98,-51.62 88.76,-42.04 82.92,-37.06 76.37,-31.82 70.43,-27.18"/>
<polygon fill="black" stroke="black" points="72.37,-24.26 62.31,-20.93 68.09,-29.8 72.37,-24.26"/>
<text text-anchor="middle" x="101.76" y="-44.64" font-family="Times,serif" font-size="8.00">0</text>
</g>
</g>
</svg>
</div>

    </td>
    <td style="font-size: 30px; vertical-align: middle;">op</td>
    <td style="width: 30%;">
        <!-- Diagram B -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="105pt" height="239pt"
 viewBox="0.00 0.00 104.56 239.17" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 235.17)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-235.17 100.56,-235.17 100.56,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="56.56" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="56.56" y="-217.25" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node2" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="56.56" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="56.56" y="-148.2" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M56.56,-207.08C56.56,-197.72 56.56,-184.04 56.56,-172.55"/>
<polygon fill="black" stroke="black" points="60.06,-172.42 56.56,-162.42 53.06,-172.42 60.06,-172.42"/>
<text text-anchor="middle" x="59.56" y="-182.72" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1 -->
<g id="node3" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="15.56" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge6" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M45.16,-141.51C38.19,-136.28 29.67,-128.74 24.56,-120.08 21.61,-115.1 19.63,-109.22 18.3,-103.58"/>
<polygon fill="black" stroke="black" points="21.68,-102.59 16.45,-93.37 14.79,-103.83 21.68,-102.59"/>
<text text-anchor="middle" x="27.56" y="-113.68" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node4" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="56.56" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="56.56" y="-10.12" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M56.56,-137.97C56.56,-115.34 56.56,-63.66 56.56,-34.34"/>
<polygon fill="black" stroke="black" points="60.06,-34.31 56.56,-24.31 53.06,-34.31 60.06,-34.31"/>
<text text-anchor="middle" x="59.56" y="-79.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M21.84,-92.34C28,-102.42 37.54,-118.01 45.06,-130.31"/>
<polygon fill="black" stroke="black" points="42.13,-132.22 50.33,-138.92 48.1,-128.56 42.13,-132.22"/>
<text text-anchor="middle" x="41.56" y="-113.68" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M14.9,-68.72C14.93,-60.78 15.99,-50.17 20.56,-42.04 24.2,-35.55 29.93,-29.96 35.75,-25.46"/>
<polygon fill="black" stroke="black" points="37.88,-28.24 44.12,-19.68 33.9,-22.48 37.88,-28.24"/>
<text text-anchor="middle" x="23.56" y="-44.64" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge5" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M66.58,-21.3C72.15,-26.58 78.55,-33.94 81.56,-42.04 93.63,-74.56 93.63,-87.57 81.56,-120.08 79.77,-124.9 76.79,-129.44 73.49,-133.45"/>
<polygon fill="black" stroke="black" points="70.87,-131.14 66.58,-140.82 75.97,-135.92 70.87,-131.14"/>
<text text-anchor="middle" x="93.56" y="-79.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M50.27,-23.3C44.11,-33.38 34.57,-48.97 27.05,-61.27"/>
<polygon fill="black" stroke="black" points="24.01,-59.52 21.78,-69.88 29.99,-63.18 24.01,-59.52"/>
<text text-anchor="middle" x="41.56" y="-44.64" font-family="Times,serif" font-size="8.00">2</text>
</g>
</g>
</svg>
</div>

    </td>
    <td style="font-size: 30px; vertical-align: middle;">=</td>
    <td style="width: 30%;">
        <!-- Diagram C -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="68pt" height="239pt"
 viewBox="0.00 0.00 68.11 239.17" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 235.17)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-235.17 64.11,-235.17 64.11,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="41.56" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="41.56" y="-217.25" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node2" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="41.56" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="41.56" y="-148.2" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M41.56,-207.08C41.56,-197.72 41.56,-184.04 41.56,-172.55"/>
<polygon fill="black" stroke="black" points="45.06,-172.42 41.56,-162.42 38.06,-172.42 45.06,-172.42"/>
<text text-anchor="middle" x="47.06" y="-182.72" font-family="Times,serif" font-size="8.00">12</text>
</g>
<!-- 2 -->
<g id="node4" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="15.56" y="-79.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge5" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M37.38,-138.33C33.65,-128.71 28.08,-114.36 23.51,-102.56"/>
<polygon fill="black" stroke="black" points="26.69,-101.09 19.81,-93.03 20.16,-103.62 26.69,-101.09"/>
<text text-anchor="middle" x="33.56" y="-113.68" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1 -->
<g id="node3" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="44.56" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="44.56" y="-10.12" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M44.31,-24.31C43.81,-46.97 42.68,-98.31 42.03,-127.59"/>
<polygon fill="black" stroke="black" points="38.52,-127.89 41.8,-137.97 45.52,-128.05 38.52,-127.89"/>
<text text-anchor="middle" x="46.56" y="-79.16" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M40,-23.54C35.79,-33.29 29.4,-48.05 24.22,-60.04"/>
<polygon fill="black" stroke="black" points="20.97,-58.72 20.22,-69.29 27.4,-61.5 20.97,-58.72"/>
<text text-anchor="middle" x="35.56" y="-44.64" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M15.01,-68.97C15.02,-61.14 15.87,-50.57 19.56,-42.04 21.51,-37.52 24.42,-33.19 27.58,-29.32"/>
<polygon fill="black" stroke="black" points="30.41,-31.41 34.59,-21.67 25.25,-26.68 30.41,-31.41"/>
<text text-anchor="middle" x="22.56" y="-44.64" font-family="Times,serif" font-size="8.00">4</text>
</g>
</g>
</svg>
</div>

    </td>
  </tr>
</table>
## Elementwise Union

`eunion` is like `eadd` but differs in how the binary op is
applied. A pair of scalars, `alpha` and `beta` define the inputs to
the operator when entries are present in one matrix but not the
other.
``` postgres-console
select print(a) as a, binaryop, print(b) as b, print(eunion(a, 3::scalar, b, 4::scalar, binaryop)) as eunion from test_fixture;
┌────────────────────┬─────────────┬────────────────────┬────────────────────┐
│         a          │  binaryop   │         b          │       eunion       │
├────────────────────┼─────────────┼────────────────────┼────────────────────┤
│      0  1  2  3    │ times_int32 │      0  1  2  3    │      0  1  2  3    │
│    ────────────    │             │    ────────────    │    ────────────    │
│  0│        0  3    │             │  0│           4    │  0│        0 12    │
│  1│  2     1  0    │             │  1│        3  1    │  1│  8     3  0    │
│  2│  2  2          │             │  2│     2     4    │  2│  8  4    12    │
│  3│  2     1       │             │  3│     0  2       │  3│  8  0  2       │
│                    │             │                    │                    │
└────────────────────┴─────────────┴────────────────────┴────────────────────┘
(1 row)

```
From a graph standpoint, elementwise union is very similar to
`eadd()`, and can be seen as the merging ("union") of two graphs,
such that the result has edges from both graphs.  Any edges that
occur in both graphs are merged with the provided binary operator.
If an edge occurs in a but not in b, it is combined with the
scalar `alpha`, if the edge occurs in the b but not in a, then
the edge is combined with scalar `beta`.
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="153pt" height="239pt"
 viewBox="0.00 0.00 153.31 239.17" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 235.17)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-235.17 149.31,-235.17 149.31,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="69.76" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="69.76" y="-217.25" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="98.76" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="98.76" y="-148.2" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M69.21,-207.05C69.22,-199.23 70.07,-188.65 73.76,-180.12 75.71,-175.61 78.62,-171.28 81.78,-167.4"/>
<polygon fill="black" stroke="black" points="84.61,-169.49 88.79,-159.75 79.45,-164.76 84.61,-169.49"/>
<text text-anchor="middle" x="76.76" y="-182.72" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="51.76" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="51.76" y="-10.12" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M55.27,-214.33C42.6,-210.23 24.72,-202.43 14.76,-189.12 -4.01,-164.06 3.88,-151.24 0.76,-120.08 -2.27,-89.91 10.52,-48.91 14.76,-42.04 18.79,-35.51 24.85,-29.86 30.91,-25.32"/>
<polygon fill="black" stroke="black" points="33.21,-27.99 39.55,-19.51 29.3,-22.19 33.21,-27.99"/>
<text text-anchor="middle" x="3.76" y="-113.68" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;0 -->
<g id="edge6" class="edge">
<title>2&#45;&gt;0</title>
<path fill="none" stroke="black" d="M94.2,-161.63C89.99,-171.38 83.6,-186.13 78.42,-198.12"/>
<polygon fill="black" stroke="black" points="75.17,-196.81 74.42,-207.37 81.6,-199.59 75.17,-196.81"/>
<text text-anchor="middle" x="89.76" y="-182.72" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="129.76" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="129.76" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M101.77,-138.2C104.08,-130.45 107.58,-119.91 111.76,-111.08 113.43,-107.55 115.43,-103.92 117.49,-100.47"/>
<polygon fill="black" stroke="black" points="120.48,-102.28 122.85,-91.95 114.56,-98.55 120.48,-102.28"/>
<text text-anchor="middle" x="114.76" y="-113.68" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;0 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;0</title>
<path fill="none" stroke="black" d="M52.73,-24.1C55.6,-56.87 64.1,-153.69 67.89,-196.9"/>
<polygon fill="black" stroke="black" points="64.43,-197.46 68.79,-207.12 71.4,-196.85 64.43,-197.46"/>
<text text-anchor="middle" x="64.76" y="-113.68" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge9" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M55.56,-24.03C63.45,-46.86 81.69,-99.69 91.76,-128.85"/>
<polygon fill="black" stroke="black" points="88.5,-130.12 95.07,-138.43 95.11,-127.83 88.5,-130.12"/>
<text text-anchor="middle" x="82.76" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M131.33,-93.05C133.1,-109.08 134.51,-139.45 123.76,-162.12 115.76,-178.98 100.68,-193.96 88.48,-204.18"/>
<polygon fill="black" stroke="black" points="86.25,-201.48 80.61,-210.45 90.61,-206.95 86.25,-201.48"/>
<text text-anchor="middle" x="132.76" y="-148.2" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M125.91,-92.87C123.08,-100.57 118.99,-111.09 114.76,-120.08 113.26,-123.26 111.57,-126.57 109.86,-129.77"/>
<polygon fill="black" stroke="black" points="106.65,-128.35 104.86,-138.79 112.77,-131.74 106.65,-128.35"/>
<text text-anchor="middle" x="121.76" y="-113.68" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M120.51,-71.08C112.33,-63.23 99.98,-51.62 88.76,-42.04 82.92,-37.06 76.37,-31.82 70.43,-27.18"/>
<polygon fill="black" stroke="black" points="72.37,-24.26 62.31,-20.93 68.09,-29.8 72.37,-24.26"/>
<text text-anchor="middle" x="101.76" y="-44.64" font-family="Times,serif" font-size="8.00">0</text>
</g>
</g>
</svg>
</div>

    </td>
    <td style="font-size: 30px; vertical-align: middle;">op</td>
    <td style="width: 30%;">
        <!-- Diagram B -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="105pt" height="239pt"
 viewBox="0.00 0.00 104.56 239.17" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 235.17)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-235.17 100.56,-235.17 100.56,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="56.56" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="56.56" y="-217.25" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node2" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="56.56" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="56.56" y="-148.2" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M56.56,-207.08C56.56,-197.72 56.56,-184.04 56.56,-172.55"/>
<polygon fill="black" stroke="black" points="60.06,-172.42 56.56,-162.42 53.06,-172.42 60.06,-172.42"/>
<text text-anchor="middle" x="59.56" y="-182.72" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1 -->
<g id="node3" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="15.56" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge6" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M45.16,-141.51C38.19,-136.28 29.67,-128.74 24.56,-120.08 21.61,-115.1 19.63,-109.22 18.3,-103.58"/>
<polygon fill="black" stroke="black" points="21.68,-102.59 16.45,-93.37 14.79,-103.83 21.68,-102.59"/>
<text text-anchor="middle" x="27.56" y="-113.68" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node4" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="56.56" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="56.56" y="-10.12" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M56.56,-137.97C56.56,-115.34 56.56,-63.66 56.56,-34.34"/>
<polygon fill="black" stroke="black" points="60.06,-34.31 56.56,-24.31 53.06,-34.31 60.06,-34.31"/>
<text text-anchor="middle" x="59.56" y="-79.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M21.84,-92.34C28,-102.42 37.54,-118.01 45.06,-130.31"/>
<polygon fill="black" stroke="black" points="42.13,-132.22 50.33,-138.92 48.1,-128.56 42.13,-132.22"/>
<text text-anchor="middle" x="41.56" y="-113.68" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M14.9,-68.72C14.93,-60.78 15.99,-50.17 20.56,-42.04 24.2,-35.55 29.93,-29.96 35.75,-25.46"/>
<polygon fill="black" stroke="black" points="37.88,-28.24 44.12,-19.68 33.9,-22.48 37.88,-28.24"/>
<text text-anchor="middle" x="23.56" y="-44.64" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge5" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M66.58,-21.3C72.15,-26.58 78.55,-33.94 81.56,-42.04 93.63,-74.56 93.63,-87.57 81.56,-120.08 79.77,-124.9 76.79,-129.44 73.49,-133.45"/>
<polygon fill="black" stroke="black" points="70.87,-131.14 66.58,-140.82 75.97,-135.92 70.87,-131.14"/>
<text text-anchor="middle" x="93.56" y="-79.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M50.27,-23.3C44.11,-33.38 34.57,-48.97 27.05,-61.27"/>
<polygon fill="black" stroke="black" points="24.01,-59.52 21.78,-69.88 29.99,-63.18 24.01,-59.52"/>
<text text-anchor="middle" x="41.56" y="-44.64" font-family="Times,serif" font-size="8.00">2</text>
</g>
</g>
</svg>
</div>

    </td>
    <td style="font-size: 30px; vertical-align: middle;">=</td>
    <td style="width: 30%;">
        <!-- Diagram C -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="120pt" height="239pt"
 viewBox="0.00 0.00 120.25 239.17" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 235.17)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-235.17 116.25,-235.17 116.25,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="72.25" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="72.25" y="-217.25" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="20.25" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="20.25" y="-148.2" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M64.8,-208.54C56.77,-198.19 43.81,-181.48 33.93,-168.74"/>
<polygon fill="black" stroke="black" points="36.58,-166.45 27.68,-160.69 31.05,-170.74 36.58,-166.45"/>
<text text-anchor="middle" x="53.25" y="-182.72" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="50.25" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="50.25" y="-10.12" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M76.46,-207.3C78.33,-201.93 80.32,-195.28 81.25,-189.12 81.85,-185.17 81.34,-184.12 81.25,-180.12 80.14,-130.72 87.98,-117.05 76.25,-69.04 73.12,-56.25 67.01,-42.85 61.5,-32.36"/>
<polygon fill="black" stroke="black" points="64.41,-30.38 56.52,-23.31 58.28,-33.76 64.41,-30.38"/>
<text text-anchor="middle" x="88.75" y="-113.68" font-family="Times,serif" font-size="8.00">12</text>
</g>
<!-- 2&#45;&gt;0 -->
<g id="edge6" class="edge">
<title>2&#45;&gt;0</title>
<path fill="none" stroke="black" d="M22.21,-162.25C24.05,-170.3 27.41,-181.12 33.25,-189.12 38.04,-195.69 44.74,-201.42 51.23,-206.01"/>
<polygon fill="black" stroke="black" points="49.48,-209.05 59.77,-211.55 53.29,-203.17 49.48,-209.05"/>
<text text-anchor="middle" x="36.25" y="-182.72" font-family="Times,serif" font-size="8.00">8</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge8" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M17.3,-138.24C13.56,-122.6 8.38,-93.01 15.25,-69.04 19.3,-54.92 28.06,-40.9 35.8,-30.42"/>
<polygon fill="black" stroke="black" points="38.57,-32.56 41.94,-22.52 33.04,-28.27 38.57,-32.56"/>
<text text-anchor="middle" x="20.75" y="-79.16" font-family="Times,serif" font-size="8.00">12</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="51.25" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="51.25" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M20.4,-137.99C20.89,-130.15 22.32,-119.57 26.25,-111.08 28.32,-106.62 31.28,-102.31 34.44,-98.44"/>
<polygon fill="black" stroke="black" points="37.28,-100.53 41.43,-90.78 32.11,-95.81 37.28,-100.53"/>
<text text-anchor="middle" x="29.25" y="-113.68" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 3&#45;&gt;0 -->
<g id="edge9" class="edge">
<title>3&#45;&gt;0</title>
<path fill="none" stroke="black" d="M61.2,-20.9C67.99,-26.24 76.49,-33.81 82.25,-42.04 89.68,-52.66 90.01,-56.5 93.25,-69.04 106.59,-120.73 115.05,-139.16 96.25,-189.12 94.54,-193.65 91.82,-197.99 88.81,-201.86"/>
<polygon fill="black" stroke="black" points="86.05,-199.7 82.04,-209.51 91.29,-204.34 86.05,-199.7"/>
<text text-anchor="middle" x="109.25" y="-113.68" font-family="Times,serif" font-size="8.00">8</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge11" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M39.25,-21C27.02,-30.7 8.21,-48.37 2.25,-69.04 -3.64,-89.45 3.43,-113.23 10.37,-129.55"/>
<polygon fill="black" stroke="black" points="7.19,-131.02 14.55,-138.64 13.55,-128.09 7.19,-131.02"/>
<text text-anchor="middle" x="5.25" y="-79.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge10" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M50.42,-24.33C50.56,-33.76 50.76,-47.46 50.93,-58.91"/>
<polygon fill="black" stroke="black" points="47.43,-59.05 51.08,-68.99 54.43,-58.94 47.43,-59.05"/>
<text text-anchor="middle" x="54.25" y="-44.64" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M52.99,-93.35C56.5,-116.11 64.48,-167.81 68.99,-197.02"/>
<polygon fill="black" stroke="black" points="65.54,-197.66 70.53,-207.01 72.46,-196.59 65.54,-197.66"/>
<text text-anchor="middle" x="67.25" y="-148.2" font-family="Times,serif" font-size="8.00">8</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M46.34,-92.69C41.79,-102.52 34.91,-117.39 29.36,-129.39"/>
<polygon fill="black" stroke="black" points="26.11,-128.08 25.09,-138.63 32.47,-131.02 26.11,-128.08"/>
<text text-anchor="middle" x="41.25" y="-113.68" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M46.22,-69.58C43.9,-64.18 41.41,-57.4 40.25,-51.04 39.17,-45.17 40,-38.87 41.56,-33.12"/>
<polygon fill="black" stroke="black" points="44.89,-34.17 44.89,-23.58 38.29,-31.86 44.89,-34.17"/>
<text text-anchor="middle" x="43.25" y="-44.64" font-family="Times,serif" font-size="8.00">0</text>
</g>
</g>
</svg>
</div>

    </td>
  </tr>
</table>
## Reduction

The entire matrix can be reduced to a scalar value:
``` postgres-console
select print(a) as a, 'plus_monoid_int32' as monoid, reduce_scalar(a) from test_fixture;
┌────────────────────┬───────────────────┬───────────────┐
│         a          │      monoid       │ reduce_scalar │
├────────────────────┼───────────────────┼───────────────┤
│      0  1  2  3    │ plus_monoid_int32 │ int32:13      │
│    ────────────    │                   │               │
│  0│        0  3    │                   │               │
│  1│  2     1  0    │                   │               │
│  2│  2  2          │                   │               │
│  3│  2     1       │                   │               │
│                    │                   │               │
└────────────────────┴───────────────────┴───────────────┘
(1 row)

```
The entire matrix can be reduced to a scalar value with a provided
monoid that changes the reduction operation:
``` postgres-console
select print(a) as a, 'min_monoid_int32' as monoid, reduce_scalar(a, 'min_monoid_int32') from test_fixture;
┌────────────────────┬──────────────────┬───────────────┐
│         a          │      monoid      │ reduce_scalar │
├────────────────────┼──────────────────┼───────────────┤
│      0  1  2  3    │ min_monoid_int32 │ int32:0       │
│    ────────────    │                  │               │
│  0│        0  3    │                  │               │
│  1│  2     1  0    │                  │               │
│  2│  2  2          │                  │               │
│  3│  2     1       │                  │               │
│                    │                  │               │
└────────────────────┴──────────────────┴───────────────┘
(1 row)

```
The matrix can also be reduced to a column vector:
``` postgres-console
select print(a) as a, 'plus_monoid_int32' as monoid, print(reduce_vector(a)) as reduce_vector from test_fixture;
┌────────────────────┬───────────────────┬───────────────┐
│         a          │      monoid       │ reduce_vector │
├────────────────────┼───────────────────┼───────────────┤
│      0  1  2  3    │ plus_monoid_int32 │               │
│    ────────────    │                   │    ───        │
│  0│        0  3    │                   │  0│  3        │
│  1│  2     1  0    │                   │  1│  3        │
│  2│  2  2          │                   │  2│  4        │
│  3│  2     1       │                   │  3│  3        │
│                    │                   │               │
└────────────────────┴───────────────────┴───────────────┘
(1 row)

```
To reduce a row vector, specify that the input should be transposed
with the descriptor `t0`:
``` postgres-console
select print(a) as a, 'plus_monoid_int32' as monoid, print(reduce_vector(a, descriptor=>'t0')) as transpose_reduce_vector from test_fixture;
┌────────────────────┬───────────────────┬─────────────────────────┐
│         a          │      monoid       │ transpose_reduce_vector │
├────────────────────┼───────────────────┼─────────────────────────┤
│      0  1  2  3    │ plus_monoid_int32 │                         │
│    ────────────    │                   │    ───                  │
│  0│        0  3    │                   │  0│  6                  │
│  1│  2     1  0    │                   │  1│  2                  │
│  2│  2  2          │                   │  2│  2                  │
│  3│  2     1       │                   │  3│  3                  │
│                    │                   │                         │
└────────────────────┴───────────────────┴─────────────────────────┘
(1 row)

```
## Matrix Matrix Multiplication

Matrix Multiplication is the heart of linear algebra.  All matrix
multiplication happens over a semiring.  For the most common form
of matrix multiplication, the outer opperation is to multiply
coresponding elements with the "times" operator and then reduce
those products with the "plus" operator.  This is called the
`plus_times` semiring:
``` postgres-console
select print(a) as a, semiring, print(b) as b, print(mxm(a, b)) as mxm from test_fixture;
┌────────────────────┬──────────────────┬────────────────────┬────────────────────┐
│         a          │     semiring     │         b          │        mxm         │
├────────────────────┼──────────────────┼────────────────────┼────────────────────┤
│      0  1  2  3    │ plus_times_int32 │      0  1  2  3    │      0  1  2  3    │
│    ────────────    │                  │    ────────────    │    ────────────    │
│  0│        0  3    │                  │  0│           4    │  0│     0  6  0    │
│  1│  2     1  0    │                  │  1│        3  1    │  1│     2  0 12    │
│  2│  2  2          │                  │  2│     2     4    │  2│        6 10    │
│  3│  2     1       │                  │  3│     0  2       │  3│     2    12    │
│                    │                  │                    │                    │
└────────────────────┴──────────────────┴────────────────────┴────────────────────┘
(1 row)

```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="153pt" height="239pt"
 viewBox="0.00 0.00 153.31 239.17" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 235.17)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-235.17 149.31,-235.17 149.31,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="69.76" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="69.76" y="-217.25" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="98.76" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="98.76" y="-148.2" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M69.21,-207.05C69.22,-199.23 70.07,-188.65 73.76,-180.12 75.71,-175.61 78.62,-171.28 81.78,-167.4"/>
<polygon fill="black" stroke="black" points="84.61,-169.49 88.79,-159.75 79.45,-164.76 84.61,-169.49"/>
<text text-anchor="middle" x="76.76" y="-182.72" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="51.76" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="51.76" y="-10.12" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M55.27,-214.33C42.6,-210.23 24.72,-202.43 14.76,-189.12 -4.01,-164.06 3.88,-151.24 0.76,-120.08 -2.27,-89.91 10.52,-48.91 14.76,-42.04 18.79,-35.51 24.85,-29.86 30.91,-25.32"/>
<polygon fill="black" stroke="black" points="33.21,-27.99 39.55,-19.51 29.3,-22.19 33.21,-27.99"/>
<text text-anchor="middle" x="3.76" y="-113.68" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;0 -->
<g id="edge6" class="edge">
<title>2&#45;&gt;0</title>
<path fill="none" stroke="black" d="M94.2,-161.63C89.99,-171.38 83.6,-186.13 78.42,-198.12"/>
<polygon fill="black" stroke="black" points="75.17,-196.81 74.42,-207.37 81.6,-199.59 75.17,-196.81"/>
<text text-anchor="middle" x="89.76" y="-182.72" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="129.76" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="129.76" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M101.77,-138.2C104.08,-130.45 107.58,-119.91 111.76,-111.08 113.43,-107.55 115.43,-103.92 117.49,-100.47"/>
<polygon fill="black" stroke="black" points="120.48,-102.28 122.85,-91.95 114.56,-98.55 120.48,-102.28"/>
<text text-anchor="middle" x="114.76" y="-113.68" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;0 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;0</title>
<path fill="none" stroke="black" d="M52.73,-24.1C55.6,-56.87 64.1,-153.69 67.89,-196.9"/>
<polygon fill="black" stroke="black" points="64.43,-197.46 68.79,-207.12 71.4,-196.85 64.43,-197.46"/>
<text text-anchor="middle" x="64.76" y="-113.68" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge9" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M55.56,-24.03C63.45,-46.86 81.69,-99.69 91.76,-128.85"/>
<polygon fill="black" stroke="black" points="88.5,-130.12 95.07,-138.43 95.11,-127.83 88.5,-130.12"/>
<text text-anchor="middle" x="82.76" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M131.33,-93.05C133.1,-109.08 134.51,-139.45 123.76,-162.12 115.76,-178.98 100.68,-193.96 88.48,-204.18"/>
<polygon fill="black" stroke="black" points="86.25,-201.48 80.61,-210.45 90.61,-206.95 86.25,-201.48"/>
<text text-anchor="middle" x="132.76" y="-148.2" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M125.91,-92.87C123.08,-100.57 118.99,-111.09 114.76,-120.08 113.26,-123.26 111.57,-126.57 109.86,-129.77"/>
<polygon fill="black" stroke="black" points="106.65,-128.35 104.86,-138.79 112.77,-131.74 106.65,-128.35"/>
<text text-anchor="middle" x="121.76" y="-113.68" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M120.51,-71.08C112.33,-63.23 99.98,-51.62 88.76,-42.04 82.92,-37.06 76.37,-31.82 70.43,-27.18"/>
<polygon fill="black" stroke="black" points="72.37,-24.26 62.31,-20.93 68.09,-29.8 72.37,-24.26"/>
<text text-anchor="middle" x="101.76" y="-44.64" font-family="Times,serif" font-size="8.00">0</text>
</g>
</g>
</svg>
</div>

    </td>
    <td style="font-size: 30px; vertical-align: middle;">op</td>
    <td style="width: 30%;">
        <!-- Diagram B -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="105pt" height="239pt"
 viewBox="0.00 0.00 104.56 239.17" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 235.17)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-235.17 100.56,-235.17 100.56,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="56.56" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="56.56" y="-217.25" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node2" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="56.56" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="56.56" y="-148.2" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M56.56,-207.08C56.56,-197.72 56.56,-184.04 56.56,-172.55"/>
<polygon fill="black" stroke="black" points="60.06,-172.42 56.56,-162.42 53.06,-172.42 60.06,-172.42"/>
<text text-anchor="middle" x="59.56" y="-182.72" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1 -->
<g id="node3" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="15.56" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge6" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M45.16,-141.51C38.19,-136.28 29.67,-128.74 24.56,-120.08 21.61,-115.1 19.63,-109.22 18.3,-103.58"/>
<polygon fill="black" stroke="black" points="21.68,-102.59 16.45,-93.37 14.79,-103.83 21.68,-102.59"/>
<text text-anchor="middle" x="27.56" y="-113.68" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node4" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="56.56" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="56.56" y="-10.12" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M56.56,-137.97C56.56,-115.34 56.56,-63.66 56.56,-34.34"/>
<polygon fill="black" stroke="black" points="60.06,-34.31 56.56,-24.31 53.06,-34.31 60.06,-34.31"/>
<text text-anchor="middle" x="59.56" y="-79.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M21.84,-92.34C28,-102.42 37.54,-118.01 45.06,-130.31"/>
<polygon fill="black" stroke="black" points="42.13,-132.22 50.33,-138.92 48.1,-128.56 42.13,-132.22"/>
<text text-anchor="middle" x="41.56" y="-113.68" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M14.9,-68.72C14.93,-60.78 15.99,-50.17 20.56,-42.04 24.2,-35.55 29.93,-29.96 35.75,-25.46"/>
<polygon fill="black" stroke="black" points="37.88,-28.24 44.12,-19.68 33.9,-22.48 37.88,-28.24"/>
<text text-anchor="middle" x="23.56" y="-44.64" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge5" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M66.58,-21.3C72.15,-26.58 78.55,-33.94 81.56,-42.04 93.63,-74.56 93.63,-87.57 81.56,-120.08 79.77,-124.9 76.79,-129.44 73.49,-133.45"/>
<polygon fill="black" stroke="black" points="70.87,-131.14 66.58,-140.82 75.97,-135.92 70.87,-131.14"/>
<text text-anchor="middle" x="93.56" y="-79.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M50.27,-23.3C44.11,-33.38 34.57,-48.97 27.05,-61.27"/>
<polygon fill="black" stroke="black" points="24.01,-59.52 21.78,-69.88 29.99,-63.18 24.01,-59.52"/>
<text text-anchor="middle" x="41.56" y="-44.64" font-family="Times,serif" font-size="8.00">2</text>
</g>
</g>
</svg>
</div>

    </td>
    <td style="font-size: 30px; vertical-align: middle;">=</td>
    <td style="width: 30%;">
        <!-- Diagram C -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="137pt" height="239pt"
 viewBox="0.00 0.00 136.89 239.17" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 235.17)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-235.17 132.89,-235.17 132.89,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="27.33" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="27.33" y="-217.25" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="27.33" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="27.33" y="-148.2" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 0&#45;&gt;1 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;1</title>
<path fill="none" stroke="black" d="M27.33,-207.08C27.33,-197.72 27.33,-184.04 27.33,-172.55"/>
<polygon fill="black" stroke="black" points="30.83,-172.42 27.33,-162.42 23.83,-172.42 30.83,-172.42"/>
<text text-anchor="middle" x="30.33" y="-182.72" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node3" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="89.33" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="89.33" y="-79.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M37.44,-209.8C48.87,-199.75 67.09,-181.77 76.33,-162.12 85.05,-143.59 88.06,-120.28 89.03,-103.59"/>
<polygon fill="black" stroke="black" points="92.54,-103.37 89.42,-93.24 85.55,-103.1 92.54,-103.37"/>
<text text-anchor="middle" x="87.33" y="-148.2" font-family="Times,serif" font-size="8.00">6</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="25.33" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="25.33" y="-10.12" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge3" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M21.15,-207.86C14.98,-196.95 5.93,-179 2.33,-162.12 -4.31,-130.93 4.9,-80.02 14.33,-42.04 15.06,-39.1 16,-36.05 17.02,-33.08"/>
<polygon fill="black" stroke="black" points="20.36,-34.17 20.64,-23.58 13.81,-31.68 20.36,-34.17"/>
<text text-anchor="middle" x="5.33" y="-113.68" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1&#45;&gt;1 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;1</title>
<path fill="none" stroke="black" d="M41.12,-155.79C51.02,-157.64 60.89,-155.75 60.89,-150.1 60.89,-146.49 56.84,-144.41 51.33,-143.87"/>
<polygon fill="black" stroke="black" points="50.92,-140.39 41.12,-144.42 51.29,-147.38 50.92,-140.39"/>
<text text-anchor="middle" x="63.89" y="-148.2" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M37.62,-140.95C44.38,-135.39 53.25,-127.67 60.33,-120.08 66.01,-113.99 71.7,-106.83 76.51,-100.38"/>
<polygon fill="black" stroke="black" points="79.42,-102.33 82.45,-92.18 73.75,-98.22 79.42,-102.33"/>
<text text-anchor="middle" x="71.33" y="-113.68" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge6" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M24.5,-138.04C21.78,-126.8 17.87,-108.87 16.33,-93.08 14.37,-72.95 17.5,-50.01 20.59,-33.86"/>
<polygon fill="black" stroke="black" points="24.06,-34.39 22.66,-23.89 17.2,-32.97 24.06,-34.39"/>
<text text-anchor="middle" x="21.83" y="-79.16" font-family="Times,serif" font-size="8.00">12</text>
</g>
<!-- 2&#45;&gt;2 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;2</title>
<path fill="none" stroke="black" d="M103.12,-86.75C113.02,-88.6 122.89,-86.71 122.89,-81.06 122.89,-77.45 118.84,-75.37 113.33,-74.83"/>
<polygon fill="black" stroke="black" points="112.92,-71.35 103.12,-75.37 113.29,-78.34 112.92,-71.35"/>
<text text-anchor="middle" x="125.89" y="-79.16" font-family="Times,serif" font-size="8.00">6</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge8" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M83.51,-69.73C78.69,-61.69 71.37,-50.51 63.33,-42.04 57.96,-36.38 51.4,-30.96 45.26,-26.37"/>
<polygon fill="black" stroke="black" points="46.92,-23.25 36.74,-20.3 42.86,-28.95 46.92,-23.25"/>
<text text-anchor="middle" x="76.83" y="-44.64" font-family="Times,serif" font-size="8.00">10</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge9" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M29.04,-23.91C30.69,-29.29 32.46,-35.94 33.33,-42.04 47.58,-141.92 36.37,-102.93 34.33,-120.08 34.02,-122.74 33.54,-125.51 32.99,-128.23"/>
<polygon fill="black" stroke="black" points="29.53,-127.65 30.61,-138.19 36.34,-129.28 29.53,-127.65"/>
<text text-anchor="middle" x="43.33" y="-79.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge10" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M39.12,-17.71C49.02,-19.56 58.89,-17.67 58.89,-12.02 58.89,-8.4 54.84,-6.33 49.33,-5.79"/>
<polygon fill="black" stroke="black" points="48.92,-2.3 39.12,-6.33 49.29,-9.29 48.92,-2.3"/>
<text text-anchor="middle" x="64.39" y="-10.12" font-family="Times,serif" font-size="8.00">12</text>
</g>
</g>
</svg>
</div>

    </td>
  </tr>
</table>
AxB can also be done with the `@` operator, mimicking the Python
syntax.  The default semiring for numeric types is `plus_times`.
``` postgres-console
select print(a) as a, '@' as "@", print(b) as b, print(a @ b) as mxm from test_fixture;
┌────────────────────┬───┬────────────────────┬────────────────────┐
│         a          │ @ │         b          │        mxm         │
├────────────────────┼───┼────────────────────┼────────────────────┤
│      0  1  2  3    │ @ │      0  1  2  3    │      0  1  2  3    │
│    ────────────    │   │    ────────────    │    ────────────    │
│  0│        0  3    │   │  0│           4    │  0│     0  6  0    │
│  1│  2     1  0    │   │  1│        3  1    │  1│     2  0 12    │
│  2│  2  2          │   │  2│     2     4    │  2│        6 10    │
│  3│  2     1       │   │  3│     0  2       │  3│     2    12    │
│                    │   │                    │                    │
└────────────────────┴───┴────────────────────┴────────────────────┘
(1 row)

```
## Matrix Vector Multiplication

Matrices can be multipled by vectors on the right taking the linear
combination of the matrices columns using the vectors elements as
coefficients:
``` postgres-console
select print(a) as a, '@' as "@", semiring, print(u) as u, print(mxv(a, u)) as mxv from test_fixture;
┌────────────────────┬───┬──────────────────┬───────────┬───────────┐
│         a          │ @ │     semiring     │     u     │    mxv    │
├────────────────────┼───┼──────────────────┼───────────┼───────────┤
│      0  1  2  3    │ @ │ plus_times_int32 │           │           │
│    ────────────    │   │                  │    ───    │    ───    │
│  0│        0  3    │   │                  │  0│       │  0│       │
│  1│  2     1  0    │   │                  │  1│  2    │  1│       │
│  2│  2  2          │   │                  │  2│       │  2│  4    │
│  3│  2     1       │   │                  │  3│       │  3│       │
│                    │   │                  │           │           │
└────────────────────┴───┴──────────────────┴───────────┴───────────┘
(1 row)

```
From a graph standpoint, matrix vector multiplication is used to
"pull" back to adjacent nodes from their incoming edges.  When
iterated, it forms the basis for working back along incoming links.
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="153pt" height="239pt"
 viewBox="0.00 0.00 153.31 239.17" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 235.17)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-235.17 149.31,-235.17 149.31,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="69.76" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="69.76" y="-217.25" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="98.76" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="98.76" y="-148.2" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M69.21,-207.05C69.22,-199.23 70.07,-188.65 73.76,-180.12 75.71,-175.61 78.62,-171.28 81.78,-167.4"/>
<polygon fill="black" stroke="black" points="84.61,-169.49 88.79,-159.75 79.45,-164.76 84.61,-169.49"/>
<text text-anchor="middle" x="76.76" y="-182.72" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="51.76" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="51.76" y="-10.12" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M55.27,-214.33C42.6,-210.23 24.72,-202.43 14.76,-189.12 -4.01,-164.06 3.88,-151.24 0.76,-120.08 -2.27,-89.91 10.52,-48.91 14.76,-42.04 18.79,-35.51 24.85,-29.86 30.91,-25.32"/>
<polygon fill="black" stroke="black" points="33.21,-27.99 39.55,-19.51 29.3,-22.19 33.21,-27.99"/>
<text text-anchor="middle" x="3.76" y="-113.68" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;0 -->
<g id="edge6" class="edge">
<title>2&#45;&gt;0</title>
<path fill="none" stroke="black" d="M94.2,-161.63C89.99,-171.38 83.6,-186.13 78.42,-198.12"/>
<polygon fill="black" stroke="black" points="75.17,-196.81 74.42,-207.37 81.6,-199.59 75.17,-196.81"/>
<text text-anchor="middle" x="89.76" y="-182.72" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="129.76" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="129.76" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M101.77,-138.2C104.08,-130.45 107.58,-119.91 111.76,-111.08 113.43,-107.55 115.43,-103.92 117.49,-100.47"/>
<polygon fill="black" stroke="black" points="120.48,-102.28 122.85,-91.95 114.56,-98.55 120.48,-102.28"/>
<text text-anchor="middle" x="114.76" y="-113.68" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;0 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;0</title>
<path fill="none" stroke="black" d="M52.73,-24.1C55.6,-56.87 64.1,-153.69 67.89,-196.9"/>
<polygon fill="black" stroke="black" points="64.43,-197.46 68.79,-207.12 71.4,-196.85 64.43,-197.46"/>
<text text-anchor="middle" x="64.76" y="-113.68" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge9" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M55.56,-24.03C63.45,-46.86 81.69,-99.69 91.76,-128.85"/>
<polygon fill="black" stroke="black" points="88.5,-130.12 95.07,-138.43 95.11,-127.83 88.5,-130.12"/>
<text text-anchor="middle" x="82.76" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M131.33,-93.05C133.1,-109.08 134.51,-139.45 123.76,-162.12 115.76,-178.98 100.68,-193.96 88.48,-204.18"/>
<polygon fill="black" stroke="black" points="86.25,-201.48 80.61,-210.45 90.61,-206.95 86.25,-201.48"/>
<text text-anchor="middle" x="132.76" y="-148.2" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M125.91,-92.87C123.08,-100.57 118.99,-111.09 114.76,-120.08 113.26,-123.26 111.57,-126.57 109.86,-129.77"/>
<polygon fill="black" stroke="black" points="106.65,-128.35 104.86,-138.79 112.77,-131.74 106.65,-128.35"/>
<text text-anchor="middle" x="121.76" y="-113.68" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M120.51,-71.08C112.33,-63.23 99.98,-51.62 88.76,-42.04 82.92,-37.06 76.37,-31.82 70.43,-27.18"/>
<polygon fill="black" stroke="black" points="72.37,-24.26 62.31,-20.93 68.09,-29.8 72.37,-24.26"/>
<text text-anchor="middle" x="101.76" y="-44.64" font-family="Times,serif" font-size="8.00">0</text>
</g>
</g>
</svg>
</div>

    </td>
    <td style="font-size: 30px; vertical-align: middle;">op</td>
    <td style="width: 30%;">
        <!-- Diagram B -->
<div>
<!-- Title: vector Pages: 1 -->
<svg width="70pt" height="177pt"
 viewBox="0.00 0.00 70.00 177.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 173)">
<title>vector</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-173 66,-173 66,4 -4,4"/>
<g id="clust1" class="cluster">
<title>cluster_vector</title>
<polygon fill="lightgray" stroke="black" stroke-dasharray="5,2" points="8,-8 8,-161 54,-161 54,-8 8,-8"/>
</g>
<!-- node0 -->
<g id="node1" class="node">
<title>node0</title>
<polygon fill="none" stroke="black" points="33,-153 29,-153 29,-149 33,-149 33,-153"/>
</g>
<!-- node1 -->
<g id="node2" class="node">
<title>node1</title>
<polygon fill="none" stroke="black" points="46,-113 16,-113 16,-96 46,-96 46,-113"/>
<text text-anchor="middle" x="31" y="-102.6" font-family="Times,serif" font-size="8.00">1:2</text>
</g>
<!-- node0&#45;&gt;node1 -->
<!-- node2 -->
<g id="node3" class="node">
<title>node2</title>
<polygon fill="none" stroke="black" points="33,-60 29,-60 29,-56 33,-56 33,-60"/>
</g>
<!-- node1&#45;&gt;node2 -->
<!-- node3 -->
<g id="node4" class="node">
<title>node3</title>
<polygon fill="none" stroke="black" points="33,-20 29,-20 29,-16 33,-16 33,-20"/>
</g>
<!-- node2&#45;&gt;node3 -->
</g>
</svg>
</div>

    </td>
    <td style="font-size: 30px; vertical-align: middle;">=</td>
    <td style="width: 30%;">
        <!-- Diagram C -->
<div>
<!-- Title: vector Pages: 1 -->
<svg width="70pt" height="177pt"
 viewBox="0.00 0.00 70.00 177.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 173)">
<title>vector</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-173 66,-173 66,4 -4,4"/>
<g id="clust1" class="cluster">
<title>cluster_vector</title>
<polygon fill="lightgray" stroke="black" stroke-dasharray="5,2" points="8,-8 8,-161 54,-161 54,-8 8,-8"/>
</g>
<!-- node0 -->
<g id="node1" class="node">
<title>node0</title>
<polygon fill="none" stroke="black" points="33,-153 29,-153 29,-149 33,-149 33,-153"/>
</g>
<!-- node1 -->
<g id="node2" class="node">
<title>node1</title>
<polygon fill="none" stroke="black" points="33,-113 29,-113 29,-109 33,-109 33,-113"/>
</g>
<!-- node0&#45;&gt;node1 -->
<!-- node2 -->
<g id="node3" class="node">
<title>node2</title>
<polygon fill="none" stroke="black" points="46,-73 16,-73 16,-56 46,-56 46,-73"/>
<text text-anchor="middle" x="31" y="-62.6" font-family="Times,serif" font-size="8.00">2:4</text>
</g>
<!-- node1&#45;&gt;node2 -->
<!-- node3 -->
<g id="node4" class="node">
<title>node3</title>
<polygon fill="none" stroke="black" points="33,-20 29,-20 29,-16 33,-16 33,-20"/>
</g>
<!-- node2&#45;&gt;node3 -->
</g>
</svg>
</div>

    </td>
  </tr>
</table>
'mxv' is also supported by the `@` operator:
``` postgres-console
select print(a) as a, '@' as "@", print(u) as u, print(a @ u) as mxv from test_fixture;
┌────────────────────┬───┬───────────┬───────────┐
│         a          │ @ │     u     │    mxv    │
├────────────────────┼───┼───────────┼───────────┤
│      0  1  2  3    │ @ │           │           │
│    ────────────    │   │    ───    │    ───    │
│  0│        0  3    │   │  0│       │  0│       │
│  1│  2     1  0    │   │  1│  2    │  1│       │
│  2│  2  2          │   │  2│       │  2│  4    │
│  3│  2     1       │   │  3│       │  3│       │
│                    │   │           │           │
└────────────────────┴───┴───────────┴───────────┘
(1 row)

```
## Vector Matrix Multiplication

Matrices can be multipled by vectors on the right taking the linear
combination of the matrices rows using the vectors elements as
coefficients:
``` postgres-console
select print(v) as v, semiring, print(b) as b, print(vxm(v, b, semiring)) as vxm from test_fixture;
┌───────────┬──────────────────┬────────────────────┬───────────┐
│     v     │     semiring     │         b          │    vxm    │
├───────────┼──────────────────┼────────────────────┼───────────┤
│           │ plus_times_int32 │      0  1  2  3    │           │
│    ───    │                  │    ────────────    │    ───    │
│  0│       │                  │  0│           4    │  0│       │
│  1│  3    │                  │  1│        3  1    │  1│  6    │
│  2│  3    │                  │  2│     2     4    │  2│  9    │
│  3│       │                  │  3│     0  2       │  3│ 15    │
│           │                  │                    │           │
└───────────┴──────────────────┴────────────────────┴───────────┘
(1 row)

```
From a graph standpoint, vector matrix multiplication is used to
"push" forward to adjacent nodes from their outgoing edges.  When
iterated, it forms the basis for working forward along outgoing
edges.
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
<div>
<!-- Title: vector Pages: 1 -->
<svg width="70pt" height="190pt"
 viewBox="0.00 0.00 70.00 190.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 186)">
<title>vector</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-186 66,-186 66,4 -4,4"/>
<g id="clust1" class="cluster">
<title>cluster_vector</title>
<polygon fill="lightgray" stroke="black" stroke-dasharray="5,2" points="8,-8 8,-174 54,-174 54,-8 8,-8"/>
</g>
<!-- node0 -->
<g id="node1" class="node">
<title>node0</title>
<polygon fill="none" stroke="black" points="33,-166 29,-166 29,-162 33,-162 33,-166"/>
</g>
<!-- node1 -->
<g id="node2" class="node">
<title>node1</title>
<polygon fill="none" stroke="black" points="46,-126 16,-126 16,-109 46,-109 46,-126"/>
<text text-anchor="middle" x="31" y="-115.6" font-family="Times,serif" font-size="8.00">1:3</text>
</g>
<!-- node0&#45;&gt;node1 -->
<!-- node2 -->
<g id="node3" class="node">
<title>node2</title>
<polygon fill="none" stroke="black" points="46,-73 16,-73 16,-56 46,-56 46,-73"/>
<text text-anchor="middle" x="31" y="-62.6" font-family="Times,serif" font-size="8.00">2:3</text>
</g>
<!-- node1&#45;&gt;node2 -->
<!-- node3 -->
<g id="node4" class="node">
<title>node3</title>
<polygon fill="none" stroke="black" points="33,-20 29,-20 29,-16 33,-16 33,-20"/>
</g>
<!-- node2&#45;&gt;node3 -->
</g>
</svg>
</div>

    </td>
    <td style="font-size: 30px; vertical-align: middle;">op</td>
    <td style="width: 30%;">
        <!-- Diagram B -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="105pt" height="239pt"
 viewBox="0.00 0.00 104.56 239.17" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 235.17)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-235.17 100.56,-235.17 100.56,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="56.56" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="56.56" y="-217.25" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node2" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="56.56" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="56.56" y="-148.2" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M56.56,-207.08C56.56,-197.72 56.56,-184.04 56.56,-172.55"/>
<polygon fill="black" stroke="black" points="60.06,-172.42 56.56,-162.42 53.06,-172.42 60.06,-172.42"/>
<text text-anchor="middle" x="59.56" y="-182.72" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1 -->
<g id="node3" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="15.56" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge6" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M45.16,-141.51C38.19,-136.28 29.67,-128.74 24.56,-120.08 21.61,-115.1 19.63,-109.22 18.3,-103.58"/>
<polygon fill="black" stroke="black" points="21.68,-102.59 16.45,-93.37 14.79,-103.83 21.68,-102.59"/>
<text text-anchor="middle" x="27.56" y="-113.68" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node4" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="56.56" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="56.56" y="-10.12" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M56.56,-137.97C56.56,-115.34 56.56,-63.66 56.56,-34.34"/>
<polygon fill="black" stroke="black" points="60.06,-34.31 56.56,-24.31 53.06,-34.31 60.06,-34.31"/>
<text text-anchor="middle" x="59.56" y="-79.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M21.84,-92.34C28,-102.42 37.54,-118.01 45.06,-130.31"/>
<polygon fill="black" stroke="black" points="42.13,-132.22 50.33,-138.92 48.1,-128.56 42.13,-132.22"/>
<text text-anchor="middle" x="41.56" y="-113.68" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M14.9,-68.72C14.93,-60.78 15.99,-50.17 20.56,-42.04 24.2,-35.55 29.93,-29.96 35.75,-25.46"/>
<polygon fill="black" stroke="black" points="37.88,-28.24 44.12,-19.68 33.9,-22.48 37.88,-28.24"/>
<text text-anchor="middle" x="23.56" y="-44.64" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge5" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M66.58,-21.3C72.15,-26.58 78.55,-33.94 81.56,-42.04 93.63,-74.56 93.63,-87.57 81.56,-120.08 79.77,-124.9 76.79,-129.44 73.49,-133.45"/>
<polygon fill="black" stroke="black" points="70.87,-131.14 66.58,-140.82 75.97,-135.92 70.87,-131.14"/>
<text text-anchor="middle" x="93.56" y="-79.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M50.27,-23.3C44.11,-33.38 34.57,-48.97 27.05,-61.27"/>
<polygon fill="black" stroke="black" points="24.01,-59.52 21.78,-69.88 29.99,-63.18 24.01,-59.52"/>
<text text-anchor="middle" x="41.56" y="-44.64" font-family="Times,serif" font-size="8.00">2</text>
</g>
</g>
</svg>
</div>

    </td>
    <td style="font-size: 30px; vertical-align: middle;">=</td>
    <td style="width: 30%;">
        <!-- Diagram C -->
<div>
<!-- Title: vector Pages: 1 -->
<svg width="76pt" height="203pt"
 viewBox="0.00 0.00 76.00 203.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 199)">
<title>vector</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-199 72,-199 72,4 -4,4"/>
<g id="clust1" class="cluster">
<title>cluster_vector</title>
<polygon fill="lightgray" stroke="black" stroke-dasharray="5,2" points="8,-8 8,-187 60,-187 60,-8 8,-8"/>
</g>
<!-- node0 -->
<g id="node1" class="node">
<title>node0</title>
<polygon fill="none" stroke="black" points="36,-179 32,-179 32,-175 36,-175 36,-179"/>
</g>
<!-- node1 -->
<g id="node2" class="node">
<title>node1</title>
<polygon fill="none" stroke="black" points="49,-139 19,-139 19,-122 49,-122 49,-139"/>
<text text-anchor="middle" x="34" y="-128.6" font-family="Times,serif" font-size="8.00">1:6</text>
</g>
<!-- node0&#45;&gt;node1 -->
<!-- node2 -->
<g id="node3" class="node">
<title>node2</title>
<polygon fill="none" stroke="black" points="49,-86 19,-86 19,-69 49,-69 49,-86"/>
<text text-anchor="middle" x="34" y="-75.6" font-family="Times,serif" font-size="8.00">2:9</text>
</g>
<!-- node1&#45;&gt;node2 -->
<!-- node3 -->
<g id="node4" class="node">
<title>node3</title>
<polygon fill="none" stroke="black" points="51.5,-33 16.5,-33 16.5,-16 51.5,-16 51.5,-33"/>
<text text-anchor="middle" x="34" y="-22.6" font-family="Times,serif" font-size="8.00">3:15</text>
</g>
<!-- node2&#45;&gt;node3 -->
</g>
</svg>
</div>

    </td>
  </tr>
</table>
'vxm' is also supported by the `@` operator:
``` postgres-console
select print(v) as v, '@' as "@", print(b) as b, print(v @ b) as vxm from test_fixture;
┌───────────┬───┬────────────────────┬───────────┐
│     v     │ @ │         b          │    vxm    │
├───────────┼───┼────────────────────┼───────────┤
│           │ @ │      0  1  2  3    │           │
│    ───    │   │    ────────────    │    ───    │
│  0│       │   │  0│           4    │  0│       │
│  1│  3    │   │  1│        3  1    │  1│  6    │
│  2│  3    │   │  2│     2     4    │  2│  9    │
│  3│       │   │  3│     0  2       │  3│ 15    │
│           │   │                    │           │
└───────────┴───┴────────────────────┴───────────┘
(1 row)

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
┌────────────────────┬───────────────┬────────────────────┐
│         a          │ indexunaryop  │      selected      │
├────────────────────┼───────────────┼────────────────────┤
│      0  1  2  3    │ valuegt_int32 │      0  1  2  3    │
│    ────────────    │               │    ────────────    │
│  0│        0  3    │               │  0│           3    │
│  1│  2     1  0    │               │  1│  2             │
│  2│  2  2          │               │  2│  2  2          │
│  3│  2     1       │               │  3│  2             │
│                    │               │                    │
└────────────────────┴───────────────┴────────────────────┘
(1 row)

```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 45%;">
        <!-- Diagram A -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="153pt" height="239pt"
 viewBox="0.00 0.00 153.31 239.17" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 235.17)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-235.17 149.31,-235.17 149.31,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="69.76" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="69.76" y="-217.25" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="98.76" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="98.76" y="-148.2" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M69.21,-207.05C69.22,-199.23 70.07,-188.65 73.76,-180.12 75.71,-175.61 78.62,-171.28 81.78,-167.4"/>
<polygon fill="black" stroke="black" points="84.61,-169.49 88.79,-159.75 79.45,-164.76 84.61,-169.49"/>
<text text-anchor="middle" x="76.76" y="-182.72" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="51.76" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="51.76" y="-10.12" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M55.27,-214.33C42.6,-210.23 24.72,-202.43 14.76,-189.12 -4.01,-164.06 3.88,-151.24 0.76,-120.08 -2.27,-89.91 10.52,-48.91 14.76,-42.04 18.79,-35.51 24.85,-29.86 30.91,-25.32"/>
<polygon fill="black" stroke="black" points="33.21,-27.99 39.55,-19.51 29.3,-22.19 33.21,-27.99"/>
<text text-anchor="middle" x="3.76" y="-113.68" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;0 -->
<g id="edge6" class="edge">
<title>2&#45;&gt;0</title>
<path fill="none" stroke="black" d="M94.2,-161.63C89.99,-171.38 83.6,-186.13 78.42,-198.12"/>
<polygon fill="black" stroke="black" points="75.17,-196.81 74.42,-207.37 81.6,-199.59 75.17,-196.81"/>
<text text-anchor="middle" x="89.76" y="-182.72" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="129.76" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="129.76" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M101.77,-138.2C104.08,-130.45 107.58,-119.91 111.76,-111.08 113.43,-107.55 115.43,-103.92 117.49,-100.47"/>
<polygon fill="black" stroke="black" points="120.48,-102.28 122.85,-91.95 114.56,-98.55 120.48,-102.28"/>
<text text-anchor="middle" x="114.76" y="-113.68" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;0 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;0</title>
<path fill="none" stroke="black" d="M52.73,-24.1C55.6,-56.87 64.1,-153.69 67.89,-196.9"/>
<polygon fill="black" stroke="black" points="64.43,-197.46 68.79,-207.12 71.4,-196.85 64.43,-197.46"/>
<text text-anchor="middle" x="64.76" y="-113.68" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge9" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M55.56,-24.03C63.45,-46.86 81.69,-99.69 91.76,-128.85"/>
<polygon fill="black" stroke="black" points="88.5,-130.12 95.07,-138.43 95.11,-127.83 88.5,-130.12"/>
<text text-anchor="middle" x="82.76" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M131.33,-93.05C133.1,-109.08 134.51,-139.45 123.76,-162.12 115.76,-178.98 100.68,-193.96 88.48,-204.18"/>
<polygon fill="black" stroke="black" points="86.25,-201.48 80.61,-210.45 90.61,-206.95 86.25,-201.48"/>
<text text-anchor="middle" x="132.76" y="-148.2" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M125.91,-92.87C123.08,-100.57 118.99,-111.09 114.76,-120.08 113.26,-123.26 111.57,-126.57 109.86,-129.77"/>
<polygon fill="black" stroke="black" points="106.65,-128.35 104.86,-138.79 112.77,-131.74 106.65,-128.35"/>
<text text-anchor="middle" x="121.76" y="-113.68" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M120.51,-71.08C112.33,-63.23 99.98,-51.62 88.76,-42.04 82.92,-37.06 76.37,-31.82 70.43,-27.18"/>
<polygon fill="black" stroke="black" points="72.37,-24.26 62.31,-20.93 68.09,-29.8 72.37,-24.26"/>
<text text-anchor="middle" x="101.76" y="-44.64" font-family="Times,serif" font-size="8.00">0</text>
</g>
</g>
</svg>
</div>

    </td>
    <td style="font-size: 30px; vertical-align: middle;">op</td>
    <td style="width: 45%;">
        <!-- Diagram B -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="63pt" height="239pt"
 viewBox="0.00 0.00 63.11 239.17" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 235.17)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-235.17 59.11,-235.17 59.11,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="17.56" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="17.56" y="-79.16" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node2" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="17.56" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="17.56" y="-10.12" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M12.29,-69.52C9.12,-61.95 5.98,-51.47 7.56,-42.04 8.05,-39.09 8.81,-36.03 9.7,-33.05"/>
<polygon fill="black" stroke="black" points="13.02,-34.14 12.98,-23.55 6.41,-31.86 13.02,-34.14"/>
<text text-anchor="middle" x="10.56" y="-44.64" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 3&#45;&gt;0 -->
<g id="edge5" class="edge">
<title>3&#45;&gt;0</title>
<path fill="none" stroke="black" d="M17.56,-24.33C17.56,-33.76 17.56,-47.46 17.56,-58.91"/>
<polygon fill="black" stroke="black" points="14.06,-58.99 17.56,-68.99 21.06,-58.99 14.06,-58.99"/>
<text text-anchor="middle" x="20.56" y="-44.64" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1 -->
<g id="node3" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="39.56" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="39.56" y="-148.2" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge2" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M36.02,-138.33C32.87,-128.71 28.16,-114.36 24.28,-102.56"/>
<polygon fill="black" stroke="black" points="27.6,-101.44 21.16,-93.03 20.95,-103.62 27.6,-101.44"/>
<text text-anchor="middle" x="33.56" y="-113.68" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2 -->
<g id="node4" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="15.56" y="-217.25" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;0 -->
<g id="edge3" class="edge">
<title>2&#45;&gt;0</title>
<path fill="none" stroke="black" d="M13.41,-207.01C10.69,-191.55 6.59,-162.7 8.56,-138.08 9.49,-126.42 11.49,-113.53 13.37,-103.05"/>
<polygon fill="black" stroke="black" points="16.82,-103.64 15.24,-93.16 9.94,-102.34 16.82,-103.64"/>
<text text-anchor="middle" x="11.56" y="-148.2" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M19.41,-207.37C22.86,-197.76 27.99,-183.4 32.22,-171.6"/>
<polygon fill="black" stroke="black" points="35.55,-172.67 35.63,-162.07 28.96,-170.31 35.55,-172.67"/>
<text text-anchor="middle" x="32.56" y="-182.72" font-family="Times,serif" font-size="8.00">2</text>
</g>
</g>
</svg>
</div>

    </td>
  </tr>
</table>
## Choosing Operators
Selection can also be done with scalars and operators:p
``` postgres-console
select print(a > 1) as "a > 1", print(a >= 1) as "a >= 1", print(a < 1) as "a < 1", print(a <= 1) as "a <= 1" from test_fixture;
┌────────────────────┬────────────────────┬────────────────────┬────────────────────┐
│       a > 1        │       a >= 1       │       a < 1        │       a <= 1       │
├────────────────────┼────────────────────┼────────────────────┼────────────────────┤
│      0  1  2  3    │      0  1  2  3    │      0  1  2  3    │      0  1  2  3    │
│    ────────────    │    ────────────    │    ────────────    │    ────────────    │
│  0│           3    │  0│           3    │  0│        0       │  0│        0       │
│  1│  2             │  1│  2     1       │  1│           0    │  1│        1  0    │
│  2│  2  2          │  2│  2  2          │  2│                │  2│                │
│  3│  2             │  3│  2     1       │  3│                │  3│        1       │
│                    │                    │                    │                    │
└────────────────────┴────────────────────┴────────────────────┴────────────────────┘
(1 row)

```
A useful select operator is `triu`, it select only upper triangular
values, this turns your graph into a direct acyclic graph (DAG) by
removing all the links "back" from higher number nodes to lower.
``` postgres-console
select print(random_matrix(8, 8, 16, seed=>0.42, max=>42)) as matrix,
       print(choose(random_matrix(8, 8, 16, seed=>0.42, max=>42), 'triu', 0)) as triu from test_fixture;
┌────────────────────────────────┬────────────────────────────────┐
│             matrix             │              triu              │
├────────────────────────────────┼────────────────────────────────┤
│      0  1  2  3  4  5  6  7    │      0  1  2  3  4  5  6  7    │
│    ────────────────────────    │    ────────────────────────    │
│  0│              6 31          │  0│              6 31          │
│  1│                   12       │  1│                   12       │
│  2│    26                      │  2│                            │
│  3│                40          │  3│                40          │
│  4│    11    15           7    │  4│                       7    │
│  5│ 20 12    22        1       │  5│                    1       │
│  6│              1             │  6│                            │
│  7│       17                   │  7│                            │
│                                │                                │
└────────────────────────────────┴────────────────────────────────┘
(1 row)

select draw(random_matrix(8, 8, 16, seed=>0.42, max=>42)) as uop_a_source,
       draw(choose(random_matrix(8, 8, 16, seed=>0.42, max=>42), 'triu', 0)) as uop_b_source
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 45%;">
        <!-- Diagram A -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="245pt" height="377pt"
 viewBox="0.00 0.00 245.24 377.25" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 373.25)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-373.25 241.24,-373.25 241.24,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="34.24" cy="-357.23" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="34.24" y="-355.33" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 4 -->
<g id="node2" class="node">
<title>4</title>
<ellipse fill="none" stroke="black" cx="135.24" cy="-288.19" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="135.24" y="-286.29" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 0&#45;&gt;4 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;4</title>
<path fill="none" stroke="black" d="M45.44,-348.8C62.35,-337.57 94.65,-316.13 115.55,-302.26"/>
<polygon fill="black" stroke="black" points="117.6,-305.1 124,-296.65 113.73,-299.27 117.6,-305.1"/>
<text text-anchor="middle" x="94.24" y="-320.81" font-family="Times,serif" font-size="8.00">6</text>
</g>
<!-- 5 -->
<g id="node3" class="node">
<title>5</title>
<ellipse fill="none" stroke="black" cx="71.24" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="71.24" y="-148.2" font-family="Times,serif" font-size="8.00">5</text>
</g>
<!-- 0&#45;&gt;5 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;5</title>
<path fill="none" stroke="black" d="M27.98,-346.15C17.09,-327.46 -3.73,-285.98 1.24,-249.17 5.5,-217.63 0.71,-205.25 20.24,-180.12 27.28,-171.08 37.98,-164.38 47.77,-159.71"/>
<polygon fill="black" stroke="black" points="49.27,-162.87 57.08,-155.71 46.51,-156.44 49.27,-162.87"/>
<text text-anchor="middle" x="6.74" y="-251.77" font-family="Times,serif" font-size="8.00">31</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="135.24" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="135.24" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 4&#45;&gt;1 -->
<g id="edge6" class="edge">
<title>4&#45;&gt;1</title>
<path fill="none" stroke="black" d="M135.24,-276.16C135.24,-243.46 135.24,-146.65 135.24,-103.38"/>
<polygon fill="black" stroke="black" points="138.74,-103.15 135.24,-93.15 131.74,-103.15 138.74,-103.15"/>
<text text-anchor="middle" x="140.74" y="-182.72" font-family="Times,serif" font-size="8.00">11</text>
</g>
<!-- 3 -->
<g id="node7" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="91.24" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="91.24" y="-217.25" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 4&#45;&gt;3 -->
<g id="edge7" class="edge">
<title>4&#45;&gt;3</title>
<path fill="none" stroke="black" d="M126.28,-277.99C121.21,-272.51 114.96,-265.25 110.24,-258.17 106.52,-252.58 103.05,-246.16 100.11,-240.21"/>
<polygon fill="black" stroke="black" points="103.15,-238.43 95.74,-230.86 96.81,-241.4 103.15,-238.43"/>
<text text-anchor="middle" x="115.74" y="-251.77" font-family="Times,serif" font-size="8.00">15</text>
</g>
<!-- 7 -->
<g id="node8" class="node">
<title>7</title>
<ellipse fill="none" stroke="black" cx="179.24" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="179.24" y="-217.25" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 4&#45;&gt;7 -->
<g id="edge8" class="edge">
<title>4&#45;&gt;7</title>
<path fill="none" stroke="black" d="M141.93,-277.01C148.52,-266.96 158.75,-251.38 166.83,-239.06"/>
<polygon fill="black" stroke="black" points="169.94,-240.7 172.5,-230.42 164.08,-236.86 169.94,-240.7"/>
<text text-anchor="middle" x="163.24" y="-251.77" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 5&#45;&gt;0 -->
<g id="edge9" class="edge">
<title>5&#45;&gt;0</title>
<path fill="none" stroke="black" d="M67.93,-162.06C66.37,-167.46 64.57,-174.1 63.24,-180.12 51.07,-235.59 41.46,-301.9 36.99,-335.1"/>
<polygon fill="black" stroke="black" points="33.52,-334.65 35.67,-345.02 40.45,-335.57 33.52,-334.65"/>
<text text-anchor="middle" x="55.74" y="-251.77" font-family="Times,serif" font-size="8.00">20</text>
</g>
<!-- 5&#45;&gt;1 -->
<g id="edge10" class="edge">
<title>5&#45;&gt;1</title>
<path fill="none" stroke="black" d="M79.87,-140.06C89.93,-129.53 106.77,-111.89 119.24,-98.83"/>
<polygon fill="black" stroke="black" points="121.96,-101.04 126.34,-91.39 116.9,-96.21 121.96,-101.04"/>
<text text-anchor="middle" x="112.74" y="-113.68" font-family="Times,serif" font-size="8.00">12</text>
</g>
<!-- 6 -->
<g id="node5" class="node">
<title>6</title>
<ellipse fill="none" stroke="black" cx="135.24" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="135.24" y="-10.12" font-family="Times,serif" font-size="8.00">6</text>
</g>
<!-- 5&#45;&gt;6 -->
<g id="edge12" class="edge">
<title>5&#45;&gt;6</title>
<path fill="none" stroke="black" d="M75.24,-138.31C81.09,-122.77 92.58,-93.29 104.24,-69.04 110.29,-56.46 117.86,-42.75 124.01,-32.03"/>
<polygon fill="black" stroke="black" points="127.17,-33.56 129.17,-23.16 121.12,-30.05 127.17,-33.56"/>
<text text-anchor="middle" x="107.24" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 5&#45;&gt;3 -->
<g id="edge11" class="edge">
<title>5&#45;&gt;3</title>
<path fill="none" stroke="black" d="M74.52,-162.07C77.37,-171.66 81.6,-185.83 85.09,-197.53"/>
<polygon fill="black" stroke="black" points="81.82,-198.79 88.03,-207.37 88.53,-196.79 81.82,-198.79"/>
<text text-anchor="middle" x="88.74" y="-182.72" font-family="Times,serif" font-size="8.00">22</text>
</g>
<!-- 1&#45;&gt;6 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;6</title>
<path fill="none" stroke="black" d="M135.24,-68.99C135.24,-59.63 135.24,-45.96 135.24,-34.47"/>
<polygon fill="black" stroke="black" points="138.74,-34.33 135.24,-24.33 131.74,-34.33 138.74,-34.33"/>
<text text-anchor="middle" x="140.74" y="-44.64" font-family="Times,serif" font-size="8.00">12</text>
</g>
<!-- 6&#45;&gt;4 -->
<g id="edge13" class="edge">
<title>6&#45;&gt;4</title>
<path fill="none" stroke="black" d="M150.24,-16.18C177.24,-22.84 231.24,-41.03 231.24,-80.06 231.24,-220.15 231.24,-220.15 231.24,-220.15 231.24,-254.91 188.41,-273.14 159.97,-281.42"/>
<polygon fill="black" stroke="black" points="158.99,-278.06 150.24,-284.03 160.81,-284.82 158.99,-278.06"/>
<text text-anchor="middle" x="234.24" y="-148.2" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2 -->
<g id="node6" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="179.24" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="179.24" y="-148.2" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M172.56,-138.92C165.97,-128.88 155.74,-113.29 147.66,-100.98"/>
<polygon fill="black" stroke="black" points="150.4,-98.78 141.99,-92.34 144.55,-102.62 150.4,-98.78"/>
<text text-anchor="middle" x="165.74" y="-113.68" font-family="Times,serif" font-size="8.00">26</text>
</g>
<!-- 3&#45;&gt;5 -->
<g id="edge5" class="edge">
<title>3&#45;&gt;5</title>
<path fill="none" stroke="black" d="M81.77,-209.46C76.7,-204.15 70.95,-196.9 68.24,-189.12 66.39,-183.79 66.1,-177.79 66.54,-172.16"/>
<polygon fill="black" stroke="black" points="70.04,-172.47 68.08,-162.06 63.12,-171.42 70.04,-172.47"/>
<text text-anchor="middle" x="73.74" y="-182.72" font-family="Times,serif" font-size="8.00">40</text>
</g>
<!-- 7&#45;&gt;2 -->
<g id="edge14" class="edge">
<title>7&#45;&gt;2</title>
<path fill="none" stroke="black" d="M179.24,-207.08C179.24,-197.72 179.24,-184.04 179.24,-172.55"/>
<polygon fill="black" stroke="black" points="182.74,-172.42 179.24,-162.42 175.74,-172.42 182.74,-172.42"/>
<text text-anchor="middle" x="184.74" y="-182.72" font-family="Times,serif" font-size="8.00">17</text>
</g>
</g>
</svg>
</div>

    </td>
    <td style="font-size: 30px; vertical-align: middle;">op</td>
    <td style="width: 45%;">
        <!-- Diagram B -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="144pt" height="170pt"
 viewBox="0.00 0.00 144.11 170.12" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 166.12)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-166.12 140.11,-166.12 140.11,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="22.56" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="22.56" y="-148.2" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 4 -->
<g id="node2" class="node">
<title>4</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="15.56" y="-79.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 0&#45;&gt;4 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;4</title>
<path fill="none" stroke="black" d="M21.4,-138.03C20.42,-128.68 19,-115 17.8,-103.51"/>
<polygon fill="black" stroke="black" points="21.26,-102.96 16.74,-93.37 14.29,-103.68 21.26,-102.96"/>
<text text-anchor="middle" x="22.56" y="-113.68" font-family="Times,serif" font-size="8.00">6</text>
</g>
<!-- 5 -->
<g id="node3" class="node">
<title>5</title>
<ellipse fill="none" stroke="black" cx="71.56" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="71.56" y="-79.16" font-family="Times,serif" font-size="8.00">5</text>
</g>
<!-- 0&#45;&gt;5 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;5</title>
<path fill="none" stroke="black" d="M29.79,-139.21C37.24,-129.01 49.04,-112.87 58.19,-100.34"/>
<polygon fill="black" stroke="black" points="61.22,-102.13 64.3,-92 55.57,-98 61.22,-102.13"/>
<text text-anchor="middle" x="55.06" y="-113.68" font-family="Times,serif" font-size="8.00">31</text>
</g>
<!-- 7 -->
<g id="node7" class="node">
<title>7</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="15.56" y="-10.12" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 4&#45;&gt;7 -->
<g id="edge5" class="edge">
<title>4&#45;&gt;7</title>
<path fill="none" stroke="black" d="M15.56,-68.99C15.56,-59.63 15.56,-45.96 15.56,-34.47"/>
<polygon fill="black" stroke="black" points="19.06,-34.33 15.56,-24.33 12.06,-34.33 19.06,-34.33"/>
<text text-anchor="middle" x="18.56" y="-44.64" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 6 -->
<g id="node5" class="node">
<title>6</title>
<ellipse fill="none" stroke="black" cx="95.56" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="95.56" y="-10.12" font-family="Times,serif" font-size="8.00">6</text>
</g>
<!-- 5&#45;&gt;6 -->
<g id="edge6" class="edge">
<title>5&#45;&gt;6</title>
<path fill="none" stroke="black" d="M75.41,-69.29C78.86,-59.67 83.99,-45.32 88.22,-33.52"/>
<polygon fill="black" stroke="black" points="91.55,-34.58 91.63,-23.99 84.96,-32.22 91.55,-34.58"/>
<text text-anchor="middle" x="87.56" y="-44.64" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="120.56" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="120.56" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;6 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;6</title>
<path fill="none" stroke="black" d="M116.54,-69.29C112.95,-59.67 107.6,-45.32 103.2,-33.52"/>
<polygon fill="black" stroke="black" points="106.42,-32.14 99.65,-23.99 99.86,-34.58 106.42,-32.14"/>
<text text-anchor="middle" x="114.06" y="-44.64" font-family="Times,serif" font-size="8.00">12</text>
</g>
<!-- 3 -->
<g id="node6" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="78.56" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="78.56" y="-148.2" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 3&#45;&gt;5 -->
<g id="edge4" class="edge">
<title>3&#45;&gt;5</title>
<path fill="none" stroke="black" d="M77.4,-138.03C76.42,-128.68 75,-115 73.8,-103.51"/>
<polygon fill="black" stroke="black" points="77.26,-102.96 72.74,-93.37 70.29,-103.68 77.26,-102.96"/>
<text text-anchor="middle" x="80.06" y="-113.68" font-family="Times,serif" font-size="8.00">40</text>
</g>
</g>
</svg>
</div>

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
┌──────────────┬──────────────────┬──────────────┬────────────────────┐
│      s       │     semiring     │      s       │     kronecker      │
├──────────────┼──────────────────┼──────────────┼────────────────────┤
│      0  1    │ plus_times_int32 │      0  1    │      0  1  2  3    │
│    ──────    │                  │    ──────    │    ────────────    │
│  0│  1  1    │                  │  0│  1  1    │  0│  1  1  1  1    │
│  1│     1    │                  │  1│     1    │  1│     1     1    │
│              │                  │              │  2│        1  1    │
│              │                  │              │  3│           1    │
│              │                  │              │                    │
└──────────────┴──────────────────┴──────────────┴────────────────────┘
(1 row)

```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="63pt" height="101pt"
 viewBox="0.00 0.00 63.11 101.08" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 97.08)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-97.08 59.11,-97.08 59.11,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="15.56" y="-79.16" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 0&#45;&gt;0 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;0</title>
<path fill="none" stroke="black" d="M29.34,-86.75C39.25,-88.6 49.11,-86.71 49.11,-81.06 49.11,-77.45 45.06,-75.37 39.55,-74.83"/>
<polygon fill="black" stroke="black" points="39.14,-71.35 29.34,-75.37 39.52,-78.34 39.14,-71.35"/>
<text text-anchor="middle" x="52.11" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="15.56" y="-10.12" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 0&#45;&gt;1 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;1</title>
<path fill="none" stroke="black" d="M15.56,-68.99C15.56,-59.63 15.56,-45.96 15.56,-34.47"/>
<polygon fill="black" stroke="black" points="19.06,-34.33 15.56,-24.33 12.06,-34.33 19.06,-34.33"/>
<text text-anchor="middle" x="18.56" y="-44.64" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;1 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;1</title>
<path fill="none" stroke="black" d="M29.34,-17.71C39.25,-19.56 49.11,-17.67 49.11,-12.02 49.11,-8.4 45.06,-6.33 39.55,-5.79"/>
<polygon fill="black" stroke="black" points="39.14,-2.3 29.34,-6.33 39.52,-9.29 39.14,-2.3"/>
<text text-anchor="middle" x="52.11" y="-10.12" font-family="Times,serif" font-size="8.00">1</text>
</g>
</g>
</svg>
</div>

    </td>
    <td style="font-size: 30px; vertical-align: middle;">op</td>
    <td style="width: 30%;">
        <!-- Diagram B -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="63pt" height="101pt"
 viewBox="0.00 0.00 63.11 101.08" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 97.08)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-97.08 59.11,-97.08 59.11,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="15.56" y="-79.16" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 0&#45;&gt;0 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;0</title>
<path fill="none" stroke="black" d="M29.34,-86.75C39.25,-88.6 49.11,-86.71 49.11,-81.06 49.11,-77.45 45.06,-75.37 39.55,-74.83"/>
<polygon fill="black" stroke="black" points="39.14,-71.35 29.34,-75.37 39.52,-78.34 39.14,-71.35"/>
<text text-anchor="middle" x="52.11" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="15.56" y="-10.12" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 0&#45;&gt;1 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;1</title>
<path fill="none" stroke="black" d="M15.56,-68.99C15.56,-59.63 15.56,-45.96 15.56,-34.47"/>
<polygon fill="black" stroke="black" points="19.06,-34.33 15.56,-24.33 12.06,-34.33 19.06,-34.33"/>
<text text-anchor="middle" x="18.56" y="-44.64" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;1 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;1</title>
<path fill="none" stroke="black" d="M29.34,-17.71C39.25,-19.56 49.11,-17.67 49.11,-12.02 49.11,-8.4 45.06,-6.33 39.55,-5.79"/>
<polygon fill="black" stroke="black" points="39.14,-2.3 29.34,-6.33 39.52,-9.29 39.14,-2.3"/>
<text text-anchor="middle" x="52.11" y="-10.12" font-family="Times,serif" font-size="8.00">1</text>
</g>
</g>
</svg>
</div>

    </td>
    <td style="font-size: 30px; vertical-align: middle;">=</td>
    <td style="width: 30%;">
        <!-- Diagram C -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="154pt" height="170pt"
 viewBox="0.00 0.00 153.56 170.12" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 166.12)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-166.12 149.56,-166.12 149.56,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="88.56" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="88.56" y="-148.2" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 0&#45;&gt;0 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;0</title>
<path fill="none" stroke="black" d="M102.34,-155.79C112.25,-157.64 122.11,-155.75 122.11,-150.1 122.11,-146.49 118.06,-144.41 112.55,-143.87"/>
<polygon fill="black" stroke="black" points="112.14,-140.39 102.34,-144.42 112.52,-147.38 112.14,-140.39"/>
<text text-anchor="middle" x="125.11" y="-148.2" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="15.56" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 0&#45;&gt;1 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;1</title>
<path fill="none" stroke="black" d="M79.01,-140.34C67.38,-129.66 47.5,-111.4 33.09,-98.16"/>
<polygon fill="black" stroke="black" points="35.07,-95.23 25.34,-91.04 30.33,-100.39 35.07,-95.23"/>
<text text-anchor="middle" x="59.56" y="-113.68" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2 -->
<g id="node3" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="88.56" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="88.56" y="-79.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M88.56,-138.03C88.56,-128.68 88.56,-115 88.56,-103.51"/>
<polygon fill="black" stroke="black" points="92.06,-103.37 88.56,-93.37 85.06,-103.37 92.06,-103.37"/>
<text text-anchor="middle" x="91.56" y="-113.68" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="88.56" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="88.56" y="-10.12" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge4" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M99.24,-141C111.21,-131.18 129.92,-113.4 137.56,-93.08 146.81,-68.44 124.87,-43.01 107.42,-27.61"/>
<polygon fill="black" stroke="black" points="109.29,-24.61 99.37,-20.9 104.81,-29.99 109.29,-24.61"/>
<text text-anchor="middle" x="142.56" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;1 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;1</title>
<path fill="none" stroke="black" d="M29.34,-86.75C39.25,-88.6 49.11,-86.71 49.11,-81.06 49.11,-77.45 45.06,-75.37 39.55,-74.83"/>
<polygon fill="black" stroke="black" points="39.14,-71.35 29.34,-75.37 39.52,-78.34 39.14,-71.35"/>
<text text-anchor="middle" x="52.11" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge6" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M25.1,-71.3C36.73,-60.62 56.61,-42.36 71.03,-29.12"/>
<polygon fill="black" stroke="black" points="73.78,-31.34 78.78,-22 69.04,-26.19 73.78,-31.34"/>
<text text-anchor="middle" x="59.56" y="-44.64" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;2 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;2</title>
<path fill="none" stroke="black" d="M102.34,-86.75C112.25,-88.6 122.11,-86.71 122.11,-81.06 122.11,-77.45 118.06,-75.37 112.55,-74.83"/>
<polygon fill="black" stroke="black" points="112.14,-71.35 102.34,-75.37 112.52,-78.34 112.14,-71.35"/>
<text text-anchor="middle" x="125.11" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge8" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M88.56,-68.99C88.56,-59.63 88.56,-45.96 88.56,-34.47"/>
<polygon fill="black" stroke="black" points="92.06,-34.33 88.56,-24.33 85.06,-34.33 92.06,-34.33"/>
<text text-anchor="middle" x="91.56" y="-44.64" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge9" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M102.34,-17.71C112.25,-19.56 122.11,-17.67 122.11,-12.02 122.11,-8.4 118.06,-6.33 112.55,-5.79"/>
<polygon fill="black" stroke="black" points="112.14,-2.3 102.34,-6.33 112.52,-9.29 112.14,-2.3"/>
<text text-anchor="middle" x="125.11" y="-10.12" font-family="Times,serif" font-size="8.00">1</text>
</g>
</g>
</svg>
</div>

    </td>
  </tr>
</table>
## Kronecker Power

There's a special function for exponentiating a matrix to itself a
certain number of times, `kronpower`:
``` postgres-console
select print(kronpower(s, 2)) from test_fixture;
┌────────────────────────────────────────────────────────┐
│                         print                          │
├────────────────────────────────────────────────────────┤
│      0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15    │
│    ────────────────────────────────────────────────    │
│  0│  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1    │
│  1│     1     1     1     1     1     1     1     1    │
│  2│        1  1        1  1        1  1        1  1    │
│  3│           1           1           1           1    │
│  4│              1  1  1  1              1  1  1  1    │
│  5│                 1     1                 1     1    │
│  6│                    1  1                    1  1    │
│  7│                       1                       1    │
│  8│                          1  1  1  1  1  1  1  1    │
│  9│                             1     1     1     1    │
│ 10│                                1  1        1  1    │
│ 11│                                   1           1    │
│ 12│                                      1  1  1  1    │
│ 13│                                         1     1    │
│ 14│                                            1  1    │
│ 15│                                               1    │
│                                                        │
└────────────────────────────────────────────────────────┘
(1 row)

```
Kronecker products can very quickly make huge graphs with power law
distributions.  These are handy synthetic graphs to mimic certain
statistical edge distributions common in sparse graph problems:
``` postgres-console
select nvals(kronpower(s, 4)) from test_fixture;
┌──────────┐
│  nvals   │
├──────────┤
│ 43046721 │
└──────────┘
(1 row)

```
## Transpose

A matrix can be transposed with the `transpose()` function:
``` postgres-console
select print(transpose(a)) from test_fixture;
┌────────────────────┐
│       print        │
├────────────────────┤
│      0  1  2  3    │
│    ────────────    │
│  0│     2  2  2    │
│  1│        2       │
│  2│  0  1     1    │
│  3│  3  0          │
│                    │
└────────────────────┘
(1 row)

```
## Apply

`apply` takes an operator of type `unaryop` and applies it to every
element of the matrix.  The 'ainv_int32' returned the additive
inverse (the negative value for integers) of every element:
``` postgres-console
select print(a) as a, unaryop, print(apply(a, unaryop)) as applied from test_fixture;
┌────────────────────┬────────────┬────────────────────┐
│         a          │  unaryop   │      applied       │
├────────────────────┼────────────┼────────────────────┤
│      0  1  2  3    │ ainv_int32 │      0  1  2  3    │
│    ────────────    │            │    ────────────    │
│  0│        0  3    │            │  0│        0 -3    │
│  1│  2     1  0    │            │  1│ -2    -1  0    │
│  2│  2  2          │            │  2│ -2 -2          │
│  3│  2     1       │            │  3│ -2    -1       │
│                    │            │                    │
└────────────────────┴────────────┴────────────────────┘
(1 row)

```
## SuiteSparse Info

The `info` function returns a descripton of the matrix from
SuiteSparse.
``` postgres-console
select info(a) from test_fixture;
┌───────────────────────────────────────────────┐
│                     info                      │
├───────────────────────────────────────────────┤
│                                               │
│   4x4 GraphBLAS int32_t matrix, bitmap by row │
│   A->matrix, 9 entries, memory: 312 bytes     │
│                                               │
│                                               │
└───────────────────────────────────────────────┘
(1 row)

```
## Matrix Duplication

The `dup` function duplicates a matrix returning a new matrix
object with the same values:
``` postgres-console
select dup(a) from test_fixture;
┌───────────────────────────────────────────────────────────────────┐
│                                dup                                │
├───────────────────────────────────────────────────────────────────┤
│ int32(4:4)[0:2:0 0:3:3 1:0:2 1:2:1 1:3:0 2:0:2 2:1:2 3:0:2 3:2:1] │
└───────────────────────────────────────────────────────────────────┘
(1 row)

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