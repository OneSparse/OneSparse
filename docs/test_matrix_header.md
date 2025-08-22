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
┌──────────────────────────┐
│          print           │
├──────────────────────────┤
│        0   1   2   3     │
│     ────────────────     │
│   0│                     │
│   1│                     │
│   2│                     │
│   3│                     │
│                          │
└──────────────────────────┘
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
┌─[ RECORD 1 ]─┬─────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ t            │ int32                                                                                                       │
│ a            │ int8(4:4)[0:2:0 0:3:-35 1:2:10 2:3:40 3:1:-19 3:2:15 3:3:31]                                                │
│ b            │ int8(4:4)[0:0:23 0:2:0 1:2:10 2:2:20 2:3:40 3:1:-19 3:2:15 3:3:34]                                          │
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
┌──────────────────────────┬──────────────────────────┐
│            a             │            b             │
├──────────────────────────┼──────────────────────────┤
│        0   1   2   3     │        0   1   2   3     │
│     ────────────────     │     ────────────────     │
│   0│           0 -35     │   0│  23       0         │
│   1│          10         │   1│          10         │
│   2│              40     │   2│          20  40     │
│   3│     -19  15  31     │   3│     -19  15  34     │
│                          │                          │
└──────────────────────────┴──────────────────────────┘
(1 row)

select draw(a) as twocol_a_source, draw(b) as twocol_b_source from test_fixture \gset
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="271pt" height="92pt"
 viewBox="0.00 0.00 271.45 91.56" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 87.56)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-87.56 267.45,-87.56 267.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-13.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-53.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-51.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M29.6,-22.53C39.72,-27.94 53.99,-35.56 65.84,-41.89"/>
<polygon fill="black" stroke="black" points="64.24,-45 74.71,-46.63 67.54,-38.83 64.24,-45"/>
<text text-anchor="middle" x="52.11" y="-39.16" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="166.78" cy="-20.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="166.78" y="-18.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M31.14,-16.04C56.59,-16.9 109.41,-18.67 140.79,-19.72"/>
<polygon fill="black" stroke="black" points="140.81,-23.22 150.92,-20.06 141.04,-16.23 140.81,-23.22"/>
<text text-anchor="middle" x="88.67" y="-21.16" font-family="Times,serif" font-size="8.00">&#45;35</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M104.21,-51.2C112.88,-49.47 123.97,-46.7 133.23,-42.56 137.36,-40.71 141.54,-38.34 145.46,-35.84"/>
<polygon fill="black" stroke="black" points="147.74,-38.52 154,-29.98 143.78,-32.75 147.74,-38.52"/>
<text text-anchor="middle" x="127.73" y="-49.16" font-family="Times,serif" font-size="8.00">40</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge6" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M151.32,-23.86C142.68,-26.08 131.59,-29.37 122.23,-33.56 118.38,-35.27 114.45,-37.37 110.7,-39.55"/>
<polygon fill="black" stroke="black" points="108.59,-36.74 101.93,-44.97 112.27,-42.69 108.59,-36.74"/>
<text text-anchor="middle" x="127.73" y="-36.16" font-family="Times,serif" font-size="8.00">15</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M160.28,-35.04C158.46,-44.71 160.62,-54.11 166.78,-54.11 170.63,-54.11 172.92,-50.44 173.65,-45.32"/>
<polygon fill="black" stroke="black" points="177.14,-44.91 173.28,-35.04 170.14,-45.16 177.14,-44.91"/>
<text text-anchor="middle" x="166.78" y="-56.71" font-family="Times,serif" font-size="8.00">31</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="247.89" cy="-60.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="247.89" y="-58.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge5" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M181.2,-27.34C193.09,-33.35 210.69,-42.25 224.68,-49.32"/>
<polygon fill="black" stroke="black" points="223.17,-52.48 233.67,-53.87 226.33,-46.24 223.17,-52.48"/>
<text text-anchor="middle" x="207.34" y="-46.16" font-family="Times,serif" font-size="8.00">&#45;19</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M232.6,-64.8C213.87,-69.8 180.03,-76.9 151.23,-72.56 138.36,-70.62 124.47,-66.51 113.14,-62.61"/>
<polygon fill="black" stroke="black" points="114.16,-59.26 103.57,-59.16 111.79,-65.85 114.16,-59.26"/>
<text text-anchor="middle" x="166.78" y="-77.16" font-family="Times,serif" font-size="8.00">10</text>
</g>
</g>
</svg>
</div>

    </td>
    <td style="width: 30%;">
        <!-- Diagram B -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="271pt" height="93pt"
 viewBox="0.00 0.00 271.45 93.11" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 89.11)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-89.11 267.45,-89.11 267.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-32.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-30.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 0&#45;&gt;0 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;0</title>
<path fill="none" stroke="black" d="M9.4,-47.04C7.67,-56.71 9.72,-66.11 15.56,-66.11 19.2,-66.11 21.37,-62.44 22.06,-57.32"/>
<polygon fill="black" stroke="black" points="25.55,-56.92 21.71,-47.04 18.56,-57.16 25.55,-56.92"/>
<text text-anchor="middle" x="15.56" y="-68.71" font-family="Times,serif" font-size="8.00">23</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-32.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-30.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M31.29,-32.56C40.35,-32.56 52.19,-32.56 62.68,-32.56"/>
<polygon fill="black" stroke="black" points="62.95,-36.06 72.95,-32.56 62.95,-29.06 62.95,-36.06"/>
<text text-anchor="middle" x="52.11" y="-35.16" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2&#45;&gt;2 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;2</title>
<path fill="none" stroke="black" d="M82.51,-47.04C80.78,-56.71 82.83,-66.11 88.67,-66.11 92.32,-66.11 94.49,-62.44 95.18,-57.32"/>
<polygon fill="black" stroke="black" points="98.66,-56.92 94.82,-47.04 91.67,-57.16 98.66,-56.92"/>
<text text-anchor="middle" x="88.67" y="-68.71" font-family="Times,serif" font-size="8.00">20</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="166.78" cy="-42.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="166.78" y="-40.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge5" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M103.4,-37.77C109.13,-39.68 115.91,-41.61 122.23,-42.56 128.21,-43.45 134.7,-43.79 140.81,-43.83"/>
<polygon fill="black" stroke="black" points="141.25,-47.32 151.18,-43.63 141.12,-40.32 141.25,-47.32"/>
<text text-anchor="middle" x="127.73" y="-46.16" font-family="Times,serif" font-size="8.00">40</text>
</g>
<!-- 1 -->
<g id="node3" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="247.89" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="247.89" y="-13.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M232.5,-12.21C213.67,-8.36 179.7,-3.2 151.23,-8.56 137.8,-11.08 123.54,-16.45 112.13,-21.47"/>
<polygon fill="black" stroke="black" points="110.55,-18.35 102.93,-25.72 113.48,-24.71 110.55,-18.35"/>
<text text-anchor="middle" x="166.78" y="-11.16" font-family="Times,serif" font-size="8.00">10</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M151.68,-37.84C146.01,-36.16 139.37,-34.46 133.23,-33.56 127.24,-32.67 120.74,-32.25 114.63,-32.07"/>
<polygon fill="black" stroke="black" points="114.29,-28.57 104.26,-32 114.24,-35.57 114.29,-28.57"/>
<text text-anchor="middle" x="127.73" y="-36.16" font-family="Times,serif" font-size="8.00">15</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge6" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M181.93,-37.73C193.4,-33.81 209.82,-28.21 223.29,-23.61"/>
<polygon fill="black" stroke="black" points="224.54,-26.88 232.87,-20.34 222.28,-20.26 224.54,-26.88"/>
<text text-anchor="middle" x="207.34" y="-33.16" font-family="Times,serif" font-size="8.00">&#45;19</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M160.28,-57.04C158.46,-66.71 160.62,-76.11 166.78,-76.11 170.63,-76.11 172.92,-72.44 173.65,-67.32"/>
<polygon fill="black" stroke="black" points="177.14,-66.91 173.28,-57.04 170.14,-67.16 177.14,-66.91"/>
<text text-anchor="middle" x="166.78" y="-78.71" font-family="Times,serif" font-size="8.00">34</text>
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

select draw(u) as twocol_a_source, draw(v) as twocol_b_source from test_fixture \gset
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
┌──────────────────────────┐
│          print           │
├──────────────────────────┤
│        0   1   2   3     │
│     ────────────────     │
│   0│   1   1   1   1     │
│   1│   1   1   1   1     │
│   2│   1   1   1   1     │
│   3│   1   1   1   1     │
│                          │
└──────────────────────────┘
(1 row)

```
One way of thinking about a "dense" matrix is a fully connected
graph, these can be constructed with the `dense_matrix()` function:

``` postgres-console
select draw(dense_matrix(4, 4, 1)) as draw_source from test_fixture \gset
```
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="258pt" height="179pt"
 viewBox="0.00 0.00 258.45 179.30" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 175.3)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-175.3 254.45,-175.3 254.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-23.3" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-21.4" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 0&#45;&gt;0 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;0</title>
<path fill="none" stroke="black" d="M9.4,-37.79C7.67,-47.45 9.72,-56.86 15.56,-56.86 19.2,-56.86 21.37,-53.18 22.06,-48.07"/>
<polygon fill="black" stroke="black" points="25.55,-47.66 21.71,-37.79 18.56,-47.9 25.55,-47.66"/>
<text text-anchor="middle" x="15.56" y="-59.46" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-111.3" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-109.4" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 0&#45;&gt;1 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;1</title>
<path fill="none" stroke="black" d="M20.95,-37.9C25.89,-52.36 35.1,-74.42 49.11,-89.3 53.51,-93.97 59.13,-97.93 64.71,-101.15"/>
<polygon fill="black" stroke="black" points="63.34,-104.39 73.83,-105.86 66.55,-98.17 63.34,-104.39"/>
<text text-anchor="middle" x="52.11" y="-95.9" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2 -->
<g id="node3" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="234.89" cy="-51.3" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="234.89" y="-49.4" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M30.68,-19.04C36.35,-17.54 42.99,-16.04 49.11,-15.3 105.9,-8.43 122.08,-11.52 177.34,-26.3 188.2,-29.21 190.99,-29.91 201.34,-34.3 204.89,-35.81 208.59,-37.56 212.16,-39.34"/>
<polygon fill="black" stroke="black" points="210.65,-42.5 221.13,-44.03 213.89,-36.3 210.65,-42.5"/>
<text text-anchor="middle" x="125.23" y="-17.9" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="161.78" cy="-51.3" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="161.78" y="-49.4" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge4" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M30.91,-26.09C55.41,-30.84 105.56,-40.58 135.87,-46.46"/>
<polygon fill="black" stroke="black" points="135.57,-49.97 146.05,-48.44 136.9,-43.1 135.57,-49.97"/>
<text text-anchor="middle" x="88.67" y="-42.9" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M78.23,-99.62C66.57,-85.2 46.57,-60.44 32.51,-43.04"/>
<polygon fill="black" stroke="black" points="35.15,-40.75 26.15,-35.17 29.71,-45.15 35.15,-40.75"/>
<text text-anchor="middle" x="52.11" y="-71.9" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;1 -->
<g id="edge6" class="edge">
<title>1&#45;&gt;1</title>
<path fill="none" stroke="black" d="M82.51,-125.79C80.78,-135.45 82.83,-144.86 88.67,-144.86 92.32,-144.86 94.49,-141.18 95.18,-136.07"/>
<polygon fill="black" stroke="black" points="98.66,-135.66 94.82,-125.79 91.67,-135.9 98.66,-135.66"/>
<text text-anchor="middle" x="88.67" y="-147.46" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge7" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M97.1,-124.88C102.72,-133.64 111.33,-144.29 122.23,-149.3 154.17,-164 172.82,-169.87 201.34,-149.3 223.93,-133 231.09,-100.38 233.23,-77.26"/>
<polygon fill="black" stroke="black" points="236.73,-77.3 233.91,-67.09 229.75,-76.84 236.73,-77.3"/>
<text text-anchor="middle" x="161.78" y="-164.9" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge8" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M103.24,-105.01C110.98,-101.08 120.62,-95.58 128.23,-89.3 134.49,-84.13 140.52,-77.66 145.65,-71.54"/>
<polygon fill="black" stroke="black" points="148.52,-73.56 152.03,-63.56 143.05,-69.19 148.52,-73.56"/>
<text text-anchor="middle" x="125.23" y="-95.9" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;0 -->
<g id="edge9" class="edge">
<title>2&#45;&gt;0</title>
<path fill="none" stroke="black" d="M222.07,-42.43C215.99,-38.01 208.39,-32.68 201.34,-28.3 171.91,-10.03 162.53,-6.09 128.23,-1.3 97.48,3 62.41,-6.27 39.91,-14.07"/>
<polygon fill="black" stroke="black" points="38.42,-10.88 30.23,-17.6 40.82,-17.46 38.42,-10.88"/>
<text text-anchor="middle" x="125.23" y="-3.9" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge10" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M224.61,-63.64C214.49,-76.05 197.09,-94.54 177.34,-103.3 157.46,-112.11 132.6,-113.6 114.43,-113.18"/>
<polygon fill="black" stroke="black" points="114.49,-109.68 104.34,-112.74 114.18,-116.68 114.49,-109.68"/>
<text text-anchor="middle" x="161.78" y="-113.9" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;2 -->
<g id="edge11" class="edge">
<title>2&#45;&gt;2</title>
<path fill="none" stroke="black" d="M228.74,-65.79C227.01,-75.45 229.06,-84.86 234.89,-84.86 238.54,-84.86 240.71,-81.18 241.4,-76.07"/>
<polygon fill="black" stroke="black" points="244.89,-75.66 241.05,-65.79 237.89,-75.9 244.89,-75.66"/>
<text text-anchor="middle" x="234.89" y="-87.46" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge12" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M219.18,-51.3C210.12,-51.3 198.28,-51.3 187.79,-51.3"/>
<polygon fill="black" stroke="black" points="187.52,-47.8 177.52,-51.3 187.52,-54.8 187.52,-47.8"/>
<text text-anchor="middle" x="198.34" y="-53.9" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;0 -->
<g id="edge13" class="edge">
<title>3&#45;&gt;0</title>
<path fill="none" stroke="black" d="M148.44,-42.78C137.41,-35.72 120.53,-26.24 104.23,-22.3 83.4,-17.27 58.96,-18.08 41.19,-19.82"/>
<polygon fill="black" stroke="black" points="40.61,-16.36 31.07,-20.98 41.41,-23.32 40.61,-16.36"/>
<text text-anchor="middle" x="88.67" y="-24.9" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge14" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M148.69,-59.84C140.95,-65.44 130.79,-73.05 122.23,-80.3 117.34,-84.44 112.25,-89.11 107.58,-93.54"/>
<polygon fill="black" stroke="black" points="104.99,-91.18 100.23,-100.64 109.85,-96.21 104.99,-91.18"/>
<text text-anchor="middle" x="125.23" y="-82.9" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge15" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M176.06,-44.67C181.83,-42.14 188.76,-39.56 195.34,-38.3 200.75,-37.26 206.44,-38.06 211.74,-39.68"/>
<polygon fill="black" stroke="black" points="210.69,-43.03 221.28,-43.52 213.31,-36.54 210.69,-43.03"/>
<text text-anchor="middle" x="198.34" y="-40.9" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge16" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M155.63,-65.79C153.89,-75.45 155.95,-84.86 161.78,-84.86 165.43,-84.86 167.6,-81.18 168.29,-76.07"/>
<polygon fill="black" stroke="black" points="171.78,-75.66 167.94,-65.79 164.78,-75.9 171.78,-75.66"/>
<text text-anchor="middle" x="161.78" y="-87.46" font-family="Times,serif" font-size="8.00">1</text>
</g>
</g>
</svg>
</div>

And another matrix named 's' which is a Sierpinsky Graph, which
we'll show off a bit later.

``` postgres-console
select print(s) from test_fixture;
┌──────────────────┐
│      print       │
├──────────────────┤
│        0   1     │
│     ────────     │
│   0│   1   1     │
│   1│       1     │
│                  │
└──────────────────┘
(1 row)

```
## Random Matrices

`random_matrix` will generate a random matrix provided the type,
number of rows, number of columns, and the number of (approximate)
values, an optional max value, and an optional random seed for
deterministic generation:
``` postgres-console
select print(random_matrix('int8', 8, 8, 0.5, 42) % 42) as random_matrix;
┌──────────────────────────────────────────┐
│              random_matrix               │
├──────────────────────────────────────────┤
│        0   1   2   3   4   5   6   7     │
│     ────────────────────────────────     │
│   0│  23       0                 -35     │
│   1│                   1  24      23     │
│   2│      28  20  -1      29             │
│   3│          27  -3     -19     -12     │
│   4│          26      20  38             │
│   5│  30   0  10  22                     │
│   6│  34          40                     │
│   7│     -30 -25          30     -12     │
│                                          │
└──────────────────────────────────────────┘
(1 row)

