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
│ a            │ int32(4:4)[0:1:2 0:2:1 0:3:1 1:0:2 1:2:3 1:3:2 2:0:1 2:1:3 2:3:3 3:0:1 3:1:2 3:2:3]                         │
│ b            │ int32(4:4)[0:3:4 1:0:4 1:2:2 1:3:1 2:1:4 2:3:2 3:1:4 3:2:1]                                                 │
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
│  0│     2  1  1    │  0│           4    │
│  1│  2     3  2    │  1│  4     2  1    │
│  2│  1  3     3    │  2│     4     2    │
│  3│  1  2  3       │  3│     4  1       │
│                    │                    │
└────────────────────┴────────────────────┘
(1 row)

select draw(a) as twocol_a_source, draw(b) as twocol_b_source from test_fixture \gset
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="258pt" height="175pt"
 viewBox="0.00 0.00 258.45 175.24" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 171.24)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-171.24 254.45,-171.24 254.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-19.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-17.34" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-107.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-105.34" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 0&#45;&gt;1 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;1</title>
<path fill="none" stroke="black" d="M20.95,-33.84C25.89,-48.29 35.1,-70.35 49.11,-85.24 53.51,-89.91 59.13,-93.87 64.71,-97.09"/>
<polygon fill="black" stroke="black" points="63.34,-100.32 73.83,-101.8 66.55,-94.1 63.34,-100.32"/>
<text text-anchor="middle" x="52.11" y="-91.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2 -->
<g id="node3" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="234.89" cy="-58.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="234.89" y="-56.34" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M30.71,-15.51C36.39,-14.2 43.02,-12.88 49.11,-12.24 51.76,-11.96 52.45,-12.21 55.11,-12.24 76.94,-12.46 82.53,-10.78 104.23,-13.24 137.7,-17.03 145.25,-22.99 177.34,-33.24 188.05,-36.66 190.99,-36.84 201.34,-41.24 204.89,-42.75 208.59,-44.49 212.16,-46.28"/>
<polygon fill="black" stroke="black" points="210.65,-49.44 221.13,-50.97 213.89,-43.24 210.65,-49.44"/>
<text text-anchor="middle" x="125.23" y="-19.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="161.78" cy="-58.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="161.78" y="-56.34" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge3" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M30.65,-23.05C55.25,-29.7 106.3,-43.5 136.63,-51.71"/>
<polygon fill="black" stroke="black" points="135.85,-55.12 146.42,-54.35 137.68,-48.36 135.85,-55.12"/>
<text text-anchor="middle" x="88.67" y="-45.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M78.23,-95.56C66.57,-81.13 46.57,-56.37 32.51,-38.98"/>
<polygon fill="black" stroke="black" points="35.15,-36.68 26.15,-31.1 29.71,-41.08 35.15,-36.68"/>
<text text-anchor="middle" x="52.11" y="-67.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M97.1,-120.82C102.72,-129.58 111.33,-140.23 122.23,-145.24 154.17,-159.93 172.45,-165.27 201.34,-145.24 221,-131.6 228.8,-104.56 231.89,-84.22"/>
<polygon fill="black" stroke="black" points="235.4,-84.38 233.14,-74.02 228.45,-83.52 235.4,-84.38"/>
<text text-anchor="middle" x="161.78" y="-160.84" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge6" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M102.86,-100.45C110.48,-96.35 120.11,-90.85 128.23,-85.24 132.79,-82.08 137.49,-78.43 141.86,-74.86"/>
<polygon fill="black" stroke="black" points="144.33,-77.36 149.71,-68.24 139.81,-72.01 144.33,-77.36"/>
<text text-anchor="middle" x="125.23" y="-90.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;0 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;0</title>
<path fill="none" stroke="black" d="M221.89,-49.64C215.77,-45.28 208.18,-39.94 201.34,-35.24 190.77,-27.98 188.87,-24.83 177.34,-19.24 156.68,-9.22 150.9,-6.84 128.23,-3.24 93.43,2.29 83.66,-1.34 49.11,-8.24 46.15,-8.83 43.09,-9.62 40.08,-10.5"/>
<polygon fill="black" stroke="black" points="38.69,-7.27 30.28,-13.72 40.88,-13.92 38.69,-7.27"/>
<text text-anchor="middle" x="125.23" y="-5.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge8" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M220.61,-65C200.96,-74.68 162.74,-92.32 128.23,-101.24 123.87,-102.36 119.18,-103.31 114.63,-104.09"/>
<polygon fill="black" stroke="black" points="113.82,-100.67 104.45,-105.63 114.87,-107.59 113.82,-100.67"/>
<text text-anchor="middle" x="161.78" y="-97.84" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge9" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M219.18,-58.24C210.12,-58.24 198.28,-58.24 187.79,-58.24"/>
<polygon fill="black" stroke="black" points="187.52,-54.74 177.52,-58.24 187.52,-61.74 187.52,-54.74"/>
<text text-anchor="middle" x="198.34" y="-60.84" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 3&#45;&gt;0 -->
<g id="edge10" class="edge">
<title>3&#45;&gt;0</title>
<path fill="none" stroke="black" d="M149.57,-48.51C138.78,-39.8 121.57,-27.61 104.23,-22.24 83.75,-15.89 59.28,-15.62 41.4,-16.66"/>
<polygon fill="black" stroke="black" points="40.92,-13.18 31.22,-17.43 41.45,-20.16 40.92,-13.18"/>
<text text-anchor="middle" x="88.67" y="-24.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge11" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M147,-63.39C139.37,-66.55 129.91,-70.99 122.23,-76.24 116.94,-79.84 111.7,-84.32 107.02,-88.74"/>
<polygon fill="black" stroke="black" points="104.38,-86.43 99.76,-95.96 109.32,-91.39 104.38,-86.43"/>
<text text-anchor="middle" x="125.23" y="-78.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge12" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M176.06,-51.61C181.83,-49.08 188.76,-46.5 195.34,-45.24 200.75,-44.2 206.44,-45 211.74,-46.61"/>
<polygon fill="black" stroke="black" points="210.69,-49.96 221.28,-50.46 213.31,-43.47 210.69,-49.96"/>
<text text-anchor="middle" x="198.34" y="-47.84" font-family="Times,serif" font-size="8.00">3</text>
</g>
</g>
</svg>
</div>

    </td>
    <td style="width: 30%;">
        <!-- Diagram B -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="258pt" height="130pt"
 viewBox="0.00 0.00 258.45 129.56" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 125.56)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-125.56 254.45,-125.56 254.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-13.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node2" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-64.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-62.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M28.94,-24.09C39.45,-31.33 54.73,-41.86 67.07,-50.36"/>
<polygon fill="black" stroke="black" points="65.2,-53.33 75.42,-56.12 69.18,-47.56 65.2,-53.33"/>
<text text-anchor="middle" x="52.11" y="-44.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1 -->
<g id="node3" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="161.78" cy="-32.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="161.78" y="-30.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M103.04,-58.54C112.75,-54.17 126.14,-48.15 137.57,-43"/>
<polygon fill="black" stroke="black" points="139.3,-46.06 146.98,-38.77 136.43,-39.68 139.3,-46.06"/>
<text text-anchor="middle" x="125.23" y="-52.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 2 -->
<g id="node4" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="234.89" cy="-76.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="234.89" y="-74.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M104.29,-65.77C128.91,-67.82 178.9,-71.98 209.09,-74.49"/>
<polygon fill="black" stroke="black" points="208.98,-78 219.23,-75.34 209.56,-71.02 208.98,-78"/>
<text text-anchor="middle" x="161.78" y="-75.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge2" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M146.2,-30.74C140.58,-30.05 134.11,-29.26 128.23,-28.56 98.41,-24.99 64.11,-21.02 41.47,-18.41"/>
<polygon fill="black" stroke="black" points="41.71,-14.91 31.37,-17.25 40.91,-21.87 41.71,-14.91"/>
<text text-anchor="middle" x="88.67" y="-28.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M146.35,-30.52C138.88,-30.05 129.77,-30.42 122.23,-33.56 116.22,-36.05 110.68,-40.2 105.93,-44.67"/>
<polygon fill="black" stroke="black" points="103.09,-42.58 98.76,-52.25 108.17,-47.39 103.09,-42.58"/>
<text text-anchor="middle" x="125.23" y="-36.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M177.44,-30.02C184.97,-29.42 194.07,-29.83 201.34,-33.56 210.05,-38.03 217.08,-46.03 222.35,-53.93"/>
<polygon fill="black" stroke="black" points="219.4,-55.82 227.54,-62.6 225.41,-52.22 219.4,-55.82"/>
<text text-anchor="middle" x="198.34" y="-36.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge6" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M223.61,-87.8C217.71,-93.4 209.8,-99.59 201.34,-102.56 168.15,-114.18 154.17,-117.25 122.23,-102.56 114.74,-99.11 108.33,-93 103.23,-86.67"/>
<polygon fill="black" stroke="black" points="105.78,-84.22 97.1,-78.14 100.09,-88.31 105.78,-84.22"/>
<text text-anchor="middle" x="161.78" y="-115.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge5" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M221.29,-68.76C211.01,-62.4 196.25,-53.27 184.13,-45.77"/>
<polygon fill="black" stroke="black" points="185.84,-42.71 175.5,-40.42 182.16,-48.66 185.84,-42.71"/>
<text text-anchor="middle" x="198.34" y="-59.16" font-family="Times,serif" font-size="8.00">4</text>
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
│  0│            -13-13          │
│  1│      -38         -13-95    │
│  2│   -38      -11-62          │
│  3│               -98-20       │
│  4│-13   -11      -20-16-79    │
│  5│-13   -62-98-20             │
│  6│   -13   -20-16             │
│  7│   -95      -79             │
│                                │
└────────────────────────────────┘
(1 row)