```
This random matrix is also a random *graph*:

``` postgres-console
select draw(random_matrix('int8', 8, 8, 0.5, 42) % 42) as draw_source \gset
```
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="512pt" height="473pt"
 viewBox="0.00 0.00 511.79 473.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 469)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-469 507.79,-469 507.79,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="93.67" cy="-105" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="93.67" y="-103.1" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 0&#45;&gt;0 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;0</title>
<path fill="none" stroke="black" d="M87.51,-119.49C85.78,-129.15 87.83,-138.56 93.67,-138.56 97.32,-138.56 99.49,-134.88 100.18,-129.77"/>
<polygon fill="black" stroke="black" points="103.66,-129.36 99.82,-119.49 96.67,-129.6 103.66,-129.36"/>
<text text-anchor="middle" x="93.67" y="-141.16" font-family="Times,serif" font-size="8.00">23</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="166.78" cy="-237" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="166.78" y="-235.1" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M99.15,-119.78C104.57,-136.65 114.57,-164.77 127.23,-187 133.39,-197.83 141.68,-208.9 149,-217.86"/>
<polygon fill="black" stroke="black" points="146.5,-220.33 155.61,-225.73 151.85,-215.82 146.5,-220.33"/>
<text text-anchor="middle" x="130.23" y="-198.6" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 7 -->
<g id="node3" class="node">
<title>7</title>
<ellipse fill="none" stroke="black" cx="329.01" cy="-287" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="329.01" y="-285.1" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 0&#45;&gt;7 -->
<g id="edge3" class="edge">
<title>0&#45;&gt;7</title>
<path fill="none" stroke="black" d="M106.56,-114.28C144.24,-143.67 261.27,-234.95 308.22,-271.57"/>
<polygon fill="black" stroke="black" points="306.1,-274.35 316.14,-277.74 310.41,-268.83 306.1,-274.35"/>
<text text-anchor="middle" x="207.34" y="-201.6" font-family="Times,serif" font-size="8.00">&#45;35</text>
</g>
<!-- 2&#45;&gt;2 -->
<g id="edge8" class="edge">
<title>2&#45;&gt;2</title>
<path fill="none" stroke="black" d="M160.63,-251.49C158.89,-261.15 160.95,-270.56 166.78,-270.56 170.43,-270.56 172.6,-266.88 173.29,-261.77"/>
<polygon fill="black" stroke="black" points="176.78,-261.36 172.94,-251.49 169.78,-261.6 176.78,-261.36"/>
<text text-anchor="middle" x="166.78" y="-273.16" font-family="Times,serif" font-size="8.00">20</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="247.89" cy="-269" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="247.89" y="-267.1" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M181.56,-242.57C193.25,-247.3 210.25,-254.18 223.98,-259.73"/>
<polygon fill="black" stroke="black" points="222.69,-262.98 233.27,-263.49 225.31,-256.5 222.69,-262.98"/>
<text text-anchor="middle" x="207.34" y="-256.6" font-family="Times,serif" font-size="8.00">28</text>
</g>
<!-- 5 -->
<g id="node6" class="node">
<title>5</title>
<ellipse fill="none" stroke="black" cx="407.12" cy="-244" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="407.12" y="-242.1" font-family="Times,serif" font-size="8.00">5</text>
</g>
<!-- 2&#45;&gt;5 -->
<g id="edge10" class="edge">
<title>2&#45;&gt;5</title>
<path fill="none" stroke="black" d="M169.23,-221.61C171.95,-200.65 179.56,-162.98 200.34,-139 243.67,-88.99 287.9,-67.82 344.56,-102 385.05,-126.42 398.96,-184.36 403.7,-218.2"/>
<polygon fill="black" stroke="black" points="400.25,-218.77 404.94,-228.26 407.19,-217.91 400.25,-218.77"/>
<text text-anchor="middle" x="288.45" y="-88.6" font-family="Times,serif" font-size="8.00">29</text>
</g>
<!-- 3 -->
<g id="node7" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="488.23" cy="-288" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="488.23" y="-286.1" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge9" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M167.8,-252.75C168.74,-295.58 177.71,-411 246.89,-411 246.89,-411 246.89,-411 408.12,-411 454.22,-411 474.73,-349.12 482.75,-313.14"/>
<polygon fill="black" stroke="black" points="486.18,-313.83 484.77,-303.33 479.33,-312.42 486.18,-313.83"/>
<text text-anchor="middle" x="329.01" y="-413.6" font-family="Times,serif" font-size="8.00">&#45;1</text>
</g>
<!-- 7&#45;&gt;2 -->
<g id="edge25" class="edge">
<title>7&#45;&gt;2</title>
<path fill="none" stroke="black" d="M316.02,-296.05C310.15,-299.93 302.78,-304.02 295.45,-306 267.89,-313.45 257.79,-306.94 232.34,-294 213.79,-284.57 196.34,-268.56 184.27,-255.9"/>
<polygon fill="black" stroke="black" points="186.84,-253.52 177.48,-248.54 181.69,-258.27 186.84,-253.52"/>
<text text-anchor="middle" x="247.89" y="-309.6" font-family="Times,serif" font-size="8.00">&#45;25</text>
</g>
<!-- 7&#45;&gt;7 -->
<g id="edge27" class="edge">
<title>7&#45;&gt;7</title>
<path fill="none" stroke="black" d="M322.51,-301.49C320.68,-311.15 322.85,-320.56 329.01,-320.56 332.86,-320.56 335.15,-316.88 335.88,-311.77"/>
<polygon fill="black" stroke="black" points="339.36,-311.35 335.5,-301.49 332.37,-311.61 339.36,-311.35"/>
<text text-anchor="middle" x="329.01" y="-323.16" font-family="Times,serif" font-size="8.00">&#45;12</text>
</g>
<!-- 7&#45;&gt;1 -->
<g id="edge24" class="edge">
<title>7&#45;&gt;1</title>
<path fill="none" stroke="black" d="M313.59,-283.72C302.35,-281.16 286.49,-277.55 273.29,-274.55"/>
<polygon fill="black" stroke="black" points="273.93,-271.11 263.4,-272.3 272.38,-277.93 273.93,-271.11"/>
<text text-anchor="middle" x="288.45" y="-281.6" font-family="Times,serif" font-size="8.00">&#45;30</text>
</g>
<!-- 7&#45;&gt;5 -->
<g id="edge26" class="edge">
<title>7&#45;&gt;5</title>
<path fill="none" stroke="black" d="M342.92,-279.71C354.18,-273.34 370.76,-263.98 384.11,-256.43"/>
<polygon fill="black" stroke="black" points="386.16,-259.3 393.15,-251.33 382.72,-253.2 386.16,-259.3"/>
<text text-anchor="middle" x="368.06" y="-270.6" font-family="Times,serif" font-size="8.00">30</text>
</g>
<!-- 1&#45;&gt;7 -->
<g id="edge6" class="edge">
<title>1&#45;&gt;7</title>
<path fill="none" stroke="black" d="M259.7,-279.21C265.63,-284.1 273.4,-289.44 281.45,-292 288.53,-294.25 296.49,-294.25 303.79,-293.33"/>
<polygon fill="black" stroke="black" points="304.66,-296.73 313.85,-291.47 303.38,-289.85 304.66,-296.73"/>
<text text-anchor="middle" x="288.45" y="-295.6" font-family="Times,serif" font-size="8.00">23</text>
</g>
<!-- 4 -->
<g id="node5" class="node">
<title>4</title>
<ellipse fill="none" stroke="black" cx="329.01" cy="-127" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="329.01" y="-125.1" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1&#45;&gt;4 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;4</title>
<path fill="none" stroke="black" d="M256.34,-255.64C269.94,-231.22 298.96,-179.14 315.59,-149.28"/>
<polygon fill="black" stroke="black" points="318.72,-150.87 320.53,-140.43 312.6,-147.46 318.72,-150.87"/>
<text text-anchor="middle" x="288.45" y="-209.6" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;5 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;5</title>
<path fill="none" stroke="black" d="M263.13,-264.84C276.12,-261.19 295.92,-256.01 313.45,-253 336.22,-249.09 362.46,-246.74 381.19,-245.43"/>
<polygon fill="black" stroke="black" points="381.51,-248.92 391.27,-244.77 381.06,-241.93 381.51,-248.92"/>
<text text-anchor="middle" x="329.01" y="-255.6" font-family="Times,serif" font-size="8.00">24</text>
</g>
<!-- 4&#45;&gt;2 -->
<g id="edge15" class="edge">
<title>4&#45;&gt;2</title>
<path fill="none" stroke="black" d="M313.47,-125.48C287.6,-123.59 233.18,-123.48 200.34,-151 182.48,-165.97 174.4,-191.82 170.75,-211.29"/>
<polygon fill="black" stroke="black" points="167.27,-210.91 169.15,-221.33 174.18,-212.01 167.27,-210.91"/>
<text text-anchor="middle" x="247.89" y="-136.6" font-family="Times,serif" font-size="8.00">26</text>
</g>
<!-- 4&#45;&gt;4 -->
<g id="edge16" class="edge">
<title>4&#45;&gt;4</title>
<path fill="none" stroke="black" d="M322.51,-141.49C320.68,-151.15 322.85,-160.56 329.01,-160.56 332.86,-160.56 335.15,-156.88 335.88,-151.77"/>
<polygon fill="black" stroke="black" points="339.36,-151.35 335.5,-141.49 332.37,-151.61 339.36,-151.35"/>
<text text-anchor="middle" x="329.01" y="-163.16" font-family="Times,serif" font-size="8.00">20</text>
</g>
<!-- 4&#45;&gt;5 -->
<g id="edge17" class="edge">
<title>4&#45;&gt;5</title>
<path fill="none" stroke="black" d="M341.03,-137.29C350.49,-146.52 364.06,-160.78 373.56,-175 383.04,-189.18 391.21,-206.55 397.01,-220.31"/>
<polygon fill="black" stroke="black" points="393.77,-221.66 400.78,-229.6 400.26,-219.02 393.77,-221.66"/>
<text text-anchor="middle" x="368.06" y="-177.6" font-family="Times,serif" font-size="8.00">38</text>
</g>
<!-- 5&#45;&gt;0 -->
<g id="edge18" class="edge">
<title>5&#45;&gt;0</title>
<path fill="none" stroke="black" d="M403.68,-228.72C395.81,-182.12 369.31,-45 330.01,-45 165.78,-45 165.78,-45 165.78,-45 141.16,-45 120.65,-66.27 108.08,-83.48"/>
<polygon fill="black" stroke="black" points="105.02,-81.75 102.25,-91.98 110.79,-85.71 105.02,-81.75"/>
<text text-anchor="middle" x="247.89" y="-47.6" font-family="Times,serif" font-size="8.00">30</text>
</g>
<!-- 5&#45;&gt;2 -->
<g id="edge20" class="edge">
<title>5&#45;&gt;2</title>
<path fill="none" stroke="black" d="M394.85,-234.23C375.88,-218.51 335.69,-188.23 295.45,-177 254.12,-165.46 209.57,-197.82 185.56,-219.39"/>
<polygon fill="black" stroke="black" points="183.17,-216.84 178.24,-226.22 187.94,-221.96 183.17,-216.84"/>
<text text-anchor="middle" x="288.45" y="-179.6" font-family="Times,serif" font-size="8.00">10</text>
</g>
<!-- 5&#45;&gt;1 -->
<g id="edge19" class="edge">
<title>5&#45;&gt;1</title>
<path fill="none" stroke="black" d="M399.42,-257.53C387.2,-281.57 361.05,-330.04 344.56,-339 313.02,-356.15 280.05,-317.43 262.24,-291.01"/>
<polygon fill="black" stroke="black" points="264.97,-288.79 256.6,-282.29 259.09,-292.59 264.97,-288.79"/>
<text text-anchor="middle" x="329.01" y="-345.6" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 5&#45;&gt;3 -->
<g id="edge21" class="edge">
<title>5&#45;&gt;3</title>
<path fill="none" stroke="black" d="M421.18,-251.27C433.09,-257.89 450.93,-267.81 465.07,-275.67"/>
<polygon fill="black" stroke="black" points="463.71,-278.92 474.15,-280.72 467.11,-272.8 463.71,-278.92"/>
<text text-anchor="middle" x="447.68" y="-270.6" font-family="Times,serif" font-size="8.00">22</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge11" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M486.99,-303.81C485.44,-345.63 475.02,-456 408.12,-456 246.89,-456 246.89,-456 246.89,-456 217.17,-456 214.04,-434.37 200.34,-408 175.82,-360.81 169.63,-297.71 168.15,-262.86"/>
<polygon fill="black" stroke="black" points="171.64,-262.47 167.82,-252.59 164.64,-262.7 171.64,-262.47"/>
<text text-anchor="middle" x="329.01" y="-458.6" font-family="Times,serif" font-size="8.00">27</text>
</g>
<!-- 3&#45;&gt;7 -->
<g id="edge14" class="edge">
<title>3&#45;&gt;7</title>
<path fill="none" stroke="black" d="M472.37,-287.91C445.39,-287.73 388.1,-287.37 355.01,-287.16"/>
<polygon fill="black" stroke="black" points="354.82,-283.66 344.8,-287.09 354.78,-290.66 354.82,-283.66"/>
<text text-anchor="middle" x="407.12" y="-289.6" font-family="Times,serif" font-size="8.00">&#45;12</text>
</g>
<!-- 3&#45;&gt;5 -->
<g id="edge13" class="edge">
<title>3&#45;&gt;5</title>
<path fill="none" stroke="black" d="M480.89,-274.03C475.48,-263.97 466.68,-251.13 454.68,-245 448.1,-241.64 440.28,-240.55 432.95,-240.54"/>
<polygon fill="black" stroke="black" points="432.53,-237.06 422.76,-241.16 432.96,-244.05 432.53,-237.06"/>
<text text-anchor="middle" x="447.68" y="-247.6" font-family="Times,serif" font-size="8.00">&#45;19</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge12" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M481.46,-302.14C479.41,-311.92 481.67,-321.56 488.23,-321.56 492.44,-321.56 494.87,-317.6 495.54,-312.19"/>
<polygon fill="black" stroke="black" points="499.03,-311.93 495,-302.14 492.04,-312.31 499.03,-311.93"/>
<text text-anchor="middle" x="488.23" y="-324.16" font-family="Times,serif" font-size="8.00">&#45;3</text>
</g>
<!-- 6 -->
<g id="node8" class="node">
<title>6</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-16" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-14.1" font-family="Times,serif" font-size="8.00">6</text>
</g>
<!-- 6&#45;&gt;0 -->
<g id="edge22" class="edge">
<title>6&#45;&gt;0</title>
<path fill="none" stroke="black" d="M26.51,-27.64C39.11,-42.37 61.01,-67.98 76.14,-85.68"/>
<polygon fill="black" stroke="black" points="73.8,-88.32 82.96,-93.64 79.12,-83.77 73.8,-88.32"/>
<text text-anchor="middle" x="54.61" y="-68.6" font-family="Times,serif" font-size="8.00">34</text>
</g>
<!-- 6&#45;&gt;3 -->
<g id="edge23" class="edge">
<title>6&#45;&gt;3</title>
<path fill="none" stroke="black" d="M30.4,-11.29C45.52,-6.57 70.52,0 92.67,0 92.67,0 92.67,0 408.12,0 462.26,0 480.72,-191.64 485.68,-262.27"/>
<polygon fill="black" stroke="black" points="482.2,-262.77 486.36,-272.52 489.19,-262.3 482.2,-262.77"/>
<text text-anchor="middle" x="247.89" y="-2.6" font-family="Times,serif" font-size="8.00">40</text>
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
┌───┬───┬──────────┐
│ i │ j │    v     │
├───┼───┼──────────┤
│ 0 │ 2 │ int8:0   │
│ 0 │ 3 │ int8:-35 │
│ 1 │ 2 │ int8:10  │
│ 2 │ 3 │ int8:40  │
│ 3 │ 1 │ int8:-19 │
│ 3 │ 2 │ int8:15  │
│ 3 │ 3 │ int8:31  │
└───┴───┴──────────┘
(7 rows)

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
┌──────────────────────────────────────────────────┐
│                   bound_matrix                   │
├──────────────────────────────────────────────────┤
│        0   1   2   3   4   5   6   7   8   9     │
│     ────────────────────────────────────────     │
│   0│   0                                         │
│   1│       1                                     │
│   2│           2                                 │
│   3│               3                             │
│   4│                   4                         │
│   5│                       5                     │
│   6│                           6                 │
│   7│                               7             │
│   8│                                   8         │
│   9│                                       9     │
│                                                  │
└──────────────────────────────────────────────────┘
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
┌──────────────────────────┐
│       set_element        │
├──────────────────────────┤
│        0   1   2   3     │
│     ────────────────     │
│   0│           0 -35     │
│   1│       1  10         │
│   2│              40     │
│   3│     -19  15  31     │
│                          │
└──────────────────────────┘
(1 row)

```
Scalar elements can be extracted individually with `get_element`
``` postgres-console
select get_element(a, 3, 2) as get_element from test_fixture;
┌─────────────┐
│ get_element │
├─────────────┤
│ int8:15     │
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
│ int8:31     │
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
select print(a) as a, binaryop, print(b) as b, print(eadd(a, b, binaryop)) as "eadd(a, b, binaryop)" from test_fixture;
┌──────────────────────────┬─────────────┬──────────────────────────┬──────────────────────────┐
│            a             │  binaryop   │            b             │   eadd(a, b, binaryop)   │
├──────────────────────────┼─────────────┼──────────────────────────┼──────────────────────────┤
│        0   1   2   3     │ times_int32 │        0   1   2   3     │        0   1   2   3     │
│     ────────────────     │             │     ────────────────     │     ────────────────     │
│   0│           0 -35     │             │   0│  23       0         │   0│  23       0 -35     │
│   1│          10         │             │   1│          10         │   1│         100         │
│   2│              40     │             │   2│          20  40     │   2│          20  64     │
│   3│     -19  15  31     │             │   3│     -19  15  34     │   3│     105 -31  30     │
│                          │             │                          │                          │
└──────────────────────────┴─────────────┴──────────────────────────┴──────────────────────────┘
(1 row)

```
Eadd can also be accomplished with binary operators specific to
OneSparse.  Different binaryops are passed to eadd to do different
elementwise operations:
``` postgres-console
select print(a |+ b) as "a |+ b", print(a |- b) as "a |- b", print(a |* b) as "a |* b", print(a |/ b) as "a |/ b" from test_fixture;
┌──────────────────────────┬──────────────────────────┬──────────────────────────┬──────────────────────────┐
│          a |+ b          │          a |- b          │          a |* b          │          a |/ b          │
├──────────────────────────┼──────────────────────────┼──────────────────────────┼──────────────────────────┤
│        0   1   2   3     │        0   1   2   3     │        0   1   2   3     │        0   1   2   3     │
│     ────────────────     │     ────────────────     │     ────────────────     │     ────────────────     │
│   0│  23       0 -35     │   0│  23       0 -35     │   0│  23       0 -35     │   0│  23       0 -35     │
│   1│          20         │   1│           0         │   1│         100         │   1│           1         │
│   2│          20  80     │   2│          20   0     │   2│          20  64     │   2│          20   1     │
│   3│     -38  30  65     │   3│       0   0  -3     │   3│     105 -31  30     │   3│       1   1   0     │
│                          │                          │                          │                          │
└──────────────────────────┴──────────────────────────┴──────────────────────────┴──────────────────────────┘
(1 row)

```
From a graph standpoint, elementwise addition can be seen as the
merging ("union") of two graphs, such that the result has edges
from both graphs.  Any edges that occur in both graphs are merged
with the provided binary operator.
``` postgres-console
select draw(a) as binop_a_source, draw(b) as binop_b_source, draw(eadd(a, b, binaryop)) as binop_c_source from test_fixture \gset
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="271pt" height="92pt"
 viewBox="0.00 0.00 271.45 91.56" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 87.56)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-87.56 267.45,-87.56 267.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-13.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-53.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-51.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M29.6,-22.53C39.72,-27.94 53.99,-35.56 65.84,-41.89"/>
<polygon fill="black" stroke="black" points="64.24,-45 74.71,-46.63 67.54,-38.83 64.24,-45"/>
<text text-anchor="middle" x="52.11" y="-39.16" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="166.78" cy="-20.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="166.78" y="-18.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M31.14,-16.04C56.59,-16.9 109.41,-18.67 140.79,-19.72"/>
<polygon fill="black" stroke="black" points="140.81,-23.22 150.92,-20.06 141.04,-16.23 140.81,-23.22"/>
<text text-anchor="middle" x="88.67" y="-21.16" font-family="Times,serif" font-size="8.00">&#45;35</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M104.21,-51.2C112.88,-49.47 123.97,-46.7 133.23,-42.56 137.36,-40.71 141.54,-38.34 145.46,-35.84"/>
<polygon fill="black" stroke="black" points="147.74,-38.52 154,-29.98 143.78,-32.75 147.74,-38.52"/>
<text text-anchor="middle" x="127.73" y="-49.16" font-family="Times,serif" font-size="8.00">40</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge6" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M151.32,-23.86C142.68,-26.08 131.59,-29.37 122.23,-33.56 118.38,-35.27 114.45,-37.37 110.7,-39.55"/>
<polygon fill="black" stroke="black" points="108.59,-36.74 101.93,-44.97 112.27,-42.69 108.59,-36.74"/>
<text text-anchor="middle" x="127.73" y="-36.16" font-family="Times,serif" font-size="8.00">15</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M160.28,-35.04C158.46,-44.71 160.62,-54.11 166.78,-54.11 170.63,-54.11 172.92,-50.44 173.65,-45.32"/>
<polygon fill="black" stroke="black" points="177.14,-44.91 173.28,-35.04 170.14,-45.16 177.14,-44.91"/>
<text text-anchor="middle" x="166.78" y="-56.71" font-family="Times,serif" font-size="8.00">31</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="247.89" cy="-60.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="247.89" y="-58.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge5" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M181.2,-27.34C193.09,-33.35 210.69,-42.25 224.68,-49.32"/>
<polygon fill="black" stroke="black" points="223.17,-52.48 233.67,-53.87 226.33,-46.24 223.17,-52.48"/>
<text text-anchor="middle" x="207.34" y="-46.16" font-family="Times,serif" font-size="8.00">&#45;19</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M232.6,-64.8C213.87,-69.8 180.03,-76.9 151.23,-72.56 138.36,-70.62 124.47,-66.51 113.14,-62.61"/>
<polygon fill="black" stroke="black" points="114.16,-59.26 103.57,-59.16 111.79,-65.85 114.16,-59.26"/>
<text text-anchor="middle" x="166.78" y="-77.16" font-family="Times,serif" font-size="8.00">10</text>
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
<svg width="271pt" height="93pt"
 viewBox="0.00 0.00 271.45 93.11" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 89.11)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-89.11 267.45,-89.11 267.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-32.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-30.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 0&#45;&gt;0 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;0</title>
<path fill="none" stroke="black" d="M9.4,-47.04C7.67,-56.71 9.72,-66.11 15.56,-66.11 19.2,-66.11 21.37,-62.44 22.06,-57.32"/>
<polygon fill="black" stroke="black" points="25.55,-56.92 21.71,-47.04 18.56,-57.16 25.55,-56.92"/>
<text text-anchor="middle" x="15.56" y="-68.71" font-family="Times,serif" font-size="8.00">23</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-32.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-30.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M31.29,-32.56C40.35,-32.56 52.19,-32.56 62.68,-32.56"/>
<polygon fill="black" stroke="black" points="62.95,-36.06 72.95,-32.56 62.95,-29.06 62.95,-36.06"/>
<text text-anchor="middle" x="52.11" y="-35.16" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2&#45;&gt;2 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;2</title>
<path fill="none" stroke="black" d="M82.51,-47.04C80.78,-56.71 82.83,-66.11 88.67,-66.11 92.32,-66.11 94.49,-62.44 95.18,-57.32"/>
<polygon fill="black" stroke="black" points="98.66,-56.92 94.82,-47.04 91.67,-57.16 98.66,-56.92"/>
<text text-anchor="middle" x="88.67" y="-68.71" font-family="Times,serif" font-size="8.00">20</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="166.78" cy="-42.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="166.78" y="-40.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge5" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M103.4,-37.77C109.13,-39.68 115.91,-41.61 122.23,-42.56 128.21,-43.45 134.7,-43.79 140.81,-43.83"/>
<polygon fill="black" stroke="black" points="141.25,-47.32 151.18,-43.63 141.12,-40.32 141.25,-47.32"/>
<text text-anchor="middle" x="127.73" y="-46.16" font-family="Times,serif" font-size="8.00">40</text>
</g>
<!-- 1 -->
<g id="node3" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="247.89" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="247.89" y="-13.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M232.5,-12.21C213.67,-8.36 179.7,-3.2 151.23,-8.56 137.8,-11.08 123.54,-16.45 112.13,-21.47"/>
<polygon fill="black" stroke="black" points="110.55,-18.35 102.93,-25.72 113.48,-24.71 110.55,-18.35"/>
<text text-anchor="middle" x="166.78" y="-11.16" font-family="Times,serif" font-size="8.00">10</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M151.68,-37.84C146.01,-36.16 139.37,-34.46 133.23,-33.56 127.24,-32.67 120.74,-32.25 114.63,-32.07"/>
<polygon fill="black" stroke="black" points="114.29,-28.57 104.26,-32 114.24,-35.57 114.29,-28.57"/>
<text text-anchor="middle" x="127.73" y="-36.16" font-family="Times,serif" font-size="8.00">15</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge6" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M181.93,-37.73C193.4,-33.81 209.82,-28.21 223.29,-23.61"/>
<polygon fill="black" stroke="black" points="224.54,-26.88 232.87,-20.34 222.28,-20.26 224.54,-26.88"/>
<text text-anchor="middle" x="207.34" y="-33.16" font-family="Times,serif" font-size="8.00">&#45;19</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M160.28,-57.04C158.46,-66.71 160.62,-76.11 166.78,-76.11 170.63,-76.11 172.92,-72.44 173.65,-67.32"/>
<polygon fill="black" stroke="black" points="177.14,-66.91 173.28,-57.04 170.14,-67.16 177.14,-66.91"/>
<text text-anchor="middle" x="166.78" y="-78.71" font-family="Times,serif" font-size="8.00">34</text>
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
<svg width="276pt" height="104pt"
 viewBox="0.00 0.00 276.45 104.11" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 100.11)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-100.11 272.45,-100.11 272.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-13.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 0&#45;&gt;0 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;0</title>
<path fill="none" stroke="black" d="M9.4,-30.04C7.67,-39.71 9.72,-49.11 15.56,-49.11 19.2,-49.11 21.37,-45.44 22.06,-40.32"/>
<polygon fill="black" stroke="black" points="25.55,-39.92 21.71,-30.04 18.56,-40.16 25.55,-39.92"/>
<text text-anchor="middle" x="15.56" y="-51.71" font-family="Times,serif" font-size="8.00">23</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-53.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-51.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M29.6,-22.53C39.72,-27.94 53.99,-35.56 65.84,-41.89"/>
<polygon fill="black" stroke="black" points="64.24,-45 74.71,-46.63 67.54,-38.83 64.24,-45"/>
<text text-anchor="middle" x="52.11" y="-39.16" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="169.78" cy="-20.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="169.78" y="-18.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge3" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M31.15,-16.04C57.11,-16.89 111.59,-18.68 143.68,-19.73"/>
<polygon fill="black" stroke="black" points="143.9,-23.24 154.01,-20.07 144.13,-16.24 143.9,-23.24"/>
<text text-anchor="middle" x="88.67" y="-21.16" font-family="Times,serif" font-size="8.00">&#45;35</text>
</g>
<!-- 2&#45;&gt;2 -->
<g id="edge5" class="edge">
<title>2&#45;&gt;2</title>
<path fill="none" stroke="black" d="M82.51,-68.04C80.78,-77.71 82.83,-87.11 88.67,-87.11 92.32,-87.11 94.49,-83.44 95.18,-78.32"/>
<polygon fill="black" stroke="black" points="98.66,-77.92 94.82,-68.04 91.67,-78.16 98.66,-77.92"/>
<text text-anchor="middle" x="88.67" y="-89.71" font-family="Times,serif" font-size="8.00">20</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge6" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M104.37,-51.49C113.75,-49.83 126.03,-47.02 136.23,-42.56 140.37,-40.74 144.56,-38.38 148.48,-35.89"/>
<polygon fill="black" stroke="black" points="150.77,-38.57 157.03,-30.03 146.8,-32.8 150.77,-38.57"/>
<text text-anchor="middle" x="129.23" y="-50.16" font-family="Times,serif" font-size="8.00">64</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M154.16,-23.53C144.8,-25.71 132.53,-29.06 122.23,-33.56 118.37,-35.24 114.42,-37.32 110.67,-39.49"/>
<polygon fill="black" stroke="black" points="108.56,-36.68 101.9,-44.92 112.25,-42.63 108.56,-36.68"/>
<text text-anchor="middle" x="129.23" y="-36.16" font-family="Times,serif" font-size="8.00">&#45;31</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge9" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M163.01,-34.69C160.96,-44.48 163.22,-54.11 169.78,-54.11 173.99,-54.11 176.42,-50.16 177.09,-44.74"/>
<polygon fill="black" stroke="black" points="180.58,-44.49 176.55,-34.69 173.59,-44.87 180.58,-44.49"/>
<text text-anchor="middle" x="169.78" y="-56.71" font-family="Times,serif" font-size="8.00">30</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="252.89" cy="-60.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="252.89" y="-58.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M184.18,-27.16C196.46,-33.22 214.9,-42.31 229.43,-49.48"/>
<polygon fill="black" stroke="black" points="228.23,-52.79 238.75,-54.07 231.33,-46.51 228.23,-52.79"/>
<text text-anchor="middle" x="211.34" y="-47.16" font-family="Times,serif" font-size="8.00">105</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M237.64,-64.68C218.59,-69.65 183.82,-76.82 154.23,-72.56 140.45,-70.57 125.52,-66.32 113.49,-62.34"/>
<polygon fill="black" stroke="black" points="114.36,-58.94 103.77,-58.98 112.07,-65.56 114.36,-58.94"/>
<text text-anchor="middle" x="169.78" y="-77.16" font-family="Times,serif" font-size="8.00">100</text>
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
select print(a) as a, binaryop, print(b) as b, print(emult(a, b, binaryop)) as "emult(a, b, binaryop)" from test_fixture;
┌──────────────────────────┬─────────────┬──────────────────────────┬──────────────────────────┐
│            a             │  binaryop   │            b             │  emult(a, b, binaryop)   │
├──────────────────────────┼─────────────┼──────────────────────────┼──────────────────────────┤
│        0   1   2   3     │ times_int32 │        0   1   2   3     │        0   1   2   3     │
│     ────────────────     │             │     ────────────────     │     ────────────────     │
│   0│           0 -35     │             │   0│  23       0         │   0│           0         │
│   1│          10         │             │   1│          10         │   1│         100         │
│   2│              40     │             │   2│          20  40     │   2│              64     │
│   3│     -19  15  31     │             │   3│     -19  15  34     │   3│     105 -31  30     │
│                          │             │                          │                          │
└──────────────────────────┴─────────────┴──────────────────────────┴──────────────────────────┘
(1 row)

```
Emult can also be accomplished with binary operators specific to
OneSparse.  Different binaryops are passed to emult to do different
elementwise operations:
``` postgres-console
select print(a &+ b) as "a &+ b", print(a &- b) as "a &- b", print(a &* b) as "a &* b", print(a &/ b) as "a &/ b" from test_fixture;
┌──────────────────────────┬──────────────────────────┬──────────────────────────┬──────────────────────────┐
│          a &+ b          │          a &- b          │          a &* b          │          a &/ b          │
├──────────────────────────┼──────────────────────────┼──────────────────────────┼──────────────────────────┤
│        0   1   2   3     │        0   1   2   3     │        0   1   2   3     │        0   1   2   3     │
│     ────────────────     │     ────────────────     │     ────────────────     │     ────────────────     │
│   0│           0         │   0│           0         │   0│           0         │   0│           0         │
│   1│          20         │   1│           0         │   1│         100         │   1│           1         │
│   2│              80     │   2│               0     │   2│              64     │   2│               1     │
│   3│     -38  30  65     │   3│       0   0  -3     │   3│     105 -31  30     │   3│       1   1   0     │
│                          │                          │                          │                          │
└──────────────────────────┴──────────────────────────┴──────────────────────────┴──────────────────────────┘
(1 row)

```
From a graph standpoint, elementwise multiplication can be seen as
the intersection of two graphs, such that the result has edges that
are only present in both graphs.  The edges are combined with the
provided binary operator.
``` postgres-console
select draw(a) as binop_a_source, draw(b) as binop_b_source, draw(emult(a, b, binaryop)) as binop_c_source from test_fixture \gset
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="271pt" height="92pt"
 viewBox="0.00 0.00 271.45 91.56" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 87.56)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-87.56 267.45,-87.56 267.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-13.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-53.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-51.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M29.6,-22.53C39.72,-27.94 53.99,-35.56 65.84,-41.89"/>
<polygon fill="black" stroke="black" points="64.24,-45 74.71,-46.63 67.54,-38.83 64.24,-45"/>
<text text-anchor="middle" x="52.11" y="-39.16" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="166.78" cy="-20.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="166.78" y="-18.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M31.14,-16.04C56.59,-16.9 109.41,-18.67 140.79,-19.72"/>
<polygon fill="black" stroke="black" points="140.81,-23.22 150.92,-20.06 141.04,-16.23 140.81,-23.22"/>
<text text-anchor="middle" x="88.67" y="-21.16" font-family="Times,serif" font-size="8.00">&#45;35</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M104.21,-51.2C112.88,-49.47 123.97,-46.7 133.23,-42.56 137.36,-40.71 141.54,-38.34 145.46,-35.84"/>
<polygon fill="black" stroke="black" points="147.74,-38.52 154,-29.98 143.78,-32.75 147.74,-38.52"/>
<text text-anchor="middle" x="127.73" y="-49.16" font-family="Times,serif" font-size="8.00">40</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge6" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M151.32,-23.86C142.68,-26.08 131.59,-29.37 122.23,-33.56 118.38,-35.27 114.45,-37.37 110.7,-39.55"/>
<polygon fill="black" stroke="black" points="108.59,-36.74 101.93,-44.97 112.27,-42.69 108.59,-36.74"/>
<text text-anchor="middle" x="127.73" y="-36.16" font-family="Times,serif" font-size="8.00">15</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M160.28,-35.04C158.46,-44.71 160.62,-54.11 166.78,-54.11 170.63,-54.11 172.92,-50.44 173.65,-45.32"/>
<polygon fill="black" stroke="black" points="177.14,-44.91 173.28,-35.04 170.14,-45.16 177.14,-44.91"/>
<text text-anchor="middle" x="166.78" y="-56.71" font-family="Times,serif" font-size="8.00">31</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="247.89" cy="-60.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="247.89" y="-58.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge5" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M181.2,-27.34C193.09,-33.35 210.69,-42.25 224.68,-49.32"/>
<polygon fill="black" stroke="black" points="223.17,-52.48 233.67,-53.87 226.33,-46.24 223.17,-52.48"/>
<text text-anchor="middle" x="207.34" y="-46.16" font-family="Times,serif" font-size="8.00">&#45;19</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M232.6,-64.8C213.87,-69.8 180.03,-76.9 151.23,-72.56 138.36,-70.62 124.47,-66.51 113.14,-62.61"/>
<polygon fill="black" stroke="black" points="114.16,-59.26 103.57,-59.16 111.79,-65.85 114.16,-59.26"/>
<text text-anchor="middle" x="166.78" y="-77.16" font-family="Times,serif" font-size="8.00">10</text>
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
<svg width="271pt" height="93pt"
 viewBox="0.00 0.00 271.45 93.11" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 89.11)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-89.11 267.45,-89.11 267.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-32.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-30.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 0&#45;&gt;0 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;0</title>
<path fill="none" stroke="black" d="M9.4,-47.04C7.67,-56.71 9.72,-66.11 15.56,-66.11 19.2,-66.11 21.37,-62.44 22.06,-57.32"/>
<polygon fill="black" stroke="black" points="25.55,-56.92 21.71,-47.04 18.56,-57.16 25.55,-56.92"/>
<text text-anchor="middle" x="15.56" y="-68.71" font-family="Times,serif" font-size="8.00">23</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-32.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-30.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M31.29,-32.56C40.35,-32.56 52.19,-32.56 62.68,-32.56"/>
<polygon fill="black" stroke="black" points="62.95,-36.06 72.95,-32.56 62.95,-29.06 62.95,-36.06"/>
<text text-anchor="middle" x="52.11" y="-35.16" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2&#45;&gt;2 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;2</title>
<path fill="none" stroke="black" d="M82.51,-47.04C80.78,-56.71 82.83,-66.11 88.67,-66.11 92.32,-66.11 94.49,-62.44 95.18,-57.32"/>
<polygon fill="black" stroke="black" points="98.66,-56.92 94.82,-47.04 91.67,-57.16 98.66,-56.92"/>
<text text-anchor="middle" x="88.67" y="-68.71" font-family="Times,serif" font-size="8.00">20</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="166.78" cy="-42.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="166.78" y="-40.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge5" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M103.4,-37.77C109.13,-39.68 115.91,-41.61 122.23,-42.56 128.21,-43.45 134.7,-43.79 140.81,-43.83"/>
<polygon fill="black" stroke="black" points="141.25,-47.32 151.18,-43.63 141.12,-40.32 141.25,-47.32"/>
<text text-anchor="middle" x="127.73" y="-46.16" font-family="Times,serif" font-size="8.00">40</text>
</g>
<!-- 1 -->
<g id="node3" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="247.89" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="247.89" y="-13.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M232.5,-12.21C213.67,-8.36 179.7,-3.2 151.23,-8.56 137.8,-11.08 123.54,-16.45 112.13,-21.47"/>
<polygon fill="black" stroke="black" points="110.55,-18.35 102.93,-25.72 113.48,-24.71 110.55,-18.35"/>
<text text-anchor="middle" x="166.78" y="-11.16" font-family="Times,serif" font-size="8.00">10</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M151.68,-37.84C146.01,-36.16 139.37,-34.46 133.23,-33.56 127.24,-32.67 120.74,-32.25 114.63,-32.07"/>
<polygon fill="black" stroke="black" points="114.29,-28.57 104.26,-32 114.24,-35.57 114.29,-28.57"/>
<text text-anchor="middle" x="127.73" y="-36.16" font-family="Times,serif" font-size="8.00">15</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge6" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M181.93,-37.73C193.4,-33.81 209.82,-28.21 223.29,-23.61"/>
<polygon fill="black" stroke="black" points="224.54,-26.88 232.87,-20.34 222.28,-20.26 224.54,-26.88"/>
<text text-anchor="middle" x="207.34" y="-33.16" font-family="Times,serif" font-size="8.00">&#45;19</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M160.28,-57.04C158.46,-66.71 160.62,-76.11 166.78,-76.11 170.63,-76.11 172.92,-72.44 173.65,-67.32"/>
<polygon fill="black" stroke="black" points="177.14,-66.91 173.28,-57.04 170.14,-67.16 177.14,-66.91"/>
<text text-anchor="middle" x="166.78" y="-78.71" font-family="Times,serif" font-size="8.00">34</text>
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
<svg width="276pt" height="93pt"
 viewBox="0.00 0.00 276.45 93.11" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 89.11)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-89.11 272.45,-89.11 272.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-32.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-30.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-32.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-30.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M31.29,-32.56C40.35,-32.56 52.19,-32.56 62.68,-32.56"/>
<polygon fill="black" stroke="black" points="62.95,-36.06 72.95,-32.56 62.95,-29.06 62.95,-36.06"/>
<text text-anchor="middle" x="52.11" y="-35.16" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="169.78" cy="-42.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="169.78" y="-40.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge3" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M103.4,-37.77C109.13,-39.68 115.91,-41.61 122.23,-42.56 129.15,-43.6 136.72,-43.93 143.7,-43.92"/>
<polygon fill="black" stroke="black" points="144.11,-47.41 154.02,-43.67 143.94,-40.41 144.11,-47.41"/>
<text text-anchor="middle" x="129.23" y="-46.16" font-family="Times,serif" font-size="8.00">64</text>
</g>
<!-- 1 -->
<g id="node3" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="252.89" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="252.89" y="-13.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M237.54,-12.32C218.39,-8.49 183.51,-3.29 154.23,-8.56 139.88,-11.14 124.56,-16.7 112.45,-21.82"/>
<polygon fill="black" stroke="black" points="110.85,-18.7 103.12,-25.94 113.68,-25.1 110.85,-18.7"/>
<text text-anchor="middle" x="169.78" y="-11.16" font-family="Times,serif" font-size="8.00">100</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge5" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M154.68,-37.84C149.01,-36.16 142.38,-34.46 136.23,-33.56 129.3,-32.54 121.73,-32.1 114.74,-31.96"/>
<polygon fill="black" stroke="black" points="114.43,-28.46 104.42,-31.94 114.42,-35.46 114.43,-28.46"/>
<text text-anchor="middle" x="129.23" y="-36.16" font-family="Times,serif" font-size="8.00">&#45;31</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge4" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M184.91,-37.85C196.89,-33.87 214.32,-28.06 228.4,-23.38"/>
<polygon fill="black" stroke="black" points="229.54,-26.68 237.93,-20.21 227.33,-20.04 229.54,-26.68"/>
<text text-anchor="middle" x="211.34" y="-33.16" font-family="Times,serif" font-size="8.00">105</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge6" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M163.01,-56.69C160.96,-66.48 163.22,-76.11 169.78,-76.11 173.99,-76.11 176.42,-72.16 177.09,-66.74"/>
<polygon fill="black" stroke="black" points="180.58,-66.49 176.55,-56.69 173.59,-66.87 180.58,-66.49"/>
<text text-anchor="middle" x="169.78" y="-78.71" font-family="Times,serif" font-size="8.00">30</text>
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
select print(a) as a, binaryop, print(b) as b, print(eunion(a, 3::scalar, b, 4::scalar, binaryop)) as "eunion(a, 3::scalar, b, 4::scalar, binaryop)" from test_fixture;
┌──────────────────────────┬─────────────┬──────────────────────────┬──────────────────────────────────────────────┐
│            a             │  binaryop   │            b             │ eunion(a, 3::scalar, b, 4::scalar, binaryop) │
├──────────────────────────┼─────────────┼──────────────────────────┼──────────────────────────────────────────────┤
│        0   1   2   3     │ times_int32 │        0   1   2   3     │        0   1   2   3                         │
│     ────────────────     │             │     ────────────────     │     ────────────────                         │
│   0│           0 -35     │             │   0│  23       0         │   0│  69       0 116                         │
│   1│          10         │             │   1│          10         │   1│         100                             │
│   2│              40     │             │   2│          20  40     │   2│          60  64                         │
│   3│     -19  15  31     │             │   3│     -19  15  34     │   3│     105 -31  30                         │
│                          │             │                          │                                              │
└──────────────────────────┴─────────────┴──────────────────────────┴──────────────────────────────────────────────┘
(1 row)

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
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="271pt" height="92pt"
 viewBox="0.00 0.00 271.45 91.56" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 87.56)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-87.56 267.45,-87.56 267.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-13.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-53.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-51.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M29.6,-22.53C39.72,-27.94 53.99,-35.56 65.84,-41.89"/>
<polygon fill="black" stroke="black" points="64.24,-45 74.71,-46.63 67.54,-38.83 64.24,-45"/>
<text text-anchor="middle" x="52.11" y="-39.16" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="166.78" cy="-20.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="166.78" y="-18.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M31.14,-16.04C56.59,-16.9 109.41,-18.67 140.79,-19.72"/>
<polygon fill="black" stroke="black" points="140.81,-23.22 150.92,-20.06 141.04,-16.23 140.81,-23.22"/>
<text text-anchor="middle" x="88.67" y="-21.16" font-family="Times,serif" font-size="8.00">&#45;35</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M104.21,-51.2C112.88,-49.47 123.97,-46.7 133.23,-42.56 137.36,-40.71 141.54,-38.34 145.46,-35.84"/>
<polygon fill="black" stroke="black" points="147.74,-38.52 154,-29.98 143.78,-32.75 147.74,-38.52"/>
<text text-anchor="middle" x="127.73" y="-49.16" font-family="Times,serif" font-size="8.00">40</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge6" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M151.32,-23.86C142.68,-26.08 131.59,-29.37 122.23,-33.56 118.38,-35.27 114.45,-37.37 110.7,-39.55"/>
<polygon fill="black" stroke="black" points="108.59,-36.74 101.93,-44.97 112.27,-42.69 108.59,-36.74"/>
<text text-anchor="middle" x="127.73" y="-36.16" font-family="Times,serif" font-size="8.00">15</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M160.28,-35.04C158.46,-44.71 160.62,-54.11 166.78,-54.11 170.63,-54.11 172.92,-50.44 173.65,-45.32"/>
<polygon fill="black" stroke="black" points="177.14,-44.91 173.28,-35.04 170.14,-45.16 177.14,-44.91"/>
<text text-anchor="middle" x="166.78" y="-56.71" font-family="Times,serif" font-size="8.00">31</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="247.89" cy="-60.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="247.89" y="-58.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge5" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M181.2,-27.34C193.09,-33.35 210.69,-42.25 224.68,-49.32"/>
<polygon fill="black" stroke="black" points="223.17,-52.48 233.67,-53.87 226.33,-46.24 223.17,-52.48"/>
<text text-anchor="middle" x="207.34" y="-46.16" font-family="Times,serif" font-size="8.00">&#45;19</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M232.6,-64.8C213.87,-69.8 180.03,-76.9 151.23,-72.56 138.36,-70.62 124.47,-66.51 113.14,-62.61"/>
<polygon fill="black" stroke="black" points="114.16,-59.26 103.57,-59.16 111.79,-65.85 114.16,-59.26"/>
<text text-anchor="middle" x="166.78" y="-77.16" font-family="Times,serif" font-size="8.00">10</text>
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
<svg width="271pt" height="93pt"
 viewBox="0.00 0.00 271.45 93.11" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 89.11)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-89.11 267.45,-89.11 267.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-32.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-30.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 0&#45;&gt;0 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;0</title>
<path fill="none" stroke="black" d="M9.4,-47.04C7.67,-56.71 9.72,-66.11 15.56,-66.11 19.2,-66.11 21.37,-62.44 22.06,-57.32"/>
<polygon fill="black" stroke="black" points="25.55,-56.92 21.71,-47.04 18.56,-57.16 25.55,-56.92"/>
<text text-anchor="middle" x="15.56" y="-68.71" font-family="Times,serif" font-size="8.00">23</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-32.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-30.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M31.29,-32.56C40.35,-32.56 52.19,-32.56 62.68,-32.56"/>
<polygon fill="black" stroke="black" points="62.95,-36.06 72.95,-32.56 62.95,-29.06 62.95,-36.06"/>
<text text-anchor="middle" x="52.11" y="-35.16" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2&#45;&gt;2 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;2</title>
<path fill="none" stroke="black" d="M82.51,-47.04C80.78,-56.71 82.83,-66.11 88.67,-66.11 92.32,-66.11 94.49,-62.44 95.18,-57.32"/>
<polygon fill="black" stroke="black" points="98.66,-56.92 94.82,-47.04 91.67,-57.16 98.66,-56.92"/>
<text text-anchor="middle" x="88.67" y="-68.71" font-family="Times,serif" font-size="8.00">20</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="166.78" cy="-42.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="166.78" y="-40.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge5" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M103.4,-37.77C109.13,-39.68 115.91,-41.61 122.23,-42.56 128.21,-43.45 134.7,-43.79 140.81,-43.83"/>
<polygon fill="black" stroke="black" points="141.25,-47.32 151.18,-43.63 141.12,-40.32 141.25,-47.32"/>
<text text-anchor="middle" x="127.73" y="-46.16" font-family="Times,serif" font-size="8.00">40</text>
</g>
<!-- 1 -->
<g id="node3" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="247.89" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="247.89" y="-13.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M232.5,-12.21C213.67,-8.36 179.7,-3.2 151.23,-8.56 137.8,-11.08 123.54,-16.45 112.13,-21.47"/>
<polygon fill="black" stroke="black" points="110.55,-18.35 102.93,-25.72 113.48,-24.71 110.55,-18.35"/>
<text text-anchor="middle" x="166.78" y="-11.16" font-family="Times,serif" font-size="8.00">10</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M151.68,-37.84C146.01,-36.16 139.37,-34.46 133.23,-33.56 127.24,-32.67 120.74,-32.25 114.63,-32.07"/>
<polygon fill="black" stroke="black" points="114.29,-28.57 104.26,-32 114.24,-35.57 114.29,-28.57"/>
<text text-anchor="middle" x="127.73" y="-36.16" font-family="Times,serif" font-size="8.00">15</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge6" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M181.93,-37.73C193.4,-33.81 209.82,-28.21 223.29,-23.61"/>
<polygon fill="black" stroke="black" points="224.54,-26.88 232.87,-20.34 222.28,-20.26 224.54,-26.88"/>
<text text-anchor="middle" x="207.34" y="-33.16" font-family="Times,serif" font-size="8.00">&#45;19</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M160.28,-57.04C158.46,-66.71 160.62,-76.11 166.78,-76.11 170.63,-76.11 172.92,-72.44 173.65,-67.32"/>
<polygon fill="black" stroke="black" points="177.14,-66.91 173.28,-57.04 170.14,-67.16 177.14,-66.91"/>
<text text-anchor="middle" x="166.78" y="-78.71" font-family="Times,serif" font-size="8.00">34</text>
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
<svg width="276pt" height="104pt"
 viewBox="0.00 0.00 276.45 104.11" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 100.11)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-100.11 272.45,-100.11 272.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-13.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 0&#45;&gt;0 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;0</title>
<path fill="none" stroke="black" d="M9.4,-30.04C7.67,-39.71 9.72,-49.11 15.56,-49.11 19.2,-49.11 21.37,-45.44 22.06,-40.32"/>
<polygon fill="black" stroke="black" points="25.55,-39.92 21.71,-30.04 18.56,-40.16 25.55,-39.92"/>
<text text-anchor="middle" x="15.56" y="-51.71" font-family="Times,serif" font-size="8.00">69</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-53.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-51.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M29.6,-22.53C39.72,-27.94 53.99,-35.56 65.84,-41.89"/>
<polygon fill="black" stroke="black" points="64.24,-45 74.71,-46.63 67.54,-38.83 64.24,-45"/>
<text text-anchor="middle" x="52.11" y="-39.16" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="169.78" cy="-20.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="169.78" y="-18.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge3" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M31.15,-16.04C57.11,-16.89 111.59,-18.68 143.68,-19.73"/>
<polygon fill="black" stroke="black" points="143.9,-23.24 154.01,-20.07 144.13,-16.24 143.9,-23.24"/>
<text text-anchor="middle" x="88.67" y="-21.16" font-family="Times,serif" font-size="8.00">116</text>
</g>
<!-- 2&#45;&gt;2 -->
<g id="edge5" class="edge">
<title>2&#45;&gt;2</title>
<path fill="none" stroke="black" d="M82.51,-68.04C80.78,-77.71 82.83,-87.11 88.67,-87.11 92.32,-87.11 94.49,-83.44 95.18,-78.32"/>
<polygon fill="black" stroke="black" points="98.66,-77.92 94.82,-68.04 91.67,-78.16 98.66,-77.92"/>
<text text-anchor="middle" x="88.67" y="-89.71" font-family="Times,serif" font-size="8.00">60</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge6" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M104.37,-51.49C113.75,-49.83 126.03,-47.02 136.23,-42.56 140.37,-40.74 144.56,-38.38 148.48,-35.89"/>
<polygon fill="black" stroke="black" points="150.77,-38.57 157.03,-30.03 146.8,-32.8 150.77,-38.57"/>
<text text-anchor="middle" x="129.23" y="-50.16" font-family="Times,serif" font-size="8.00">64</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M154.16,-23.53C144.8,-25.71 132.53,-29.06 122.23,-33.56 118.37,-35.24 114.42,-37.32 110.67,-39.49"/>
<polygon fill="black" stroke="black" points="108.56,-36.68 101.9,-44.92 112.25,-42.63 108.56,-36.68"/>
<text text-anchor="middle" x="129.23" y="-36.16" font-family="Times,serif" font-size="8.00">&#45;31</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge9" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M163.01,-34.69C160.96,-44.48 163.22,-54.11 169.78,-54.11 173.99,-54.11 176.42,-50.16 177.09,-44.74"/>
<polygon fill="black" stroke="black" points="180.58,-44.49 176.55,-34.69 173.59,-44.87 180.58,-44.49"/>
<text text-anchor="middle" x="169.78" y="-56.71" font-family="Times,serif" font-size="8.00">30</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="252.89" cy="-60.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="252.89" y="-58.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M184.18,-27.16C196.46,-33.22 214.9,-42.31 229.43,-49.48"/>
<polygon fill="black" stroke="black" points="228.23,-52.79 238.75,-54.07 231.33,-46.51 228.23,-52.79"/>
<text text-anchor="middle" x="211.34" y="-47.16" font-family="Times,serif" font-size="8.00">105</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M237.64,-64.68C218.59,-69.65 183.82,-76.82 154.23,-72.56 140.45,-70.57 125.52,-66.32 113.49,-62.34"/>
<polygon fill="black" stroke="black" points="114.36,-58.94 103.77,-58.98 112.07,-65.56 114.36,-58.94"/>
<text text-anchor="middle" x="169.78" y="-77.16" font-family="Times,serif" font-size="8.00">100</text>
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
select print(b) as b, 'plus_monoid_int32' as monoid, reduce_scalar(b) from test_fixture;
┌──────────────────────────┬───────────────────┬───────────────┐
│            b             │      monoid       │ reduce_scalar │
├──────────────────────────┼───────────────────┼───────────────┤
│        0   1   2   3     │ plus_monoid_int32 │ int8:123      │
│     ────────────────     │                   │               │
│   0│  23       0         │                   │               │
│   1│          10         │                   │               │
│   2│          20  40     │                   │               │
│   3│     -19  15  34     │                   │               │
│                          │                   │               │
└──────────────────────────┴───────────────────┴───────────────┘
(1 row)

```
The entire matrix can be reduced to a scalar value with a provided
monoid that changes the reduction operation:
``` postgres-console
select print(b) as b, 'min_monoid_int32' as monoid, reduce_scalar(b, 'min_monoid_int32') from test_fixture;
┌──────────────────────────┬──────────────────┬───────────────┐
│            b             │      monoid      │ reduce_scalar │
├──────────────────────────┼──────────────────┼───────────────┤
│        0   1   2   3     │ min_monoid_int32 │ int8:-19      │
│     ────────────────     │                  │               │
│   0│  23       0         │                  │               │
│   1│          10         │                  │               │
│   2│          20  40     │                  │               │
│   3│     -19  15  34     │                  │               │
│                          │                  │               │
└──────────────────────────┴──────────────────┴───────────────┘
(1 row)

```
The matrix can also be reduced to a column vector:
``` postgres-console
select print(b) as b, 'plus_monoid_int32' as monoid, print(reduce_cols(b)) as reduce_cols from test_fixture;
┌──────────────────────────┬───────────────────┬─────────────┐
│            b             │      monoid       │ reduce_cols │
├──────────────────────────┼───────────────────┼─────────────┤
│        0   1   2   3     │ plus_monoid_int32 │             │
│     ────────────────     │                   │    ───      │
│   0│  23       0         │                   │  0│ 23      │
│   1│          10         │                   │  1│ 10      │
│   2│          20  40     │                   │  2│ 60      │
│   3│     -19  15  34     │                   │  3│ 30      │
│                          │                   │             │
└──────────────────────────┴───────────────────┴─────────────┘
(1 row)

```
To reduce a row vector:
``` postgres-console
select print(b) as b, 'plus_monoid_int32' as monoid, print(reduce_rows(b)) as reduce_rows from test_fixture;
┌──────────────────────────┬───────────────────┬─────────────┐
│            b             │      monoid       │ reduce_rows │
├──────────────────────────┼───────────────────┼─────────────┤
│        0   1   2   3     │ plus_monoid_int32 │             │
│     ────────────────     │                   │    ───      │
│   0│  23       0         │                   │  0│ 23      │
│   1│          10         │                   │  1│-19      │
│   2│          20  40     │                   │  2│ 45      │
│   3│     -19  15  34     │                   │  3│ 74      │
│                          │                   │             │
└──────────────────────────┴───────────────────┴─────────────┘
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
select print(a) as a, semiring, print(b) as b, print(mxm(a, b)) as "mxm(a, b)" from test_fixture;
┌──────────────────────────┬──────────────────┬──────────────────────────┬──────────────────────────┐
│            a             │     semiring     │            b             │        mxm(a, b)         │
├──────────────────────────┼──────────────────┼──────────────────────────┼──────────────────────────┤
│        0   1   2   3     │ plus_times_int32 │        0   1   2   3     │        0   1   2   3     │
│     ────────────────     │                  │     ────────────────     │     ────────────────     │
│   0│           0 -35     │                  │   0│  23       0         │   0│    -103 -13  90     │
│   1│          10         │                  │   1│          10         │   1│         -56-112     │
│   2│              40     │                  │   2│          20  40     │   2│       8  88  80     │
│   3│     -19  15  31     │                  │   3│     -19  15  34     │   3│     -77  63 118     │
│                          │                  │                          │                          │
└──────────────────────────┴──────────────────┴──────────────────────────┴──────────────────────────┘
(1 row)

select draw(a) as binop_a_source, draw(b) as binop_b_source, draw(mxm(a, b)) as binop_c_source from test_fixture \gset
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="271pt" height="92pt"
 viewBox="0.00 0.00 271.45 91.56" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 87.56)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-87.56 267.45,-87.56 267.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-13.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-53.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-51.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M29.6,-22.53C39.72,-27.94 53.99,-35.56 65.84,-41.89"/>
<polygon fill="black" stroke="black" points="64.24,-45 74.71,-46.63 67.54,-38.83 64.24,-45"/>
<text text-anchor="middle" x="52.11" y="-39.16" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="166.78" cy="-20.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="166.78" y="-18.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M31.14,-16.04C56.59,-16.9 109.41,-18.67 140.79,-19.72"/>
<polygon fill="black" stroke="black" points="140.81,-23.22 150.92,-20.06 141.04,-16.23 140.81,-23.22"/>
<text text-anchor="middle" x="88.67" y="-21.16" font-family="Times,serif" font-size="8.00">&#45;35</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M104.21,-51.2C112.88,-49.47 123.97,-46.7 133.23,-42.56 137.36,-40.71 141.54,-38.34 145.46,-35.84"/>
<polygon fill="black" stroke="black" points="147.74,-38.52 154,-29.98 143.78,-32.75 147.74,-38.52"/>
<text text-anchor="middle" x="127.73" y="-49.16" font-family="Times,serif" font-size="8.00">40</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge6" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M151.32,-23.86C142.68,-26.08 131.59,-29.37 122.23,-33.56 118.38,-35.27 114.45,-37.37 110.7,-39.55"/>
<polygon fill="black" stroke="black" points="108.59,-36.74 101.93,-44.97 112.27,-42.69 108.59,-36.74"/>
<text text-anchor="middle" x="127.73" y="-36.16" font-family="Times,serif" font-size="8.00">15</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M160.28,-35.04C158.46,-44.71 160.62,-54.11 166.78,-54.11 170.63,-54.11 172.92,-50.44 173.65,-45.32"/>
<polygon fill="black" stroke="black" points="177.14,-44.91 173.28,-35.04 170.14,-45.16 177.14,-44.91"/>
<text text-anchor="middle" x="166.78" y="-56.71" font-family="Times,serif" font-size="8.00">31</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="247.89" cy="-60.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="247.89" y="-58.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge5" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M181.2,-27.34C193.09,-33.35 210.69,-42.25 224.68,-49.32"/>
<polygon fill="black" stroke="black" points="223.17,-52.48 233.67,-53.87 226.33,-46.24 223.17,-52.48"/>
<text text-anchor="middle" x="207.34" y="-46.16" font-family="Times,serif" font-size="8.00">&#45;19</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M232.6,-64.8C213.87,-69.8 180.03,-76.9 151.23,-72.56 138.36,-70.62 124.47,-66.51 113.14,-62.61"/>
<polygon fill="black" stroke="black" points="114.16,-59.26 103.57,-59.16 111.79,-65.85 114.16,-59.26"/>
<text text-anchor="middle" x="166.78" y="-77.16" font-family="Times,serif" font-size="8.00">10</text>
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
<svg width="271pt" height="93pt"
 viewBox="0.00 0.00 271.45 93.11" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 89.11)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-89.11 267.45,-89.11 267.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-32.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-30.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 0&#45;&gt;0 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;0</title>
<path fill="none" stroke="black" d="M9.4,-47.04C7.67,-56.71 9.72,-66.11 15.56,-66.11 19.2,-66.11 21.37,-62.44 22.06,-57.32"/>
<polygon fill="black" stroke="black" points="25.55,-56.92 21.71,-47.04 18.56,-57.16 25.55,-56.92"/>
<text text-anchor="middle" x="15.56" y="-68.71" font-family="Times,serif" font-size="8.00">23</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-32.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-30.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M31.29,-32.56C40.35,-32.56 52.19,-32.56 62.68,-32.56"/>
<polygon fill="black" stroke="black" points="62.95,-36.06 72.95,-32.56 62.95,-29.06 62.95,-36.06"/>
<text text-anchor="middle" x="52.11" y="-35.16" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2&#45;&gt;2 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;2</title>
<path fill="none" stroke="black" d="M82.51,-47.04C80.78,-56.71 82.83,-66.11 88.67,-66.11 92.32,-66.11 94.49,-62.44 95.18,-57.32"/>
<polygon fill="black" stroke="black" points="98.66,-56.92 94.82,-47.04 91.67,-57.16 98.66,-56.92"/>
<text text-anchor="middle" x="88.67" y="-68.71" font-family="Times,serif" font-size="8.00">20</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="166.78" cy="-42.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="166.78" y="-40.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge5" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M103.4,-37.77C109.13,-39.68 115.91,-41.61 122.23,-42.56 128.21,-43.45 134.7,-43.79 140.81,-43.83"/>
<polygon fill="black" stroke="black" points="141.25,-47.32 151.18,-43.63 141.12,-40.32 141.25,-47.32"/>
<text text-anchor="middle" x="127.73" y="-46.16" font-family="Times,serif" font-size="8.00">40</text>
</g>
<!-- 1 -->
<g id="node3" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="247.89" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="247.89" y="-13.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M232.5,-12.21C213.67,-8.36 179.7,-3.2 151.23,-8.56 137.8,-11.08 123.54,-16.45 112.13,-21.47"/>
<polygon fill="black" stroke="black" points="110.55,-18.35 102.93,-25.72 113.48,-24.71 110.55,-18.35"/>
<text text-anchor="middle" x="166.78" y="-11.16" font-family="Times,serif" font-size="8.00">10</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M151.68,-37.84C146.01,-36.16 139.37,-34.46 133.23,-33.56 127.24,-32.67 120.74,-32.25 114.63,-32.07"/>
<polygon fill="black" stroke="black" points="114.29,-28.57 104.26,-32 114.24,-35.57 114.29,-28.57"/>
<text text-anchor="middle" x="127.73" y="-36.16" font-family="Times,serif" font-size="8.00">15</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge6" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M181.93,-37.73C193.4,-33.81 209.82,-28.21 223.29,-23.61"/>
<polygon fill="black" stroke="black" points="224.54,-26.88 232.87,-20.34 222.28,-20.26 224.54,-26.88"/>
<text text-anchor="middle" x="207.34" y="-33.16" font-family="Times,serif" font-size="8.00">&#45;19</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M160.28,-57.04C158.46,-66.71 160.62,-76.11 166.78,-76.11 170.63,-76.11 172.92,-72.44 173.65,-67.32"/>
<polygon fill="black" stroke="black" points="177.14,-66.91 173.28,-57.04 170.14,-67.16 177.14,-66.91"/>
<text text-anchor="middle" x="166.78" y="-78.71" font-family="Times,serif" font-size="8.00">34</text>
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
<svg width="284pt" height="157pt"
 viewBox="0.00 0.00 284.45 156.89" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 152.89)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-152.89 280.45,-152.89 280.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-22.89" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-20.99" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="101.67" cy="-87.89" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="101.67" y="-85.99" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 0&#45;&gt;1 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;1</title>
<path fill="none" stroke="black" d="M28.25,-31.93C41.73,-42.35 64.13,-59.66 80.45,-72.27"/>
<polygon fill="black" stroke="black" points="78.75,-75.38 88.8,-78.72 83.03,-69.84 78.75,-75.38"/>
<text text-anchor="middle" x="58.61" y="-63.49" font-family="Times,serif" font-size="8.00">&#45;103</text>
</g>
<!-- 2 -->
<g id="node3" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="182.78" cy="-32.89" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="182.78" y="-30.99" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M31.22,-23.78C59.44,-25.49 121.6,-29.25 156.59,-31.37"/>
<polygon fill="black" stroke="black" points="156.72,-34.88 166.91,-31.99 157.14,-27.9 156.72,-34.88"/>
<text text-anchor="middle" x="101.67" y="-30.49" font-family="Times,serif" font-size="8.00">&#45;13</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="260.89" cy="-64.89" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="260.89" y="-62.99" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge3" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M30.6,-18.31C61.6,-8.93 137.95,10.13 198.34,-7.89 217.87,-13.72 234.44,-30.21 245.46,-43.97"/>
<polygon fill="black" stroke="black" points="242.75,-46.19 251.57,-52.06 248.33,-41.97 242.75,-46.19"/>
<text text-anchor="middle" x="142.23" y="-3.49" font-family="Times,serif" font-size="8.00">90</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M116.81,-82.73C126.42,-78.87 139.16,-73.03 149.23,-65.89 154.83,-61.92 160.31,-56.93 165.13,-52.05"/>
<polygon fill="black" stroke="black" points="167.81,-54.3 172.09,-44.61 162.7,-49.52 167.81,-54.3"/>
<text text-anchor="middle" x="142.23" y="-76.49" font-family="Times,serif" font-size="8.00">&#45;56</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M110.1,-101.48C115.72,-110.23 124.33,-120.88 135.23,-125.89 172.42,-143 192.23,-146.95 227.34,-125.89 240.46,-118.02 248.59,-102.86 253.44,-89.62"/>
<polygon fill="black" stroke="black" points="256.8,-90.62 256.54,-80.03 250.14,-88.47 256.8,-90.62"/>
<text text-anchor="middle" x="182.78" y="-142.49" font-family="Times,serif" font-size="8.00">&#45;112</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge6" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M168.16,-38.66C158.55,-43.03 145.6,-49.51 135.23,-56.89 130.01,-60.6 124.8,-65.12 120.12,-69.54"/>
<polygon fill="black" stroke="black" points="117.49,-67.22 112.84,-76.74 122.41,-72.19 117.49,-67.22"/>
<text text-anchor="middle" x="142.23" y="-59.49" font-family="Times,serif" font-size="8.00">8</text>
</g>
<!-- 2&#45;&gt;2 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;2</title>
<path fill="none" stroke="black" d="M176.28,-47.38C174.46,-57.04 176.62,-66.45 182.78,-66.45 186.63,-66.45 188.92,-62.78 189.65,-57.66"/>
<polygon fill="black" stroke="black" points="193.14,-57.25 189.28,-47.38 186.14,-57.5 193.14,-57.25"/>
<text text-anchor="middle" x="182.78" y="-69.05" font-family="Times,serif" font-size="8.00">88</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge8" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M198.45,-30.65C207.16,-29.94 218.25,-30.14 227.34,-33.89 233.35,-36.38 238.89,-40.52 243.64,-44.99"/>
<polygon fill="black" stroke="black" points="241.4,-47.72 250.81,-52.57 246.48,-42.91 241.4,-47.72"/>
<text text-anchor="middle" x="221.84" y="-36.49" font-family="Times,serif" font-size="8.00">80</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge9" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M245.99,-70.62C233.7,-75.39 215.15,-81.88 198.34,-84.89 176.84,-88.75 171.06,-86.21 149.23,-86.89 142.23,-87.11 134.63,-87.3 127.65,-87.45"/>
<polygon fill="black" stroke="black" points="127.28,-83.95 117.35,-87.65 127.42,-90.95 127.28,-83.95"/>
<text text-anchor="middle" x="182.78" y="-89.49" font-family="Times,serif" font-size="8.00">&#45;77</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge10" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M246.4,-59.22C235.4,-54.6 219.61,-47.96 206.63,-42.5"/>
<polygon fill="black" stroke="black" points="207.96,-39.26 197.39,-38.61 205.25,-45.72 207.96,-39.26"/>
<text text-anchor="middle" x="221.84" y="-52.49" font-family="Times,serif" font-size="8.00">63</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge11" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M254.4,-79.38C252.57,-89.04 254.73,-98.45 260.89,-98.45 264.74,-98.45 267.03,-94.78 267.76,-89.66"/>
<polygon fill="black" stroke="black" points="271.25,-89.25 267.39,-79.38 264.26,-89.5 271.25,-89.25"/>
<text text-anchor="middle" x="260.89" y="-101.05" font-family="Times,serif" font-size="8.00">118</text>
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
select print(a) as a, '@' as "@", print(b) as b, print(a @ b) as "a @ b" from test_fixture;
┌──────────────────────────┬───┬──────────────────────────┬──────────────────────────┐
│            a             │ @ │            b             │          a @ b           │
├──────────────────────────┼───┼──────────────────────────┼──────────────────────────┤
│        0   1   2   3     │ @ │        0   1   2   3     │        0   1   2   3     │
│     ────────────────     │   │     ────────────────     │     ────────────────     │
│   0│           0 -35     │   │   0│  23       0         │   0│    -103 -13  90     │
│   1│          10         │   │   1│          10         │   1│         -56-112     │
│   2│              40     │   │   2│          20  40     │   2│       8  88  80     │
│   3│     -19  15  31     │   │   3│     -19  15  34     │   3│     -77  63 118     │
│                          │   │                          │                          │
└──────────────────────────┴───┴──────────────────────────┴──────────────────────────┘
(1 row)

```
## Matrix Vector Multiplication

Matrices can be multipled by vectors on the right taking the linear
combination of the matrices columns using the vectors elements as
coefficients:
``` postgres-console
select print(a) as a, '@' as "@", semiring, print(u) as u, print(mxv(a, u)) as "mxv(a, u)" from test_fixture;
┌──────────────────────────┬───┬──────────────────┬───────────┬───────────┐
│            a             │ @ │     semiring     │     u     │ mxv(a, u) │
├──────────────────────────┼───┼──────────────────┼───────────┼───────────┤
│        0   1   2   3     │ @ │ plus_times_int32 │           │           │
│     ────────────────     │   │                  │    ───    │    ───    │
│   0│           0 -35     │   │                  │  0│       │  0│       │
│   1│          10         │   │                  │  1│  2    │  1│       │
│   2│              40     │   │                  │  2│       │  2│       │
│   3│     -19  15  31     │   │                  │  3│       │  3│-38    │
│                          │   │                  │           │           │
└──────────────────────────┴───┴──────────────────┴───────────┴───────────┘
(1 row)

```
From a graph standpoint, matrix vector multiplication is used to
"pull" back to adjacent nodes from their incoming edges.  When
iterated, it forms the basis for working back along incoming links.
``` postgres-console
select draw(a) as binop_a_source, draw(u) as binop_b_source, draw(mxv(a, u)) as binop_c_source from test_fixture \gset
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="271pt" height="92pt"
 viewBox="0.00 0.00 271.45 91.56" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 87.56)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-87.56 267.45,-87.56 267.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-13.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-53.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-51.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M29.6,-22.53C39.72,-27.94 53.99,-35.56 65.84,-41.89"/>
<polygon fill="black" stroke="black" points="64.24,-45 74.71,-46.63 67.54,-38.83 64.24,-45"/>
<text text-anchor="middle" x="52.11" y="-39.16" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="166.78" cy="-20.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="166.78" y="-18.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M31.14,-16.04C56.59,-16.9 109.41,-18.67 140.79,-19.72"/>
<polygon fill="black" stroke="black" points="140.81,-23.22 150.92,-20.06 141.04,-16.23 140.81,-23.22"/>
<text text-anchor="middle" x="88.67" y="-21.16" font-family="Times,serif" font-size="8.00">&#45;35</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M104.21,-51.2C112.88,-49.47 123.97,-46.7 133.23,-42.56 137.36,-40.71 141.54,-38.34 145.46,-35.84"/>
<polygon fill="black" stroke="black" points="147.74,-38.52 154,-29.98 143.78,-32.75 147.74,-38.52"/>
<text text-anchor="middle" x="127.73" y="-49.16" font-family="Times,serif" font-size="8.00">40</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge6" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M151.32,-23.86C142.68,-26.08 131.59,-29.37 122.23,-33.56 118.38,-35.27 114.45,-37.37 110.7,-39.55"/>
<polygon fill="black" stroke="black" points="108.59,-36.74 101.93,-44.97 112.27,-42.69 108.59,-36.74"/>
<text text-anchor="middle" x="127.73" y="-36.16" font-family="Times,serif" font-size="8.00">15</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M160.28,-35.04C158.46,-44.71 160.62,-54.11 166.78,-54.11 170.63,-54.11 172.92,-50.44 173.65,-45.32"/>
<polygon fill="black" stroke="black" points="177.14,-44.91 173.28,-35.04 170.14,-45.16 177.14,-44.91"/>
<text text-anchor="middle" x="166.78" y="-56.71" font-family="Times,serif" font-size="8.00">31</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="247.89" cy="-60.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="247.89" y="-58.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge5" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M181.2,-27.34C193.09,-33.35 210.69,-42.25 224.68,-49.32"/>
<polygon fill="black" stroke="black" points="223.17,-52.48 233.67,-53.87 226.33,-46.24 223.17,-52.48"/>
<text text-anchor="middle" x="207.34" y="-46.16" font-family="Times,serif" font-size="8.00">&#45;19</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M232.6,-64.8C213.87,-69.8 180.03,-76.9 151.23,-72.56 138.36,-70.62 124.47,-66.51 113.14,-62.61"/>
<polygon fill="black" stroke="black" points="114.16,-59.26 103.57,-59.16 111.79,-65.85 114.16,-59.26"/>
<text text-anchor="middle" x="166.78" y="-77.16" font-family="Times,serif" font-size="8.00">10</text>
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
<svg width="78pt" height="177pt"
 viewBox="0.00 0.00 78.00 177.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 173)">
<title>vector</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-173 74,-173 74,4 -4,4"/>
<g id="clust1" class="cluster">
<title>cluster_vector</title>
<polygon fill="lightgray" stroke="black" stroke-dasharray="5,2" points="8,-8 8,-161 62,-161 62,-8 8,-8"/>
</g>
<!-- node0 -->
<g id="node1" class="node">
<title>node0</title>
<polygon fill="none" stroke="black" points="37,-153 33,-153 33,-149 37,-149 37,-153"/>
</g>
<!-- node1 -->
<g id="node2" class="node">
<title>node1</title>
<polygon fill="none" stroke="black" points="37,-113 33,-113 33,-109 37,-109 37,-113"/>
</g>
<!-- node0&#45;&gt;node1 -->
<!-- node2 -->
<g id="node3" class="node">
<title>node2</title>
<polygon fill="none" stroke="black" points="37,-73 33,-73 33,-69 37,-69 37,-73"/>
</g>
<!-- node1&#45;&gt;node2 -->
<!-- node3 -->
<g id="node4" class="node">
<title>node3</title>
<polygon fill="none" stroke="black" points="54,-33 16,-33 16,-16 54,-16 54,-33"/>
<text text-anchor="middle" x="35" y="-22.6" font-family="Times,serif" font-size="8.00">3:&#45;38</text>
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
select print(a) as a, '@' as "@", print(u) as u, print(a @ u) as "a @ u" from test_fixture;
┌──────────────────────────┬───┬───────────┬───────────┐
│            a             │ @ │     u     │   a @ u   │
├──────────────────────────┼───┼───────────┼───────────┤
│        0   1   2   3     │ @ │           │           │
│     ────────────────     │   │    ───    │    ───    │
│   0│           0 -35     │   │  0│       │  0│       │
│   1│          10         │   │  1│  2    │  1│       │
│   2│              40     │   │  2│       │  2│       │
│   3│     -19  15  31     │   │  3│       │  3│-38    │
│                          │   │           │           │
└──────────────────────────┴───┴───────────┴───────────┘
(1 row)

```
## Vector Matrix Multiplication