```
This random matrix is also a random *graph*:

``` postgres-console
select draw(random_matrix(8, 8, 16, seed=>0.42, max=>42)) as draw_source \gset
```
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="642pt" height="413pt"
 viewBox="0.00 0.00 641.90 413.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 409)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-409 637.9,-409 637.9,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-86" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-84.1" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 4 -->
<g id="node2" class="node">
<title>4</title>
<ellipse fill="none" stroke="black" cx="101.67" cy="-185" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="101.67" y="-183.1" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 0&#45;&gt;4 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;4</title>
<path fill="none" stroke="black" d="M25.3,-98.62C31.65,-107.58 40.59,-119.78 49.11,-130 59.8,-142.82 72.63,-156.51 82.82,-167.03"/>
<polygon fill="black" stroke="black" points="80.52,-169.68 90.01,-174.38 85.52,-164.79 80.52,-169.68"/>
<text text-anchor="middle" x="58.61" y="-152.6" font-family="Times,serif" font-size="8.00">&#45;136</text>
</g>
<!-- 5 -->
<g id="node3" class="node">
<title>5</title>
<ellipse fill="none" stroke="black" cx="446.12" cy="-146" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="446.12" y="-144.1" font-family="Times,serif" font-size="8.00">5</text>
</g>
<!-- 0&#45;&gt;5 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;5</title>
<path fill="none" stroke="black" d="M27.4,-74.87C42.42,-60.69 71.08,-38 100.67,-38 100.67,-38 100.67,-38 361.01,-38 403.04,-38 427.14,-89.55 438.03,-121.42"/>
<polygon fill="black" stroke="black" points="434.71,-122.53 441.1,-130.98 441.37,-120.39 434.71,-122.53"/>
<text text-anchor="middle" x="230.84" y="-40.6" font-family="Times,serif" font-size="8.00">&#45;134</text>
</g>
<!-- 4&#45;&gt;0 -->
<g id="edge11" class="edge">
<title>4&#45;&gt;0</title>
<path fill="none" stroke="black" d="M96.21,-170.28C91.22,-155.7 81.97,-133.37 68.11,-118 60.18,-109.2 49.26,-101.95 39.51,-96.6"/>
<polygon fill="black" stroke="black" points="40.81,-93.33 30.31,-91.88 37.61,-99.56 40.81,-93.33"/>
<text text-anchor="middle" x="58.61" y="-120.6" font-family="Times,serif" font-size="8.00">&#45;136</text>
</g>
<!-- 4&#45;&gt;5 -->
<g id="edge13" class="edge">
<title>4&#45;&gt;5</title>
<path fill="none" stroke="black" d="M116.02,-178.73C121.81,-176.26 128.73,-173.63 135.23,-172 238.61,-145.98 366.87,-144.63 420.13,-145.37"/>
<polygon fill="black" stroke="black" points="420.26,-148.88 430.32,-145.56 420.39,-141.88 420.26,-148.88"/>
<text text-anchor="middle" x="273.89" y="-153.6" font-family="Times,serif" font-size="8.00">&#45;202</text>
</g>
<!-- 2 -->
<g id="node5" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="360.01" cy="-193" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="360.01" y="-191.1" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 4&#45;&gt;2 -->
<g id="edge12" class="edge">
<title>4&#45;&gt;2</title>
<path fill="none" stroke="black" d="M117.2,-187.67C122.81,-188.59 129.3,-189.51 135.23,-190 207.23,-195.98 292.7,-194.86 334.08,-193.79"/>
<polygon fill="black" stroke="black" points="334.41,-197.29 344.31,-193.51 334.22,-190.29 334.41,-197.29"/>
<text text-anchor="middle" x="230.84" y="-196.6" font-family="Times,serif" font-size="8.00">&#45;114</text>
</g>
<!-- 6 -->
<g id="node6" class="node">
<title>6</title>
<ellipse fill="none" stroke="black" cx="618.35" cy="-289" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="618.35" y="-287.1" font-family="Times,serif" font-size="8.00">6</text>
</g>
<!-- 4&#45;&gt;6 -->
<g id="edge14" class="edge">
<title>4&#45;&gt;6</title>
<path fill="none" stroke="black" d="M103.85,-200.68C107.03,-239.39 116.78,-338.28 135.23,-365 150.41,-387 160.04,-396 186.78,-396 186.78,-396 186.78,-396 533.23,-396 559.97,-396 567.22,-385.15 584.79,-365 597.63,-350.27 606.03,-329.7 611.07,-313.68"/>
<polygon fill="black" stroke="black" points="614.48,-314.5 613.91,-303.92 607.75,-312.54 614.48,-314.5"/>
<text text-anchor="middle" x="360.01" y="-398.6" font-family="Times,serif" font-size="8.00">&#45;163</text>
</g>
<!-- 7 -->
<g id="node7" class="node">
<title>7</title>
<ellipse fill="none" stroke="black" cx="187.78" cy="-263" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="187.78" y="-261.1" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 4&#45;&gt;7 -->
<g id="edge15" class="edge">
<title>4&#45;&gt;7</title>
<path fill="none" stroke="black" d="M113.67,-195.2C127.52,-208.04 151.53,-230.31 168.24,-245.81"/>
<polygon fill="black" stroke="black" points="166.07,-248.57 175.78,-252.8 170.83,-243.43 166.07,-248.57"/>
<text text-anchor="middle" x="144.73" y="-232.6" font-family="Times,serif" font-size="8.00">&#45;796</text>
</g>
<!-- 5&#45;&gt;0 -->
<g id="edge16" class="edge">
<title>5&#45;&gt;0</title>
<path fill="none" stroke="black" d="M443.54,-130.65C438.75,-93.17 420.83,0 361.01,0 100.67,0 100.67,0 100.67,0 65.48,0 40.18,-37.25 26.99,-62.72"/>
<polygon fill="black" stroke="black" points="23.8,-61.28 22.53,-71.8 30.08,-64.36 23.8,-61.28"/>
<text text-anchor="middle" x="230.84" y="-2.6" font-family="Times,serif" font-size="8.00">&#45;134</text>
</g>
<!-- 5&#45;&gt;4 -->
<g id="edge19" class="edge">
<title>5&#45;&gt;4</title>
<path fill="none" stroke="black" d="M436.37,-133.45C422.61,-115.34 394.02,-84 361.01,-84 186.78,-84 186.78,-84 186.78,-84 147.01,-84 122.4,-130.82 110.75,-160.7"/>
<polygon fill="black" stroke="black" points="107.46,-159.5 107.29,-170.1 114.03,-161.92 107.46,-159.5"/>
<text text-anchor="middle" x="273.89" y="-86.6" font-family="Times,serif" font-size="8.00">&#45;202</text>
</g>
<!-- 5&#45;&gt;2 -->
<g id="edge17" class="edge">
<title>5&#45;&gt;2</title>
<path fill="none" stroke="black" d="M430.42,-149.08C419.7,-151.73 405.1,-156.22 393.56,-163 388.13,-166.19 382.84,-170.42 378.15,-174.69"/>
<polygon fill="black" stroke="black" points="375.64,-172.25 370.92,-181.74 380.53,-177.26 375.64,-172.25"/>
<text text-anchor="middle" x="403.06" y="-165.6" font-family="Times,serif" font-size="8.00">&#45;621</text>
</g>
<!-- 3 -->
<g id="node8" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="532.23" cy="-204" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="532.23" y="-202.1" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 5&#45;&gt;3 -->
<g id="edge18" class="edge">
<title>5&#45;&gt;3</title>
<path fill="none" stroke="black" d="M458.15,-156.38C464.27,-161.9 472.15,-168.64 479.68,-174 488.94,-180.6 499.75,-187.06 509.07,-192.28"/>
<polygon fill="black" stroke="black" points="507.66,-195.5 518.11,-197.23 511.02,-189.36 507.66,-195.5"/>
<text text-anchor="middle" x="489.18" y="-187.6" font-family="Times,serif" font-size="8.00">&#45;980</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="273.89" cy="-275" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="273.89" y="-273.1" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M281.77,-261.1C287.57,-250.35 296.64,-235.58 307.45,-225 315.92,-216.71 326.9,-209.53 336.55,-204.08"/>
<polygon fill="black" stroke="black" points="338.45,-207.03 345.62,-199.23 335.15,-200.86 338.45,-207.03"/>
<text text-anchor="middle" x="316.95" y="-227.6" font-family="Times,serif" font-size="8.00">&#45;387</text>
</g>
<!-- 1&#45;&gt;6 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;6</title>
<path fill="none" stroke="black" d="M287.38,-282.81C293.24,-286.02 300.46,-289.37 307.45,-291 315.68,-292.91 318.01,-291.03 326.45,-291 424.82,-290.66 541.89,-289.68 592.26,-289.23"/>
<polygon fill="black" stroke="black" points="592.54,-292.73 602.51,-289.14 592.48,-285.73 592.54,-292.73"/>
<text text-anchor="middle" x="446.12" y="-292.6" font-family="Times,serif" font-size="8.00">&#45;131</text>
</g>
<!-- 1&#45;&gt;7 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;7</title>
<path fill="none" stroke="black" d="M258.41,-272.93C246.04,-271.17 227.98,-268.59 213.35,-266.5"/>
<polygon fill="black" stroke="black" points="213.83,-263.04 203.43,-265.09 212.84,-269.97 213.83,-263.04"/>
<text text-anchor="middle" x="230.84" y="-272.6" font-family="Times,serif" font-size="8.00">&#45;959</text>
</g>
<!-- 2&#45;&gt;4 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;4</title>
<path fill="none" stroke="black" d="M345.68,-186.63C339.9,-184.14 332.97,-181.53 326.45,-180 255.6,-163.37 168.66,-173.75 127.08,-180.52"/>
<polygon fill="black" stroke="black" points="126.33,-177.09 117.06,-182.22 127.51,-183.99 126.33,-177.09"/>
<text text-anchor="middle" x="230.84" y="-173.6" font-family="Times,serif" font-size="8.00">&#45;114</text>
</g>
<!-- 2&#45;&gt;5 -->
<g id="edge8" class="edge">
<title>2&#45;&gt;5</title>
<path fill="none" stroke="black" d="M375.08,-188.5C385.75,-184.8 400.56,-179.04 412.56,-172 417.2,-169.28 421.89,-165.95 426.21,-162.59"/>
<polygon fill="black" stroke="black" points="428.44,-165.29 433.96,-156.24 424,-159.87 428.44,-165.29"/>
<text text-anchor="middle" x="403.06" y="-182.6" font-family="Times,serif" font-size="8.00">&#45;621</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge6" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M350.28,-205.45C344.08,-213.91 335.32,-225.13 326.45,-234 316.82,-243.64 304.99,-253.18 295.09,-260.63"/>
<polygon fill="black" stroke="black" points="292.98,-257.85 287,-266.59 297.13,-263.48 292.98,-257.85"/>
<text text-anchor="middle" x="316.95" y="-252.6" font-family="Times,serif" font-size="8.00">&#45;387</text>
</g>
<!-- 6&#45;&gt;4 -->
<g id="edge22" class="edge">
<title>6&#45;&gt;4</title>
<path fill="none" stroke="black" d="M608.6,-301.55C594.83,-319.66 566.25,-351 533.23,-351 186.78,-351 186.78,-351 186.78,-351 124.72,-351 108.44,-257.53 104.17,-210.86"/>
<polygon fill="black" stroke="black" points="107.64,-210.31 103.36,-200.62 100.66,-210.86 107.64,-210.31"/>
<text text-anchor="middle" x="360.01" y="-353.6" font-family="Times,serif" font-size="8.00">&#45;163</text>
</g>
<!-- 6&#45;&gt;1 -->
<g id="edge20" class="edge">
<title>6&#45;&gt;1</title>
<path fill="none" stroke="black" d="M603.64,-283.1C587.23,-276.59 558.74,-267 533.23,-267 402.06,-267 402.06,-267 402.06,-267 366.45,-267 325.33,-270.16 299.72,-272.51"/>
<polygon fill="black" stroke="black" points="299.38,-269.03 289.75,-273.46 300.04,-276 299.38,-269.03"/>
<text text-anchor="middle" x="446.12" y="-269.6" font-family="Times,serif" font-size="8.00">&#45;131</text>
</g>
<!-- 6&#45;&gt;3 -->
<g id="edge21" class="edge">
<title>6&#45;&gt;3</title>
<path fill="none" stroke="black" d="M615.04,-273.8C611.59,-256.56 603.39,-228.93 584.79,-214 577.33,-208.01 567.32,-205.25 558.11,-204.08"/>
<polygon fill="black" stroke="black" points="558.09,-200.57 547.86,-203.34 557.58,-207.55 558.09,-200.57"/>
<text text-anchor="middle" x="575.29" y="-216.6" font-family="Times,serif" font-size="8.00">&#45;204</text>
</g>
<!-- 7&#45;&gt;4 -->
<g id="edge24" class="edge">
<title>7&#45;&gt;4</title>
<path fill="none" stroke="black" d="M172.27,-260.18C161.09,-257.4 145.81,-252.19 135.23,-243 124.66,-233.84 116.81,-220.36 111.46,-208.71"/>
<polygon fill="black" stroke="black" points="114.68,-207.33 107.56,-199.48 108.23,-210.06 114.68,-207.33"/>
<text text-anchor="middle" x="144.73" y="-255.6" font-family="Times,serif" font-size="8.00">&#45;796</text>
</g>
<!-- 7&#45;&gt;1 -->
<g id="edge23" class="edge">
<title>7&#45;&gt;1</title>
<path fill="none" stroke="black" d="M202.54,-257.22C213.04,-253.61 227.79,-250.3 240.34,-254 244.62,-255.26 248.87,-257.28 252.82,-259.58"/>
<polygon fill="black" stroke="black" points="251.08,-262.63 261.35,-265.23 254.94,-256.79 251.08,-262.63"/>
<text text-anchor="middle" x="230.84" y="-256.6" font-family="Times,serif" font-size="8.00">&#45;959</text>
</g>
<!-- 3&#45;&gt;5 -->
<g id="edge9" class="edge">
<title>3&#45;&gt;5</title>
<path fill="none" stroke="black" d="M523.2,-191.25C517.31,-182.8 508.62,-172.01 498.68,-165 490.52,-159.25 480.42,-155.02 471.33,-152.04"/>
<polygon fill="black" stroke="black" points="472.23,-148.66 461.64,-149.18 470.24,-155.37 472.23,-148.66"/>
<text text-anchor="middle" x="489.18" y="-167.6" font-family="Times,serif" font-size="8.00">&#45;980</text>
</g>
<!-- 3&#45;&gt;6 -->
<g id="edge10" class="edge">
<title>3&#45;&gt;6</title>
<path fill="none" stroke="black" d="M545.92,-212.31C556.89,-219.84 572.82,-231.62 584.79,-244 591.7,-251.15 598.33,-259.85 603.78,-267.68"/>
<polygon fill="black" stroke="black" points="600.91,-269.68 609.39,-276.03 606.72,-265.78 600.91,-269.68"/>
<text text-anchor="middle" x="575.29" y="-246.6" font-family="Times,serif" font-size="8.00">&#45;204</text>
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
│ 0 │ 1 │ int32:2 │
│ 0 │ 2 │ int32:1 │
│ 0 │ 3 │ int32:1 │
│ 1 │ 0 │ int32:2 │
│ 1 │ 2 │ int32:3 │
│ 1 │ 3 │ int32:2 │
│ 2 │ 0 │ int32:1 │
│ 2 │ 1 │ int32:3 │
│ 2 │ 3 │ int32:3 │
│ 3 │ 0 │ int32:1 │
│ 3 │ 1 │ int32:2 │
│ 3 │ 2 │ int32:3 │
└───┴───┴─────────┘
(12 rows)

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
│  0│     2  1  1    │
│  1│  2  1  3  2    │
│  2│  1  3     3    │
│  3│  1  2  3       │
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
│ int32:3     │
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
select print(a) as a, binaryop, print(b) as b, print(eadd(a, b, binaryop)) as "eadd(a, b, binaryop)" from test_fixture;
┌────────────────────┬─────────────┬────────────────────┬──────────────────────┐
│         a          │  binaryop   │         b          │ eadd(a, b, binaryop) │
├────────────────────┼─────────────┼────────────────────┼──────────────────────┤
│      0  1  2  3    │ times_int32 │      0  1  2  3    │      0  1  2  3      │
│    ────────────    │             │    ────────────    │    ────────────      │
│  0│     2  1  1    │             │  0│           4    │  0│     2  1  4      │
│  1│  2     3  2    │             │  1│  4     2  1    │  1│  8     6  2      │
│  2│  1  3     3    │             │  2│     4     2    │  2│  1 12     6      │
│  3│  1  2  3       │             │  3│     4  1       │  3│  1  8  3         │
│                    │             │                    │                      │
└────────────────────┴─────────────┴────────────────────┴──────────────────────┘
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
│  0│     2  1  5    │  0│     2  1 -3    │  0│     2  1  4    │  0│     2  1  0    │
│  1│  6     5  3    │  1│ -2     1  1    │  1│  8     6  2    │  1│  0     1  2    │
│  2│  1  7     5    │  2│  1 -1     1    │  2│  1 12     6    │  2│  1  0     1    │
│  3│  1  6  4       │  3│  1 -2  2       │  3│  1  8  3       │  3│  1  0  3       │
│                    │                    │                    │                    │
└────────────────────┴────────────────────┴────────────────────┴────────────────────┘
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
<svg width="258pt" height="175pt"
 viewBox="0.00 0.00 258.45 175.24" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 171.24)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-171.24 254.45,-171.24 254.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-19.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-17.34" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-107.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-105.34" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 0&#45;&gt;1 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;1</title>
<path fill="none" stroke="black" d="M20.95,-33.84C25.89,-48.29 35.1,-70.35 49.11,-85.24 53.51,-89.91 59.13,-93.87 64.71,-97.09"/>
<polygon fill="black" stroke="black" points="63.34,-100.32 73.83,-101.8 66.55,-94.1 63.34,-100.32"/>
<text text-anchor="middle" x="52.11" y="-91.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2 -->
<g id="node3" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="234.89" cy="-58.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="234.89" y="-56.34" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M30.71,-15.51C36.39,-14.2 43.02,-12.88 49.11,-12.24 51.76,-11.96 52.45,-12.21 55.11,-12.24 76.94,-12.46 82.53,-10.78 104.23,-13.24 137.7,-17.03 145.25,-22.99 177.34,-33.24 188.05,-36.66 190.99,-36.84 201.34,-41.24 204.89,-42.75 208.59,-44.49 212.16,-46.28"/>
<polygon fill="black" stroke="black" points="210.65,-49.44 221.13,-50.97 213.89,-43.24 210.65,-49.44"/>
<text text-anchor="middle" x="125.23" y="-19.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="161.78" cy="-58.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="161.78" y="-56.34" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge3" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M30.65,-23.05C55.25,-29.7 106.3,-43.5 136.63,-51.71"/>
<polygon fill="black" stroke="black" points="135.85,-55.12 146.42,-54.35 137.68,-48.36 135.85,-55.12"/>
<text text-anchor="middle" x="88.67" y="-45.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M78.23,-95.56C66.57,-81.13 46.57,-56.37 32.51,-38.98"/>
<polygon fill="black" stroke="black" points="35.15,-36.68 26.15,-31.1 29.71,-41.08 35.15,-36.68"/>
<text text-anchor="middle" x="52.11" y="-67.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M97.1,-120.82C102.72,-129.58 111.33,-140.23 122.23,-145.24 154.17,-159.93 172.45,-165.27 201.34,-145.24 221,-131.6 228.8,-104.56 231.89,-84.22"/>
<polygon fill="black" stroke="black" points="235.4,-84.38 233.14,-74.02 228.45,-83.52 235.4,-84.38"/>
<text text-anchor="middle" x="161.78" y="-160.84" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge6" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M102.86,-100.45C110.48,-96.35 120.11,-90.85 128.23,-85.24 132.79,-82.08 137.49,-78.43 141.86,-74.86"/>
<polygon fill="black" stroke="black" points="144.33,-77.36 149.71,-68.24 139.81,-72.01 144.33,-77.36"/>
<text text-anchor="middle" x="125.23" y="-90.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;0 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;0</title>
<path fill="none" stroke="black" d="M221.89,-49.64C215.77,-45.28 208.18,-39.94 201.34,-35.24 190.77,-27.98 188.87,-24.83 177.34,-19.24 156.68,-9.22 150.9,-6.84 128.23,-3.24 93.43,2.29 83.66,-1.34 49.11,-8.24 46.15,-8.83 43.09,-9.62 40.08,-10.5"/>
<polygon fill="black" stroke="black" points="38.69,-7.27 30.28,-13.72 40.88,-13.92 38.69,-7.27"/>
<text text-anchor="middle" x="125.23" y="-5.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge8" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M220.61,-65C200.96,-74.68 162.74,-92.32 128.23,-101.24 123.87,-102.36 119.18,-103.31 114.63,-104.09"/>
<polygon fill="black" stroke="black" points="113.82,-100.67 104.45,-105.63 114.87,-107.59 113.82,-100.67"/>
<text text-anchor="middle" x="161.78" y="-97.84" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge9" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M219.18,-58.24C210.12,-58.24 198.28,-58.24 187.79,-58.24"/>
<polygon fill="black" stroke="black" points="187.52,-54.74 177.52,-58.24 187.52,-61.74 187.52,-54.74"/>
<text text-anchor="middle" x="198.34" y="-60.84" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 3&#45;&gt;0 -->
<g id="edge10" class="edge">
<title>3&#45;&gt;0</title>
<path fill="none" stroke="black" d="M149.57,-48.51C138.78,-39.8 121.57,-27.61 104.23,-22.24 83.75,-15.89 59.28,-15.62 41.4,-16.66"/>
<polygon fill="black" stroke="black" points="40.92,-13.18 31.22,-17.43 41.45,-20.16 40.92,-13.18"/>
<text text-anchor="middle" x="88.67" y="-24.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge11" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M147,-63.39C139.37,-66.55 129.91,-70.99 122.23,-76.24 116.94,-79.84 111.7,-84.32 107.02,-88.74"/>
<polygon fill="black" stroke="black" points="104.38,-86.43 99.76,-95.96 109.32,-91.39 104.38,-86.43"/>
<text text-anchor="middle" x="125.23" y="-78.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge12" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M176.06,-51.61C181.83,-49.08 188.76,-46.5 195.34,-45.24 200.75,-44.2 206.44,-45 211.74,-46.61"/>
<polygon fill="black" stroke="black" points="210.69,-49.96 221.28,-50.46 213.31,-43.47 210.69,-49.96"/>
<text text-anchor="middle" x="198.34" y="-47.84" font-family="Times,serif" font-size="8.00">3</text>
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
<svg width="258pt" height="130pt"
 viewBox="0.00 0.00 258.45 129.56" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 125.56)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-125.56 254.45,-125.56 254.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-13.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node2" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-64.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-62.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M28.94,-24.09C39.45,-31.33 54.73,-41.86 67.07,-50.36"/>
<polygon fill="black" stroke="black" points="65.2,-53.33 75.42,-56.12 69.18,-47.56 65.2,-53.33"/>
<text text-anchor="middle" x="52.11" y="-44.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1 -->
<g id="node3" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="161.78" cy="-32.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="161.78" y="-30.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M103.04,-58.54C112.75,-54.17 126.14,-48.15 137.57,-43"/>
<polygon fill="black" stroke="black" points="139.3,-46.06 146.98,-38.77 136.43,-39.68 139.3,-46.06"/>
<text text-anchor="middle" x="125.23" y="-52.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 2 -->
<g id="node4" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="234.89" cy="-76.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="234.89" y="-74.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M104.29,-65.77C128.91,-67.82 178.9,-71.98 209.09,-74.49"/>
<polygon fill="black" stroke="black" points="208.98,-78 219.23,-75.34 209.56,-71.02 208.98,-78"/>
<text text-anchor="middle" x="161.78" y="-75.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge2" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M146.2,-30.74C140.58,-30.05 134.11,-29.26 128.23,-28.56 98.41,-24.99 64.11,-21.02 41.47,-18.41"/>
<polygon fill="black" stroke="black" points="41.71,-14.91 31.37,-17.25 40.91,-21.87 41.71,-14.91"/>
<text text-anchor="middle" x="88.67" y="-28.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M146.35,-30.52C138.88,-30.05 129.77,-30.42 122.23,-33.56 116.22,-36.05 110.68,-40.2 105.93,-44.67"/>
<polygon fill="black" stroke="black" points="103.09,-42.58 98.76,-52.25 108.17,-47.39 103.09,-42.58"/>
<text text-anchor="middle" x="125.23" y="-36.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M177.44,-30.02C184.97,-29.42 194.07,-29.83 201.34,-33.56 210.05,-38.03 217.08,-46.03 222.35,-53.93"/>
<polygon fill="black" stroke="black" points="219.4,-55.82 227.54,-62.6 225.41,-52.22 219.4,-55.82"/>
<text text-anchor="middle" x="198.34" y="-36.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge6" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M223.61,-87.8C217.71,-93.4 209.8,-99.59 201.34,-102.56 168.15,-114.18 154.17,-117.25 122.23,-102.56 114.74,-99.11 108.33,-93 103.23,-86.67"/>
<polygon fill="black" stroke="black" points="105.78,-84.22 97.1,-78.14 100.09,-88.31 105.78,-84.22"/>
<text text-anchor="middle" x="161.78" y="-115.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge5" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M221.29,-68.76C211.01,-62.4 196.25,-53.27 184.13,-45.77"/>
<polygon fill="black" stroke="black" points="185.84,-42.71 175.5,-40.42 182.16,-48.66 185.84,-42.71"/>
<text text-anchor="middle" x="198.34" y="-59.16" font-family="Times,serif" font-size="8.00">4</text>
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
<svg width="258pt" height="175pt"
 viewBox="0.00 0.00 258.45 175.24" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 171.24)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-171.24 254.45,-171.24 254.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-19.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-17.34" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-107.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-105.34" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 0&#45;&gt;1 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;1</title>
<path fill="none" stroke="black" d="M20.95,-33.84C25.89,-48.29 35.1,-70.35 49.11,-85.24 53.51,-89.91 59.13,-93.87 64.71,-97.09"/>
<polygon fill="black" stroke="black" points="63.34,-100.32 73.83,-101.8 66.55,-94.1 63.34,-100.32"/>
<text text-anchor="middle" x="52.11" y="-91.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2 -->
<g id="node3" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="234.89" cy="-58.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="234.89" y="-56.34" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M30.71,-15.51C36.39,-14.2 43.02,-12.88 49.11,-12.24 51.76,-11.96 52.45,-12.21 55.11,-12.24 76.94,-12.46 82.53,-10.78 104.23,-13.24 137.7,-17.03 145.25,-22.99 177.34,-33.24 188.05,-36.66 190.99,-36.84 201.34,-41.24 204.89,-42.75 208.59,-44.49 212.16,-46.28"/>
<polygon fill="black" stroke="black" points="210.65,-49.44 221.13,-50.97 213.89,-43.24 210.65,-49.44"/>
<text text-anchor="middle" x="125.23" y="-19.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="161.78" cy="-58.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="161.78" y="-56.34" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge3" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M30.65,-23.05C55.25,-29.7 106.3,-43.5 136.63,-51.71"/>
<polygon fill="black" stroke="black" points="135.85,-55.12 146.42,-54.35 137.68,-48.36 135.85,-55.12"/>
<text text-anchor="middle" x="88.67" y="-45.84" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M78.23,-95.56C66.57,-81.13 46.57,-56.37 32.51,-38.98"/>
<polygon fill="black" stroke="black" points="35.15,-36.68 26.15,-31.1 29.71,-41.08 35.15,-36.68"/>
<text text-anchor="middle" x="52.11" y="-67.84" font-family="Times,serif" font-size="8.00">8</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M97.1,-120.82C102.72,-129.58 111.33,-140.23 122.23,-145.24 154.17,-159.93 172.45,-165.27 201.34,-145.24 221,-131.6 228.8,-104.56 231.89,-84.22"/>
<polygon fill="black" stroke="black" points="235.4,-84.38 233.14,-74.02 228.45,-83.52 235.4,-84.38"/>
<text text-anchor="middle" x="161.78" y="-160.84" font-family="Times,serif" font-size="8.00">6</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge6" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M102.86,-100.45C110.48,-96.35 120.11,-90.85 128.23,-85.24 132.79,-82.08 137.49,-78.43 141.86,-74.86"/>
<polygon fill="black" stroke="black" points="144.33,-77.36 149.71,-68.24 139.81,-72.01 144.33,-77.36"/>
<text text-anchor="middle" x="125.23" y="-90.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;0 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;0</title>
<path fill="none" stroke="black" d="M221.89,-49.64C215.77,-45.28 208.18,-39.94 201.34,-35.24 190.77,-27.98 188.87,-24.83 177.34,-19.24 156.68,-9.22 150.9,-6.84 128.23,-3.24 93.43,2.29 83.66,-1.34 49.11,-8.24 46.15,-8.83 43.09,-9.62 40.08,-10.5"/>
<polygon fill="black" stroke="black" points="38.69,-7.27 30.28,-13.72 40.88,-13.92 38.69,-7.27"/>
<text text-anchor="middle" x="125.23" y="-5.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge8" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M220.61,-65C200.96,-74.68 162.74,-92.32 128.23,-101.24 123.87,-102.36 119.18,-103.31 114.63,-104.09"/>
<polygon fill="black" stroke="black" points="113.82,-100.67 104.45,-105.63 114.87,-107.59 113.82,-100.67"/>
<text text-anchor="middle" x="161.78" y="-97.84" font-family="Times,serif" font-size="8.00">12</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge9" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M219.18,-58.24C210.12,-58.24 198.28,-58.24 187.79,-58.24"/>
<polygon fill="black" stroke="black" points="187.52,-54.74 177.52,-58.24 187.52,-61.74 187.52,-54.74"/>
<text text-anchor="middle" x="198.34" y="-60.84" font-family="Times,serif" font-size="8.00">6</text>
</g>
<!-- 3&#45;&gt;0 -->
<g id="edge10" class="edge">
<title>3&#45;&gt;0</title>
<path fill="none" stroke="black" d="M149.57,-48.51C138.78,-39.8 121.57,-27.61 104.23,-22.24 83.75,-15.89 59.28,-15.62 41.4,-16.66"/>
<polygon fill="black" stroke="black" points="40.92,-13.18 31.22,-17.43 41.45,-20.16 40.92,-13.18"/>
<text text-anchor="middle" x="88.67" y="-24.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge11" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M147,-63.39C139.37,-66.55 129.91,-70.99 122.23,-76.24 116.94,-79.84 111.7,-84.32 107.02,-88.74"/>
<polygon fill="black" stroke="black" points="104.38,-86.43 99.76,-95.96 109.32,-91.39 104.38,-86.43"/>
<text text-anchor="middle" x="125.23" y="-78.84" font-family="Times,serif" font-size="8.00">8</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge12" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M176.06,-51.61C181.83,-49.08 188.76,-46.5 195.34,-45.24 200.75,-44.2 206.44,-45 211.74,-46.61"/>
<polygon fill="black" stroke="black" points="210.69,-49.96 221.28,-50.46 213.31,-43.47 210.69,-49.96"/>
<text text-anchor="middle" x="198.34" y="-47.84" font-family="Times,serif" font-size="8.00">3</text>
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
┌────────────────────┬─────────────┬────────────────────┬───────────────────────┐
│         a          │  binaryop   │         b          │ emult(a, b, binaryop) │
├────────────────────┼─────────────┼────────────────────┼───────────────────────┤
│      0  1  2  3    │ times_int32 │      0  1  2  3    │      0  1  2  3       │
│    ────────────    │             │    ────────────    │    ────────────       │
│  0│     2  1  1    │             │  0│           4    │  0│           4       │
│  1│  2     3  2    │             │  1│  4     2  1    │  1│  8     6  2       │
│  2│  1  3     3    │             │  2│     4     2    │  2│    12     6       │
│  3│  1  2  3       │             │  3│     4  1       │  3│     8  3          │
│                    │             │                    │                       │
└────────────────────┴─────────────┴────────────────────┴───────────────────────┘
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
│  0│           5    │  0│          -3    │  0│           4    │  0│           0    │
│  1│  6     5  3    │  1│ -2     1  1    │  1│  8     6  2    │  1│  0     1  2    │
│  2│     7     5    │  2│    -1     1    │  2│    12     6    │  2│     0     1    │
│  3│     6  4       │  3│    -2  2       │  3│     8  3       │  3│     0  3       │
│                    │                    │                    │                    │
└────────────────────┴────────────────────┴────────────────────┴────────────────────┘
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
<svg width="258pt" height="175pt"
 viewBox="0.00 0.00 258.45 175.24" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 171.24)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-171.24 254.45,-171.24 254.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-19.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-17.34" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-107.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-105.34" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 0&#45;&gt;1 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;1</title>
<path fill="none" stroke="black" d="M20.95,-33.84C25.89,-48.29 35.1,-70.35 49.11,-85.24 53.51,-89.91 59.13,-93.87 64.71,-97.09"/>
<polygon fill="black" stroke="black" points="63.34,-100.32 73.83,-101.8 66.55,-94.1 63.34,-100.32"/>
<text text-anchor="middle" x="52.11" y="-91.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2 -->
<g id="node3" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="234.89" cy="-58.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="234.89" y="-56.34" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M30.71,-15.51C36.39,-14.2 43.02,-12.88 49.11,-12.24 51.76,-11.96 52.45,-12.21 55.11,-12.24 76.94,-12.46 82.53,-10.78 104.23,-13.24 137.7,-17.03 145.25,-22.99 177.34,-33.24 188.05,-36.66 190.99,-36.84 201.34,-41.24 204.89,-42.75 208.59,-44.49 212.16,-46.28"/>
<polygon fill="black" stroke="black" points="210.65,-49.44 221.13,-50.97 213.89,-43.24 210.65,-49.44"/>
<text text-anchor="middle" x="125.23" y="-19.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="161.78" cy="-58.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="161.78" y="-56.34" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge3" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M30.65,-23.05C55.25,-29.7 106.3,-43.5 136.63,-51.71"/>
<polygon fill="black" stroke="black" points="135.85,-55.12 146.42,-54.35 137.68,-48.36 135.85,-55.12"/>
<text text-anchor="middle" x="88.67" y="-45.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M78.23,-95.56C66.57,-81.13 46.57,-56.37 32.51,-38.98"/>
<polygon fill="black" stroke="black" points="35.15,-36.68 26.15,-31.1 29.71,-41.08 35.15,-36.68"/>
<text text-anchor="middle" x="52.11" y="-67.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M97.1,-120.82C102.72,-129.58 111.33,-140.23 122.23,-145.24 154.17,-159.93 172.45,-165.27 201.34,-145.24 221,-131.6 228.8,-104.56 231.89,-84.22"/>
<polygon fill="black" stroke="black" points="235.4,-84.38 233.14,-74.02 228.45,-83.52 235.4,-84.38"/>
<text text-anchor="middle" x="161.78" y="-160.84" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge6" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M102.86,-100.45C110.48,-96.35 120.11,-90.85 128.23,-85.24 132.79,-82.08 137.49,-78.43 141.86,-74.86"/>
<polygon fill="black" stroke="black" points="144.33,-77.36 149.71,-68.24 139.81,-72.01 144.33,-77.36"/>
<text text-anchor="middle" x="125.23" y="-90.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;0 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;0</title>
<path fill="none" stroke="black" d="M221.89,-49.64C215.77,-45.28 208.18,-39.94 201.34,-35.24 190.77,-27.98 188.87,-24.83 177.34,-19.24 156.68,-9.22 150.9,-6.84 128.23,-3.24 93.43,2.29 83.66,-1.34 49.11,-8.24 46.15,-8.83 43.09,-9.62 40.08,-10.5"/>
<polygon fill="black" stroke="black" points="38.69,-7.27 30.28,-13.72 40.88,-13.92 38.69,-7.27"/>
<text text-anchor="middle" x="125.23" y="-5.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge8" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M220.61,-65C200.96,-74.68 162.74,-92.32 128.23,-101.24 123.87,-102.36 119.18,-103.31 114.63,-104.09"/>
<polygon fill="black" stroke="black" points="113.82,-100.67 104.45,-105.63 114.87,-107.59 113.82,-100.67"/>
<text text-anchor="middle" x="161.78" y="-97.84" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge9" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M219.18,-58.24C210.12,-58.24 198.28,-58.24 187.79,-58.24"/>
<polygon fill="black" stroke="black" points="187.52,-54.74 177.52,-58.24 187.52,-61.74 187.52,-54.74"/>
<text text-anchor="middle" x="198.34" y="-60.84" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 3&#45;&gt;0 -->
<g id="edge10" class="edge">
<title>3&#45;&gt;0</title>
<path fill="none" stroke="black" d="M149.57,-48.51C138.78,-39.8 121.57,-27.61 104.23,-22.24 83.75,-15.89 59.28,-15.62 41.4,-16.66"/>
<polygon fill="black" stroke="black" points="40.92,-13.18 31.22,-17.43 41.45,-20.16 40.92,-13.18"/>
<text text-anchor="middle" x="88.67" y="-24.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge11" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M147,-63.39C139.37,-66.55 129.91,-70.99 122.23,-76.24 116.94,-79.84 111.7,-84.32 107.02,-88.74"/>
<polygon fill="black" stroke="black" points="104.38,-86.43 99.76,-95.96 109.32,-91.39 104.38,-86.43"/>
<text text-anchor="middle" x="125.23" y="-78.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge12" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M176.06,-51.61C181.83,-49.08 188.76,-46.5 195.34,-45.24 200.75,-44.2 206.44,-45 211.74,-46.61"/>
<polygon fill="black" stroke="black" points="210.69,-49.96 221.28,-50.46 213.31,-43.47 210.69,-49.96"/>
<text text-anchor="middle" x="198.34" y="-47.84" font-family="Times,serif" font-size="8.00">3</text>
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
<svg width="258pt" height="130pt"
 viewBox="0.00 0.00 258.45 129.56" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 125.56)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-125.56 254.45,-125.56 254.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-13.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node2" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-64.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-62.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M28.94,-24.09C39.45,-31.33 54.73,-41.86 67.07,-50.36"/>
<polygon fill="black" stroke="black" points="65.2,-53.33 75.42,-56.12 69.18,-47.56 65.2,-53.33"/>
<text text-anchor="middle" x="52.11" y="-44.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1 -->
<g id="node3" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="161.78" cy="-32.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="161.78" y="-30.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M103.04,-58.54C112.75,-54.17 126.14,-48.15 137.57,-43"/>
<polygon fill="black" stroke="black" points="139.3,-46.06 146.98,-38.77 136.43,-39.68 139.3,-46.06"/>
<text text-anchor="middle" x="125.23" y="-52.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 2 -->
<g id="node4" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="234.89" cy="-76.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="234.89" y="-74.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M104.29,-65.77C128.91,-67.82 178.9,-71.98 209.09,-74.49"/>
<polygon fill="black" stroke="black" points="208.98,-78 219.23,-75.34 209.56,-71.02 208.98,-78"/>
<text text-anchor="middle" x="161.78" y="-75.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge2" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M146.2,-30.74C140.58,-30.05 134.11,-29.26 128.23,-28.56 98.41,-24.99 64.11,-21.02 41.47,-18.41"/>
<polygon fill="black" stroke="black" points="41.71,-14.91 31.37,-17.25 40.91,-21.87 41.71,-14.91"/>
<text text-anchor="middle" x="88.67" y="-28.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M146.35,-30.52C138.88,-30.05 129.77,-30.42 122.23,-33.56 116.22,-36.05 110.68,-40.2 105.93,-44.67"/>
<polygon fill="black" stroke="black" points="103.09,-42.58 98.76,-52.25 108.17,-47.39 103.09,-42.58"/>
<text text-anchor="middle" x="125.23" y="-36.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M177.44,-30.02C184.97,-29.42 194.07,-29.83 201.34,-33.56 210.05,-38.03 217.08,-46.03 222.35,-53.93"/>
<polygon fill="black" stroke="black" points="219.4,-55.82 227.54,-62.6 225.41,-52.22 219.4,-55.82"/>
<text text-anchor="middle" x="198.34" y="-36.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge6" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M223.61,-87.8C217.71,-93.4 209.8,-99.59 201.34,-102.56 168.15,-114.18 154.17,-117.25 122.23,-102.56 114.74,-99.11 108.33,-93 103.23,-86.67"/>
<polygon fill="black" stroke="black" points="105.78,-84.22 97.1,-78.14 100.09,-88.31 105.78,-84.22"/>
<text text-anchor="middle" x="161.78" y="-115.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge5" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M221.29,-68.76C211.01,-62.4 196.25,-53.27 184.13,-45.77"/>
<polygon fill="black" stroke="black" points="185.84,-42.71 175.5,-40.42 182.16,-48.66 185.84,-42.71"/>
<text text-anchor="middle" x="198.34" y="-59.16" font-family="Times,serif" font-size="8.00">4</text>
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
<svg width="263pt" height="131pt"
 viewBox="0.00 0.00 263.45 130.56" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 126.56)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-126.56 259.45,-126.56 259.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-13.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node2" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-64.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-62.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M28.94,-24.09C39.45,-31.33 54.73,-41.86 67.07,-50.36"/>
<polygon fill="black" stroke="black" points="65.2,-53.33 75.42,-56.12 69.18,-47.56 65.2,-53.33"/>
<text text-anchor="middle" x="52.11" y="-44.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1 -->
<g id="node3" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="161.78" cy="-32.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="161.78" y="-30.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M103.04,-58.54C112.75,-54.17 126.14,-48.15 137.57,-43"/>
<polygon fill="black" stroke="black" points="139.3,-46.06 146.98,-38.77 136.43,-39.68 139.3,-46.06"/>
<text text-anchor="middle" x="125.23" y="-52.16" font-family="Times,serif" font-size="8.00">8</text>
</g>
<!-- 2 -->
<g id="node4" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="239.89" cy="-76.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="239.89" y="-74.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M104.25,-65.73C129.7,-67.78 182.52,-72.02 213.91,-74.55"/>
<polygon fill="black" stroke="black" points="213.79,-78.05 224.03,-75.36 214.35,-71.07 213.79,-78.05"/>
<text text-anchor="middle" x="161.78" y="-74.16" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge2" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M146.2,-30.74C140.58,-30.05 134.11,-29.26 128.23,-28.56 98.41,-24.99 64.11,-21.02 41.47,-18.41"/>
<polygon fill="black" stroke="black" points="41.71,-14.91 31.37,-17.25 40.91,-21.87 41.71,-14.91"/>
<text text-anchor="middle" x="88.67" y="-28.16" font-family="Times,serif" font-size="8.00">8</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M146.35,-30.52C138.88,-30.05 129.77,-30.42 122.23,-33.56 116.22,-36.05 110.68,-40.2 105.93,-44.67"/>
<polygon fill="black" stroke="black" points="103.09,-42.58 98.76,-52.25 108.17,-47.39 103.09,-42.58"/>
<text text-anchor="middle" x="125.23" y="-36.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M177.28,-29.83C186.15,-28.89 197.45,-29.01 206.34,-33.56 215.06,-38.02 222.09,-46.02 227.36,-53.91"/>
<polygon fill="black" stroke="black" points="224.41,-55.81 232.55,-62.59 230.42,-52.21 224.41,-55.81"/>
<text text-anchor="middle" x="200.84" y="-36.16" font-family="Times,serif" font-size="8.00">6</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge6" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M228.61,-87.8C222.71,-93.4 214.8,-99.59 206.34,-102.56 171.06,-114.92 156.19,-118.18 122.23,-102.56 114.74,-99.11 108.33,-93 103.23,-86.67"/>
<polygon fill="black" stroke="black" points="105.78,-84.22 97.1,-78.14 100.09,-88.31 105.78,-84.22"/>
<text text-anchor="middle" x="161.78" y="-116.16" font-family="Times,serif" font-size="8.00">6</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge5" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M226.17,-69.2C214.79,-62.62 197.88,-52.84 184.38,-45.04"/>
<polygon fill="black" stroke="black" points="186.1,-42 175.69,-40.02 182.6,-48.06 186.1,-42"/>
<text text-anchor="middle" x="200.84" y="-60.16" font-family="Times,serif" font-size="8.00">12</text>
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
┌────────────────────┬─────────────┬────────────────────┬──────────────────────────────────────────────┐
│         a          │  binaryop   │         b          │ eunion(a, 3::scalar, b, 4::scalar, binaryop) │
├────────────────────┼─────────────┼────────────────────┼──────────────────────────────────────────────┤
│      0  1  2  3    │ times_int32 │      0  1  2  3    │      0  1  2  3                              │
│    ────────────    │             │    ────────────    │    ────────────                              │
│  0│     2  1  1    │             │  0│           4    │  0│     8  4  4                              │
│  1│  2     3  2    │             │  1│  4     2  1    │  1│  8     6  2                              │
│  2│  1  3     3    │             │  2│     4     2    │  2│  4 12     6                              │
│  3│  1  2  3       │             │  3│     4  1       │  3│  4  8  3                                 │
│                    │             │                    │                                              │
└────────────────────┴─────────────┴────────────────────┴──────────────────────────────────────────────┘
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
<svg width="258pt" height="175pt"
 viewBox="0.00 0.00 258.45 175.24" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 171.24)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-171.24 254.45,-171.24 254.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-19.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-17.34" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-107.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-105.34" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 0&#45;&gt;1 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;1</title>
<path fill="none" stroke="black" d="M20.95,-33.84C25.89,-48.29 35.1,-70.35 49.11,-85.24 53.51,-89.91 59.13,-93.87 64.71,-97.09"/>
<polygon fill="black" stroke="black" points="63.34,-100.32 73.83,-101.8 66.55,-94.1 63.34,-100.32"/>
<text text-anchor="middle" x="52.11" y="-91.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2 -->
<g id="node3" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="234.89" cy="-58.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="234.89" y="-56.34" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M30.71,-15.51C36.39,-14.2 43.02,-12.88 49.11,-12.24 51.76,-11.96 52.45,-12.21 55.11,-12.24 76.94,-12.46 82.53,-10.78 104.23,-13.24 137.7,-17.03 145.25,-22.99 177.34,-33.24 188.05,-36.66 190.99,-36.84 201.34,-41.24 204.89,-42.75 208.59,-44.49 212.16,-46.28"/>
<polygon fill="black" stroke="black" points="210.65,-49.44 221.13,-50.97 213.89,-43.24 210.65,-49.44"/>
<text text-anchor="middle" x="125.23" y="-19.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="161.78" cy="-58.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="161.78" y="-56.34" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge3" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M30.65,-23.05C55.25,-29.7 106.3,-43.5 136.63,-51.71"/>
<polygon fill="black" stroke="black" points="135.85,-55.12 146.42,-54.35 137.68,-48.36 135.85,-55.12"/>
<text text-anchor="middle" x="88.67" y="-45.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M78.23,-95.56C66.57,-81.13 46.57,-56.37 32.51,-38.98"/>
<polygon fill="black" stroke="black" points="35.15,-36.68 26.15,-31.1 29.71,-41.08 35.15,-36.68"/>
<text text-anchor="middle" x="52.11" y="-67.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M97.1,-120.82C102.72,-129.58 111.33,-140.23 122.23,-145.24 154.17,-159.93 172.45,-165.27 201.34,-145.24 221,-131.6 228.8,-104.56 231.89,-84.22"/>
<polygon fill="black" stroke="black" points="235.4,-84.38 233.14,-74.02 228.45,-83.52 235.4,-84.38"/>
<text text-anchor="middle" x="161.78" y="-160.84" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge6" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M102.86,-100.45C110.48,-96.35 120.11,-90.85 128.23,-85.24 132.79,-82.08 137.49,-78.43 141.86,-74.86"/>
<polygon fill="black" stroke="black" points="144.33,-77.36 149.71,-68.24 139.81,-72.01 144.33,-77.36"/>
<text text-anchor="middle" x="125.23" y="-90.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;0 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;0</title>
<path fill="none" stroke="black" d="M221.89,-49.64C215.77,-45.28 208.18,-39.94 201.34,-35.24 190.77,-27.98 188.87,-24.83 177.34,-19.24 156.68,-9.22 150.9,-6.84 128.23,-3.24 93.43,2.29 83.66,-1.34 49.11,-8.24 46.15,-8.83 43.09,-9.62 40.08,-10.5"/>
<polygon fill="black" stroke="black" points="38.69,-7.27 30.28,-13.72 40.88,-13.92 38.69,-7.27"/>
<text text-anchor="middle" x="125.23" y="-5.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge8" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M220.61,-65C200.96,-74.68 162.74,-92.32 128.23,-101.24 123.87,-102.36 119.18,-103.31 114.63,-104.09"/>
<polygon fill="black" stroke="black" points="113.82,-100.67 104.45,-105.63 114.87,-107.59 113.82,-100.67"/>
<text text-anchor="middle" x="161.78" y="-97.84" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge9" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M219.18,-58.24C210.12,-58.24 198.28,-58.24 187.79,-58.24"/>
<polygon fill="black" stroke="black" points="187.52,-54.74 177.52,-58.24 187.52,-61.74 187.52,-54.74"/>
<text text-anchor="middle" x="198.34" y="-60.84" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 3&#45;&gt;0 -->
<g id="edge10" class="edge">
<title>3&#45;&gt;0</title>
<path fill="none" stroke="black" d="M149.57,-48.51C138.78,-39.8 121.57,-27.61 104.23,-22.24 83.75,-15.89 59.28,-15.62 41.4,-16.66"/>
<polygon fill="black" stroke="black" points="40.92,-13.18 31.22,-17.43 41.45,-20.16 40.92,-13.18"/>
<text text-anchor="middle" x="88.67" y="-24.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge11" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M147,-63.39C139.37,-66.55 129.91,-70.99 122.23,-76.24 116.94,-79.84 111.7,-84.32 107.02,-88.74"/>
<polygon fill="black" stroke="black" points="104.38,-86.43 99.76,-95.96 109.32,-91.39 104.38,-86.43"/>
<text text-anchor="middle" x="125.23" y="-78.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge12" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M176.06,-51.61C181.83,-49.08 188.76,-46.5 195.34,-45.24 200.75,-44.2 206.44,-45 211.74,-46.61"/>
<polygon fill="black" stroke="black" points="210.69,-49.96 221.28,-50.46 213.31,-43.47 210.69,-49.96"/>
<text text-anchor="middle" x="198.34" y="-47.84" font-family="Times,serif" font-size="8.00">3</text>
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
<svg width="258pt" height="130pt"
 viewBox="0.00 0.00 258.45 129.56" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 125.56)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-125.56 254.45,-125.56 254.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-13.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node2" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-64.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-62.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M28.94,-24.09C39.45,-31.33 54.73,-41.86 67.07,-50.36"/>
<polygon fill="black" stroke="black" points="65.2,-53.33 75.42,-56.12 69.18,-47.56 65.2,-53.33"/>
<text text-anchor="middle" x="52.11" y="-44.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1 -->
<g id="node3" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="161.78" cy="-32.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="161.78" y="-30.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M103.04,-58.54C112.75,-54.17 126.14,-48.15 137.57,-43"/>
<polygon fill="black" stroke="black" points="139.3,-46.06 146.98,-38.77 136.43,-39.68 139.3,-46.06"/>
<text text-anchor="middle" x="125.23" y="-52.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 2 -->
<g id="node4" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="234.89" cy="-76.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="234.89" y="-74.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M104.29,-65.77C128.91,-67.82 178.9,-71.98 209.09,-74.49"/>
<polygon fill="black" stroke="black" points="208.98,-78 219.23,-75.34 209.56,-71.02 208.98,-78"/>
<text text-anchor="middle" x="161.78" y="-75.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge2" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M146.2,-30.74C140.58,-30.05 134.11,-29.26 128.23,-28.56 98.41,-24.99 64.11,-21.02 41.47,-18.41"/>
<polygon fill="black" stroke="black" points="41.71,-14.91 31.37,-17.25 40.91,-21.87 41.71,-14.91"/>
<text text-anchor="middle" x="88.67" y="-28.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M146.35,-30.52C138.88,-30.05 129.77,-30.42 122.23,-33.56 116.22,-36.05 110.68,-40.2 105.93,-44.67"/>
<polygon fill="black" stroke="black" points="103.09,-42.58 98.76,-52.25 108.17,-47.39 103.09,-42.58"/>
<text text-anchor="middle" x="125.23" y="-36.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M177.44,-30.02C184.97,-29.42 194.07,-29.83 201.34,-33.56 210.05,-38.03 217.08,-46.03 222.35,-53.93"/>
<polygon fill="black" stroke="black" points="219.4,-55.82 227.54,-62.6 225.41,-52.22 219.4,-55.82"/>
<text text-anchor="middle" x="198.34" y="-36.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge6" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M223.61,-87.8C217.71,-93.4 209.8,-99.59 201.34,-102.56 168.15,-114.18 154.17,-117.25 122.23,-102.56 114.74,-99.11 108.33,-93 103.23,-86.67"/>
<polygon fill="black" stroke="black" points="105.78,-84.22 97.1,-78.14 100.09,-88.31 105.78,-84.22"/>
<text text-anchor="middle" x="161.78" y="-115.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge5" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M221.29,-68.76C211.01,-62.4 196.25,-53.27 184.13,-45.77"/>
<polygon fill="black" stroke="black" points="185.84,-42.71 175.5,-40.42 182.16,-48.66 185.84,-42.71"/>
<text text-anchor="middle" x="198.34" y="-59.16" font-family="Times,serif" font-size="8.00">4</text>
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
<svg width="258pt" height="175pt"
 viewBox="0.00 0.00 258.45 175.24" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 171.24)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-171.24 254.45,-171.24 254.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-19.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-17.34" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-107.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-105.34" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 0&#45;&gt;1 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;1</title>
<path fill="none" stroke="black" d="M20.95,-33.84C25.89,-48.29 35.1,-70.35 49.11,-85.24 53.51,-89.91 59.13,-93.87 64.71,-97.09"/>
<polygon fill="black" stroke="black" points="63.34,-100.32 73.83,-101.8 66.55,-94.1 63.34,-100.32"/>
<text text-anchor="middle" x="52.11" y="-91.84" font-family="Times,serif" font-size="8.00">8</text>
</g>
<!-- 2 -->
<g id="node3" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="234.89" cy="-58.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="234.89" y="-56.34" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M30.71,-15.51C36.39,-14.2 43.02,-12.88 49.11,-12.24 51.76,-11.96 52.45,-12.21 55.11,-12.24 76.94,-12.46 82.53,-10.78 104.23,-13.24 137.7,-17.03 145.25,-22.99 177.34,-33.24 188.05,-36.66 190.99,-36.84 201.34,-41.24 204.89,-42.75 208.59,-44.49 212.16,-46.28"/>
<polygon fill="black" stroke="black" points="210.65,-49.44 221.13,-50.97 213.89,-43.24 210.65,-49.44"/>
<text text-anchor="middle" x="125.23" y="-19.84" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="161.78" cy="-58.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="161.78" y="-56.34" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge3" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M30.65,-23.05C55.25,-29.7 106.3,-43.5 136.63,-51.71"/>
<polygon fill="black" stroke="black" points="135.85,-55.12 146.42,-54.35 137.68,-48.36 135.85,-55.12"/>
<text text-anchor="middle" x="88.67" y="-45.84" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M78.23,-95.56C66.57,-81.13 46.57,-56.37 32.51,-38.98"/>
<polygon fill="black" stroke="black" points="35.15,-36.68 26.15,-31.1 29.71,-41.08 35.15,-36.68"/>
<text text-anchor="middle" x="52.11" y="-67.84" font-family="Times,serif" font-size="8.00">8</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M97.1,-120.82C102.72,-129.58 111.33,-140.23 122.23,-145.24 154.17,-159.93 172.45,-165.27 201.34,-145.24 221,-131.6 228.8,-104.56 231.89,-84.22"/>
<polygon fill="black" stroke="black" points="235.4,-84.38 233.14,-74.02 228.45,-83.52 235.4,-84.38"/>
<text text-anchor="middle" x="161.78" y="-160.84" font-family="Times,serif" font-size="8.00">6</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge6" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M102.86,-100.45C110.48,-96.35 120.11,-90.85 128.23,-85.24 132.79,-82.08 137.49,-78.43 141.86,-74.86"/>
<polygon fill="black" stroke="black" points="144.33,-77.36 149.71,-68.24 139.81,-72.01 144.33,-77.36"/>
<text text-anchor="middle" x="125.23" y="-90.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;0 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;0</title>
<path fill="none" stroke="black" d="M221.89,-49.64C215.77,-45.28 208.18,-39.94 201.34,-35.24 190.77,-27.98 188.87,-24.83 177.34,-19.24 156.68,-9.22 150.9,-6.84 128.23,-3.24 93.43,2.29 83.66,-1.34 49.11,-8.24 46.15,-8.83 43.09,-9.62 40.08,-10.5"/>
<polygon fill="black" stroke="black" points="38.69,-7.27 30.28,-13.72 40.88,-13.92 38.69,-7.27"/>
<text text-anchor="middle" x="125.23" y="-5.84" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge8" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M220.61,-65C200.96,-74.68 162.74,-92.32 128.23,-101.24 123.87,-102.36 119.18,-103.31 114.63,-104.09"/>
<polygon fill="black" stroke="black" points="113.82,-100.67 104.45,-105.63 114.87,-107.59 113.82,-100.67"/>
<text text-anchor="middle" x="161.78" y="-97.84" font-family="Times,serif" font-size="8.00">12</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge9" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M219.18,-58.24C210.12,-58.24 198.28,-58.24 187.79,-58.24"/>
<polygon fill="black" stroke="black" points="187.52,-54.74 177.52,-58.24 187.52,-61.74 187.52,-54.74"/>
<text text-anchor="middle" x="198.34" y="-60.84" font-family="Times,serif" font-size="8.00">6</text>
</g>
<!-- 3&#45;&gt;0 -->
<g id="edge10" class="edge">
<title>3&#45;&gt;0</title>
<path fill="none" stroke="black" d="M149.57,-48.51C138.78,-39.8 121.57,-27.61 104.23,-22.24 83.75,-15.89 59.28,-15.62 41.4,-16.66"/>
<polygon fill="black" stroke="black" points="40.92,-13.18 31.22,-17.43 41.45,-20.16 40.92,-13.18"/>
<text text-anchor="middle" x="88.67" y="-24.84" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge11" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M147,-63.39C139.37,-66.55 129.91,-70.99 122.23,-76.24 116.94,-79.84 111.7,-84.32 107.02,-88.74"/>
<polygon fill="black" stroke="black" points="104.38,-86.43 99.76,-95.96 109.32,-91.39 104.38,-86.43"/>
<text text-anchor="middle" x="125.23" y="-78.84" font-family="Times,serif" font-size="8.00">8</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge12" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M176.06,-51.61C181.83,-49.08 188.76,-46.5 195.34,-45.24 200.75,-44.2 206.44,-45 211.74,-46.61"/>
<polygon fill="black" stroke="black" points="210.69,-49.96 221.28,-50.46 213.31,-43.47 210.69,-49.96"/>
<text text-anchor="middle" x="198.34" y="-47.84" font-family="Times,serif" font-size="8.00">3</text>
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
┌────────────────────┬───────────────────┬───────────────┐
│         b          │      monoid       │ reduce_scalar │
├────────────────────┼───────────────────┼───────────────┤
│      0  1  2  3    │ plus_monoid_int32 │ int32:22      │
│    ────────────    │                   │               │
│  0│           4    │                   │               │
│  1│  4     2  1    │                   │               │
│  2│     4     2    │                   │               │
│  3│     4  1       │                   │               │
│                    │                   │               │
└────────────────────┴───────────────────┴───────────────┘
(1 row)

```
The entire matrix can be reduced to a scalar value with a provided
monoid that changes the reduction operation:
``` postgres-console
select print(b) as b, 'min_monoid_int32' as monoid, reduce_scalar(b, 'min_monoid_int32') from test_fixture;
┌────────────────────┬──────────────────┬───────────────┐
│         b          │      monoid      │ reduce_scalar │
├────────────────────┼──────────────────┼───────────────┤
│      0  1  2  3    │ min_monoid_int32 │ int32:1       │
│    ────────────    │                  │               │
│  0│           4    │                  │               │
│  1│  4     2  1    │                  │               │
│  2│     4     2    │                  │               │
│  3│     4  1       │                  │               │
│                    │                  │               │
└────────────────────┴──────────────────┴───────────────┘
(1 row)

```
The matrix can also be reduced to a column vector:
``` postgres-console
select print(b) as b, 'plus_monoid_int32' as monoid, print(reduce_cols(b)) as reduce_cols from test_fixture;
┌────────────────────┬───────────────────┬─────────────┐
│         b          │      monoid       │ reduce_cols │
├────────────────────┼───────────────────┼─────────────┤
│      0  1  2  3    │ plus_monoid_int32 │             │
│    ────────────    │                   │    ───      │
│  0│           4    │                   │  0│  4      │
│  1│  4     2  1    │                   │  1│  7      │
│  2│     4     2    │                   │  2│  6      │
│  3│     4  1       │                   │  3│  5      │
│                    │                   │             │
└────────────────────┴───────────────────┴─────────────┘
(1 row)

```
To reduce a row vector:
``` postgres-console
select print(b) as b, 'plus_monoid_int32' as monoid, print(reduce_rows(b)) as reduce_rows from test_fixture;
┌────────────────────┬───────────────────┬─────────────┐
│         b          │      monoid       │ reduce_rows │
├────────────────────┼───────────────────┼─────────────┤
│      0  1  2  3    │ plus_monoid_int32 │             │
│    ────────────    │                   │    ───      │
│  0│           4    │                   │  0│  4      │
│  1│  4     2  1    │                   │  1│  8      │
│  2│     4     2    │                   │  2│  3      │
│  3│     4  1       │                   │  3│  7      │
│                    │                   │             │
└────────────────────┴───────────────────┴─────────────┘
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
┌────────────────────┬──────────────────┬────────────────────┬────────────────────┐
│         a          │     semiring     │         b          │     mxm(a, b)      │
├────────────────────┼──────────────────┼────────────────────┼────────────────────┤
│      0  1  2  3    │ plus_times_int32 │      0  1  2  3    │      0  1  2  3    │
│    ────────────    │                  │    ────────────    │    ────────────    │
│  0│     2  1  1    │                  │  0│           4    │  0│  8  8  5  4    │
│  1│  2     3  2    │                  │  1│  4     2  1    │  1│    20  2 14    │
│  2│  1  3     3    │                  │  2│     4     2    │  2│ 12 12  9  7    │
│  3│  1  2  3       │                  │  3│     4  1       │  3│  8 12  4 12    │
│                    │                  │                    │                    │
└────────────────────┴──────────────────┴────────────────────┴────────────────────┘
(1 row)

select draw(a) as binop_a_source, draw(b) as binop_b_source, draw(mxm(a, b)) as binop_c_source from test_fixture \gset
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 30%;">
        <!-- Diagram A -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="258pt" height="175pt"
 viewBox="0.00 0.00 258.45 175.24" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 171.24)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-171.24 254.45,-171.24 254.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-19.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-17.34" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-107.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-105.34" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 0&#45;&gt;1 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;1</title>
<path fill="none" stroke="black" d="M20.95,-33.84C25.89,-48.29 35.1,-70.35 49.11,-85.24 53.51,-89.91 59.13,-93.87 64.71,-97.09"/>
<polygon fill="black" stroke="black" points="63.34,-100.32 73.83,-101.8 66.55,-94.1 63.34,-100.32"/>
<text text-anchor="middle" x="52.11" y="-91.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2 -->
<g id="node3" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="234.89" cy="-58.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="234.89" y="-56.34" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M30.71,-15.51C36.39,-14.2 43.02,-12.88 49.11,-12.24 51.76,-11.96 52.45,-12.21 55.11,-12.24 76.94,-12.46 82.53,-10.78 104.23,-13.24 137.7,-17.03 145.25,-22.99 177.34,-33.24 188.05,-36.66 190.99,-36.84 201.34,-41.24 204.89,-42.75 208.59,-44.49 212.16,-46.28"/>
<polygon fill="black" stroke="black" points="210.65,-49.44 221.13,-50.97 213.89,-43.24 210.65,-49.44"/>
<text text-anchor="middle" x="125.23" y="-19.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="161.78" cy="-58.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="161.78" y="-56.34" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge3" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M30.65,-23.05C55.25,-29.7 106.3,-43.5 136.63,-51.71"/>
<polygon fill="black" stroke="black" points="135.85,-55.12 146.42,-54.35 137.68,-48.36 135.85,-55.12"/>
<text text-anchor="middle" x="88.67" y="-45.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M78.23,-95.56C66.57,-81.13 46.57,-56.37 32.51,-38.98"/>
<polygon fill="black" stroke="black" points="35.15,-36.68 26.15,-31.1 29.71,-41.08 35.15,-36.68"/>
<text text-anchor="middle" x="52.11" y="-67.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M97.1,-120.82C102.72,-129.58 111.33,-140.23 122.23,-145.24 154.17,-159.93 172.45,-165.27 201.34,-145.24 221,-131.6 228.8,-104.56 231.89,-84.22"/>
<polygon fill="black" stroke="black" points="235.4,-84.38 233.14,-74.02 228.45,-83.52 235.4,-84.38"/>
<text text-anchor="middle" x="161.78" y="-160.84" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge6" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M102.86,-100.45C110.48,-96.35 120.11,-90.85 128.23,-85.24 132.79,-82.08 137.49,-78.43 141.86,-74.86"/>
<polygon fill="black" stroke="black" points="144.33,-77.36 149.71,-68.24 139.81,-72.01 144.33,-77.36"/>
<text text-anchor="middle" x="125.23" y="-90.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;0 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;0</title>
<path fill="none" stroke="black" d="M221.89,-49.64C215.77,-45.28 208.18,-39.94 201.34,-35.24 190.77,-27.98 188.87,-24.83 177.34,-19.24 156.68,-9.22 150.9,-6.84 128.23,-3.24 93.43,2.29 83.66,-1.34 49.11,-8.24 46.15,-8.83 43.09,-9.62 40.08,-10.5"/>
<polygon fill="black" stroke="black" points="38.69,-7.27 30.28,-13.72 40.88,-13.92 38.69,-7.27"/>
<text text-anchor="middle" x="125.23" y="-5.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge8" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M220.61,-65C200.96,-74.68 162.74,-92.32 128.23,-101.24 123.87,-102.36 119.18,-103.31 114.63,-104.09"/>
<polygon fill="black" stroke="black" points="113.82,-100.67 104.45,-105.63 114.87,-107.59 113.82,-100.67"/>
<text text-anchor="middle" x="161.78" y="-97.84" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge9" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M219.18,-58.24C210.12,-58.24 198.28,-58.24 187.79,-58.24"/>
<polygon fill="black" stroke="black" points="187.52,-54.74 177.52,-58.24 187.52,-61.74 187.52,-54.74"/>
<text text-anchor="middle" x="198.34" y="-60.84" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 3&#45;&gt;0 -->
<g id="edge10" class="edge">
<title>3&#45;&gt;0</title>
<path fill="none" stroke="black" d="M149.57,-48.51C138.78,-39.8 121.57,-27.61 104.23,-22.24 83.75,-15.89 59.28,-15.62 41.4,-16.66"/>
<polygon fill="black" stroke="black" points="40.92,-13.18 31.22,-17.43 41.45,-20.16 40.92,-13.18"/>
<text text-anchor="middle" x="88.67" y="-24.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge11" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M147,-63.39C139.37,-66.55 129.91,-70.99 122.23,-76.24 116.94,-79.84 111.7,-84.32 107.02,-88.74"/>
<polygon fill="black" stroke="black" points="104.38,-86.43 99.76,-95.96 109.32,-91.39 104.38,-86.43"/>
<text text-anchor="middle" x="125.23" y="-78.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge12" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M176.06,-51.61C181.83,-49.08 188.76,-46.5 195.34,-45.24 200.75,-44.2 206.44,-45 211.74,-46.61"/>
<polygon fill="black" stroke="black" points="210.69,-49.96 221.28,-50.46 213.31,-43.47 210.69,-49.96"/>
<text text-anchor="middle" x="198.34" y="-47.84" font-family="Times,serif" font-size="8.00">3</text>
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
<svg width="258pt" height="130pt"
 viewBox="0.00 0.00 258.45 129.56" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 125.56)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-125.56 254.45,-125.56 254.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-13.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node2" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-64.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-62.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M28.94,-24.09C39.45,-31.33 54.73,-41.86 67.07,-50.36"/>
<polygon fill="black" stroke="black" points="65.2,-53.33 75.42,-56.12 69.18,-47.56 65.2,-53.33"/>
<text text-anchor="middle" x="52.11" y="-44.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1 -->
<g id="node3" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="161.78" cy="-32.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="161.78" y="-30.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M103.04,-58.54C112.75,-54.17 126.14,-48.15 137.57,-43"/>
<polygon fill="black" stroke="black" points="139.3,-46.06 146.98,-38.77 136.43,-39.68 139.3,-46.06"/>
<text text-anchor="middle" x="125.23" y="-52.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 2 -->
<g id="node4" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="234.89" cy="-76.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="234.89" y="-74.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M104.29,-65.77C128.91,-67.82 178.9,-71.98 209.09,-74.49"/>
<polygon fill="black" stroke="black" points="208.98,-78 219.23,-75.34 209.56,-71.02 208.98,-78"/>
<text text-anchor="middle" x="161.78" y="-75.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge2" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M146.2,-30.74C140.58,-30.05 134.11,-29.26 128.23,-28.56 98.41,-24.99 64.11,-21.02 41.47,-18.41"/>
<polygon fill="black" stroke="black" points="41.71,-14.91 31.37,-17.25 40.91,-21.87 41.71,-14.91"/>
<text text-anchor="middle" x="88.67" y="-28.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M146.35,-30.52C138.88,-30.05 129.77,-30.42 122.23,-33.56 116.22,-36.05 110.68,-40.2 105.93,-44.67"/>
<polygon fill="black" stroke="black" points="103.09,-42.58 98.76,-52.25 108.17,-47.39 103.09,-42.58"/>
<text text-anchor="middle" x="125.23" y="-36.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M177.44,-30.02C184.97,-29.42 194.07,-29.83 201.34,-33.56 210.05,-38.03 217.08,-46.03 222.35,-53.93"/>
<polygon fill="black" stroke="black" points="219.4,-55.82 227.54,-62.6 225.41,-52.22 219.4,-55.82"/>
<text text-anchor="middle" x="198.34" y="-36.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge6" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M223.61,-87.8C217.71,-93.4 209.8,-99.59 201.34,-102.56 168.15,-114.18 154.17,-117.25 122.23,-102.56 114.74,-99.11 108.33,-93 103.23,-86.67"/>
<polygon fill="black" stroke="black" points="105.78,-84.22 97.1,-78.14 100.09,-88.31 105.78,-84.22"/>
<text text-anchor="middle" x="161.78" y="-115.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge5" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M221.29,-68.76C211.01,-62.4 196.25,-53.27 184.13,-45.77"/>
<polygon fill="black" stroke="black" points="185.84,-42.71 175.5,-40.42 182.16,-48.66 185.84,-42.71"/>
<text text-anchor="middle" x="198.34" y="-59.16" font-family="Times,serif" font-size="8.00">4</text>
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
<svg width="263pt" height="142pt"
 viewBox="0.00 0.00 263.45 141.85" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 137.85)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-137.85 259.45,-137.85 259.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-47.85" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-45.95" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 0&#45;&gt;0 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;0</title>
<path fill="none" stroke="black" d="M9.4,-62.34C7.67,-72 9.72,-81.41 15.56,-81.41 19.2,-81.41 21.37,-77.74 22.06,-72.62"/>
<polygon fill="black" stroke="black" points="25.55,-72.22 21.71,-62.34 18.56,-72.46 25.55,-72.22"/>
<text text-anchor="middle" x="15.56" y="-84.01" font-family="Times,serif" font-size="8.00">8</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-47.85" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-45.95" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 0&#45;&gt;1 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;1</title>
<path fill="none" stroke="black" d="M31.29,-47.85C40.35,-47.85 52.19,-47.85 62.68,-47.85"/>
<polygon fill="black" stroke="black" points="62.95,-51.35 72.95,-47.85 62.95,-44.35 62.95,-51.35"/>
<text text-anchor="middle" x="52.11" y="-50.45" font-family="Times,serif" font-size="8.00">8</text>
</g>
<!-- 2 -->
<g id="node3" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="166.78" cy="-47.85" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="166.78" y="-45.95" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M28.24,-38.21C39.1,-29.92 56.11,-18.57 73.11,-13.85 86.44,-10.16 90.84,-10.4 104.23,-13.85 118.96,-17.65 133.95,-25.82 145.44,-33.19"/>
<polygon fill="black" stroke="black" points="143.59,-36.16 153.84,-38.84 147.49,-30.35 143.59,-36.16"/>
<text text-anchor="middle" x="88.67" y="-16.45" font-family="Times,serif" font-size="8.00">5</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="239.89" cy="-58.85" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="239.89" y="-56.95" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge4" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M22.34,-62.05C30.53,-79.72 47.43,-108.49 73.11,-115.85 119.78,-129.24 135.28,-127.78 182.34,-115.85 194.1,-112.87 197.18,-110.81 206.34,-102.85 213.66,-96.5 220.32,-88.21 225.68,-80.55"/>
<polygon fill="black" stroke="black" points="228.74,-82.27 231.34,-72 222.9,-78.41 228.74,-82.27"/>
<text text-anchor="middle" x="127.73" y="-127.45" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1&#45;&gt;1 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;1</title>
<path fill="none" stroke="black" d="M82.51,-62.34C80.78,-72 82.83,-81.41 88.67,-81.41 92.32,-81.41 94.49,-77.74 95.18,-72.62"/>
<polygon fill="black" stroke="black" points="98.66,-72.22 94.82,-62.34 91.67,-72.46 98.66,-72.22"/>
<text text-anchor="middle" x="88.67" y="-84.01" font-family="Times,serif" font-size="8.00">20</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge6" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M103.76,-52.64C109.43,-54.33 116.07,-56.02 122.23,-56.85 128.7,-57.73 135.64,-57.05 142.05,-55.74"/>
<polygon fill="black" stroke="black" points="143.34,-59.02 152.13,-53.11 141.57,-52.25 143.34,-59.02"/>
<text text-anchor="middle" x="127.73" y="-59.45" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge7" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M101.04,-38.4C118.48,-25.09 152.89,-3.57 182.34,-13.85 197.44,-19.13 211.53,-30.31 221.87,-40.21"/>
<polygon fill="black" stroke="black" points="219.47,-42.77 229,-47.41 224.45,-37.84 219.47,-42.77"/>
<text text-anchor="middle" x="166.78" y="-16.45" font-family="Times,serif" font-size="8.00">14</text>
</g>
<!-- 2&#45;&gt;0 -->
<g id="edge8" class="edge">
<title>2&#45;&gt;0</title>
<path fill="none" stroke="black" d="M156.39,-59.66C145.44,-72.38 126.03,-91.88 104.23,-99.85 91.24,-104.61 86,-104.88 73.11,-99.85 56.71,-93.46 42.19,-79.84 31.97,-68.14"/>
<polygon fill="black" stroke="black" points="34.39,-65.58 25.32,-60.1 28.99,-70.04 34.39,-65.58"/>
<text text-anchor="middle" x="88.67" y="-105.45" font-family="Times,serif" font-size="8.00">12</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge9" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M151.15,-47.85C140.84,-47.85 126.78,-47.85 114.71,-47.85"/>
<polygon fill="black" stroke="black" points="114.34,-44.35 104.34,-47.85 114.34,-51.35 114.34,-44.35"/>
<text text-anchor="middle" x="127.73" y="-50.45" font-family="Times,serif" font-size="8.00">12</text>
</g>
<!-- 2&#45;&gt;2 -->
<g id="edge10" class="edge">
<title>2&#45;&gt;2</title>
<path fill="none" stroke="black" d="M160.63,-62.34C158.89,-72 160.95,-81.41 166.78,-81.41 170.43,-81.41 172.6,-77.74 173.29,-72.62"/>
<polygon fill="black" stroke="black" points="176.78,-72.22 172.94,-62.34 169.78,-72.46 176.78,-72.22"/>
<text text-anchor="middle" x="166.78" y="-84.01" font-family="Times,serif" font-size="8.00">9</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge11" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M182.19,-44.2C189.48,-42.9 198.45,-42.12 206.34,-43.85 209.85,-44.62 213.43,-45.81 216.88,-47.19"/>
<polygon fill="black" stroke="black" points="215.49,-50.4 226.04,-51.37 218.4,-44.03 215.49,-50.4"/>
<text text-anchor="middle" x="203.34" y="-46.45" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 3&#45;&gt;0 -->
<g id="edge12" class="edge">
<title>3&#45;&gt;0</title>
<path fill="none" stroke="black" d="M230.34,-46.09C224.34,-37.84 215.74,-27.3 206.34,-19.85 196.99,-12.45 193.94,-10.59 182.34,-7.85 135.09,3.3 120.71,1.67 73.11,-7.85 61.78,-10.12 58.84,-11.62 49.11,-17.85 43.81,-21.25 38.56,-25.55 33.88,-29.83"/>
<polygon fill="black" stroke="black" points="31.38,-27.37 26.62,-36.83 36.25,-32.41 31.38,-27.37"/>
<text text-anchor="middle" x="127.73" y="-3.45" font-family="Times,serif" font-size="8.00">8</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge13" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M228.39,-69.68C217.84,-79.71 200.56,-93.94 182.34,-99.85 169.18,-104.12 164.21,-104.61 151.23,-99.85 133.51,-93.37 117.37,-79.29 106.07,-67.41"/>
<polygon fill="black" stroke="black" points="108.36,-64.73 99.06,-59.66 103.17,-69.42 108.36,-64.73"/>
<text text-anchor="middle" x="166.78" y="-105.45" font-family="Times,serif" font-size="8.00">12</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge14" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M224.18,-56.58C215.03,-55.16 203.03,-53.31 192.46,-51.67"/>
<polygon fill="black" stroke="black" points="192.93,-48.2 182.52,-50.13 191.86,-55.12 192.93,-48.2"/>
<text text-anchor="middle" x="203.34" y="-55.45" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 3&#45;&gt;3 -->
<g id="edge15" class="edge">
<title>3&#45;&gt;3</title>
<path fill="none" stroke="black" d="M233.74,-73.34C232.01,-83 234.06,-92.41 239.89,-92.41 243.54,-92.41 245.71,-88.74 246.4,-83.62"/>
<polygon fill="black" stroke="black" points="249.89,-83.22 246.05,-73.34 242.89,-83.46 249.89,-83.22"/>
<text text-anchor="middle" x="239.89" y="-95.01" font-family="Times,serif" font-size="8.00">12</text>
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
┌────────────────────┬───┬────────────────────┬────────────────────┐
│         a          │ @ │         b          │       a @ b        │
├────────────────────┼───┼────────────────────┼────────────────────┤
│      0  1  2  3    │ @ │      0  1  2  3    │      0  1  2  3    │
│    ────────────    │   │    ────────────    │    ────────────    │
│  0│     2  1  1    │   │  0│           4    │  0│  8  8  5  4    │
│  1│  2     3  2    │   │  1│  4     2  1    │  1│    20  2 14    │
│  2│  1  3     3    │   │  2│     4     2    │  2│ 12 12  9  7    │
│  3│  1  2  3       │   │  3│     4  1       │  3│  8 12  4 12    │
│                    │   │                    │                    │
└────────────────────┴───┴────────────────────┴────────────────────┘
(1 row)

```
## Matrix Vector Multiplication

Matrices can be multipled by vectors on the right taking the linear
combination of the matrices columns using the vectors elements as
coefficients:
``` postgres-console
select print(a) as a, '@' as "@", semiring, print(u) as u, print(mxv(a, u)) as "mxv(a, u)" from test_fixture;
┌────────────────────┬───┬──────────────────┬───────────┬───────────┐
│         a          │ @ │     semiring     │     u     │ mxv(a, u) │
├────────────────────┼───┼──────────────────┼───────────┼───────────┤
│      0  1  2  3    │ @ │ plus_times_int32 │           │           │
│    ────────────    │   │                  │    ───    │    ───    │
│  0│     2  1  1    │   │                  │  0│       │  0│  4    │
│  1│  2     3  2    │   │                  │  1│  2    │  1│       │
│  2│  1  3     3    │   │                  │  2│       │  2│  6    │
│  3│  1  2  3       │   │                  │  3│       │  3│  4    │
│                    │   │                  │           │           │
└────────────────────┴───┴──────────────────┴───────────┴───────────┘
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
<svg width="258pt" height="175pt"
 viewBox="0.00 0.00 258.45 175.24" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 171.24)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-171.24 254.45,-171.24 254.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-19.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-17.34" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-107.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-105.34" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 0&#45;&gt;1 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;1</title>
<path fill="none" stroke="black" d="M20.95,-33.84C25.89,-48.29 35.1,-70.35 49.11,-85.24 53.51,-89.91 59.13,-93.87 64.71,-97.09"/>
<polygon fill="black" stroke="black" points="63.34,-100.32 73.83,-101.8 66.55,-94.1 63.34,-100.32"/>
<text text-anchor="middle" x="52.11" y="-91.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2 -->
<g id="node3" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="234.89" cy="-58.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="234.89" y="-56.34" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M30.71,-15.51C36.39,-14.2 43.02,-12.88 49.11,-12.24 51.76,-11.96 52.45,-12.21 55.11,-12.24 76.94,-12.46 82.53,-10.78 104.23,-13.24 137.7,-17.03 145.25,-22.99 177.34,-33.24 188.05,-36.66 190.99,-36.84 201.34,-41.24 204.89,-42.75 208.59,-44.49 212.16,-46.28"/>
<polygon fill="black" stroke="black" points="210.65,-49.44 221.13,-50.97 213.89,-43.24 210.65,-49.44"/>
<text text-anchor="middle" x="125.23" y="-19.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="161.78" cy="-58.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="161.78" y="-56.34" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge3" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M30.65,-23.05C55.25,-29.7 106.3,-43.5 136.63,-51.71"/>
<polygon fill="black" stroke="black" points="135.85,-55.12 146.42,-54.35 137.68,-48.36 135.85,-55.12"/>
<text text-anchor="middle" x="88.67" y="-45.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M78.23,-95.56C66.57,-81.13 46.57,-56.37 32.51,-38.98"/>
<polygon fill="black" stroke="black" points="35.15,-36.68 26.15,-31.1 29.71,-41.08 35.15,-36.68"/>
<text text-anchor="middle" x="52.11" y="-67.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M97.1,-120.82C102.72,-129.58 111.33,-140.23 122.23,-145.24 154.17,-159.93 172.45,-165.27 201.34,-145.24 221,-131.6 228.8,-104.56 231.89,-84.22"/>
<polygon fill="black" stroke="black" points="235.4,-84.38 233.14,-74.02 228.45,-83.52 235.4,-84.38"/>
<text text-anchor="middle" x="161.78" y="-160.84" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge6" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M102.86,-100.45C110.48,-96.35 120.11,-90.85 128.23,-85.24 132.79,-82.08 137.49,-78.43 141.86,-74.86"/>
<polygon fill="black" stroke="black" points="144.33,-77.36 149.71,-68.24 139.81,-72.01 144.33,-77.36"/>
<text text-anchor="middle" x="125.23" y="-90.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;0 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;0</title>
<path fill="none" stroke="black" d="M221.89,-49.64C215.77,-45.28 208.18,-39.94 201.34,-35.24 190.77,-27.98 188.87,-24.83 177.34,-19.24 156.68,-9.22 150.9,-6.84 128.23,-3.24 93.43,2.29 83.66,-1.34 49.11,-8.24 46.15,-8.83 43.09,-9.62 40.08,-10.5"/>
<polygon fill="black" stroke="black" points="38.69,-7.27 30.28,-13.72 40.88,-13.92 38.69,-7.27"/>
<text text-anchor="middle" x="125.23" y="-5.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge8" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M220.61,-65C200.96,-74.68 162.74,-92.32 128.23,-101.24 123.87,-102.36 119.18,-103.31 114.63,-104.09"/>
<polygon fill="black" stroke="black" points="113.82,-100.67 104.45,-105.63 114.87,-107.59 113.82,-100.67"/>
<text text-anchor="middle" x="161.78" y="-97.84" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge9" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M219.18,-58.24C210.12,-58.24 198.28,-58.24 187.79,-58.24"/>
<polygon fill="black" stroke="black" points="187.52,-54.74 177.52,-58.24 187.52,-61.74 187.52,-54.74"/>
<text text-anchor="middle" x="198.34" y="-60.84" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 3&#45;&gt;0 -->
<g id="edge10" class="edge">
<title>3&#45;&gt;0</title>
<path fill="none" stroke="black" d="M149.57,-48.51C138.78,-39.8 121.57,-27.61 104.23,-22.24 83.75,-15.89 59.28,-15.62 41.4,-16.66"/>
<polygon fill="black" stroke="black" points="40.92,-13.18 31.22,-17.43 41.45,-20.16 40.92,-13.18"/>
<text text-anchor="middle" x="88.67" y="-24.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge11" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M147,-63.39C139.37,-66.55 129.91,-70.99 122.23,-76.24 116.94,-79.84 111.7,-84.32 107.02,-88.74"/>
<polygon fill="black" stroke="black" points="104.38,-86.43 99.76,-95.96 109.32,-91.39 104.38,-86.43"/>
<text text-anchor="middle" x="125.23" y="-78.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge12" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M176.06,-51.61C181.83,-49.08 188.76,-46.5 195.34,-45.24 200.75,-44.2 206.44,-45 211.74,-46.61"/>
<polygon fill="black" stroke="black" points="210.69,-49.96 221.28,-50.46 213.31,-43.47 210.69,-49.96"/>
<text text-anchor="middle" x="198.34" y="-47.84" font-family="Times,serif" font-size="8.00">3</text>
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
<svg width="70pt" height="203pt"
 viewBox="0.00 0.00 70.00 203.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 199)">
<title>vector</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-199 66,-199 66,4 -4,4"/>
<g id="clust1" class="cluster">
<title>cluster_vector</title>
<polygon fill="lightgray" stroke="black" stroke-dasharray="5,2" points="8,-8 8,-187 54,-187 54,-8 8,-8"/>
</g>
<!-- node0 -->
<g id="node1" class="node">
<title>node0</title>
<polygon fill="none" stroke="black" points="46,-179 16,-179 16,-162 46,-162 46,-179"/>
<text text-anchor="middle" x="31" y="-168.6" font-family="Times,serif" font-size="8.00">0:4</text>
</g>
<!-- node1 -->
<g id="node2" class="node">
<title>node1</title>
<polygon fill="none" stroke="black" points="33,-126 29,-126 29,-122 33,-122 33,-126"/>
</g>
<!-- node0&#45;&gt;node1 -->
<!-- node2 -->
<g id="node3" class="node">
<title>node2</title>
<polygon fill="none" stroke="black" points="46,-86 16,-86 16,-69 46,-69 46,-86"/>
<text text-anchor="middle" x="31" y="-75.6" font-family="Times,serif" font-size="8.00">2:6</text>
</g>
<!-- node1&#45;&gt;node2 -->
<!-- node3 -->
<g id="node4" class="node">
<title>node3</title>
<polygon fill="none" stroke="black" points="46,-33 16,-33 16,-16 46,-16 46,-33"/>
<text text-anchor="middle" x="31" y="-22.6" font-family="Times,serif" font-size="8.00">3:4</text>
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
┌────────────────────┬───┬───────────┬───────────┐
│         a          │ @ │     u     │   a @ u   │
├────────────────────┼───┼───────────┼───────────┤
│      0  1  2  3    │ @ │           │           │
│    ────────────    │   │    ───    │    ───    │
│  0│     2  1  1    │   │  0│       │  0│  4    │
│  1│  2     3  2    │   │  1│  2    │  1│       │
│  2│  1  3     3    │   │  2│       │  2│  6    │
│  3│  1  2  3       │   │  3│       │  3│  4    │
│                    │   │           │           │
└────────────────────┴───┴───────────┴───────────┘
(1 row)

```
## Vector Matrix Multiplication