Matrices can be multipled by vectors on the right taking the linear
combination of the matrices rows using the vectors elements as
coefficients:
``` postgres-console
select print(v) as v, semiring, print(b) as b, print(vxm(v, b, semiring)) as "vxm(v, b, semiring)" from test_fixture;
┌───────────┬──────────────────┬──────────────────────────┬─────────────────────┐
│     v     │     semiring     │            b             │ vxm(v, b, semiring) │
├───────────┼──────────────────┼──────────────────────────┼─────────────────────┤
│           │ plus_times_int32 │        0   1   2   3     │                     │
│    ───    │                  │     ────────────────     │    ───              │
│  0│       │                  │   0│  23       0         │  0│                 │
│  1│  3    │                  │   1│          10         │  1│                 │
│  2│  3    │                  │   2│          20  40     │  2│ 90              │
│  3│       │                  │   3│     -19  15  34     │  3│120              │
│           │                  │                          │                     │
└───────────┴──────────────────┴──────────────────────────┴─────────────────────┘
(1 row)

```
From a graph standpoint, vector matrix multiplication is used to
"push" forward to adjacent nodes from their outgoing edges.  When
iterated, it forms the basis for working forward along outgoing
edges.
``` postgres-console
select draw(v) as binop_a_source, draw(b) as binop_b_source, draw(vxm(v, b)) as binop_c_source from test_fixture \gset
```
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
<svg width="271pt" height="93pt"
 viewBox="0.00 0.00 271.45 93.11" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 89.11)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-89.11 267.45,-89.11 267.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-32.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-30.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 0&#45;&gt;0 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;0</title>
<path fill="none" stroke="black" d="M9.4,-47.04C7.67,-56.71 9.72,-66.11 15.56,-66.11 19.2,-66.11 21.37,-62.44 22.06,-57.32"/>
<polygon fill="black" stroke="black" points="25.55,-56.92 21.71,-47.04 18.56,-57.16 25.55,-56.92"/>
<text text-anchor="middle" x="15.56" y="-68.71" font-family="Times,serif" font-size="8.00">23</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-32.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-30.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M31.29,-32.56C40.35,-32.56 52.19,-32.56 62.68,-32.56"/>
<polygon fill="black" stroke="black" points="62.95,-36.06 72.95,-32.56 62.95,-29.06 62.95,-36.06"/>
<text text-anchor="middle" x="52.11" y="-35.16" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2&#45;&gt;2 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;2</title>
<path fill="none" stroke="black" d="M82.51,-47.04C80.78,-56.71 82.83,-66.11 88.67,-66.11 92.32,-66.11 94.49,-62.44 95.18,-57.32"/>
<polygon fill="black" stroke="black" points="98.66,-56.92 94.82,-47.04 91.67,-57.16 98.66,-56.92"/>
<text text-anchor="middle" x="88.67" y="-68.71" font-family="Times,serif" font-size="8.00">20</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="166.78" cy="-42.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="166.78" y="-40.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge5" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M103.4,-37.77C109.13,-39.68 115.91,-41.61 122.23,-42.56 128.21,-43.45 134.7,-43.79 140.81,-43.83"/>
<polygon fill="black" stroke="black" points="141.25,-47.32 151.18,-43.63 141.12,-40.32 141.25,-47.32"/>
<text text-anchor="middle" x="127.73" y="-46.16" font-family="Times,serif" font-size="8.00">40</text>
</g>
<!-- 1 -->
<g id="node3" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="247.89" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="247.89" y="-13.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M232.5,-12.21C213.67,-8.36 179.7,-3.2 151.23,-8.56 137.8,-11.08 123.54,-16.45 112.13,-21.47"/>
<polygon fill="black" stroke="black" points="110.55,-18.35 102.93,-25.72 113.48,-24.71 110.55,-18.35"/>
<text text-anchor="middle" x="166.78" y="-11.16" font-family="Times,serif" font-size="8.00">10</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M151.68,-37.84C146.01,-36.16 139.37,-34.46 133.23,-33.56 127.24,-32.67 120.74,-32.25 114.63,-32.07"/>
<polygon fill="black" stroke="black" points="114.29,-28.57 104.26,-32 114.24,-35.57 114.29,-28.57"/>
<text text-anchor="middle" x="127.73" y="-36.16" font-family="Times,serif" font-size="8.00">15</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge6" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M181.93,-37.73C193.4,-33.81 209.82,-28.21 223.29,-23.61"/>
<polygon fill="black" stroke="black" points="224.54,-26.88 232.87,-20.34 222.28,-20.26 224.54,-26.88"/>
<text text-anchor="middle" x="207.34" y="-33.16" font-family="Times,serif" font-size="8.00">&#45;19</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M160.28,-57.04C158.46,-66.71 160.62,-76.11 166.78,-76.11 170.63,-76.11 172.92,-72.44 173.65,-67.32"/>
<polygon fill="black" stroke="black" points="177.14,-66.91 173.28,-57.04 170.14,-67.16 177.14,-66.91"/>
<text text-anchor="middle" x="166.78" y="-78.71" font-family="Times,serif" font-size="8.00">34</text>
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
<svg width="82pt" height="190pt"
 viewBox="0.00 0.00 82.00 190.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 186)">