Matrices can be multipled by vectors on the right taking the linear
combination of the matrices rows using the vectors elements as
coefficients:
``` postgres-console
select print(v) as v, semiring, print(b) as b, print(vxm(v, b, semiring)) as "vxm(v, b, semiring)" from test_fixture;
┌───────────┬──────────────────┬────────────────────┬─────────────────────┐
│     v     │     semiring     │         b          │ vxm(v, b, semiring) │
├───────────┼──────────────────┼────────────────────┼─────────────────────┤
│           │ plus_times_int32 │      0  1  2  3    │                     │
│    ───    │                  │    ────────────    │    ───              │
│  0│       │                  │  0│           4    │  0│ 12              │
│  1│  3    │                  │  1│  4     2  1    │  1│ 12              │
│  2│  3    │                  │  2│     4     2    │  2│  6              │
│  3│       │                  │  3│     4  1       │  3│  9              │
│           │                  │                    │                     │
└───────────┴──────────────────┴────────────────────┴─────────────────────┘
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
<svg width="258pt" height="130pt"
 viewBox="0.00 0.00 258.45 129.56" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 125.56)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-125.56 254.45,-125.56 254.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-13.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3 -->
<g id="node2" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-64.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-62.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M28.94,-24.09C39.45,-31.33 54.73,-41.86 67.07,-50.36"/>
<polygon fill="black" stroke="black" points="65.2,-53.33 75.42,-56.12 69.18,-47.56 65.2,-53.33"/>
<text text-anchor="middle" x="52.11" y="-44.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1 -->
<g id="node3" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="161.78" cy="-32.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="161.78" y="-30.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M103.04,-58.54C112.75,-54.17 126.14,-48.15 137.57,-43"/>
<polygon fill="black" stroke="black" points="139.3,-46.06 146.98,-38.77 136.43,-39.68 139.3,-46.06"/>
<text text-anchor="middle" x="125.23" y="-52.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 2 -->
<g id="node4" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="234.89" cy="-76.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="234.89" y="-74.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M104.29,-65.77C128.91,-67.82 178.9,-71.98 209.09,-74.49"/>
<polygon fill="black" stroke="black" points="208.98,-78 219.23,-75.34 209.56,-71.02 208.98,-78"/>
<text text-anchor="middle" x="161.78" y="-75.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge2" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M146.2,-30.74C140.58,-30.05 134.11,-29.26 128.23,-28.56 98.41,-24.99 64.11,-21.02 41.47,-18.41"/>
<polygon fill="black" stroke="black" points="41.71,-14.91 31.37,-17.25 40.91,-21.87 41.71,-14.91"/>
<text text-anchor="middle" x="88.67" y="-28.16" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M146.35,-30.52C138.88,-30.05 129.77,-30.42 122.23,-33.56 116.22,-36.05 110.68,-40.2 105.93,-44.67"/>
<polygon fill="black" stroke="black" points="103.09,-42.58 98.76,-52.25 108.17,-47.39 103.09,-42.58"/>
<text text-anchor="middle" x="125.23" y="-36.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M177.44,-30.02C184.97,-29.42 194.07,-29.83 201.34,-33.56 210.05,-38.03 217.08,-46.03 222.35,-53.93"/>
<polygon fill="black" stroke="black" points="219.4,-55.82 227.54,-62.6 225.41,-52.22 219.4,-55.82"/>
<text text-anchor="middle" x="198.34" y="-36.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge6" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M223.61,-87.8C217.71,-93.4 209.8,-99.59 201.34,-102.56 168.15,-114.18 154.17,-117.25 122.23,-102.56 114.74,-99.11 108.33,-93 103.23,-86.67"/>
<polygon fill="black" stroke="black" points="105.78,-84.22 97.1,-78.14 100.09,-88.31 105.78,-84.22"/>
<text text-anchor="middle" x="161.78" y="-115.16" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge5" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M221.29,-68.76C211.01,-62.4 196.25,-53.27 184.13,-45.77"/>
<polygon fill="black" stroke="black" points="185.84,-42.71 175.5,-40.42 182.16,-48.66 185.84,-42.71"/>
<text text-anchor="middle" x="198.34" y="-59.16" font-family="Times,serif" font-size="8.00">4</text>
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
<svg width="76pt" height="216pt"
 viewBox="0.00 0.00 76.00 216.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 212)">
<title>vector</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-212 72,-212 72,4 -4,4"/>
<g id="clust1" class="cluster">
<title>cluster_vector</title>
<polygon fill="lightgray" stroke="black" stroke-dasharray="5,2" points="8,-8 8,-200 60,-200 60,-8 8,-8"/>
</g>
<!-- node0 -->
<g id="node1" class="node">
<title>node0</title>
<polygon fill="none" stroke="black" points="51.5,-192 16.5,-192 16.5,-175 51.5,-175 51.5,-192"/>
<text text-anchor="middle" x="34" y="-181.6" font-family="Times,serif" font-size="8.00">0:12</text>
</g>
<!-- node1 -->
<g id="node2" class="node">
<title>node1</title>
<polygon fill="none" stroke="black" points="51.5,-139 16.5,-139 16.5,-122 51.5,-122 51.5,-139"/>
<text text-anchor="middle" x="34" y="-128.6" font-family="Times,serif" font-size="8.00">1:12</text>
</g>
<!-- node0&#45;&gt;node1 -->
<!-- node2 -->
<g id="node3" class="node">
<title>node2</title>
<polygon fill="none" stroke="black" points="49,-86 19,-86 19,-69 49,-69 49,-86"/>
<text text-anchor="middle" x="34" y="-75.6" font-family="Times,serif" font-size="8.00">2:6</text>
</g>
<!-- node1&#45;&gt;node2 -->
<!-- node3 -->
<g id="node4" class="node">
<title>node3</title>
<polygon fill="none" stroke="black" points="49,-33 19,-33 19,-16 49,-16 49,-33"/>
<text text-anchor="middle" x="34" y="-22.6" font-family="Times,serif" font-size="8.00">3:9</text>
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
┌───────────┬───┬────────────────────┬───────────┐
│     v     │ @ │         b          │   v @ b   │
├───────────┼───┼────────────────────┼───────────┤
│           │ @ │      0  1  2  3    │           │
│    ───    │   │    ────────────    │    ───    │
│  0│       │   │  0│           4    │  0│ 12    │
│  1│  3    │   │  1│  4     2  1    │  1│ 12    │
│  2│  3    │   │  2│     4     2    │  2│  6    │
│  3│       │   │  3│     4  1       │  3│  9    │
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
│  0│     2  1  1    │               │  0│     2          │
│  1│  2     3  2    │               │  1│  2     3  2    │
│  2│  1  3     3    │               │  2│     3     3    │
│  3│  1  2  3       │               │  3│     2  3       │
│                    │               │                    │
└────────────────────┴───────────────┴────────────────────┘
(1 row)

select draw(a) as uop_a_source, draw(choose(a, indexunaryop, 1)) as uop_b_source from test_fixture \gset
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 45%;">
        <!-- Diagram A -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="258pt" height="175pt"
 viewBox="0.00 0.00 258.45 175.24" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 171.24)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-171.24 254.45,-171.24 254.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-19.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-17.34" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-107.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-105.34" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 0&#45;&gt;1 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;1</title>
<path fill="none" stroke="black" d="M20.95,-33.84C25.89,-48.29 35.1,-70.35 49.11,-85.24 53.51,-89.91 59.13,-93.87 64.71,-97.09"/>
<polygon fill="black" stroke="black" points="63.34,-100.32 73.83,-101.8 66.55,-94.1 63.34,-100.32"/>
<text text-anchor="middle" x="52.11" y="-91.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2 -->
<g id="node3" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="234.89" cy="-58.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="234.89" y="-56.34" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 0&#45;&gt;2 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;2</title>
<path fill="none" stroke="black" d="M30.71,-15.51C36.39,-14.2 43.02,-12.88 49.11,-12.24 51.76,-11.96 52.45,-12.21 55.11,-12.24 76.94,-12.46 82.53,-10.78 104.23,-13.24 137.7,-17.03 145.25,-22.99 177.34,-33.24 188.05,-36.66 190.99,-36.84 201.34,-41.24 204.89,-42.75 208.59,-44.49 212.16,-46.28"/>
<polygon fill="black" stroke="black" points="210.65,-49.44 221.13,-50.97 213.89,-43.24 210.65,-49.44"/>
<text text-anchor="middle" x="125.23" y="-19.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="161.78" cy="-58.24" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="161.78" y="-56.34" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 0&#45;&gt;3 -->
<g id="edge3" class="edge">
<title>0&#45;&gt;3</title>
<path fill="none" stroke="black" d="M30.65,-23.05C55.25,-29.7 106.3,-43.5 136.63,-51.71"/>
<polygon fill="black" stroke="black" points="135.85,-55.12 146.42,-54.35 137.68,-48.36 135.85,-55.12"/>
<text text-anchor="middle" x="88.67" y="-45.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M78.23,-95.56C66.57,-81.13 46.57,-56.37 32.51,-38.98"/>
<polygon fill="black" stroke="black" points="35.15,-36.68 26.15,-31.1 29.71,-41.08 35.15,-36.68"/>
<text text-anchor="middle" x="52.11" y="-67.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M97.1,-120.82C102.72,-129.58 111.33,-140.23 122.23,-145.24 154.17,-159.93 172.45,-165.27 201.34,-145.24 221,-131.6 228.8,-104.56 231.89,-84.22"/>
<polygon fill="black" stroke="black" points="235.4,-84.38 233.14,-74.02 228.45,-83.52 235.4,-84.38"/>
<text text-anchor="middle" x="161.78" y="-160.84" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge6" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M102.86,-100.45C110.48,-96.35 120.11,-90.85 128.23,-85.24 132.79,-82.08 137.49,-78.43 141.86,-74.86"/>
<polygon fill="black" stroke="black" points="144.33,-77.36 149.71,-68.24 139.81,-72.01 144.33,-77.36"/>
<text text-anchor="middle" x="125.23" y="-90.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;0 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;0</title>
<path fill="none" stroke="black" d="M221.89,-49.64C215.77,-45.28 208.18,-39.94 201.34,-35.24 190.77,-27.98 188.87,-24.83 177.34,-19.24 156.68,-9.22 150.9,-6.84 128.23,-3.24 93.43,2.29 83.66,-1.34 49.11,-8.24 46.15,-8.83 43.09,-9.62 40.08,-10.5"/>
<polygon fill="black" stroke="black" points="38.69,-7.27 30.28,-13.72 40.88,-13.92 38.69,-7.27"/>
<text text-anchor="middle" x="125.23" y="-5.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge8" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M220.61,-65C200.96,-74.68 162.74,-92.32 128.23,-101.24 123.87,-102.36 119.18,-103.31 114.63,-104.09"/>
<polygon fill="black" stroke="black" points="113.82,-100.67 104.45,-105.63 114.87,-107.59 113.82,-100.67"/>
<text text-anchor="middle" x="161.78" y="-97.84" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge9" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M219.18,-58.24C210.12,-58.24 198.28,-58.24 187.79,-58.24"/>
<polygon fill="black" stroke="black" points="187.52,-54.74 177.52,-58.24 187.52,-61.74 187.52,-54.74"/>
<text text-anchor="middle" x="198.34" y="-60.84" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 3&#45;&gt;0 -->
<g id="edge10" class="edge">
<title>3&#45;&gt;0</title>
<path fill="none" stroke="black" d="M149.57,-48.51C138.78,-39.8 121.57,-27.61 104.23,-22.24 83.75,-15.89 59.28,-15.62 41.4,-16.66"/>
<polygon fill="black" stroke="black" points="40.92,-13.18 31.22,-17.43 41.45,-20.16 40.92,-13.18"/>
<text text-anchor="middle" x="88.67" y="-24.84" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge11" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M147,-63.39C139.37,-66.55 129.91,-70.99 122.23,-76.24 116.94,-79.84 111.7,-84.32 107.02,-88.74"/>
<polygon fill="black" stroke="black" points="104.38,-86.43 99.76,-95.96 109.32,-91.39 104.38,-86.43"/>
<text text-anchor="middle" x="125.23" y="-78.84" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge12" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M176.06,-51.61C181.83,-49.08 188.76,-46.5 195.34,-45.24 200.75,-44.2 206.44,-45 211.74,-46.61"/>
<polygon fill="black" stroke="black" points="210.69,-49.96 221.28,-50.46 213.31,-43.47 210.69,-49.96"/>
<text text-anchor="middle" x="198.34" y="-47.84" font-family="Times,serif" font-size="8.00">3</text>
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
<svg width="258pt" height="80pt"
 viewBox="0.00 0.00 258.45 80.31" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 76.31)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-76.31 254.45,-76.31 254.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-46.31" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-44.41" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="88.67" cy="-46.31" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="88.67" y="-44.41" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 0&#45;&gt;1 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;1</title>
<path fill="none" stroke="black" d="M31.29,-46.31C40.35,-46.31 52.19,-46.31 62.68,-46.31"/>
<polygon fill="black" stroke="black" points="62.95,-49.81 72.95,-46.31 62.95,-42.81 62.95,-49.81"/>
<text text-anchor="middle" x="52.11" y="-48.91" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;0 -->
<g id="edge2" class="edge">
<title>1&#45;&gt;0</title>
<path fill="none" stroke="black" d="M74.39,-39.68C68.62,-37.15 61.69,-34.58 55.11,-33.31 49.7,-32.27 44.01,-33.07 38.71,-34.69"/>
<polygon fill="black" stroke="black" points="37.14,-31.55 29.18,-38.53 39.76,-38.04 37.14,-31.55"/>
<text text-anchor="middle" x="52.11" y="-35.91" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2 -->
<g id="node3" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="234.89" cy="-46.31" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="234.89" y="-44.41" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M103.85,-51.5C115.15,-55.35 131.47,-60.26 146.23,-62.31 159.92,-64.21 163.64,-64.21 177.34,-62.31 188.29,-60.79 200.1,-57.69 210.09,-54.62"/>
<polygon fill="black" stroke="black" points="211.28,-57.91 219.72,-51.5 209.12,-51.26 211.28,-57.91"/>
<text text-anchor="middle" x="161.78" y="-65.91" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="161.78" cy="-37.31" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="161.78" y="-35.41" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 1&#45;&gt;3 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;3</title>
<path fill="none" stroke="black" d="M104.4,-44.45C113.56,-43.29 125.55,-41.77 136.13,-40.43"/>
<polygon fill="black" stroke="black" points="136.58,-43.9 146.07,-39.17 135.71,-36.96 136.58,-43.9"/>
<text text-anchor="middle" x="125.23" y="-44.91" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge5" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M223.5,-34.97C213.02,-24.46 195.81,-9.54 177.34,-3.31 164.23,1.1 159.33,1.1 146.23,-3.31 131.65,-8.22 117.86,-18.55 107.54,-27.85"/>
<polygon fill="black" stroke="black" points="104.9,-25.54 100.07,-34.97 109.72,-30.61 104.9,-25.54"/>
<text text-anchor="middle" x="161.78" y="-5.91" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 2&#45;&gt;3 -->
<g id="edge6" class="edge">
<title>2&#45;&gt;3</title>
<path fill="none" stroke="black" d="M221.04,-38.83C215.23,-35.86 208.15,-32.8 201.34,-31.31 196.67,-30.29 191.65,-30.28 186.82,-30.82"/>
<polygon fill="black" stroke="black" points="185.99,-27.41 176.78,-32.65 187.24,-34.3 185.99,-27.41"/>
<text text-anchor="middle" x="198.34" y="-33.91" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 3&#45;&gt;1 -->
<g id="edge7" class="edge">
<title>3&#45;&gt;1</title>
<path fill="none" stroke="black" d="M146.78,-32.65C139.43,-30.82 130.3,-29.54 122.23,-31.31 118.71,-32.08 115.13,-33.27 111.69,-34.64"/>
<polygon fill="black" stroke="black" points="110.16,-31.49 102.52,-38.83 113.07,-37.86 110.16,-31.49"/>
<text text-anchor="middle" x="125.23" y="-33.91" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 3&#45;&gt;2 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;2</title>
<path fill="none" stroke="black" d="M177.52,-39.18C186.67,-40.34 198.67,-41.85 209.24,-43.19"/>
<polygon fill="black" stroke="black" points="208.82,-46.67 219.18,-44.45 209.7,-39.72 208.82,-46.67"/>
<text text-anchor="middle" x="198.34" y="-44.91" font-family="Times,serif" font-size="8.00">3</text>
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
│  0│     2          │  0│     2  1  1    │  0│                │  0│        1  1    │
│  1│  2     3  2    │  1│  2     3  2    │  1│                │  1│                │
│  2│     3     3    │  2│  1  3     3    │  2│                │  2│  1             │
│  3│     2  3       │  3│  1  2  3       │  3│                │  3│  1             │
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
│  0│            -13-13          │  0│            -13-13          │
│  1│      -38         -13-95    │  1│      -38         -13-95    │
│  2│   -38      -11-62          │  2│            -11-62          │
│  3│               -98-20       │  3│               -98-20       │
│  4│-13   -11      -20-16-79    │  4│               -20-16-79    │
│  5│-13   -62-98-20             │  5│                            │
│  6│   -13   -20-16             │  6│                            │
│  7│   -95      -79             │  7│                            │
│                                │                                │
└────────────────────────────────┴────────────────────────────────┘
(1 row)

select draw(random_matrix(8, 8, 16, seed=>0.42, max=>42)) as uop_a_source,
       draw(choose(random_matrix(8, 8, 16, seed=>0.42, max=>42), 'triu', 0)) as uop_b_source
       from test_fixture \gset
```
<table style="width: 100%; table-layout: fixed;" class="dot-table">
  <tr>
    <td style="width: 45%;">
        <!-- Diagram A -->
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="642pt" height="413pt"
 viewBox="0.00 0.00 641.90 413.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 409)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-409 637.9,-409 637.9,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-86" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-84.1" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 4 -->