<title>vector</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-186 78,-186 78,4 -4,4"/>
<g id="clust1" class="cluster">
<title>cluster_vector</title>
<polygon fill="lightgray" stroke="black" stroke-dasharray="5,2" points="8,-8 8,-174 66,-174 66,-8 8,-8"/>
</g>
<!-- node0 -->
<g id="node1" class="node">
<title>node0</title>
<polygon fill="none" stroke="black" points="39,-166 35,-166 35,-162 39,-162 39,-166"/>
</g>
<!-- node1 -->
<g id="node2" class="node">
<title>node1</title>
<polygon fill="none" stroke="black" points="39,-126 35,-126 35,-122 39,-122 39,-126"/>
</g>
<!-- node0&#45;&gt;node1 -->
<!-- node2 -->
<g id="node3" class="node">
<title>node2</title>
<polygon fill="none" stroke="black" points="54.5,-86 19.5,-86 19.5,-69 54.5,-69 54.5,-86"/>
<text text-anchor="middle" x="37" y="-75.6" font-family="Times,serif" font-size="8.00">2:90</text>
</g>
<!-- node1&#45;&gt;node2 -->
<!-- node3 -->
<g id="node4" class="node">
<title>node3</title>
<polygon fill="none" stroke="black" points="57.5,-33 16.5,-33 16.5,-16 57.5,-16 57.5,-33"/>
<text text-anchor="middle" x="37" y="-22.6" font-family="Times,serif" font-size="8.00">3:120</text>
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
select print(v) as v, '@' as "@", print(b) as b, print(v @ b) as "v @ b" from test_fixture;
┌───────────┬───┬──────────────────────────┬───────────┐
│     v     │ @ │            b             │   v @ b   │
├───────────┼───┼──────────────────────────┼───────────┤
│           │ @ │        0   1   2   3     │           │
│    ───    │   │     ────────────────     │    ───    │
│  0│       │   │   0│  23       0         │  0│       │
│  1│  3    │   │   1│          10         │  1│       │
│  2│  3    │   │   2│          20  40     │  2│ 90    │
│  3│       │   │   3│     -19  15  34     │  3│120    │
│           │   │                          │           │
└───────────┴───┴──────────────────────────┴───────────┘
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
┌──────────────────────────┬───────────────┬──────────────────────────┐
│            a             │ indexunaryop  │         selected         │
├──────────────────────────┼───────────────┼──────────────────────────┤
│        0   1   2   3     │ valuegt_int32 │        0   1   2   3     │
│     ────────────────     │               │     ────────────────     │
│   0│           0 -35     │               │   0│                     │
│   1│          10         │               │   1│          10         │
│   2│              40     │               │   2│              40     │
│   3│     -19  15  31     │               │   3│          15  31     │
│                          │               │                          │
└──────────────────────────┴───────────────┴──────────────────────────┘
(1 row)

select draw(a) as uop_a_source, draw(choose(a, indexunaryop, 1)) as uop_b_source from test_fixture \gset
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 45%;">
        <!-- Diagram A -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="271pt" height="92pt"
 viewBox="0.00 0.00 271.45 91.56" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 87.56)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-87.56 267.45,-87.56 267.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-13.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-53.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-51.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M29.6,-22.53C39.72,-27.94 53.99,-35.56 65.84,-41.89"/>
<polygon fill="black" stroke="black" points="64.24,-45 74.71,-46.63 67.54,-38.83 64.24,-45"/>
<text text-anchor="middle" x="52.11" y="-39.16" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="166.78" cy="-20.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="166.78" y="-18.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M31.14,-16.04C56.59,-16.9 109.41,-18.67 140.79,-19.72"/>
<polygon fill="black" stroke="black" points="140.81,-23.22 150.92,-20.06 141.04,-16.23 140.81,-23.22"/>
<text text-anchor="middle" x="88.67" y="-21.16" font-family="Times,serif" font-size="8.00">&#45;35</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M104.21,-51.2C112.88,-49.47 123.97,-46.7 133.23,-42.56 137.36,-40.71 141.54,-38.34 145.46,-35.84"/>
<polygon fill="black" stroke="black" points="147.74,-38.52 154,-29.98 143.78,-32.75 147.74,-38.52"/>
<text text-anchor="middle" x="127.73" y="-49.16" font-family="Times,serif" font-size="8.00">40</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge6" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M151.32,-23.86C142.68,-26.08 131.59,-29.37 122.23,-33.56 118.38,-35.27 114.45,-37.37 110.7,-39.55"/>
<polygon fill="black" stroke="black" points="108.59,-36.74 101.93,-44.97 112.27,-42.69 108.59,-36.74"/>
<text text-anchor="middle" x="127.73" y="-36.16" font-family="Times,serif" font-size="8.00">15</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M160.28,-35.04C158.46,-44.71 160.62,-54.11 166.78,-54.11 170.63,-54.11 172.92,-50.44 173.65,-45.32"/>
<polygon fill="black" stroke="black" points="177.14,-44.91 173.28,-35.04 170.14,-45.16 177.14,-44.91"/>
<text text-anchor="middle" x="166.78" y="-56.71" font-family="Times,serif" font-size="8.00">31</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="247.89" cy="-60.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="247.89" y="-58.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge5" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M181.2,-27.34C193.09,-33.35 210.69,-42.25 224.68,-49.32"/>
<polygon fill="black" stroke="black" points="223.17,-52.48 233.67,-53.87 226.33,-46.24 223.17,-52.48"/>
<text text-anchor="middle" x="207.34" y="-46.16" font-family="Times,serif" font-size="8.00">&#45;19</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M232.6,-64.8C213.87,-69.8 180.03,-76.9 151.23,-72.56 138.36,-70.62 124.47,-66.51 113.14,-62.61"/>
<polygon fill="black" stroke="black" points="114.16,-59.26 103.57,-59.16 111.79,-65.85 114.16,-59.26"/>
<text text-anchor="middle" x="166.78" y="-77.16" font-family="Times,serif" font-size="8.00">10</text>
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
<svg width="195pt" height="66pt"
 viewBox="0.00 0.00 195.34 66.11" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 62.11)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-62.11 191.34,-62.11 191.34,4 -4,4"/>
<!-- 1 -->
<g id="node1" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-13.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="93.67" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="93.67" y="-13.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge1" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M31.23,-15.56C41.55,-15.56 55.61,-15.56 67.68,-15.56"/>
<polygon fill="black" stroke="black" points="68.04,-19.06 78.04,-15.56 68.04,-12.06 68.04,-19.06"/>
<text text-anchor="middle" x="54.61" y="-18.16" font-family="Times,serif" font-size="8.00">10</text>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="171.78" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="171.78" y="-13.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge2" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M109.34,-15.56C119.66,-15.56 133.73,-15.56 145.79,-15.56"/>
<polygon fill="black" stroke="black" points="146.15,-19.06 156.15,-15.56 146.15,-12.06 146.15,-19.06"/>
<text text-anchor="middle" x="132.73" y="-18.16" font-family="Times,serif" font-size="8.00">40</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M157.51,-8.93C151.74,-6.4 144.8,-3.82 138.23,-2.56 131.31,-1.23 123.92,-2.4 117.23,-4.5"/>
<polygon fill="black" stroke="black" points="115.5,-1.42 107.46,-8.33 118.05,-7.94 115.5,-1.42"/>
<text text-anchor="middle" x="132.73" y="-5.16" font-family="Times,serif" font-size="8.00">15</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge4" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M165.28,-30.04C163.46,-39.71 165.62,-49.11 171.78,-49.11 175.63,-49.11 177.92,-45.44 178.65,-40.32"/>
<polygon fill="black" stroke="black" points="182.14,-39.91 178.28,-30.04 175.14,-40.16 182.14,-39.91"/>
<text text-anchor="middle" x="171.78" y="-51.71" font-family="Times,serif" font-size="8.00">31</text>
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
┌──────────────────────────┬──────────────────────────┬──────────────────────────┬──────────────────────────┐
│          a > 1           │          a >= 1          │          a < 1           │          a <= 1          │
├──────────────────────────┼──────────────────────────┼──────────────────────────┼──────────────────────────┤
│        0   1   2   3     │        0   1   2   3     │        0   1   2   3     │        0   1   2   3     │
│     ────────────────     │     ────────────────     │     ────────────────     │     ────────────────     │
│   0│                     │   0│                     │   0│           0 -35     │   0│           0 -35     │
│   1│          10         │   1│          10         │   1│                     │   1│                     │
│   2│              40     │   2│              40     │   2│                     │   2│                     │
│   3│          15  31     │   3│          15  31     │   3│     -19             │   3│     -19             │
│                          │                          │                          │                          │
└──────────────────────────┴──────────────────────────┴──────────────────────────┴──────────────────────────┘
(1 row)

```
A useful select operator is `triu`, it select only upper triangular
values, this turns your graph into a direct acyclic graph (DAG) by
removing all the links "back" from higher number nodes to lower.
``` postgres-console
select print(random_matrix('int8', 8, 8, 0.5, 42) % 42) as matrix,
       print(choose(random_matrix('uint8', 8, 8, 1, 42), 'triu', 0) % 42) as triu from test_fixture;
┌──────────────────────────────────────────┬──────────────────────────────────────────┐
│                  matrix                  │                   triu                   │
├──────────────────────────────────────────┼──────────────────────────────────────────┤
│        0   1   2   3   4   5   6   7     │        0   1   2   3   4   5   6   7     │
│     ────────────────────────────────     │     ────────────────────────────────     │
│   0│  23       0                 -35     │   0│  23  41   0                  11     │
│   1│                   1  24      23     │   1│          25  23   1  24      23     │
│   2│      28  20  -1      29             │   2│          20   3  39   5             │
│   3│          27  -3     -19     -12     │   3│               1  15  10  14  37     │
│   4│          26      20  38             │   4│                  20  23  23         │
│   5│  30   0  10  22                     │   5│                          26   2     │
│   6│  34          40                     │   6│                                     │
│   7│     -30 -25          30     -12     │   7│                              34     │
│                                          │                                          │
└──────────────────────────────────────────┴──────────────────────────────────────────┘
(1 row)

select draw(random_matrix('int8', 8, 8, 0.5, 42) % 42) as uop_a_source,
       draw(choose(random_matrix('int8', 8, 8, 0.5, 42) % 42, 'triu', 0)) as uop_b_source
       from test_fixture \gset
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 45%;">
        <!-- Diagram A -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="512pt" height="473pt"
 viewBox="0.00 0.00 511.79 473.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 469)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-469 507.79,-469 507.79,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="93.67" cy="-105" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="93.67" y="-103.1" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 0&#45;&gt;0 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;0</title>
<path fill="none" stroke="black" d="M87.51,-119.49C85.78,-129.15 87.83,-138.56 93.67,-138.56 97.32,-138.56 99.49,-134.88 100.18,-129.77"/>
<polygon fill="black" stroke="black" points="103.66,-129.36 99.82,-119.49 96.67,-129.6 103.66,-129.36"/>
<text text-anchor="middle" x="93.67" y="-141.16" font-family="Times,serif" font-size="8.00">23</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="166.78" cy="-237" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="166.78" y="-235.1" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M99.15,-119.78C104.57,-136.65 114.57,-164.77 127.23,-187 133.39,-197.83 141.68,-208.9 149,-217.86"/>
<polygon fill="black" stroke="black" points="146.5,-220.33 155.61,-225.73 151.85,-215.82 146.5,-220.33"/>
<text text-anchor="middle" x="130.23" y="-198.6" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 7 -->
<g id="node3" class="node">
<title>7</title>
<ellipse fill="none" stroke="black" cx="329.01" cy="-287" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="329.01" y="-285.1" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 0&#45;&gt;7 -->
<g id="edge3" class="edge">
<title>0&#45;&gt;7</title>
<path fill="none" stroke="black" d="M106.56,-114.28C144.24,-143.67 261.27,-234.95 308.22,-271.57"/>
<polygon fill="black" stroke="black" points="306.1,-274.35 316.14,-277.74 310.41,-268.83 306.1,-274.35"/>
<text text-anchor="middle" x="207.34" y="-201.6" font-family="Times,serif" font-size="8.00">&#45;35</text>
</g>
<!-- 2&#45;&gt;2 -->
<g id="edge8" class="edge">
<title>2&#45;&gt;2</title>
<path fill="none" stroke="black" d="M160.63,-251.49C158.89,-261.15 160.95,-270.56 166.78,-270.56 170.43,-270.56 172.6,-266.88 173.29,-261.77"/>
<polygon fill="black" stroke="black" points="176.78,-261.36 172.94,-251.49 169.78,-261.6 176.78,-261.36"/>
<text text-anchor="middle" x="166.78" y="-273.16" font-family="Times,serif" font-size="8.00">20</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="247.89" cy="-269" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="247.89" y="-267.1" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M181.56,-242.57C193.25,-247.3 210.25,-254.18 223.98,-259.73"/>
<polygon fill="black" stroke="black" points="222.69,-262.98 233.27,-263.49 225.31,-256.5 222.69,-262.98"/>
<text text-anchor="middle" x="207.34" y="-256.6" font-family="Times,serif" font-size="8.00">28</text>
</g>
<!-- 5 -->
<g id="node6" class="node">
<title>5</title>
<ellipse fill="none" stroke="black" cx="407.12" cy="-244" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="407.12" y="-242.1" font-family="Times,serif" font-size="8.00">5</text>
</g>
<!-- 2&#45;&gt;5 -->
<g id="edge10" class="edge">
<title>2&#45;&gt;5</title>
<path fill="none" stroke="black" d="M169.23,-221.61C171.95,-200.65 179.56,-162.98 200.34,-139 243.67,-88.99 287.9,-67.82 344.56,-102 385.05,-126.42 398.96,-184.36 403.7,-218.2"/>
<polygon fill="black" stroke="black" points="400.25,-218.77 404.94,-228.26 407.19,-217.91 400.25,-218.77"/>
<text text-anchor="middle" x="288.45" y="-88.6" font-family="Times,serif" font-size="8.00">29</text>
</g>
<!-- 3 -->
<g id="node7" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="488.23" cy="-288" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="488.23" y="-286.1" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge9" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M167.8,-252.75C168.74,-295.58 177.71,-411 246.89,-411 246.89,-411 246.89,-411 408.12,-411 454.22,-411 474.73,-349.12 482.75,-313.14"/>
<polygon fill="black" stroke="black" points="486.18,-313.83 484.77,-303.33 479.33,-312.42 486.18,-313.83"/>
<text text-anchor="middle" x="329.01" y="-413.6" font-family="Times,serif" font-size="8.00">&#45;1</text>
</g>
<!-- 7&#45;&gt;2 -->
<g id="edge25" class="edge">
<title>7&#45;&gt;2</title>
<path fill="none" stroke="black" d="M316.02,-296.05C310.15,-299.93 302.78,-304.02 295.45,-306 267.89,-313.45 257.79,-306.94 232.34,-294 213.79,-284.57 196.34,-268.56 184.27,-255.9"/>
<polygon fill="black" stroke="black" points="186.84,-253.52 177.48,-248.54 181.69,-258.27 186.84,-253.52"/>
<text text-anchor="middle" x="247.89" y="-309.6" font-family="Times,serif" font-size="8.00">&#45;25</text>
</g>
<!-- 7&#45;&gt;7 -->
<g id="edge27" class="edge">
<title>7&#45;&gt;7</title>
<path fill="none" stroke="black" d="M322.51,-301.49C320.68,-311.15 322.85,-320.56 329.01,-320.56 332.86,-320.56 335.15,-316.88 335.88,-311.77"/>
<polygon fill="black" stroke="black" points="339.36,-311.35 335.5,-301.49 332.37,-311.61 339.36,-311.35"/>
<text text-anchor="middle" x="329.01" y="-323.16" font-family="Times,serif" font-size="8.00">&#45;12</text>
</g>
<!-- 7&#45;&gt;1 -->
<g id="edge24" class="edge">
<title>7&#45;&gt;1</title>
<path fill="none" stroke="black" d="M313.59,-283.72C302.35,-281.16 286.49,-277.55 273.29,-274.55"/>
<polygon fill="black" stroke="black" points="273.93,-271.11 263.4,-272.3 272.38,-277.93 273.93,-271.11"/>
<text text-anchor="middle" x="288.45" y="-281.6" font-family="Times,serif" font-size="8.00">&#45;30</text>
</g>
<!-- 7&#45;&gt;5 -->
<g id="edge26" class="edge">
<title>7&#45;&gt;5</title>
<path fill="none" stroke="black" d="M342.92,-279.71C354.18,-273.34 370.76,-263.98 384.11,-256.43"/>
<polygon fill="black" stroke="black" points="386.16,-259.3 393.15,-251.33 382.72,-253.2 386.16,-259.3"/>
<text text-anchor="middle" x="368.06" y="-270.6" font-family="Times,serif" font-size="8.00">30</text>
</g>
<!-- 1&#45;&gt;7 -->
<g id="edge6" class="edge">
<title>1&#45;&gt;7</title>
<path fill="none" stroke="black" d="M259.7,-279.21C265.63,-284.1 273.4,-289.44 281.45,-292 288.53,-294.25 296.49,-294.25 303.79,-293.33"/>
<polygon fill="black" stroke="black" points="304.66,-296.73 313.85,-291.47 303.38,-289.85 304.66,-296.73"/>
<text text-anchor="middle" x="288.45" y="-295.6" font-family="Times,serif" font-size="8.00">23</text>
</g>
<!-- 4 -->
<g id="node5" class="node">
<title>4</title>
<ellipse fill="none" stroke="black" cx="329.01" cy="-127" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="329.01" y="-125.1" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1&#45;&gt;4 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;4</title>
<path fill="none" stroke="black" d="M256.34,-255.64C269.94,-231.22 298.96,-179.14 315.59,-149.28"/>
<polygon fill="black" stroke="black" points="318.72,-150.87 320.53,-140.43 312.6,-147.46 318.72,-150.87"/>
<text text-anchor="middle" x="288.45" y="-209.6" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;5 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;5</title>
<path fill="none" stroke="black" d="M263.13,-264.84C276.12,-261.19 295.92,-256.01 313.45,-253 336.22,-249.09 362.46,-246.74 381.19,-245.43"/>
<polygon fill="black" stroke="black" points="381.51,-248.92 391.27,-244.77 381.06,-241.93 381.51,-248.92"/>
<text text-anchor="middle" x="329.01" y="-255.6" font-family="Times,serif" font-size="8.00">24</text>
</g>
<!-- 4&#45;&gt;2 -->
<g id="edge15" class="edge">
<title>4&#45;&gt;2</title>
<path fill="none" stroke="black" d="M313.47,-125.48C287.6,-123.59 233.18,-123.48 200.34,-151 182.48,-165.97 174.4,-191.82 170.75,-211.29"/>
<polygon fill="black" stroke="black" points="167.27,-210.91 169.15,-221.33 174.18,-212.01 167.27,-210.91"/>
<text text-anchor="middle" x="247.89" y="-136.6" font-family="Times,serif" font-size="8.00">26</text>
</g>
<!-- 4&#45;&gt;4 -->
<g id="edge16" class="edge">
<title>4&#45;&gt;4</title>
<path fill="none" stroke="black" d="M322.51,-141.49C320.68,-151.15 322.85,-160.56 329.01,-160.56 332.86,-160.56 335.15,-156.88 335.88,-151.77"/>
<polygon fill="black" stroke="black" points="339.36,-151.35 335.5,-141.49 332.37,-151.61 339.36,-151.35"/>
<text text-anchor="middle" x="329.01" y="-163.16" font-family="Times,serif" font-size="8.00">20</text>
</g>
<!-- 4&#45;&gt;5 -->
<g id="edge17" class="edge">
<title>4&#45;&gt;5</title>
<path fill="none" stroke="black" d="M341.03,-137.29C350.49,-146.52 364.06,-160.78 373.56,-175 383.04,-189.18 391.21,-206.55 397.01,-220.31"/>
<polygon fill="black" stroke="black" points="393.77,-221.66 400.78,-229.6 400.26,-219.02 393.77,-221.66"/>
<text text-anchor="middle" x="368.06" y="-177.6" font-family="Times,serif" font-size="8.00">38</text>
</g>
<!-- 5&#45;&gt;0 -->
<g id="edge18" class="edge">
<title>5&#45;&gt;0</title>
<path fill="none" stroke="black" d="M403.68,-228.72C395.81,-182.12 369.31,-45 330.01,-45 165.78,-45 165.78,-45 165.78,-45 141.16,-45 120.65,-66.27 108.08,-83.48"/>
<polygon fill="black" stroke="black" points="105.02,-81.75 102.25,-91.98 110.79,-85.71 105.02,-81.75"/>
<text text-anchor="middle" x="247.89" y="-47.6" font-family="Times,serif" font-size="8.00">30</text>
</g>
<!-- 5&#45;&gt;2 -->
<g id="edge20" class="edge">
<title>5&#45;&gt;2</title>
<path fill="none" stroke="black" d="M394.85,-234.23C375.88,-218.51 335.69,-188.23 295.45,-177 254.12,-165.46 209.57,-197.82 185.56,-219.39"/>
<polygon fill="black" stroke="black" points="183.17,-216.84 178.24,-226.22 187.94,-221.96 183.17,-216.84"/>
<text text-anchor="middle" x="288.45" y="-179.6" font-family="Times,serif" font-size="8.00">10</text>
</g>
<!-- 5&#45;&gt;1 -->
<g id="edge19" class="edge">
<title>5&#45;&gt;1</title>
<path fill="none" stroke="black" d="M399.42,-257.53C387.2,-281.57 361.05,-330.04 344.56,-339 313.02,-356.15 280.05,-317.43 262.24,-291.01"/>
<polygon fill="black" stroke="black" points="264.97,-288.79 256.6,-282.29 259.09,-292.59 264.97,-288.79"/>
<text text-anchor="middle" x="329.01" y="-345.6" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 5&#45;&gt;3 -->
<g id="edge21" class="edge">
<title>5&#45;&gt;3</title>
<path fill="none" stroke="black" d="M421.18,-251.27C433.09,-257.89 450.93,-267.81 465.07,-275.67"/>
<polygon fill="black" stroke="black" points="463.71,-278.92 474.15,-280.72 467.11,-272.8 463.71,-278.92"/>
<text text-anchor="middle" x="447.68" y="-270.6" font-family="Times,serif" font-size="8.00">22</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge11" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M486.99,-303.81C485.44,-345.63 475.02,-456 408.12,-456 246.89,-456 246.89,-456 246.89,-456 217.17,-456 214.04,-434.37 200.34,-408 175.82,-360.81 169.63,-297.71 168.15,-262.86"/>
<polygon fill="black" stroke="black" points="171.64,-262.47 167.82,-252.59 164.64,-262.7 171.64,-262.47"/>
<text text-anchor="middle" x="329.01" y="-458.6" font-family="Times,serif" font-size="8.00">27</text>
</g>
<!-- 3&#45;&gt;7 -->
<g id="edge14" class="edge">
<title>3&#45;&gt;7</title>
<path fill="none" stroke="black" d="M472.37,-287.91C445.39,-287.73 388.1,-287.37 355.01,-287.16"/>
<polygon fill="black" stroke="black" points="354.82,-283.66 344.8,-287.09 354.78,-290.66 354.82,-283.66"/>
<text text-anchor="middle" x="407.12" y="-289.6" font-family="Times,serif" font-size="8.00">&#45;12</text>
</g>
<!-- 3&#45;&gt;5 -->
<g id="edge13" class="edge">
<title>3&#45;&gt;5</title>
<path fill="none" stroke="black" d="M480.89,-274.03C475.48,-263.97 466.68,-251.13 454.68,-245 448.1,-241.64 440.28,-240.55 432.95,-240.54"/>
<polygon fill="black" stroke="black" points="432.53,-237.06 422.76,-241.16 432.96,-244.05 432.53,-237.06"/>
<text text-anchor="middle" x="447.68" y="-247.6" font-family="Times,serif" font-size="8.00">&#45;19</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge12" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M481.46,-302.14C479.41,-311.92 481.67,-321.56 488.23,-321.56 492.44,-321.56 494.87,-317.6 495.54,-312.19"/>
<polygon fill="black" stroke="black" points="499.03,-311.93 495,-302.14 492.04,-312.31 499.03,-311.93"/>
<text text-anchor="middle" x="488.23" y="-324.16" font-family="Times,serif" font-size="8.00">&#45;3</text>
</g>
<!-- 6 -->
<g id="node8" class="node">
<title>6</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-16" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-14.1" font-family="Times,serif" font-size="8.00">6</text>
</g>
<!-- 6&#45;&gt;0 -->
<g id="edge22" class="edge">
<title>6&#45;&gt;0</title>
<path fill="none" stroke="black" d="M26.51,-27.64C39.11,-42.37 61.01,-67.98 76.14,-85.68"/>
<polygon fill="black" stroke="black" points="73.8,-88.32 82.96,-93.64 79.12,-83.77 73.8,-88.32"/>
<text text-anchor="middle" x="54.61" y="-68.6" font-family="Times,serif" font-size="8.00">34</text>
</g>
<!-- 6&#45;&gt;3 -->
<g id="edge23" class="edge">
<title>6&#45;&gt;3</title>
<path fill="none" stroke="black" d="M30.4,-11.29C45.52,-6.57 70.52,0 92.67,0 92.67,0 92.67,0 408.12,0 462.26,0 480.72,-191.64 485.68,-262.27"/>
<polygon fill="black" stroke="black" points="482.2,-262.77 486.36,-272.52 489.19,-262.3 482.2,-262.77"/>
<text text-anchor="middle" x="247.89" y="-2.6" font-family="Times,serif" font-size="8.00">40</text>
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
<svg width="274pt" height="284pt"
 viewBox="0.00 0.00 274.45 283.56" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 279.56)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-279.56 270.45,-279.56 270.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-13.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 0&#45;&gt;0 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;0</title>