<g id="node2" class="node">
<title>4</title>
<ellipse fill="none" stroke="black" cx="101.67" cy="-185" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="101.67" y="-183.1" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 0&#45;&gt;4 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;4</title>
<path fill="none" stroke="black" d="M25.3,-98.62C31.65,-107.58 40.59,-119.78 49.11,-130 59.8,-142.82 72.63,-156.51 82.82,-167.03"/>
<polygon fill="black" stroke="black" points="80.52,-169.68 90.01,-174.38 85.52,-164.79 80.52,-169.68"/>
<text text-anchor="middle" x="58.61" y="-152.6" font-family="Times,serif" font-size="8.00">&#45;136</text>
</g>
<!-- 5 -->
<g id="node3" class="node">
<title>5</title>
<ellipse fill="none" stroke="black" cx="446.12" cy="-146" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="446.12" y="-144.1" font-family="Times,serif" font-size="8.00">5</text>
</g>
<!-- 0&#45;&gt;5 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;5</title>
<path fill="none" stroke="black" d="M27.4,-74.87C42.42,-60.69 71.08,-38 100.67,-38 100.67,-38 100.67,-38 361.01,-38 403.04,-38 427.14,-89.55 438.03,-121.42"/>
<polygon fill="black" stroke="black" points="434.71,-122.53 441.1,-130.98 441.37,-120.39 434.71,-122.53"/>
<text text-anchor="middle" x="230.84" y="-40.6" font-family="Times,serif" font-size="8.00">&#45;134</text>
</g>
<!-- 4&#45;&gt;0 -->
<g id="edge11" class="edge">
<title>4&#45;&gt;0</title>
<path fill="none" stroke="black" d="M96.21,-170.28C91.22,-155.7 81.97,-133.37 68.11,-118 60.18,-109.2 49.26,-101.95 39.51,-96.6"/>
<polygon fill="black" stroke="black" points="40.81,-93.33 30.31,-91.88 37.61,-99.56 40.81,-93.33"/>
<text text-anchor="middle" x="58.61" y="-120.6" font-family="Times,serif" font-size="8.00">&#45;136</text>
</g>
<!-- 4&#45;&gt;5 -->
<g id="edge13" class="edge">
<title>4&#45;&gt;5</title>
<path fill="none" stroke="black" d="M116.02,-178.73C121.81,-176.26 128.73,-173.63 135.23,-172 238.61,-145.98 366.87,-144.63 420.13,-145.37"/>
<polygon fill="black" stroke="black" points="420.26,-148.88 430.32,-145.56 420.39,-141.88 420.26,-148.88"/>
<text text-anchor="middle" x="273.89" y="-153.6" font-family="Times,serif" font-size="8.00">&#45;202</text>
</g>
<!-- 2 -->
<g id="node5" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="360.01" cy="-193" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="360.01" y="-191.1" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 4&#45;&gt;2 -->
<g id="edge12" class="edge">
<title>4&#45;&gt;2</title>
<path fill="none" stroke="black" d="M117.2,-187.67C122.81,-188.59 129.3,-189.51 135.23,-190 207.23,-195.98 292.7,-194.86 334.08,-193.79"/>
<polygon fill="black" stroke="black" points="334.41,-197.29 344.31,-193.51 334.22,-190.29 334.41,-197.29"/>
<text text-anchor="middle" x="230.84" y="-196.6" font-family="Times,serif" font-size="8.00">&#45;114</text>
</g>
<!-- 6 -->
<g id="node6" class="node">
<title>6</title>
<ellipse fill="none" stroke="black" cx="618.35" cy="-289" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="618.35" y="-287.1" font-family="Times,serif" font-size="8.00">6</text>
</g>
<!-- 4&#45;&gt;6 -->
<g id="edge14" class="edge">
<title>4&#45;&gt;6</title>
<path fill="none" stroke="black" d="M103.85,-200.68C107.03,-239.39 116.78,-338.28 135.23,-365 150.41,-387 160.04,-396 186.78,-396 186.78,-396 186.78,-396 533.23,-396 559.97,-396 567.22,-385.15 584.79,-365 597.63,-350.27 606.03,-329.7 611.07,-313.68"/>
<polygon fill="black" stroke="black" points="614.48,-314.5 613.91,-303.92 607.75,-312.54 614.48,-314.5"/>
<text text-anchor="middle" x="360.01" y="-398.6" font-family="Times,serif" font-size="8.00">&#45;163</text>
</g>
<!-- 7 -->
<g id="node7" class="node">
<title>7</title>
<ellipse fill="none" stroke="black" cx="187.78" cy="-263" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="187.78" y="-261.1" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 4&#45;&gt;7 -->
<g id="edge15" class="edge">
<title>4&#45;&gt;7</title>
<path fill="none" stroke="black" d="M113.67,-195.2C127.52,-208.04 151.53,-230.31 168.24,-245.81"/>
<polygon fill="black" stroke="black" points="166.07,-248.57 175.78,-252.8 170.83,-243.43 166.07,-248.57"/>
<text text-anchor="middle" x="144.73" y="-232.6" font-family="Times,serif" font-size="8.00">&#45;796</text>
</g>
<!-- 5&#45;&gt;0 -->
<g id="edge16" class="edge">
<title>5&#45;&gt;0</title>
<path fill="none" stroke="black" d="M443.54,-130.65C438.75,-93.17 420.83,0 361.01,0 100.67,0 100.67,0 100.67,0 65.48,0 40.18,-37.25 26.99,-62.72"/>
<polygon fill="black" stroke="black" points="23.8,-61.28 22.53,-71.8 30.08,-64.36 23.8,-61.28"/>
<text text-anchor="middle" x="230.84" y="-2.6" font-family="Times,serif" font-size="8.00">&#45;134</text>
</g>
<!-- 5&#45;&gt;4 -->
<g id="edge19" class="edge">
<title>5&#45;&gt;4</title>
<path fill="none" stroke="black" d="M436.37,-133.45C422.61,-115.34 394.02,-84 361.01,-84 186.78,-84 186.78,-84 186.78,-84 147.01,-84 122.4,-130.82 110.75,-160.7"/>
<polygon fill="black" stroke="black" points="107.46,-159.5 107.29,-170.1 114.03,-161.92 107.46,-159.5"/>
<text text-anchor="middle" x="273.89" y="-86.6" font-family="Times,serif" font-size="8.00">&#45;202</text>
</g>
<!-- 5&#45;&gt;2 -->
<g id="edge17" class="edge">
<title>5&#45;&gt;2</title>
<path fill="none" stroke="black" d="M430.42,-149.08C419.7,-151.73 405.1,-156.22 393.56,-163 388.13,-166.19 382.84,-170.42 378.15,-174.69"/>
<polygon fill="black" stroke="black" points="375.64,-172.25 370.92,-181.74 380.53,-177.26 375.64,-172.25"/>
<text text-anchor="middle" x="403.06" y="-165.6" font-family="Times,serif" font-size="8.00">&#45;621</text>
</g>
<!-- 3 -->
<g id="node8" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="532.23" cy="-204" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="532.23" y="-202.1" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 5&#45;&gt;3 -->
<g id="edge18" class="edge">
<title>5&#45;&gt;3</title>
<path fill="none" stroke="black" d="M458.15,-156.38C464.27,-161.9 472.15,-168.64 479.68,-174 488.94,-180.6 499.75,-187.06 509.07,-192.28"/>
<polygon fill="black" stroke="black" points="507.66,-195.5 518.11,-197.23 511.02,-189.36 507.66,-195.5"/>
<text text-anchor="middle" x="489.18" y="-187.6" font-family="Times,serif" font-size="8.00">&#45;980</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="273.89" cy="-275" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="273.89" y="-273.1" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M281.77,-261.1C287.57,-250.35 296.64,-235.58 307.45,-225 315.92,-216.71 326.9,-209.53 336.55,-204.08"/>
<polygon fill="black" stroke="black" points="338.45,-207.03 345.62,-199.23 335.15,-200.86 338.45,-207.03"/>
<text text-anchor="middle" x="316.95" y="-227.6" font-family="Times,serif" font-size="8.00">&#45;387</text>
</g>
<!-- 1&#45;&gt;6 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;6</title>
<path fill="none" stroke="black" d="M287.38,-282.81C293.24,-286.02 300.46,-289.37 307.45,-291 315.68,-292.91 318.01,-291.03 326.45,-291 424.82,-290.66 541.89,-289.68 592.26,-289.23"/>
<polygon fill="black" stroke="black" points="592.54,-292.73 602.51,-289.14 592.48,-285.73 592.54,-292.73"/>
<text text-anchor="middle" x="446.12" y="-292.6" font-family="Times,serif" font-size="8.00">&#45;131</text>
</g>
<!-- 1&#45;&gt;7 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;7</title>
<path fill="none" stroke="black" d="M258.41,-272.93C246.04,-271.17 227.98,-268.59 213.35,-266.5"/>
<polygon fill="black" stroke="black" points="213.83,-263.04 203.43,-265.09 212.84,-269.97 213.83,-263.04"/>
<text text-anchor="middle" x="230.84" y="-272.6" font-family="Times,serif" font-size="8.00">&#45;959</text>
</g>
<!-- 2&#45;&gt;4 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;4</title>
<path fill="none" stroke="black" d="M345.68,-186.63C339.9,-184.14 332.97,-181.53 326.45,-180 255.6,-163.37 168.66,-173.75 127.08,-180.52"/>
<polygon fill="black" stroke="black" points="126.33,-177.09 117.06,-182.22 127.51,-183.99 126.33,-177.09"/>
<text text-anchor="middle" x="230.84" y="-173.6" font-family="Times,serif" font-size="8.00">&#45;114</text>
</g>
<!-- 2&#45;&gt;5 -->
<g id="edge8" class="edge">
<title>2&#45;&gt;5</title>
<path fill="none" stroke="black" d="M375.08,-188.5C385.75,-184.8 400.56,-179.04 412.56,-172 417.2,-169.28 421.89,-165.95 426.21,-162.59"/>
<polygon fill="black" stroke="black" points="428.44,-165.29 433.96,-156.24 424,-159.87 428.44,-165.29"/>
<text text-anchor="middle" x="403.06" y="-182.6" font-family="Times,serif" font-size="8.00">&#45;621</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge6" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M350.28,-205.45C344.08,-213.91 335.32,-225.13 326.45,-234 316.82,-243.64 304.99,-253.18 295.09,-260.63"/>
<polygon fill="black" stroke="black" points="292.98,-257.85 287,-266.59 297.13,-263.48 292.98,-257.85"/>
<text text-anchor="middle" x="316.95" y="-252.6" font-family="Times,serif" font-size="8.00">&#45;387</text>
</g>
<!-- 6&#45;&gt;4 -->
<g id="edge22" class="edge">
<title>6&#45;&gt;4</title>
<path fill="none" stroke="black" d="M608.6,-301.55C594.83,-319.66 566.25,-351 533.23,-351 186.78,-351 186.78,-351 186.78,-351 124.72,-351 108.44,-257.53 104.17,-210.86"/>
<polygon fill="black" stroke="black" points="107.64,-210.31 103.36,-200.62 100.66,-210.86 107.64,-210.31"/>
<text text-anchor="middle" x="360.01" y="-353.6" font-family="Times,serif" font-size="8.00">&#45;163</text>
</g>
<!-- 6&#45;&gt;1 -->
<g id="edge20" class="edge">
<title>6&#45;&gt;1</title>
<path fill="none" stroke="black" d="M603.64,-283.1C587.23,-276.59 558.74,-267 533.23,-267 402.06,-267 402.06,-267 402.06,-267 366.45,-267 325.33,-270.16 299.72,-272.51"/>
<polygon fill="black" stroke="black" points="299.38,-269.03 289.75,-273.46 300.04,-276 299.38,-269.03"/>
<text text-anchor="middle" x="446.12" y="-269.6" font-family="Times,serif" font-size="8.00">&#45;131</text>
</g>
<!-- 6&#45;&gt;3 -->
<g id="edge21" class="edge">
<title>6&#45;&gt;3</title>
<path fill="none" stroke="black" d="M615.04,-273.8C611.59,-256.56 603.39,-228.93 584.79,-214 577.33,-208.01 567.32,-205.25 558.11,-204.08"/>
<polygon fill="black" stroke="black" points="558.09,-200.57 547.86,-203.34 557.58,-207.55 558.09,-200.57"/>
<text text-anchor="middle" x="575.29" y="-216.6" font-family="Times,serif" font-size="8.00">&#45;204</text>
</g>
<!-- 7&#45;&gt;4 -->
<g id="edge24" class="edge">
<title>7&#45;&gt;4</title>
<path fill="none" stroke="black" d="M172.27,-260.18C161.09,-257.4 145.81,-252.19 135.23,-243 124.66,-233.84 116.81,-220.36 111.46,-208.71"/>
<polygon fill="black" stroke="black" points="114.68,-207.33 107.56,-199.48 108.23,-210.06 114.68,-207.33"/>
<text text-anchor="middle" x="144.73" y="-255.6" font-family="Times,serif" font-size="8.00">&#45;796</text>
</g>
<!-- 7&#45;&gt;1 -->
<g id="edge23" class="edge">
<title>7&#45;&gt;1</title>
<path fill="none" stroke="black" d="M202.54,-257.22C213.04,-253.61 227.79,-250.3 240.34,-254 244.62,-255.26 248.87,-257.28 252.82,-259.58"/>
<polygon fill="black" stroke="black" points="251.08,-262.63 261.35,-265.23 254.94,-256.79 251.08,-262.63"/>
<text text-anchor="middle" x="230.84" y="-256.6" font-family="Times,serif" font-size="8.00">&#45;959</text>
</g>
<!-- 3&#45;&gt;5 -->
<g id="edge9" class="edge">
<title>3&#45;&gt;5</title>
<path fill="none" stroke="black" d="M523.2,-191.25C517.31,-182.8 508.62,-172.01 498.68,-165 490.52,-159.25 480.42,-155.02 471.33,-152.04"/>
<polygon fill="black" stroke="black" points="472.23,-148.66 461.64,-149.18 470.24,-155.37 472.23,-148.66"/>
<text text-anchor="middle" x="489.18" y="-167.6" font-family="Times,serif" font-size="8.00">&#45;980</text>
</g>
<!-- 3&#45;&gt;6 -->
<g id="edge10" class="edge">
<title>3&#45;&gt;6</title>
<path fill="none" stroke="black" d="M545.92,-212.31C556.89,-219.84 572.82,-231.62 584.79,-244 591.7,-251.15 598.33,-259.85 603.78,-267.68"/>
<polygon fill="black" stroke="black" points="600.91,-269.68 609.39,-276.03 606.72,-265.78 600.91,-269.68"/>
<text text-anchor="middle" x="575.29" y="-246.6" font-family="Times,serif" font-size="8.00">&#45;204</text>
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
<svg width="297pt" height="216pt"
 viewBox="0.00 0.00 297.45 216.11" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 212.11)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-212.11 293.45,-212.11 293.45,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="101.67" cy="-111.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="101.67" y="-109.66" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 4 -->