<path fill="none" stroke="black" d="M9.4,-30.04C7.67,-39.71 9.72,-49.11 15.56,-49.11 19.2,-49.11 21.37,-45.44 22.06,-40.32"/>
<polygon fill="black" stroke="black" points="25.55,-39.92 21.71,-30.04 18.56,-40.16 25.55,-39.92"/>
<text text-anchor="middle" x="15.56" y="-51.71" font-family="Times,serif" font-size="8.00">23</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-43.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-41.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M30.27,-20.96C40.01,-24.79 53.32,-30.03 64.67,-34.5"/>
<polygon fill="black" stroke="black" points="63.41,-37.77 74,-38.17 65.98,-31.25 63.41,-37.77"/>
<text text-anchor="middle" x="52.11" y="-33.16" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 7 -->
<g id="node3" class="node">
<title>7</title>
<ellipse fill="none" stroke="black" cx="250.89" cy="-43.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="250.89" y="-41.66" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 0&#45;&gt;7 -->
<g id="edge3" class="edge">
<title>0&#45;&gt;7</title>
<path fill="none" stroke="black" d="M31.19,-14.11C60.68,-11.58 129.26,-7.56 185.34,-18.56 199.71,-21.37 215.03,-27.18 227.14,-32.48"/>
<polygon fill="black" stroke="black" points="225.91,-35.77 236.46,-36.74 228.82,-29.4 225.91,-35.77"/>
<text text-anchor="middle" x="129.23" y="-15.16" font-family="Times,serif" font-size="8.00">&#45;35</text>
</g>
<!-- 2&#45;&gt;2 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;2</title>
<path fill="none" stroke="black" d="M82.51,-58.04C80.78,-67.71 82.83,-77.11 88.67,-77.11 92.32,-77.11 94.49,-73.44 95.18,-68.32"/>
<polygon fill="black" stroke="black" points="98.66,-67.92 94.82,-58.04 91.67,-68.16 98.66,-67.92"/>
<text text-anchor="middle" x="88.67" y="-79.71" font-family="Times,serif" font-size="8.00">20</text>
</g>
<!-- 5 -->
<g id="node6" class="node">
<title>5</title>
<ellipse fill="none" stroke="black" cx="250.89" cy="-163.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="250.89" y="-161.66" font-family="Times,serif" font-size="8.00">5</text>
</g>
<!-- 2&#45;&gt;5 -->
<g id="edge9" class="edge">
<title>2&#45;&gt;5</title>
<path fill="none" stroke="black" d="M101.4,-53.36C114.21,-63.98 135.41,-81.31 154.23,-95.56 179.61,-114.78 209.65,-135.87 229.2,-149.38"/>
<polygon fill="black" stroke="black" points="227.42,-152.41 237.64,-155.2 231.39,-146.64 227.42,-152.41"/>
<text text-anchor="middle" x="169.78" y="-121.16" font-family="Times,serif" font-size="8.00">29</text>
</g>
<!-- 3 -->
<g id="node7" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="169.78" cy="-43.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="169.78" y="-41.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge8" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M104.55,-43.56C115.6,-43.56 130.93,-43.56 143.84,-43.56"/>
<polygon fill="black" stroke="black" points="143.97,-47.06 153.97,-43.56 143.97,-40.06 143.97,-47.06"/>
<text text-anchor="middle" x="129.23" y="-46.16" font-family="Times,serif" font-size="8.00">&#45;1</text>
</g>
<!-- 7&#45;&gt;7 -->
<g id="edge15" class="edge">
<title>7&#45;&gt;7</title>
<path fill="none" stroke="black" d="M244.13,-57.69C242.08,-67.48 244.33,-77.11 250.89,-77.11 255.1,-77.11 257.54,-73.16 258.2,-67.74"/>
<polygon fill="black" stroke="black" points="261.7,-67.49 257.66,-57.69 254.71,-67.87 261.7,-67.49"/>
<text text-anchor="middle" x="250.89" y="-79.71" font-family="Times,serif" font-size="8.00">&#45;12</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-210.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-208.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;7 -->
<g id="edge6" class="edge">
<title>1&#45;&gt;7</title>
<path fill="none" stroke="black" d="M102.91,-203.3C128.12,-188.92 183.49,-154.54 217.34,-112.56 228.11,-99.2 236.32,-81.69 241.8,-67.65"/>
<polygon fill="black" stroke="black" points="245.12,-68.74 245.3,-58.15 238.56,-66.32 245.12,-68.74"/>
<text text-anchor="middle" x="169.78" y="-173.16" font-family="Times,serif" font-size="8.00">23</text>
</g>
<!-- 4 -->
<g id="node5" class="node">
<title>4</title>
<ellipse fill="none" stroke="black" cx="169.78" cy="-210.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="169.78" y="-208.66" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1&#45;&gt;4 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;4</title>
<path fill="none" stroke="black" d="M104.55,-210.56C115.6,-210.56 130.93,-210.56 143.84,-210.56"/>
<polygon fill="black" stroke="black" points="143.97,-214.06 153.97,-210.56 143.97,-207.06 143.97,-214.06"/>
<text text-anchor="middle" x="129.23" y="-213.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;5 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;5</title>
<path fill="none" stroke="black" d="M99.17,-222.55C116.36,-242.73 153.76,-279.67 185.34,-262.56 214.38,-246.82 232.43,-211.72 241.82,-187.84"/>
<polygon fill="black" stroke="black" points="245.14,-188.97 245.32,-178.38 238.57,-186.54 245.14,-188.97"/>
<text text-anchor="middle" x="169.78" y="-269.16" font-family="Times,serif" font-size="8.00">24</text>
</g>
<!-- 4&#45;&gt;4 -->
<g id="edge13" class="edge">
<title>4&#45;&gt;4</title>
<path fill="none" stroke="black" d="M163.01,-224.69C160.96,-234.48 163.22,-244.11 169.78,-244.11 173.99,-244.11 176.42,-240.16 177.09,-234.74"/>
<polygon fill="black" stroke="black" points="180.58,-234.49 176.55,-224.69 173.59,-234.87 180.58,-234.49"/>
<text text-anchor="middle" x="169.78" y="-246.71" font-family="Times,serif" font-size="8.00">20</text>
</g>
<!-- 4&#45;&gt;5 -->
<g id="edge14" class="edge">
<title>4&#45;&gt;5</title>
<path fill="none" stroke="black" d="M183.49,-203C195.6,-195.81 214.07,-184.84 228.46,-176.29"/>
<polygon fill="black" stroke="black" points="230.41,-179.21 237.22,-171.09 226.83,-173.19 230.41,-179.21"/>
<text text-anchor="middle" x="210.34" y="-193.16" font-family="Times,serif" font-size="8.00">38</text>
</g>
<!-- 3&#45;&gt;7 -->
<g id="edge12" class="edge">
<title>3&#45;&gt;7</title>
<path fill="none" stroke="black" d="M185.66,-43.56C196.71,-43.56 212.04,-43.56 224.95,-43.56"/>
<polygon fill="black" stroke="black" points="225.08,-47.06 235.08,-43.56 225.08,-40.06 225.08,-47.06"/>
<text text-anchor="middle" x="210.34" y="-46.16" font-family="Times,serif" font-size="8.00">&#45;12</text>
</g>
<!-- 3&#45;&gt;5 -->
<g id="edge11" class="edge">
<title>3&#45;&gt;5</title>
<path fill="none" stroke="black" d="M181.2,-54.56C191.2,-65.45 206.36,-82.87 217.34,-99.56 225.83,-112.46 233.73,-127.86 239.61,-140.32"/>
<polygon fill="black" stroke="black" points="236.52,-141.96 243.88,-149.58 242.88,-139.03 236.52,-141.96"/>
<text text-anchor="middle" x="210.34" y="-102.16" font-family="Times,serif" font-size="8.00">&#45;19</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge10" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M163.01,-57.69C160.96,-67.48 163.22,-77.11 169.78,-77.11 173.99,-77.11 176.42,-73.16 177.09,-67.74"/>
<polygon fill="black" stroke="black" points="180.58,-67.49 176.55,-57.69 173.59,-67.87 180.58,-67.49"/>
<text text-anchor="middle" x="169.78" y="-79.71" font-family="Times,serif" font-size="8.00">&#45;3</text>
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
┌──────────────────┬──────────────────┬──────────────────┬──────────────────────────┐
│        s         │     semiring     │        s         │        kronecker         │
├──────────────────┼──────────────────┼──────────────────┼──────────────────────────┤
│        0   1     │ plus_times_int32 │        0   1     │        0   1   2   3     │
│     ────────     │                  │     ────────     │     ────────────────     │
│   0│   1   1     │                  │   0│   1   1     │   0│   1   1   1   1     │
│   1│       1     │                  │   1│       1     │   1│       1       1     │
│                  │                  │                  │   2│           1   1     │
│                  │                  │                  │   3│               1     │
│                  │                  │                  │                          │
└──────────────────┴──────────────────┴──────────────────┴──────────────────────────┘
(1 row)

select draw(s) as binop_a_source, draw(s) as binop_b_source, draw(kronecker(s, s, semiring)) as binop_c_source from test_fixture \gset
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="112pt" height="66pt"
 viewBox="0.00 0.00 112.23 66.11" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 62.11)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-62.11 108.23,-62.11 108.23,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-13.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 0&#45;&gt;0 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;0</title>
<path fill="none" stroke="black" d="M9.4,-30.04C7.67,-39.71 9.72,-49.11 15.56,-49.11 19.2,-49.11 21.37,-45.44 22.06,-40.32"/>
<polygon fill="black" stroke="black" points="25.55,-39.92 21.71,-30.04 18.56,-40.16 25.55,-39.92"/>
<text text-anchor="middle" x="15.56" y="-51.71" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-13.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 0&#45;&gt;1 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;1</title>
<path fill="none" stroke="black" d="M31.29,-15.56C40.35,-15.56 52.19,-15.56 62.68,-15.56"/>
<polygon fill="black" stroke="black" points="62.95,-19.06 72.95,-15.56 62.95,-12.06 62.95,-19.06"/>
<text text-anchor="middle" x="52.11" y="-18.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;1 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;1</title>
<path fill="none" stroke="black" d="M82.51,-30.04C80.78,-39.71 82.83,-49.11 88.67,-49.11 92.32,-49.11 94.49,-45.44 95.18,-40.32"/>
<polygon fill="black" stroke="black" points="98.66,-39.92 94.82,-30.04 91.67,-40.16 98.66,-39.92"/>
<text text-anchor="middle" x="88.67" y="-51.71" font-family="Times,serif" font-size="8.00">1</text>
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
<svg width="112pt" height="66pt"
 viewBox="0.00 0.00 112.23 66.11" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 62.11)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-62.11 108.23,-62.11 108.23,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-13.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 0&#45;&gt;0 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;0</title>
<path fill="none" stroke="black" d="M9.4,-30.04C7.67,-39.71 9.72,-49.11 15.56,-49.11 19.2,-49.11 21.37,-45.44 22.06,-40.32"/>
<polygon fill="black" stroke="black" points="25.55,-39.92 21.71,-30.04 18.56,-40.16 25.55,-39.92"/>
<text text-anchor="middle" x="15.56" y="-51.71" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-13.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 0&#45;&gt;1 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;1</title>
<path fill="none" stroke="black" d="M31.29,-15.56C40.35,-15.56 52.19,-15.56 62.68,-15.56"/>
<polygon fill="black" stroke="black" points="62.95,-19.06 72.95,-15.56 62.95,-12.06 62.95,-19.06"/>
<text text-anchor="middle" x="52.11" y="-18.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;1 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;1</title>
<path fill="none" stroke="black" d="M82.51,-30.04C80.78,-39.71 82.83,-49.11 88.67,-49.11 92.32,-49.11 94.49,-45.44 95.18,-40.32"/>
<polygon fill="black" stroke="black" points="98.66,-39.92 94.82,-30.04 91.67,-40.16 98.66,-39.92"/>
<text text-anchor="middle" x="88.67" y="-51.71" font-family="Times,serif" font-size="8.00">1</text>
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
<svg width="185pt" height="163pt"
 viewBox="0.00 0.00 185.34 163.33" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 159.33)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-159.33 181.34,-159.33 181.34,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-36.77" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-34.87" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 0&#45;&gt;0 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;0</title>
<path fill="none" stroke="black" d="M9.4,-51.26C7.67,-60.92 9.72,-70.33 15.56,-70.33 19.2,-70.33 21.37,-66.65 22.06,-61.54"/>
<polygon fill="black" stroke="black" points="25.55,-61.13 21.71,-51.26 18.56,-61.37 25.55,-61.13"/>
<text text-anchor="middle" x="15.56" y="-72.93" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-112.77" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-110.87" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 0&#45;&gt;1 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;1</title>
<path fill="none" stroke="black" d="M26.75,-47.66C38.15,-59.85 56.82,-79.81 70.5,-94.42"/>
<polygon fill="black" stroke="black" points="68.11,-96.99 77.5,-101.9 73.22,-92.21 68.11,-96.99"/>
<text text-anchor="middle" x="52.11" y="-79.37" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2 -->
<g id="node3" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-36.77" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-34.87" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M31.29,-36.77C40.35,-36.77 52.19,-36.77 62.68,-36.77"/>
<polygon fill="black" stroke="black" points="62.95,-40.27 72.95,-36.77 62.95,-33.27 62.95,-40.27"/>
<text text-anchor="middle" x="52.11" y="-39.37" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="161.78" cy="-36.77" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="161.78" y="-34.87" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge4" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M28.24,-27.13C39.1,-18.84 56.11,-7.49 73.11,-2.77 86.44,0.92 90.9,0.92 104.23,-2.77 117.37,-6.42 130.53,-14.03 140.84,-21.13"/>
<polygon fill="black" stroke="black" points="138.95,-24.09 149.1,-27.13 143.06,-18.42 138.95,-24.09"/>
<text text-anchor="middle" x="88.67" y="-5.37" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;1 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;1</title>
<path fill="none" stroke="black" d="M82.51,-127.26C80.78,-136.92 82.83,-146.33 88.67,-146.33 92.32,-146.33 94.49,-142.65 95.18,-137.54"/>
<polygon fill="black" stroke="black" points="98.66,-137.13 94.82,-127.26 91.67,-137.37 98.66,-137.13"/>
<text text-anchor="middle" x="88.67" y="-148.93" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge6" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M99.86,-101.88C111.26,-89.69 129.94,-69.74 143.61,-55.13"/>
<polygon fill="black" stroke="black" points="146.34,-57.33 150.61,-47.64 141.22,-52.55 146.34,-57.33"/>
<text text-anchor="middle" x="125.23" y="-79.37" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;2 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;2</title>
<path fill="none" stroke="black" d="M82.51,-51.26C80.78,-60.92 82.83,-70.33 88.67,-70.33 92.32,-70.33 94.49,-66.65 95.18,-61.54"/>
<polygon fill="black" stroke="black" points="98.66,-61.13 94.82,-51.26 91.67,-61.37 98.66,-61.13"/>
<text text-anchor="middle" x="88.67" y="-72.93" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge8" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M104.4,-36.77C113.46,-36.77 125.3,-36.77 135.8,-36.77"/>
<polygon fill="black" stroke="black" points="136.07,-40.27 146.07,-36.77 136.07,-33.27 136.07,-40.27"/>
<text text-anchor="middle" x="125.23" y="-39.37" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge9" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M155.63,-51.26C153.89,-60.92 155.95,-70.33 161.78,-70.33 165.43,-70.33 167.6,-66.65 168.29,-61.54"/>
<polygon fill="black" stroke="black" points="171.78,-61.13 167.94,-51.26 164.78,-61.37 171.78,-61.13"/>
<text text-anchor="middle" x="161.78" y="-72.93" font-family="Times,serif" font-size="8.00">1</text>
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
┌──────────────────────────────────────────────────────────────────────────┐
│                                  print                                   │
├──────────────────────────────────────────────────────────────────────────┤
│        0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15     │
│     ────────────────────────────────────────────────────────────────     │
│   0│   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1     │
│   1│       1       1       1       1       1       1       1       1     │
│   2│           1   1           1   1           1   1           1   1     │
│   3│               1               1               1               1     │
│   4│                   1   1   1   1                   1   1   1   1     │
│   5│                       1       1                       1       1     │
│   6│                           1   1                           1   1     │
│   7│                               1                               1     │
│   8│                                   1   1   1   1   1   1   1   1     │
│   9│                                       1       1       1       1     │
│  10│                                           1   1           1   1     │
│  11│                                               1               1     │
│  12│                                                   1   1   1   1     │
│  13│                                                       1       1     │
│  14│                                                           1   1     │
│  15│                                                               1     │
│                                                                          │
└──────────────────────────────────────────────────────────────────────────┘
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
┌──────────────────────────┐
│          print           │
├──────────────────────────┤
│        0   1   2   3     │
│     ────────────────     │
│   0│                     │
│   1│             -19     │
│   2│   0  10      15     │
│   3│ -35      40  31     │
│                          │
└──────────────────────────┘
(1 row)

```
## Apply

`apply` takes an operator of type `unaryop` and applies it to every
element of the matrix.  The 'ainv_int32' returned the additive
inverse (the negative value for integers) of every element:
``` postgres-console
select print(a) as a, unaryop, print(apply(a, unaryop)) as applied from test_fixture;
┌──────────────────────────┬────────────┬──────────────────────────┐
│            a             │  unaryop   │         applied          │
├──────────────────────────┼────────────┼──────────────────────────┤
│        0   1   2   3     │ ainv_int32 │        0   1   2   3     │
│     ────────────────     │            │     ────────────────     │
│   0│           0 -35     │            │   0│           0  35     │
│   1│          10         │            │   1│         -10         │
│   2│              40     │            │   2│             -40     │
│   3│     -19  15  31     │            │   3│      19 -15 -31     │
│                          │            │                          │
└──────────────────────────┴────────────┴──────────────────────────┘
(1 row)

```
## SuiteSparse Info

The `info` function returns a descripton of the matrix from
SuiteSparse.
``` postgres-console
select info(a) from test_fixture;
┌──────────────────────────────────────────────┐
│                     info                     │
├──────────────────────────────────────────────┤
│                                              │
│   4x4 GraphBLAS int8_t matrix, bitmap by row │
│   A->matrix, 7 entries, memory: 272 bytes    │
│                                              │
│                                              │
└──────────────────────────────────────────────┘
(1 row)

```
## Matrix Duplication

The `dup` function duplicates a matrix returning a new matrix
object with the same values:
``` postgres-console
select dup(a) from test_fixture;
┌──────────────────────────────────────────────────────────────┐
│                             dup                              │
├──────────────────────────────────────────────────────────────┤
│ int8(4:4)[0:2:0 0:3:-35 1:2:10 2:3:40 3:1:-19 3:2:15 3:3:31] │
└──────────────────────────────────────────────────────────────┘
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