<g id="node2" class="node">
<title>4</title>
<ellipse fill="none" stroke="black" cx="187.78" cy="-71.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="187.78" y="-69.66" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 0&#45;&gt;4 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;4</title>
<path fill="none" stroke="black" d="M116.18,-105.13C129.06,-99.01 148.69,-89.67 163.99,-82.4"/>
<polygon fill="black" stroke="black" points="165.77,-85.42 173.3,-77.97 162.77,-79.1 165.77,-85.42"/>
<text text-anchor="middle" x="144.73" y="-97.16" font-family="Times,serif" font-size="8.00">&#45;136</text>
</g>
<!-- 5 -->
<g id="node3" class="node">
<title>5</title>
<ellipse fill="none" stroke="black" cx="273.89" cy="-95.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="273.89" y="-93.66" font-family="Times,serif" font-size="8.00">5</text>
</g>
<!-- 0&#45;&gt;5 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;5</title>
<path fill="none" stroke="black" d="M117.03,-114.22C142.4,-118.37 196.59,-124.84 240.34,-113.56 244.31,-112.53 248.31,-110.91 252.08,-109.05"/>
<polygon fill="black" stroke="black" points="253.83,-112.08 260.84,-104.14 250.41,-105.97 253.83,-112.08"/>
<text text-anchor="middle" x="187.78" y="-122.16" font-family="Times,serif" font-size="8.00">&#45;134</text>
</g>
<!-- 4&#45;&gt;5 -->
<g id="edge10" class="edge">
<title>4&#45;&gt;5</title>
<path fill="none" stroke="black" d="M203.61,-69.58C214.13,-68.71 228.43,-68.65 240.34,-72.56 245.1,-74.12 249.78,-76.64 254.03,-79.45"/>
<polygon fill="black" stroke="black" points="251.95,-82.27 262.07,-85.41 256.12,-76.64 251.95,-82.27"/>
<text text-anchor="middle" x="230.84" y="-75.16" font-family="Times,serif" font-size="8.00">&#45;202</text>
</g>
<!-- 6 -->
<g id="node6" class="node">
<title>6</title>
<ellipse fill="none" stroke="black" cx="273.89" cy="-192.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="273.89" y="-190.66" font-family="Times,serif" font-size="8.00">6</text>
</g>
<!-- 4&#45;&gt;6 -->
<g id="edge11" class="edge">
<title>4&#45;&gt;6</title>
<path fill="none" stroke="black" d="M200.1,-81.46C211.32,-91.69 228.5,-108.53 240.34,-125.56 249.69,-139.01 257.76,-155.56 263.52,-168.84"/>
<polygon fill="black" stroke="black" points="260.38,-170.39 267.47,-178.26 266.83,-167.69 260.38,-170.39"/>
<text text-anchor="middle" x="230.84" y="-128.16" font-family="Times,serif" font-size="8.00">&#45;163</text>
</g>
<!-- 7 -->
<g id="node7" class="node">
<title>7</title>
<ellipse fill="none" stroke="black" cx="273.89" cy="-15.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="273.89" y="-13.66" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 4&#45;&gt;7 -->
<g id="edge12" class="edge">
<title>4&#45;&gt;7</title>
<path fill="none" stroke="black" d="M201.19,-63.29C214.49,-54.44 235.82,-40.24 251.77,-29.62"/>
<polygon fill="black" stroke="black" points="254.05,-32.31 260.43,-23.85 250.17,-26.48 254.05,-32.31"/>
<text text-anchor="middle" x="230.84" y="-52.16" font-family="Times,serif" font-size="8.00">&#45;796</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="15.56" cy="-38.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="15.56" y="-36.66" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2 -->
<g id="node5" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="101.67" cy="-38.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="101.67" y="-36.66" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&gt;2 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;2</title>
<path fill="none" stroke="black" d="M31.21,-38.56C43.44,-38.56 61.14,-38.56 75.61,-38.56"/>
<polygon fill="black" stroke="black" points="75.9,-42.06 85.9,-38.56 75.9,-35.06 75.9,-42.06"/>
<text text-anchor="middle" x="58.61" y="-41.16" font-family="Times,serif" font-size="8.00">&#45;387</text>
</g>
<!-- 1&#45;&gt;6 -->
<g id="edge4" class="edge">
<title>1&#45;&gt;6</title>
<path fill="none" stroke="black" d="M23.66,-52.05C42.39,-85.82 96.91,-172.91 172.23,-196.56 197.38,-204.45 227.81,-201.67 248.58,-198.04"/>
<polygon fill="black" stroke="black" points="249.34,-201.46 258.49,-196.11 248.01,-194.58 249.34,-201.46"/>
<text text-anchor="middle" x="144.73" y="-191.16" font-family="Times,serif" font-size="8.00">&#45;131</text>
</g>
<!-- 1&#45;&gt;7 -->
<g id="edge5" class="edge">
<title>1&#45;&gt;7</title>
<path fill="none" stroke="black" d="M30.06,-32.31C43.83,-26.27 65.96,-17.49 86.11,-13.56 143.22,-2.4 212.04,-7.87 248.09,-12.13"/>
<polygon fill="black" stroke="black" points="247.82,-15.63 258.18,-13.4 248.69,-8.68 247.82,-15.63"/>
<text text-anchor="middle" x="144.73" y="-11.16" font-family="Times,serif" font-size="8.00">&#45;959</text>
</g>
<!-- 2&#45;&gt;4 -->
<g id="edge6" class="edge">
<title>2&#45;&gt;4</title>
<path fill="none" stroke="black" d="M116.56,-44.01C129.3,-49 148.42,-56.5 163.49,-62.42"/>
<polygon fill="black" stroke="black" points="162.56,-65.81 173.15,-66.21 165.12,-59.3 162.56,-65.81"/>
<text text-anchor="middle" x="144.73" y="-61.16" font-family="Times,serif" font-size="8.00">&#45;114</text>
</g>
<!-- 2&#45;&gt;5 -->
<g id="edge7" class="edge">
<title>2&#45;&gt;5</title>
<path fill="none" stroke="black" d="M116.8,-33.5C143.21,-25 200.97,-10.94 240.34,-34.56 253.46,-42.43 261.59,-57.59 266.44,-70.83"/>
<polygon fill="black" stroke="black" points="263.14,-71.98 269.54,-80.42 269.8,-69.83 263.14,-71.98"/>
<text text-anchor="middle" x="187.78" y="-25.16" font-family="Times,serif" font-size="8.00">&#45;621</text>
</g>
<!-- 3 -->
<g id="node8" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="187.78" cy="-171.56" rx="15.61" ry="15.61"/>
<text text-anchor="middle" x="187.78" y="-169.66" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 3&#45;&gt;5 -->
<g id="edge8" class="edge">
<title>3&#45;&gt;5</title>
<path fill="none" stroke="black" d="M201.96,-164.83C212.94,-158.85 228.63,-149.38 240.34,-138.56 247.26,-132.16 253.77,-124.15 259.12,-116.78"/>
<polygon fill="black" stroke="black" points="262,-118.77 264.83,-108.56 256.25,-114.78 262,-118.77"/>
<text text-anchor="middle" x="230.84" y="-155.16" font-family="Times,serif" font-size="8.00">&#45;980</text>
</g>
<!-- 3&#45;&gt;6 -->
<g id="edge9" class="edge">
<title>3&#45;&gt;6</title>
<path fill="none" stroke="black" d="M203.05,-175.12C215.5,-178.23 233.82,-182.8 248.58,-186.49"/>
<polygon fill="black" stroke="black" points="248.01,-189.95 258.56,-188.98 249.7,-183.16 248.01,-189.95"/>
<text text-anchor="middle" x="230.84" y="-186.16" font-family="Times,serif" font-size="8.00">&#45;204</text>
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
│  0│     2  1  1    │
│  1│  2     3  2    │
│  2│  1  3     3    │
│  3│  1  2  3       │
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
│  0│     2  1  1    │            │  0│    -2 -1 -1    │
│  1│  2     3  2    │            │  1│ -2    -3 -2    │
│  2│  1  3     3    │            │  2│ -1 -3    -3    │
│  3│  1  2  3       │            │  3│ -1 -2 -3       │
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
│   A->matrix, 12 entries, memory: 320 bytes    │
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
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                         dup                                         │
├─────────────────────────────────────────────────────────────────────────────────────┤
│ int32(4:4)[0:1:2 0:2:1 0:3:1 1:0:2 1:2:3 1:3:2 2:0:1 2:1:3 2:3:3 3:0:1 3:1:2 3:2:3] │
└─────────────────────────────────────────────────────────────────────────────────────┘
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