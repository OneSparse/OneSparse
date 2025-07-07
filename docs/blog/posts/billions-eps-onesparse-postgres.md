---
draft: true
authors:
  - michelp
date: 2025-07-01
categories:
  - onesparse
  - postgres
  - graphblas
  - suitesparse
  - graphs
  - linear algebra
---
# Billions of Edges Per Second on Postgres Graphs
For years now those of us over at [The GraphBLAS Forum](graphblas.org)
have been working hard to bring a standardized, state-of-the-art
parallel graph traversal API to all possible architectures.  This API
work has led to the evolution of the high performance graph linear
algebra library [SuiteSparse
GraphBLAS](https://people.engr.tamu.edu/davis/GraphBLAS.html) by
[Dr. Timothy Davis et al]() at at [Texas A&M University]().
SuiteSparse contains powerful optimized kernels for sparse graph
computation, the ability for users to define their own edge types and
operators, and has a built-in JIT compiler that can target many
computing architectures including [Nvidia CUDA]().
<!-- more -->
OneSparse is more than just fast graphs, its graph algorithms are
expressed using [Linear
Algebra](https://en.wikipedia.org/wiki/Linear_algebra) operations
to traverse and process graphs that are represented as matrices.
In this powerful approach inspired by the [mathematical roots of
graph theory](https://en.wikipedia.org/wiki/Adjacency_matrix), sets
of nodes are sparse Vectors and graphs of nodes are sparse
Matrices.
This differs quite a bit from most graph processing libraries that
focus on individual nodes and edges, which are inherently serial
concepts.  Writing efficient bulk parallel decisions at such a low
level is complex, and hard to retarget to new architectures due to
close-to-the-metal complexities that all new and coming families of
parallel processors possess.

By basing itself on math, the GraphBLAS leverages the rules and
objcets of linear algebra, and in particular Sparse [Matrix
Multiplication](https://en.wikipedia.org/wiki/Matrix_multiplication),
to efficiently schedule graph operations in bulk across many cores,
or whatever other processing unit the particular architecture
employs.  The key idea is in mind is that graphs and matricies are
conceptuals reflections of each other, an idea understood now for
hundreds of years.  Creating a Graph in OneSparse is the same as
creating a matrix.

![The Graph Matrix Duality](/images/GraphMatrix.png)

By specifying operations in a high level algebraic form, the
library implementor can use [advanced sparse compression
techniques](https://en.wikipedia.org/wiki/Sparse_matrix),
statistical and structural knowledge of the data, and other
optimizations to target the best approach for a given architecture
on a per-operation basis.
## OneSparse
OneSparse is a Postgres extension that brings the power of SuiteSparse
to Postgres. Have you found yourself spending too much time
extracting, transforming, and loading graphs into side databases that
need constant money, maintanance and impedence matching with SQL.
With OneSparse, it's easy to turn SQL data into graphs and back again
in pure SQL.
OneSparse can serialize and deserialize graphs into Postgres
objects to and from an on-disk state.  This state can come from
various sources, but the simplest is the ["TOAST" storage]() of
large variable length data arrays in rows.  However, the one major
limitation of this approach is that Postgres is limited by design
to a maximum TOAST size of about 1 gigabyte which typically limits
TOASTed graphs to a few hundred million edges at most.
TOAST storage is very useful for small graphs, or sub-graphs of a
large graph, but graphs with many billions of edges blow right past
this size limit, so OneSparse also provides functions for loading
much larger graphs from either SQL queries, Large Object storage,
or most optimally, from compressed files on the server filesystem
itself.
For the purposes of a quick demo, we will use a very small graph
that is very commonly studied in graph theory called the
[Newman/karate]() graph, which can be downloaded from the
[SuiteSparse Matrix Collection]().
``` postgres-console
create extension if not exists onesparse;
```
Now we can create graphs, a simple approach for small graphs that
fit into the TOAST limit is to make a materialized view that loads
the graph data from disk using the standard [Matrix Market]()
format:
``` postgres-console
create materialized view if not exists karate as
    select mmread('/home/postgres/onesparse/demo/karate.mtx') as graph;
select graph from karate;
┌────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     graph                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      │
├────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│ bool(34:34)[0:1:t 0:2:t 0:3:t 0:4:t 0:5:t 0:6:t 0:7:t 0:8:t 0:10:t 0:11:t 0:12:t 0:13:t 0:17:t 0:19:t 0:21:t 0:31:t 1:0:t 1:2:t 1:3:t 1:7:t 1:13:t 1:17:t 1:19:t 1:21:t 1:30:t 2:0:t 2:1:t 2:3:t 2:7:t 2:8:t 2:9:t 2:13:t 2:27:t 2:28:t 2:32:t 3:0:t 3:1:t 3:2:t 3:7:t 3:12:t 3:13:t 4:0:t 4:6:t 4:10:t 5:0:t 5:6:t 5:10:t 5:16:t 6:0:t 6:4:t 6:5:t 6:16:t 7:0:t 7:1:t 7:2:t 7:3:t 8:0:t 8:2:t 8:30:t 8:32:t 8:33:t 9:2:t 9:33:t 10:0:t 10:4:t 10:5:t 11:0:t 12:0:t 12:3:t 13:0:t 13:1:t 13:2:t 13:3:t 13:33:t 14:32:t 14:33:t 15:32:t 15:33:t 16:5:t 16:6:t 17:0:t 17:1:t 18:32:t 18:33:t 19:0:t 19:1:t 19:33:t 20:32:t 20:33:t 21:0:t 21:1:t 22:32:t 22:33:t 23:25:t 23:27:t 23:29:t 23:32:t 23:33:t 24:25:t 24:27:t 24:31:t 25:23:t 25:24:t 25:31:t 26:29:t 26:33:t 27:2:t 27:23:t 27:24:t 27:33:t 28:2:t 28:31:t 28:33:t 29:23:t 29:26:t 29:32:t 29:33:t 30:1:t 30:8:t 30:32:t 30:33:t 31:0:t 31:24:t 31:25:t 31:28:t 31:32:t 31:33:t 32:2:t 32:8:t 32:14:t 32:15:t 32:18:t 32:20:t 32:22:t 32:23:t 32:29:t 32:30:t 32:31:t 32:33:t 33:8:t 33:9:t 33:13:t 33:14:t 33:15:t 33:18:t 33:19:t 33:20:t 33:22:t 33:23:t 33:26:t 33:27:t 33:28:t 33:29:t 33:30:t 33:31:t 33:32:t] │
└────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
(1 row)

```

Like all SQL types in Postgres, an object must have a literal
input/output format that can recreate the object.  The default
format shown here shows a list of *edge coordinate*.  To see the
matrix like representation, OneSparse provides a `print()` function:
``` postgres-console
select print(graph) from karate;
┌──────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                                    print                                                     │
├──────────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│      0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33    │
│    ──────────────────────────────────────────────────────────────────────────────────────────────────────    │
│  0│     t  t  t  t  t  t  t  t     t  t  t  t           t     t     t                             t          │
│  1│  t     t  t           t                 t           t     t     t                          t             │
│  2│  t  t     t           t  t  t           t                                         t  t           t       │
│  3│  t  t  t              t              t  t                                                                │
│  4│  t                 t           t                                                                         │
│  5│  t                 t           t                 t                                                       │
│  6│  t           t  t                                t                                                       │
│  7│  t  t  t  t                                                                                              │
│  8│  t     t                                                                                   t     t  t    │
│  9│        t                                                                                            t    │
│ 10│  t           t  t                                                                                        │
│ 11│  t                                                                                                       │
│ 12│  t        t                                                                                              │
│ 13│  t  t  t  t                                                                                         t    │
│ 14│                                                                                                  t  t    │
│ 15│                                                                                                  t  t    │
│ 16│                 t  t                                                                                     │
│ 17│  t  t                                                                                                    │
│ 18│                                                                                                  t  t    │
│ 19│  t  t                                                                                               t    │
│ 20│                                                                                                  t  t    │
│ 21│  t  t                                                                                                    │
│ 22│                                                                                                  t  t    │
│ 23│                                                                             t     t     t        t  t    │
│ 24│                                                                             t     t           t          │
│ 25│                                                                       t  t                    t          │
│ 26│                                                                                         t           t    │
│ 27│        t                                                              t  t                          t    │
│ 28│        t                                                                                      t     t    │
│ 29│                                                                       t        t                 t  t    │
│ 30│     t                    t                                                                       t  t    │
│ 31│  t                                                                       t  t        t           t  t    │
│ 32│        t                 t                 t  t        t     t     t  t                 t  t  t     t    │
│ 33│                          t  t           t  t  t        t  t  t     t  t        t  t  t  t  t  t  t       │
│                                                                                                              │
└──────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
(1 row)

```
This graph can now be accessed from SQL using the powerful
GraphBLAS API exposed by OneSparse.  For example, we can query the
number of rows, columns and values in the graph:
``` postgres-console
select nrows(graph), ncols(graph), nvals(graph) from karate;
┌───────┬───────┬───────┐
│ nrows │ ncols │ nvals │
├───────┼───────┼───────┤
│    34 │    34 │   156 │
└───────┴───────┴───────┘
(1 row)

```
At first a matrix might seem like an odd way to store a graph, but
now we bring the power of Linear Algebra into play.  For example,
let's suppose you want to know the "degree" of every node in the
graph, that is, how many edges connect to the node.

This can be done by reducing the Matrix to a Vector, which sums all
the column vectors of the matrix into a vector which contains the
degree for each node:
``` postgres-console
select reduce_cols(cast_to(graph, 'int32')) from karate;
┌────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                                                                  reduce_cols                                                                                   │
├────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│ int32(34)[0:16 1:9 2:10 3:6 4:3 5:4 6:4 7:4 8:5 9:2 10:3 11:1 12:2 13:5 14:2 15:2 16:2 17:2 18:2 19:3 20:2 21:2 22:2 23:5 24:3 25:3 26:2 27:4 28:3 29:4 30:4 31:6 32:12 33:17] │
└────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
(1 row)

```
A vector maps a node's row index to a value.  Now look at the graph
where nodes are labled and color mapped according to the degree of
each node:
``` postgres-console
select draw(triu(graph),
            reduce_cols(cast_to(graph, 'int32')),
            false, false, true, 0.5, 'Degree of Karate Graph')
    as draw_source from karate \gset
```
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="779pt" height="367pt"
 viewBox="0.00 0.00 778.50 367.35" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 363.35)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-363.35 774.5,-363.35 774.5,4 -4,4"/>
<text text-anchor="middle" x="385.25" y="-7.4" font-family="Times,serif" font-size="12.00">Degree of Karate Graph</text>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="#bf0000" fill-opacity="0.498039" stroke="black" cx="450.52" cy="-196.67" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="450.52" y="-194.77" font-family="Times,serif" font-size="8.00">0 : 16</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="#7fff7f" fill-opacity="0.498039" stroke="black" cx="393.74" cy="-190.45" rx="25" ry="12.04"/>
<text text-anchor="middle" x="393.74" y="-188.55" font-family="Times,serif" font-size="8.00">1 : 9</text>
</g>
<!-- 0&#45;&#45;1 -->
<g id="edge1" class="edge">
<title>0&#45;&#45;1</title>
<path fill="none" stroke="black" d="M423.12,-193.67C421.42,-193.48 419.72,-193.29 418.03,-193.11"/>
</g>
<!-- 2 -->
<g id="node3" class="node">
<title>2</title>
<ellipse fill="#bfff3f" fill-opacity="0.498039" stroke="black" cx="297.87" cy="-157.02" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="297.87" y="-155.12" font-family="Times,serif" font-size="8.00">2 : 10</text>
</g>
<!-- 0&#45;&#45;2 -->
<g id="edge2" class="edge">
<title>0&#45;&#45;2</title>
<path fill="none" stroke="black" d="M426.33,-190.39C397.83,-182.98 350.76,-170.75 322.2,-163.34"/>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="#00bfff" fill-opacity="0.498039" stroke="black" cx="415.54" cy="-127.23" rx="25" ry="12.04"/>
<text text-anchor="middle" x="415.54" y="-125.33" font-family="Times,serif" font-size="8.00">3 : 6</text>
</g>
<!-- 0&#45;&#45;3 -->
<g id="edge3" class="edge">
<title>0&#45;&#45;3</title>
<path fill="none" stroke="black" d="M444.58,-184.89C438.19,-172.2 428.07,-152.1 421.62,-139.3"/>
</g>
<!-- 4 -->
<g id="node5" class="node">
<title>4</title>
<ellipse fill="#0000ff" fill-opacity="0.498039" stroke="black" cx="580.19" cy="-250.56" rx="25" ry="12.04"/>
<text text-anchor="middle" x="580.19" y="-248.66" font-family="Times,serif" font-size="8.00">4 : 3</text>
</g>
<!-- 0&#45;&#45;4 -->
<g id="edge4" class="edge">
<title>0&#45;&#45;4</title>
<path fill="none" stroke="black" d="M470.78,-205.09C495.57,-215.39 537.2,-232.69 561.33,-242.72"/>
</g>
<!-- 5 -->
<g id="node6" class="node">
<title>5</title>
<ellipse fill="#003fff" fill-opacity="0.498039" stroke="black" cx="632.84" cy="-242.48" rx="25" ry="12.04"/>
<text text-anchor="middle" x="632.84" y="-240.58" font-family="Times,serif" font-size="8.00">5 : 4</text>
</g>
<!-- 0&#45;&#45;5 -->
<g id="edge5" class="edge">
<title>0&#45;&#45;5</title>
<path fill="none" stroke="black" d="M475.11,-202.85C510.78,-211.81 576.35,-228.29 610.64,-236.9"/>
</g>
<!-- 6 -->
<g id="node7" class="node">
<title>6</title>
<ellipse fill="#003fff" fill-opacity="0.498039" stroke="black" cx="637.55" cy="-205.51" rx="25" ry="12.04"/>
<text text-anchor="middle" x="637.55" y="-203.61" font-family="Times,serif" font-size="8.00">6 : 4</text>
</g>
<!-- 0&#45;&#45;6 -->
<g id="edge6" class="edge">
<title>0&#45;&#45;6</title>
<path fill="none" stroke="black" d="M478.92,-198.01C515.4,-199.74 578.13,-202.7 612.77,-204.34"/>
</g>
<!-- 7 -->
<g id="node8" class="node">
<title>7</title>
<ellipse fill="#003fff" fill-opacity="0.498039" stroke="black" cx="392.05" cy="-102.14" rx="25" ry="12.04"/>
<text text-anchor="middle" x="392.05" y="-100.24" font-family="Times,serif" font-size="8.00">7 : 4</text>
</g>
<!-- 0&#45;&#45;7 -->
<g id="edge7" class="edge">
<title>0&#45;&#45;7</title>
<path fill="none" stroke="black" d="M443.11,-184.7C431.8,-166.41 410.37,-131.76 399.21,-113.72"/>
</g>
<!-- 8 -->
<g id="node9" class="node">
<title>8</title>
<ellipse fill="#007fff" fill-opacity="0.498039" stroke="black" cx="283.37" cy="-233.07" rx="25" ry="12.04"/>
<text text-anchor="middle" x="283.37" y="-231.17" font-family="Times,serif" font-size="8.00">8 : 5</text>
</g>
<!-- 0&#45;&#45;8 -->
<g id="edge8" class="edge">
<title>0&#45;&#45;8</title>
<path fill="none" stroke="black" d="M425.13,-202.2C392.71,-209.26 337.1,-221.37 306.04,-228.14"/>
</g>
<!-- 10 -->
<g id="node11" class="node">
<title>10</title>
<ellipse fill="#0000ff" fill-opacity="0.498039" stroke="black" cx="588.8" cy="-194.46" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="588.8" y="-192.56" font-family="Times,serif" font-size="8.00">10 : 3</text>
</g>
<!-- 0&#45;&#45;10 -->
<g id="edge9" class="edge">
<title>0&#45;&#45;10</title>
<path fill="none" stroke="black" d="M478.83,-196.22C502.7,-195.84 536.67,-195.3 560.52,-194.91"/>
</g>
<!-- 11 -->
<g id="node12" class="node">
<title>11</title>
<ellipse fill="#00007f" fill-opacity="0.498039" stroke="black" cx="522.77" cy="-329.68" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="522.77" y="-327.78" font-family="Times,serif" font-size="8.00">11 : 1</text>
</g>
<!-- 0&#45;&#45;11 -->
<g id="edge10" class="edge">
<title>0&#45;&#45;11</title>
<path fill="none" stroke="black" d="M456.93,-208.47C470.61,-233.65 502.51,-292.39 516.27,-317.71"/>
</g>
<!-- 12 -->
<g id="node13" class="node">
<title>12</title>
<ellipse fill="#0000bf" fill-opacity="0.498039" stroke="black" cx="501.99" cy="-71.06" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="501.99" y="-69.16" font-family="Times,serif" font-size="8.00">12 : 2</text>
</g>
<!-- 0&#45;&#45;12 -->
<g id="edge11" class="edge">
<title>0&#45;&#45;12</title>
<path fill="none" stroke="black" d="M455.45,-184.63C465.27,-160.68 487.13,-107.32 496.99,-83.25"/>
</g>
<!-- 13 -->
<g id="node14" class="node">
<title>13</title>
<ellipse fill="#007fff" fill-opacity="0.498039" stroke="black" cx="335.24" cy="-185.57" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="335.24" y="-183.67" font-family="Times,serif" font-size="8.00">13 : 5</text>
</g>
<!-- 0&#45;&#45;13 -->
<g id="edge12" class="edge">
<title>0&#45;&#45;13</title>
<path fill="none" stroke="black" d="M422.91,-194.01C404.79,-192.27 381.18,-190 363.03,-188.25"/>
</g>
<!-- 17 -->
<g id="node18" class="node">
<title>17</title>
<ellipse fill="#0000bf" fill-opacity="0.498039" stroke="black" cx="507.29" cy="-203.38" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="507.29" y="-201.48" font-family="Times,serif" font-size="8.00">17 : 2</text>
</g>
<!-- 0&#45;&#45;17 -->
<g id="edge13" class="edge">
<title>0&#45;&#45;17</title>
<path fill="none" stroke="black" d="M477.91,-199.9C478.59,-199.99 479.27,-200.07 479.95,-200.15"/>
</g>
<!-- 19 -->
<g id="node20" class="node">
<title>19</title>
<ellipse fill="#0000ff" fill-opacity="0.498039" stroke="black" cx="335.25" cy="-258.07" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="335.25" y="-256.17" font-family="Times,serif" font-size="8.00">19 : 3</text>
</g>
<!-- 0&#45;&#45;19 -->
<g id="edge14" class="edge">
<title>0&#45;&#45;19</title>
<path fill="none" stroke="black" d="M432.51,-206.26C410.85,-217.8 374.73,-237.04 353.13,-248.54"/>
</g>
<!-- 21 -->
<g id="node22" class="node">
<title>21</title>
<ellipse fill="#0000bf" fill-opacity="0.498039" stroke="black" cx="503.25" cy="-133.9" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="503.25" y="-132" font-family="Times,serif" font-size="8.00">21 : 2</text>
</g>
<!-- 0&#45;&#45;21 -->
<g id="edge15" class="edge">
<title>0&#45;&#45;21</title>
<path fill="none" stroke="black" d="M460.19,-185.16C469.7,-173.83 484.14,-156.64 493.64,-145.34"/>
</g>
<!-- 31 -->
<g id="node32" class="node">
<title>31</title>
<ellipse fill="#00bfff" fill-opacity="0.498039" stroke="black" cx="227.26" cy="-142.96" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="227.26" y="-141.06" font-family="Times,serif" font-size="8.00">31 : 6</text>
</g>
<!-- 0&#45;&#45;31 -->
<g id="edge16" class="edge">
<title>0&#45;&#45;31</title>
<path fill="none" stroke="black" d="M425.77,-190.72C382.86,-180.39 295.11,-159.28 252.12,-148.94"/>
</g>
<!-- 1&#45;&#45;2 -->
<g id="edge17" class="edge">
<title>1&#45;&#45;2</title>
<path fill="none" stroke="black" d="M373.41,-183.36C357.69,-177.88 335.96,-170.3 319.84,-164.68"/>
</g>
<!-- 1&#45;&#45;3 -->
<g id="edge18" class="edge">
<title>1&#45;&#45;3</title>
<path fill="none" stroke="black" d="M397.84,-178.55C401.69,-167.4 407.42,-150.78 411.31,-139.49"/>
</g>
<!-- 1&#45;&#45;7 -->
<g id="edge19" class="edge">
<title>1&#45;&#45;7</title>
<path fill="none" stroke="black" d="M393.5,-178.17C393.18,-161.3 392.6,-131.13 392.28,-114.31"/>
</g>
<!-- 1&#45;&#45;13 -->
<g id="edge20" class="edge">
<title>1&#45;&#45;13</title>
<path fill="none" stroke="black" d="M369.27,-188.41C367.27,-188.24 365.23,-188.07 363.2,-187.9"/>
</g>
<!-- 1&#45;&#45;17 -->
<g id="edge21" class="edge">
<title>1&#45;&#45;17</title>
<path fill="none" stroke="black" d="M418.1,-193.22C436.21,-195.28 460.95,-198.1 479.78,-200.24"/>
</g>
<!-- 1&#45;&#45;19 -->
<g id="edge22" class="edge">
<title>1&#45;&#45;19</title>
<path fill="none" stroke="black" d="M384.08,-201.61C373.31,-214.07 355.97,-234.11 345.12,-246.66"/>
</g>
<!-- 1&#45;&#45;21 -->
<g id="edge23" class="edge">
<title>1&#45;&#45;21</title>
<path fill="none" stroke="black" d="M410.85,-181.61C431.06,-171.17 464.54,-153.89 485.16,-143.24"/>
</g>
<!-- 30 -->
<g id="node31" class="node">
<title>30</title>
<ellipse fill="#003fff" fill-opacity="0.498039" stroke="black" cx="253.19" cy="-259.36" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="253.19" y="-257.46" font-family="Times,serif" font-size="8.00">30 : 4</text>
</g>
<!-- 1&#45;&#45;30 -->
<g id="edge24" class="edge">
<title>1&#45;&#45;30</title>
<path fill="none" stroke="black" d="M376.22,-199.03C349.58,-212.1 299.11,-236.84 271.82,-250.22"/>
</g>
<!-- 2&#45;&#45;3 -->
<g id="edge25" class="edge">
<title>2&#45;&#45;3</title>
<path fill="none" stroke="black" d="M322.25,-150.84C343.26,-145.53 373.3,-137.92 393.57,-132.79"/>
</g>
<!-- 2&#45;&#45;7 -->
<g id="edge26" class="edge">
<title>2&#45;&#45;7</title>
<path fill="none" stroke="black" d="M314.71,-147.21C332.06,-137.09 358.92,-121.44 375.99,-111.5"/>
</g>
<!-- 2&#45;&#45;8 -->
<g id="edge27" class="edge">
<title>2&#45;&#45;8</title>
<path fill="none" stroke="black" d="M295.54,-169.24C292.85,-183.35 288.45,-206.43 285.74,-220.63"/>
</g>
<!-- 9 -->
<g id="node10" class="node">
<title>9</title>
<ellipse fill="#0000bf" fill-opacity="0.498039" stroke="black" cx="217.52" cy="-90.05" rx="25" ry="12.04"/>
<text text-anchor="middle" x="217.52" y="-88.15" font-family="Times,serif" font-size="8.00">9 : 2</text>
</g>
<!-- 2&#45;&#45;9 -->
<g id="edge28" class="edge">
<title>2&#45;&#45;9</title>
<path fill="none" stroke="black" d="M284.96,-146.26C269.88,-133.69 245.09,-113.03 230.13,-100.56"/>
</g>
<!-- 2&#45;&#45;13 -->
<g id="edge29" class="edge">
<title>2&#45;&#45;13</title>
<path fill="none" stroke="black" d="M311.79,-167.65C314.94,-170.06 318.29,-172.62 321.44,-175.03"/>
</g>
<!-- 27 -->
<g id="node28" class="node">
<title>27</title>
<ellipse fill="#003fff" fill-opacity="0.498039" stroke="black" cx="161.79" cy="-112.66" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="161.79" y="-110.76" font-family="Times,serif" font-size="8.00">27 : 4</text>
</g>
<!-- 2&#45;&#45;27 -->
<g id="edge30" class="edge">
<title>2&#45;&#45;27</title>
<path fill="none" stroke="black" d="M275.4,-149.69C250.19,-141.47 209.52,-128.22 184.3,-119.99"/>
</g>
<!-- 28 -->
<g id="node29" class="node">
<title>28</title>
<ellipse fill="#0000ff" fill-opacity="0.498039" stroke="black" cx="216.9" cy="-190.96" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="216.9" y="-189.06" font-family="Times,serif" font-size="8.00">28 : 3</text>
</g>
<!-- 2&#45;&#45;28 -->
<g id="edge31" class="edge">
<title>2&#45;&#45;28</title>
<path fill="none" stroke="black" d="M277.44,-165.58C265.07,-170.77 249.36,-177.35 237.05,-182.52"/>
</g>
<!-- 32 -->
<g id="node33" class="node">
<title>32</title>
<ellipse fill="#ffbf00" fill-opacity="0.498039" stroke="black" cx="149.95" cy="-249.86" rx="31.64" ry="12.04"/>
<text text-anchor="middle" x="149.95" y="-247.96" font-family="Times,serif" font-size="8.00">32 : 12</text>
</g>
<!-- 2&#45;&#45;32 -->
<g id="edge32" class="edge">
<title>2&#45;&#45;32</title>
<path fill="none" stroke="black" d="M281.76,-167.13C253.46,-184.9 195.1,-221.52 166.49,-239.48"/>
</g>
<!-- 3&#45;&#45;7 -->
<g id="edge33" class="edge">
<title>3&#45;&#45;7</title>
<path fill="none" stroke="black" d="M405.17,-116.15C404.28,-115.2 403.39,-114.25 402.5,-113.3"/>
</g>
<!-- 3&#45;&#45;12 -->
<g id="edge34" class="edge">
<title>3&#45;&#45;12</title>
<path fill="none" stroke="black" d="M430.6,-117.44C446.17,-107.32 470.39,-91.59 486.23,-81.3"/>
</g>
<!-- 3&#45;&#45;13 -->
<g id="edge35" class="edge">
<title>3&#45;&#45;13</title>
<path fill="none" stroke="black" d="M401.55,-137.39C387,-147.97 364.31,-164.45 349.61,-175.13"/>
</g>
<!-- 4&#45;&#45;6 -->
<g id="edge36" class="edge">
<title>4&#45;&#45;6</title>
<path fill="none" stroke="black" d="M593.49,-240.11C602.76,-232.83 615.06,-223.16 624.31,-215.9"/>
</g>
<!-- 4&#45;&#45;10 -->
<g id="edge37" class="edge">
<title>4&#45;&#45;10</title>
<path fill="none" stroke="black" d="M582.06,-238.39C583.5,-228.98 585.49,-216.01 586.93,-206.61"/>
</g>
<!-- 5&#45;&#45;6 -->
<g id="edge38" class="edge">
<title>5&#45;&#45;6</title>
<path fill="none" stroke="black" d="M634.38,-230.37C634.9,-226.33 635.47,-221.82 635.99,-217.77"/>
</g>
<!-- 5&#45;&#45;10 -->
<g id="edge39" class="edge">
<title>5&#45;&#45;10</title>
<path fill="none" stroke="black" d="M622.62,-231.34C615.61,-223.7 606.33,-213.58 599.26,-205.87"/>
</g>
<!-- 16 -->
<g id="node17" class="node">
<title>16</title>
<ellipse fill="#0000bf" fill-opacity="0.498039" stroke="black" cx="742.22" cy="-240.81" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="742.22" y="-238.91" font-family="Times,serif" font-size="8.00">16 : 2</text>
</g>
<!-- 5&#45;&#45;16 -->
<g id="edge40" class="edge">
<title>5&#45;&#45;16</title>
<path fill="none" stroke="black" d="M657.66,-242.1C674.3,-241.85 696.26,-241.51 713.63,-241.25"/>
</g>
<!-- 6&#45;&#45;16 -->
<g id="edge41" class="edge">
<title>6&#45;&#45;16</title>
<path fill="none" stroke="black" d="M657.98,-212.4C675.83,-218.42 701.68,-227.14 720.03,-233.33"/>
</g>
<!-- 8&#45;&#45;30 -->
<g id="edge42" class="edge">
<title>8&#45;&#45;30</title>
<path fill="none" stroke="black" d="M271.1,-243.76C269.4,-245.24 267.65,-246.76 265.95,-248.25"/>
</g>
<!-- 8&#45;&#45;32 -->
<g id="edge43" class="edge">
<title>8&#45;&#45;32</title>
<path fill="none" stroke="black" d="M259.21,-236.11C237.13,-238.89 204.38,-243.01 180.5,-246.02"/>
</g>
<!-- 33 -->
<g id="node34" class="node">
<title>33</title>
<ellipse fill="#7f0000" fill-opacity="0.498039" stroke="black" cx="162.72" cy="-221.56" rx="31.64" ry="12.04"/>
<text text-anchor="middle" x="162.72" y="-219.66" font-family="Times,serif" font-size="8.00">33 : 17</text>
</g>
<!-- 8&#45;&#45;33 -->
<g id="edge44" class="edge">
<title>8&#45;&#45;33</title>
<path fill="none" stroke="black" d="M258.95,-230.74C240.23,-228.96 214.24,-226.48 193.99,-224.54"/>
</g>
<!-- 9&#45;&#45;33 -->
<g id="edge45" class="edge">
<title>9&#45;&#45;33</title>
<path fill="none" stroke="black" d="M212.46,-102.19C202.02,-127.24 178.19,-184.43 167.77,-209.45"/>
</g>
<!-- 13&#45;&#45;33 -->
<g id="edge46" class="edge">
<title>13&#45;&#45;33</title>
<path fill="none" stroke="black" d="M309.79,-190.88C278.01,-197.51 223.84,-208.81 190.86,-215.69"/>
</g>
<!-- 14 -->
<g id="node15" class="node">
<title>14</title>
<ellipse fill="#0000bf" fill-opacity="0.498039" stroke="black" cx="48.93" cy="-293.69" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="48.93" y="-291.79" font-family="Times,serif" font-size="8.00">14 : 2</text>
</g>
<!-- 14&#45;&#45;32 -->
<g id="edge47" class="edge">
<title>14&#45;&#45;32</title>
<path fill="none" stroke="black" d="M68.88,-285.03C86.25,-277.5 111.33,-266.62 128.99,-258.96"/>
</g>
<!-- 14&#45;&#45;33 -->
<g id="edge48" class="edge">
<title>14&#45;&#45;33</title>
<path fill="none" stroke="black" d="M64.75,-283.66C86.25,-270.03 124.49,-245.79 146.3,-231.97"/>
</g>
<!-- 15 -->
<g id="node16" class="node">
<title>15</title>
<ellipse fill="#0000bf" fill-opacity="0.498039" stroke="black" cx="157.77" cy="-347.33" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="157.77" y="-345.43" font-family="Times,serif" font-size="8.00">15 : 2</text>
</g>
<!-- 15&#45;&#45;32 -->
<g id="edge49" class="edge">
<title>15&#45;&#45;32</title>
<path fill="none" stroke="black" d="M156.78,-334.98C155.27,-316.23 152.43,-280.8 150.93,-262.11"/>
</g>
<!-- 15&#45;&#45;33 -->
<g id="edge50" class="edge">
<title>15&#45;&#45;33</title>
<path fill="none" stroke="black" d="M158.24,-335.27C159.19,-311.29 161.29,-257.87 162.24,-233.77"/>
</g>
<!-- 18 -->
<g id="node19" class="node">
<title>18</title>
<ellipse fill="#0000bf" fill-opacity="0.498039" stroke="black" cx="76.97" cy="-323.09" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="76.97" y="-321.19" font-family="Times,serif" font-size="8.00">18 : 2</text>
</g>
<!-- 18&#45;&#45;32 -->
<g id="edge51" class="edge">
<title>18&#45;&#45;32</title>
<path fill="none" stroke="black" d="M88.05,-311.97C101.61,-298.37 124.46,-275.44 138.28,-261.58"/>
</g>
<!-- 18&#45;&#45;33 -->
<g id="edge52" class="edge">
<title>18&#45;&#45;33</title>
<path fill="none" stroke="black" d="M86.81,-311.45C103.35,-291.86 136.53,-252.57 153,-233.07"/>
</g>
<!-- 19&#45;&#45;33 -->
<g id="edge53" class="edge">
<title>19&#45;&#45;33</title>
<path fill="none" stroke="black" d="M309.79,-252.68C277.92,-245.94 223.52,-234.43 190.57,-227.45"/>
</g>
<!-- 20 -->
<g id="node21" class="node">
<title>20</title>
<ellipse fill="#0000bf" fill-opacity="0.498039" stroke="black" cx="111.75" cy="-346.1" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="111.75" y="-344.2" font-family="Times,serif" font-size="8.00">20 : 2</text>
</g>
<!-- 20&#45;&#45;32 -->
<g id="edge54" class="edge">
<title>20&#45;&#45;32</title>
<path fill="none" stroke="black" d="M116.59,-333.91C123.93,-315.4 137.82,-280.41 145.15,-261.95"/>
</g>
<!-- 20&#45;&#45;33 -->
<g id="edge55" class="edge">
<title>20&#45;&#45;33</title>
<path fill="none" stroke="black" d="M116.63,-334.16C126.35,-310.42 148.01,-257.51 157.77,-233.65"/>
</g>
<!-- 22 -->
<g id="node23" class="node">
<title>22</title>
<ellipse fill="#0000bf" fill-opacity="0.498039" stroke="black" cx="39.38" cy="-253.01" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="39.38" y="-251.11" font-family="Times,serif" font-size="8.00">22 : 2</text>
</g>
<!-- 22&#45;&#45;32 -->
<g id="edge56" class="edge">
<title>22&#45;&#45;32</title>
<path fill="none" stroke="black" d="M67.85,-252.2C83.21,-251.76 102.21,-251.22 118.06,-250.77"/>
</g>
<!-- 22&#45;&#45;33 -->
<g id="edge57" class="edge">
<title>22&#45;&#45;33</title>
<path fill="none" stroke="black" d="M63.74,-246.79C84.67,-241.46 114.76,-233.79 136.27,-228.3"/>
</g>
<!-- 23 -->
<g id="node24" class="node">
<title>23</title>
<ellipse fill="#007fff" fill-opacity="0.498039" stroke="black" cx="89.52" cy="-161.43" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="89.52" y="-159.53" font-family="Times,serif" font-size="8.00">23 : 5</text>
</g>
<!-- 25 -->
<g id="node26" class="node">
<title>25</title>
<ellipse fill="#0000ff" fill-opacity="0.498039" stroke="black" cx="91.99" cy="-85.49" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="91.99" y="-83.59" font-family="Times,serif" font-size="8.00">25 : 3</text>
</g>
<!-- 23&#45;&#45;25 -->
<g id="edge58" class="edge">
<title>23&#45;&#45;25</title>
<path fill="none" stroke="black" d="M89.92,-149.23C90.38,-135.14 91.13,-112.09 91.59,-97.91"/>
</g>
<!-- 23&#45;&#45;27 -->
<g id="edge59" class="edge">
<title>23&#45;&#45;27</title>
<path fill="none" stroke="black" d="M104.85,-151.09C117.11,-142.81 134.23,-131.26 146.49,-122.99"/>
</g>
<!-- 29 -->
<g id="node30" class="node">
<title>29</title>
<ellipse fill="#003fff" fill-opacity="0.498039" stroke="black" cx="45.77" cy="-196.19" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="45.77" y="-194.29" font-family="Times,serif" font-size="8.00">29 : 4</text>
</g>
<!-- 23&#45;&#45;29 -->
<g id="edge60" class="edge">
<title>23&#45;&#45;29</title>
<path fill="none" stroke="black" d="M76.16,-172.05C70.83,-176.28 64.7,-181.15 59.35,-185.4"/>
</g>
<!-- 23&#45;&#45;32 -->
<g id="edge61" class="edge">
<title>23&#45;&#45;32</title>
<path fill="none" stroke="black" d="M97.42,-172.99C108.94,-189.85 130.23,-221 141.85,-238.02"/>
</g>
<!-- 23&#45;&#45;33 -->
<g id="edge62" class="edge">
<title>23&#45;&#45;33</title>
<path fill="none" stroke="black" d="M102.61,-172.18C115.77,-182.99 135.96,-199.58 149.25,-210.5"/>
</g>
<!-- 24 -->
<g id="node25" class="node">
<title>24</title>
<ellipse fill="#0000ff" fill-opacity="0.498039" stroke="black" cx="137.81" cy="-33.02" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="137.81" y="-31.12" font-family="Times,serif" font-size="8.00">24 : 3</text>
</g>
<!-- 24&#45;&#45;25 -->
<g id="edge63" class="edge">
<title>24&#45;&#45;25</title>
<path fill="none" stroke="black" d="M127.86,-44.41C120.18,-53.21 109.59,-65.33 101.91,-74.13"/>
</g>
<!-- 24&#45;&#45;27 -->
<g id="edge64" class="edge">
<title>24&#45;&#45;27</title>
<path fill="none" stroke="black" d="M141.45,-45.12C145.98,-60.17 153.68,-85.72 158.19,-100.71"/>
</g>
<!-- 24&#45;&#45;31 -->
<g id="edge65" class="edge">
<title>24&#45;&#45;31</title>
<path fill="none" stroke="black" d="M147.38,-44.79C164.6,-65.95 200.59,-110.18 217.76,-131.28"/>
</g>
<!-- 25&#45;&#45;31 -->
<g id="edge66" class="edge">
<title>25&#45;&#45;31</title>
<path fill="none" stroke="black" d="M112.24,-94.09C137.74,-104.93 181.31,-123.44 206.87,-134.3"/>
</g>
<!-- 26 -->
<g id="node27" class="node">
<title>26</title>
<ellipse fill="#0000bf" fill-opacity="0.498039" stroke="black" cx="28.28" cy="-121.79" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="28.28" y="-119.89" font-family="Times,serif" font-size="8.00">26 : 2</text>
</g>
<!-- 26&#45;&#45;29 -->
<g id="edge67" class="edge">
<title>26&#45;&#45;29</title>
<path fill="none" stroke="black" d="M31.17,-134.08C34.43,-147.94 39.7,-170.36 42.93,-184.13"/>
</g>
<!-- 26&#45;&#45;33 -->
<g id="edge68" class="edge">
<title>26&#45;&#45;33</title>
<path fill="none" stroke="black" d="M42.67,-132.47C68.41,-151.57 122.07,-191.39 148.03,-210.66"/>
</g>
<!-- 27&#45;&#45;33 -->
<g id="edge69" class="edge">
<title>27&#45;&#45;33</title>
<path fill="none" stroke="black" d="M161.89,-124.73C162.07,-145.66 162.44,-188.46 162.62,-209.44"/>
</g>
<!-- 28&#45;&#45;31 -->
<g id="edge70" class="edge">
<title>28&#45;&#45;31</title>
<path fill="none" stroke="black" d="M219.52,-178.85C221.08,-171.63 223.05,-162.49 224.61,-155.24"/>
</g>
<!-- 28&#45;&#45;33 -->
<g id="edge71" class="edge">
<title>28&#45;&#45;33</title>
<path fill="none" stroke="black" d="M199.76,-200.64C193.68,-204.07 186.83,-207.94 180.69,-211.41"/>
</g>
<!-- 29&#45;&#45;32 -->
<g id="edge72" class="edge">
<title>29&#45;&#45;32</title>
<path fill="none" stroke="black" d="M63.91,-205.54C82.68,-215.21 111.87,-230.25 130.95,-240.08"/>
</g>
<!-- 29&#45;&#45;33 -->
<g id="edge73" class="edge">
<title>29&#45;&#45;33</title>
<path fill="none" stroke="black" d="M71.14,-201.69C89.95,-205.77 115.58,-211.33 134.94,-215.53"/>
</g>
<!-- 30&#45;&#45;32 -->
<g id="edge74" class="edge">
<title>30&#45;&#45;32</title>
<path fill="none" stroke="black" d="M225.52,-256.81C211.79,-255.55 195.14,-254.02 180.97,-252.72"/>
</g>
<!-- 30&#45;&#45;33 -->
<g id="edge75" class="edge">
<title>30&#45;&#45;33</title>
<path fill="none" stroke="black" d="M232.66,-250.78C218.17,-244.73 198.8,-236.63 184.11,-230.5"/>
</g>
<!-- 31&#45;&#45;32 -->
<g id="edge76" class="edge">
<title>31&#45;&#45;32</title>
<path fill="none" stroke="black" d="M218.7,-154.81C203.83,-175.36 173.44,-217.38 158.56,-237.96"/>
</g>
<!-- 31&#45;&#45;33 -->
<g id="edge77" class="edge">
<title>31&#45;&#45;33</title>
<path fill="none" stroke="black" d="M217.74,-154.56C205.54,-169.42 184.51,-195.03 172.28,-209.91"/>
</g>
<!-- 32&#45;&#45;33 -->
<g id="edge78" class="edge">
<title>32&#45;&#45;33</title>
<path fill="none" stroke="black" d="M155.44,-237.7C156.03,-236.38 156.64,-235.04 157.23,-233.72"/>
</g>
</g>
</svg>
</div>


!!! note The draw() function is part of the OneSparse doctest
    infrastructure, it's used to generate these doctest diagrams
    in-line with this documentation, which is also a test.

We saw the core idea about how graphs and matrices can be one and
the same, but now we see the relationship to vectors and graphs.
Vectors represent sets of nodes in the graph.  They map the node to
some useful piece of information, like the nodes degree.  Vector
reduction is a core operation of linear algebra, and the GraphBLAS
fully embaces this.  Vectors often end up being the result of an
algebraic graph algorithm.

Degree was simple enough, but what about more advanced algorithms?
This is where the GraphBLAS community really bring the power of
deep graph research with the [LAGraph]() library.  LAGraph is a
collection of optimized graph algorithms using the GraphBLAS API.
OneSparse also wraps LAgraph functions and exposes them to SQL.
For example, let's [PageRank]() the karate graph:
``` postgres-console
select draw(triu(graph),
            pagerank(graph),
            false, false, true, 0.5, 'PageRank The Karate Graph')
    as draw_source from karate \gset
```
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="925pt" height="424pt"
 viewBox="0.00 0.00 925.18 423.83" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 419.83)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-419.83 921.18,-419.83 921.18,4 -4,4"/>
<text text-anchor="middle" x="458.59" y="-7.4" font-family="Times,serif" font-size="12.00">PageRank The Karate Graph</text>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="#ab0000" fill-opacity="0.498039" stroke="black" cx="535.59" cy="-226.08" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="535.59" y="-224.18" font-family="Times,serif" font-size="8.00">0 : 0.09</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="#65ff99" fill-opacity="0.498039" stroke="black" cx="468.6" cy="-218.74" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="468.6" y="-216.84" font-family="Times,serif" font-size="8.00">1 : 0.05</text>
</g>
<!-- 0&#45;&#45;1 -->
<g id="edge1" class="edge">
<title>0&#45;&#45;1</title>
<path fill="none" stroke="black" d="M503.66,-222.58C502.59,-222.46 501.5,-222.34 500.43,-222.22"/>
</g>
<!-- 2 -->
<g id="node3" class="node">
<title>2</title>
<ellipse fill="#94ff6a" fill-opacity="0.498039" stroke="black" cx="355.51" cy="-179.3" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="355.51" y="-177.4" font-family="Times,serif" font-size="8.00">2 : 0.05</text>
</g>
<!-- 0&#45;&#45;2 -->
<g id="edge2" class="edge">
<title>0&#45;&#45;2</title>
<path fill="none" stroke="black" d="M508.24,-218.97C474.19,-210.13 416.42,-195.12 382.53,-186.32"/>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="#00a6ff" fill-opacity="0.498039" stroke="black" cx="494.32" cy="-144.15" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="494.32" y="-142.25" font-family="Times,serif" font-size="8.00">3 : 0.03</text>
</g>
<!-- 0&#45;&#45;3 -->
<g id="edge3" class="edge">
<title>0&#45;&#45;3</title>
<path fill="none" stroke="black" d="M529.5,-213.99C521.65,-198.41 508.1,-171.5 500.3,-156.03"/>
</g>
<!-- 4 -->
<g id="node5" class="node">
<title>4</title>
<ellipse fill="#000bff" fill-opacity="0.498039" stroke="black" cx="688.56" cy="-289.65" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="688.56" y="-287.75" font-family="Times,serif" font-size="8.00">4 : 0.02</text>
</g>
<!-- 0&#45;&#45;4 -->
<g id="edge4" class="edge">
<title>0&#45;&#45;4</title>
<path fill="none" stroke="black" d="M557.5,-235.19C586.58,-247.27 637.62,-268.48 666.68,-280.56"/>
</g>
<!-- 5 -->
<g id="node6" class="node">
<title>5</title>
<ellipse fill="#005aff" fill-opacity="0.498039" stroke="black" cx="750.67" cy="-280.12" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="750.67" y="-278.22" font-family="Times,serif" font-size="8.00">5 : 0.02</text>
</g>
<!-- 0&#45;&#45;5 -->
<g id="edge5" class="edge">
<title>0&#45;&#45;5</title>
<path fill="none" stroke="black" d="M563.27,-233.03C604.56,-243.41 681.86,-262.83 723.1,-273.19"/>
</g>
<!-- 6 -->
<g id="node7" class="node">
<title>6</title>
<ellipse fill="#005aff" fill-opacity="0.498039" stroke="black" cx="756.22" cy="-236.5" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="756.22" y="-234.6" font-family="Times,serif" font-size="8.00">6 : 0.02</text>
</g>
<!-- 0&#45;&#45;6 -->
<g id="edge6" class="edge">
<title>0&#45;&#45;6</title>
<path fill="none" stroke="black" d="M568.62,-227.64C610.21,-229.6 681.27,-232.96 722.97,-234.93"/>
</g>
<!-- 7 -->
<g id="node8" class="node">
<title>7</title>
<ellipse fill="#0027ff" fill-opacity="0.498039" stroke="black" cx="466.61" cy="-114.56" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="466.61" y="-112.66" font-family="Times,serif" font-size="8.00">7 : 0.02</text>
</g>
<!-- 0&#45;&#45;7 -->
<g id="edge7" class="edge">
<title>0&#45;&#45;7</title>
<path fill="none" stroke="black" d="M528.21,-214.14C514.93,-192.68 487.18,-147.81 473.94,-126.41"/>
</g>
<!-- 8 -->
<g id="node9" class="node">
<title>8</title>
<ellipse fill="#0062ff" fill-opacity="0.498039" stroke="black" cx="338.4" cy="-269.02" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="338.4" y="-267.12" font-family="Times,serif" font-size="8.00">8 : 0.02</text>
</g>
<!-- 0&#45;&#45;8 -->
<g id="edge8" class="edge">
<title>0&#45;&#45;8</title>
<path fill="none" stroke="black" d="M506.91,-232.32C469.53,-240.46 404.51,-254.63 367.11,-262.77"/>
</g>
<!-- 10 -->
<g id="node11" class="node">
<title>10</title>
<ellipse fill="#000bff" fill-opacity="0.498039" stroke="black" cx="698.71" cy="-223.48" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="698.71" y="-221.58" font-family="Times,serif" font-size="8.00">10 : 0.02</text>
</g>
<!-- 0&#45;&#45;10 -->
<g id="edge9" class="edge">
<title>0&#45;&#45;10</title>
<path fill="none" stroke="black" d="M568.99,-225.54C595.74,-225.12 633.27,-224.52 661.08,-224.08"/>
</g>
<!-- 11 -->
<g id="node12" class="node">
<title>11</title>
<ellipse fill="#00007f" fill-opacity="0.498039" stroke="black" cx="620.82" cy="-382.99" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="620.82" y="-381.09" font-family="Times,serif" font-size="8.00">11 : 0.00</text>
</g>
<!-- 0&#45;&#45;11 -->
<g id="edge10" class="edge">
<title>0&#45;&#45;11</title>
<path fill="none" stroke="black" d="M542.13,-238.13C558.04,-267.41 598.56,-342.01 614.37,-371.11"/>
</g>
<!-- 12 -->
<g id="node13" class="node">
<title>12</title>
<ellipse fill="#0000b8" fill-opacity="0.498039" stroke="black" cx="596.31" cy="-77.9" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="596.31" y="-76" font-family="Times,serif" font-size="8.00">12 : 0.01</text>
</g>
<!-- 0&#45;&#45;12 -->
<g id="edge11" class="edge">
<title>0&#45;&#45;12</title>
<path fill="none" stroke="black" d="M540.56,-213.95C551.94,-186.17 579.82,-118.14 591.27,-90.19"/>
</g>
<!-- 13 -->
<g id="node14" class="node">
<title>13</title>
<ellipse fill="#005fff" fill-opacity="0.498039" stroke="black" cx="399.6" cy="-212.99" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="399.6" y="-211.09" font-family="Times,serif" font-size="8.00">13 : 0.02</text>
</g>
<!-- 0&#45;&#45;13 -->
<g id="edge12" class="edge">
<title>0&#45;&#45;13</title>
<path fill="none" stroke="black" d="M503.36,-222.98C483.1,-221.03 456.91,-218.51 435.91,-216.48"/>
</g>
<!-- 17 -->
<g id="node18" class="node">
<title>17</title>
<ellipse fill="#0000b7" fill-opacity="0.498039" stroke="black" cx="602.56" cy="-233.99" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="602.56" y="-232.09" font-family="Times,serif" font-size="8.00">17 : 0.01</text>
</g>
<!-- 0&#45;&#45;17 -->
<g id="edge13" class="edge">
<title>0&#45;&#45;17</title>
<path fill="none" stroke="black" d="M567.5,-229.85C567.64,-229.86 567.77,-229.88 567.91,-229.9"/>
</g>
<!-- 19 -->
<g id="node20" class="node">
<title>19</title>
<ellipse fill="#0000ef" fill-opacity="0.498039" stroke="black" cx="399.61" cy="-298.51" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="399.61" y="-296.61" font-family="Times,serif" font-size="8.00">19 : 0.01</text>
</g>
<!-- 0&#45;&#45;19 -->
<g id="edge14" class="edge">
<title>0&#45;&#45;19</title>
<path fill="none" stroke="black" d="M516.68,-236.15C490.99,-249.83 445.29,-274.17 419.23,-288.05"/>
</g>
<!-- 21 -->
<g id="node22" class="node">
<title>21</title>
<ellipse fill="#0000b7" fill-opacity="0.498039" stroke="black" cx="597.79" cy="-152.03" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="597.79" y="-150.13" font-family="Times,serif" font-size="8.00">21 : 0.01</text>
</g>
<!-- 0&#45;&#45;21 -->
<g id="edge15" class="edge">
<title>0&#45;&#45;21</title>
<path fill="none" stroke="black" d="M545.31,-214.51C556.92,-200.68 576.26,-177.66 587.93,-163.76"/>
</g>
<!-- 31 -->
<g id="node32" class="node">
<title>31</title>
<ellipse fill="#00b4ff" fill-opacity="0.498039" stroke="black" cx="272.21" cy="-162.72" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="272.21" y="-160.82" font-family="Times,serif" font-size="8.00">31 : 0.03</text>
</g>
<!-- 0&#45;&#45;31 -->
<g id="edge16" class="edge">
<title>0&#45;&#45;31</title>
<path fill="none" stroke="black" d="M507.9,-219.42C458.11,-207.44 353.86,-182.36 302.35,-169.97"/>
</g>
<!-- 1&#45;&#45;2 -->
<g id="edge17" class="edge">
<title>1&#45;&#45;2</title>
<path fill="none" stroke="black" d="M444.62,-210.37C425.43,-203.68 398.64,-194.34 379.46,-187.65"/>
</g>
<!-- 1&#45;&#45;3 -->
<g id="edge18" class="edge">
<title>1&#45;&#45;3</title>
<path fill="none" stroke="black" d="M472.73,-206.75C477.51,-192.92 485.31,-170.28 490.12,-156.36"/>
</g>
<!-- 1&#45;&#45;7 -->
<g id="edge19" class="edge">
<title>1&#45;&#45;7</title>
<path fill="none" stroke="black" d="M468.36,-206.38C467.98,-186.22 467.22,-146.52 466.84,-126.58"/>
</g>
<!-- 1&#45;&#45;13 -->
<g id="edge20" class="edge">
<title>1&#45;&#45;13</title>
<path fill="none" stroke="black" d="M436.12,-216.03C435.98,-216.02 435.84,-216.01 435.7,-216"/>
</g>
<!-- 1&#45;&#45;17 -->
<g id="edge21" class="edge">
<title>1&#45;&#45;17</title>
<path fill="none" stroke="black" d="M500.35,-222.35C520.31,-224.62 546.11,-227.56 566.79,-229.92"/>
</g>
<!-- 1&#45;&#45;19 -->
<g id="edge22" class="edge">
<title>1&#45;&#45;19</title>
<path fill="none" stroke="black" d="M458.42,-230.51C445.38,-245.59 422.89,-271.58 409.83,-286.69"/>
</g>
<!-- 1&#45;&#45;21 -->
<g id="edge23" class="edge">
<title>1&#45;&#45;21</title>
<path fill="none" stroke="black" d="M487.94,-208.75C512.16,-196.25 553.42,-174.94 577.89,-162.3"/>
</g>
<!-- 30 -->
<g id="node31" class="node">
<title>30</title>
<ellipse fill="#0028ff" fill-opacity="0.498039" stroke="black" cx="302.8" cy="-300.03" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="302.8" y="-298.13" font-family="Times,serif" font-size="8.00">30 : 0.02</text>
</g>
<!-- 1&#45;&#45;30 -->
<g id="edge24" class="edge">
<title>1&#45;&#45;30</title>
<path fill="none" stroke="black" d="M448.6,-228.54C416.91,-244.08 355.45,-274.22 323.39,-289.94"/>
</g>
<!-- 2&#45;&#45;3 -->
<g id="edge25" class="edge">
<title>2&#45;&#45;3</title>
<path fill="none" stroke="black" d="M382.94,-172.36C407.27,-166.19 442.64,-157.24 466.96,-151.08"/>
</g>
<!-- 2&#45;&#45;7 -->
<g id="edge26" class="edge">
<title>2&#45;&#45;7</title>
<path fill="none" stroke="black" d="M373.36,-168.9C394.09,-156.82 428.09,-137.01 448.8,-124.94"/>
</g>
<!-- 2&#45;&#45;8 -->
<g id="edge27" class="edge">
<title>2&#45;&#45;8</title>
<path fill="none" stroke="black" d="M353.2,-191.4C349.94,-208.52 344,-239.65 340.73,-256.83"/>
</g>
<!-- 9 -->
<g id="node10" class="node">
<title>9</title>
<ellipse fill="#0000b4" fill-opacity="0.498039" stroke="black" cx="260.71" cy="-100.3" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="260.71" y="-98.4" font-family="Times,serif" font-size="8.00">9 : 0.01</text>
</g>
<!-- 2&#45;&#45;9 -->
<g id="edge28" class="edge">
<title>2&#45;&#45;9</title>
<path fill="none" stroke="black" d="M341.93,-167.98C323.81,-152.88 291.92,-126.31 273.97,-111.34"/>
</g>
<!-- 2&#45;&#45;13 -->
<g id="edge29" class="edge">
<title>2&#45;&#45;13</title>
<path fill="none" stroke="black" d="M369.95,-190.33C374.77,-194.02 380.14,-198.12 384.98,-201.82"/>
</g>
<!-- 27 -->
<g id="node28" class="node">
<title>27</title>
<ellipse fill="#0033ff" fill-opacity="0.498039" stroke="black" cx="194.97" cy="-126.97" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="194.97" y="-125.07" font-family="Times,serif" font-size="8.00">27 : 0.02</text>
</g>
<!-- 2&#45;&#45;27 -->
<g id="edge30" class="edge">
<title>2&#45;&#45;27</title>
<path fill="none" stroke="black" d="M330.78,-171.24C301.14,-161.57 251.68,-145.45 221.34,-135.56"/>
</g>
<!-- 28 -->
<g id="node29" class="node">
<title>28</title>
<ellipse fill="#0000ef" fill-opacity="0.498039" stroke="black" cx="259.99" cy="-219.34" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="259.99" y="-217.44" font-family="Times,serif" font-size="8.00">28 : 0.01</text>
</g>
<!-- 2&#45;&#45;28 -->
<g id="edge31" class="edge">
<title>2&#45;&#45;28</title>
<path fill="none" stroke="black" d="M333.36,-188.59C318.26,-194.92 298.31,-203.28 283.02,-209.69"/>
</g>
<!-- 32 -->
<g id="node33" class="node">
<title>32</title>
<ellipse fill="#ffc600" fill-opacity="0.498039" stroke="black" cx="181" cy="-288.83" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="181" y="-286.93" font-family="Times,serif" font-size="8.00">32 : 0.07</text>
</g>
<!-- 2&#45;&#45;32 -->
<g id="edge32" class="edge">
<title>2&#45;&#45;32</title>
<path fill="none" stroke="black" d="M338.78,-189.8C305.59,-210.63 231.76,-256.97 198.2,-278.04"/>
</g>
<!-- 3&#45;&#45;7 -->
<g id="edge33" class="edge">
<title>3&#45;&#45;7</title>
<path fill="none" stroke="black" d="M483.37,-132.46C481.43,-130.39 479.42,-128.24 477.48,-126.17"/>
</g>
<!-- 3&#45;&#45;12 -->
<g id="edge34" class="edge">
<title>3&#45;&#45;12</title>
<path fill="none" stroke="black" d="M510.71,-133.51C529.63,-121.22 560.58,-101.11 579.62,-88.74"/>
</g>
<!-- 3&#45;&#45;13 -->
<g id="edge35" class="edge">
<title>3&#45;&#45;13</title>
<path fill="none" stroke="black" d="M479.1,-155.21C461.43,-168.06 432.44,-189.12 414.78,-201.96"/>
</g>
<!-- 4&#45;&#45;6 -->
<g id="edge36" class="edge">
<title>4&#45;&#45;6</title>
<path fill="none" stroke="black" d="M702.58,-278.64C714.19,-269.52 730.61,-256.62 742.22,-247.5"/>
</g>
<!-- 4&#45;&#45;10 -->
<g id="edge37" class="edge">
<title>4&#45;&#45;10</title>
<path fill="none" stroke="black" d="M690.42,-277.51C692.25,-265.57 695.04,-247.45 696.86,-235.54"/>
</g>
<!-- 5&#45;&#45;6 -->
<g id="edge38" class="edge">
<title>5&#45;&#45;6</title>
<path fill="none" stroke="black" d="M752.25,-267.74C753.01,-261.79 753.91,-254.69 754.66,-248.77"/>
</g>
<!-- 5&#45;&#45;10 -->
<g id="edge39" class="edge">
<title>5&#45;&#45;10</title>
<path fill="none" stroke="black" d="M740.16,-268.66C731.26,-258.96 718.56,-245.11 709.57,-235.31"/>
</g>
<!-- 16 -->
<g id="node17" class="node">
<title>16</title>
<ellipse fill="#0000d0" fill-opacity="0.498039" stroke="black" cx="879.71" cy="-278.15" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="879.71" y="-276.25" font-family="Times,serif" font-size="8.00">16 : 0.01</text>
</g>
<!-- 5&#45;&#45;16 -->
<g id="edge40" class="edge">
<title>5&#45;&#45;16</title>
<path fill="none" stroke="black" d="M783.9,-279.61C801.67,-279.34 823.63,-279.01 842.03,-278.73"/>
</g>
<!-- 6&#45;&#45;16 -->
<g id="edge41" class="edge">
<title>6&#45;&#45;16</title>
<path fill="none" stroke="black" d="M780.62,-244.73C801.71,-251.84 832.1,-262.1 853.66,-269.37"/>
</g>
<!-- 8&#45;&#45;30 -->
<g id="edge42" class="edge">
<title>8&#45;&#45;30</title>
<path fill="none" stroke="black" d="M325.55,-280.22C322.36,-282.99 318.95,-285.97 315.75,-288.75"/>
</g>
<!-- 8&#45;&#45;32 -->
<g id="edge43" class="edge">
<title>8&#45;&#45;32</title>
<path fill="none" stroke="black" d="M306.93,-272.98C280.64,-276.29 243.2,-281 215.98,-284.43"/>
</g>
<!-- 33 -->
<g id="node34" class="node">
<title>33</title>
<ellipse fill="#7f0000" fill-opacity="0.498039" stroke="black" cx="196.07" cy="-255.44" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="196.07" y="-253.54" font-family="Times,serif" font-size="8.00">33 : 0.10</text>
</g>
<!-- 8&#45;&#45;33 -->
<g id="edge44" class="edge">
<title>8&#45;&#45;33</title>
<path fill="none" stroke="black" d="M306.1,-265.94C284.12,-263.84 255.01,-261.07 232.25,-258.89"/>
</g>
<!-- 9&#45;&#45;33 -->
<g id="edge45" class="edge">
<title>9&#45;&#45;33</title>
<path fill="none" stroke="black" d="M255.75,-112.22C243.75,-141.02 213.25,-214.2 201.14,-243.27"/>
</g>
<!-- 13&#45;&#45;33 -->
<g id="edge46" class="edge">
<title>13&#45;&#45;33</title>
<path fill="none" stroke="black" d="M367.8,-219.62C329.56,-227.6 265.79,-240.9 227.65,-248.85"/>
</g>
<!-- 14 -->
<g id="node15" class="node">
<title>14</title>
<ellipse fill="#0000b7" fill-opacity="0.498039" stroke="black" cx="61.83" cy="-340.53" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="61.83" y="-338.63" font-family="Times,serif" font-size="8.00">14 : 0.01</text>
</g>
<!-- 14&#45;&#45;32 -->
<g id="edge47" class="edge">
<title>14&#45;&#45;32</title>
<path fill="none" stroke="black" d="M84.24,-330.81C105.55,-321.56 137.41,-307.74 158.69,-298.51"/>
</g>
<!-- 14&#45;&#45;33 -->
<g id="edge48" class="edge">
<title>14&#45;&#45;33</title>
<path fill="none" stroke="black" d="M78.83,-329.75C104.51,-313.48 152.93,-282.78 178.78,-266.4"/>
</g>
<!-- 15 -->
<g id="node16" class="node">
<title>15</title>
<ellipse fill="#0000b7" fill-opacity="0.498039" stroke="black" cx="190.23" cy="-403.81" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="190.23" y="-401.91" font-family="Times,serif" font-size="8.00">15 : 0.01</text>
</g>
<!-- 15&#45;&#45;32 -->
<g id="edge49" class="edge">
<title>15&#45;&#45;32</title>
<path fill="none" stroke="black" d="M189.24,-391.5C187.46,-369.37 183.75,-323.11 181.98,-301.05"/>
</g>
<!-- 15&#45;&#45;33 -->
<g id="edge50" class="edge">
<title>15&#45;&#45;33</title>
<path fill="none" stroke="black" d="M190.71,-391.67C191.8,-363.84 194.48,-295.73 195.59,-267.75"/>
</g>
<!-- 18 -->
<g id="node19" class="node">
<title>18</title>
<ellipse fill="#0000b7" fill-opacity="0.498039" stroke="black" cx="94.91" cy="-375.22" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="94.91" y="-373.32" font-family="Times,serif" font-size="8.00">18 : 0.01</text>
</g>
<!-- 18&#45;&#45;32 -->
<g id="edge51" class="edge">
<title>18&#45;&#45;32</title>
<path fill="none" stroke="black" d="M106.52,-363.57C123.04,-346.99 153.15,-316.78 169.58,-300.3"/>
</g>
<!-- 18&#45;&#45;33 -->
<g id="edge52" class="edge">
<title>18&#45;&#45;33</title>
<path fill="none" stroke="black" d="M104.98,-363.3C124.37,-340.34 166.68,-290.24 186.04,-267.32"/>
</g>
<!-- 19&#45;&#45;33 -->
<g id="edge53" class="edge">
<title>19&#45;&#45;33</title>
<path fill="none" stroke="black" d="M368.25,-291.87C330,-283.78 265.73,-270.18 227.46,-262.08"/>
</g>
<!-- 20 -->
<g id="node21" class="node">
<title>20</title>
<ellipse fill="#0000b7" fill-opacity="0.498039" stroke="black" cx="135.94" cy="-402.36" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="135.94" y="-400.46" font-family="Times,serif" font-size="8.00">20 : 0.01</text>
</g>
<!-- 20&#45;&#45;32 -->
<g id="edge54" class="edge">
<title>20&#45;&#45;32</title>
<path fill="none" stroke="black" d="M140.76,-390.21C149.43,-368.36 167.57,-322.68 176.21,-300.89"/>
</g>
<!-- 20&#45;&#45;33 -->
<g id="edge55" class="edge">
<title>20&#45;&#45;33</title>
<path fill="none" stroke="black" d="M140.86,-390.34C152.14,-362.79 179.74,-295.34 191.08,-267.63"/>
</g>
<!-- 22 -->
<g id="node23" class="node">
<title>22</title>
<ellipse fill="#0000b7" fill-opacity="0.498039" stroke="black" cx="50.56" cy="-292.54" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="50.56" y="-290.64" font-family="Times,serif" font-size="8.00">22 : 0.01</text>
</g>
<!-- 22&#45;&#45;32 -->
<g id="edge56" class="edge">
<title>22&#45;&#45;32</title>
<path fill="none" stroke="black" d="M87.94,-291.48C105.41,-290.98 126.15,-290.39 143.62,-289.9"/>
</g>
<!-- 22&#45;&#45;33 -->
<g id="edge57" class="edge">
<title>22&#45;&#45;33</title>
<path fill="none" stroke="black" d="M80.01,-285.03C105.25,-278.6 141.44,-269.37 166.67,-262.94"/>
</g>
<!-- 23 -->
<g id="node24" class="node">
<title>23</title>
<ellipse fill="#0075ff" fill-opacity="0.498039" stroke="black" cx="109.72" cy="-184.51" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="109.72" y="-182.61" font-family="Times,serif" font-size="8.00">23 : 0.03</text>
</g>
<!-- 25 -->
<g id="node26" class="node">
<title>25</title>
<ellipse fill="#0000ff" fill-opacity="0.498039" stroke="black" cx="112.63" cy="-94.92" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="112.63" y="-93.02" font-family="Times,serif" font-size="8.00">25 : 0.02</text>
</g>
<!-- 23&#45;&#45;25 -->
<g id="edge58" class="edge">
<title>23&#45;&#45;25</title>
<path fill="none" stroke="black" d="M110.11,-172.43C110.67,-155.33 111.68,-124.24 112.24,-107.09"/>
</g>
<!-- 23&#45;&#45;27 -->
<g id="edge59" class="edge">
<title>23&#45;&#45;27</title>
<path fill="none" stroke="black" d="M126.16,-173.41C141.35,-163.16 163.79,-148.01 178.88,-137.83"/>
</g>
<!-- 29 -->
<g id="node30" class="node">
<title>29</title>
<ellipse fill="#003bff" fill-opacity="0.498039" stroke="black" cx="58.1" cy="-225.51" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="58.1" y="-223.61" font-family="Times,serif" font-size="8.00">29 : 0.02</text>
</g>
<!-- 23&#45;&#45;29 -->
<g id="edge60" class="edge">
<title>23&#45;&#45;29</title>
<path fill="none" stroke="black" d="M95.61,-195.71C88.33,-201.5 79.47,-208.53 72.19,-214.32"/>
</g>
<!-- 23&#45;&#45;32 -->
<g id="edge61" class="edge">
<title>23&#45;&#45;32</title>
<path fill="none" stroke="black" d="M117.9,-196.48C131.65,-216.6 159.24,-256.98 172.92,-277"/>
</g>
<!-- 23&#45;&#45;33 -->
<g id="edge62" class="edge">
<title>23&#45;&#45;33</title>
<path fill="none" stroke="black" d="M123.59,-195.9C139.71,-209.14 166.13,-230.85 182.23,-244.07"/>
</g>
<!-- 24 -->
<g id="node25" class="node">
<title>24</title>
<ellipse fill="#0001ff" fill-opacity="0.498039" stroke="black" cx="166.68" cy="-33.02" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="166.68" y="-31.12" font-family="Times,serif" font-size="8.00">24 : 0.02</text>
</g>
<!-- 24&#45;&#45;25 -->
<g id="edge63" class="edge">
<title>24&#45;&#45;25</title>
<path fill="none" stroke="black" d="M156.51,-44.66C146.85,-55.73 132.4,-72.28 122.75,-83.33"/>
</g>
<!-- 24&#45;&#45;27 -->
<g id="edge64" class="edge">
<title>24&#45;&#45;27</title>
<path fill="none" stroke="black" d="M170.38,-45.3C175.83,-63.41 185.96,-97.04 191.36,-114.98"/>
</g>
<!-- 24&#45;&#45;31 -->
<g id="edge65" class="edge">
<title>24&#45;&#45;31</title>
<path fill="none" stroke="black" d="M176.42,-44.99C196.52,-69.7 242.41,-126.09 262.5,-150.78"/>
</g>
<!-- 25&#45;&#45;31 -->
<g id="edge66" class="edge">
<title>25&#45;&#45;31</title>
<path fill="none" stroke="black" d="M135.5,-104.63C165.83,-117.52 219.08,-140.14 249.39,-153.02"/>
</g>
<!-- 26 -->
<g id="node27" class="node">
<title>26</title>
<ellipse fill="#0000bc" fill-opacity="0.498039" stroke="black" cx="37.48" cy="-137.74" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="37.48" y="-135.84" font-family="Times,serif" font-size="8.00">26 : 0.01</text>
</g>
<!-- 26&#45;&#45;29 -->
<g id="edge67" class="edge">
<title>26&#45;&#45;29</title>
<path fill="none" stroke="black" d="M40.34,-149.95C44.28,-166.71 51.33,-196.69 55.26,-213.41"/>
</g>
<!-- 26&#45;&#45;33 -->
<g id="edge68" class="edge">
<title>26&#45;&#45;33</title>
<path fill="none" stroke="black" d="M52.4,-148.82C82.62,-171.25 150.79,-221.83 181.08,-244.31"/>
</g>
<!-- 27&#45;&#45;33 -->
<g id="edge69" class="edge">
<title>27&#45;&#45;33</title>
<path fill="none" stroke="black" d="M195.08,-139.29C195.29,-163.9 195.76,-218.91 195.97,-243.35"/>
</g>
<!-- 28&#45;&#45;31 -->
<g id="edge70" class="edge">
<title>28&#45;&#45;31</title>
<path fill="none" stroke="black" d="M262.58,-207.33C264.63,-197.85 267.47,-184.68 269.54,-175.1"/>
</g>
<!-- 28&#45;&#45;33 -->
<g id="edge71" class="edge">
<title>28&#45;&#45;33</title>
<path fill="none" stroke="black" d="M241.16,-229.98C232.91,-234.64 223.25,-240.09 214.99,-244.76"/>
</g>
<!-- 29&#45;&#45;32 -->
<g id="edge72" class="edge">
<title>29&#45;&#45;32</title>
<path fill="none" stroke="black" d="M78.12,-235.82C100.97,-247.6 138.14,-266.75 160.99,-278.52"/>
</g>
<!-- 29&#45;&#45;33 -->
<g id="edge73" class="edge">
<title>29&#45;&#45;33</title>
<path fill="none" stroke="black" d="M89.41,-232.3C111.99,-237.2 142.34,-243.78 164.89,-248.68"/>
</g>
<!-- 30&#45;&#45;32 -->
<g id="edge74" class="edge">
<title>30&#45;&#45;32</title>
<path fill="none" stroke="black" d="M266.59,-296.7C251.02,-295.27 232.86,-293.6 217.29,-292.17"/>
</g>
<!-- 30&#45;&#45;33 -->
<g id="edge75" class="edge">
<title>30&#45;&#45;33</title>
<path fill="none" stroke="black" d="M279.91,-290.47C261.9,-282.94 236.93,-272.51 218.93,-264.99"/>
</g>
<!-- 31&#45;&#45;32 -->
<g id="edge76" class="edge">
<title>31&#45;&#45;32</title>
<path fill="none" stroke="black" d="M263.47,-174.81C245.99,-198.98 206.94,-252.98 189.59,-276.96"/>
</g>
<!-- 31&#45;&#45;33 -->
<g id="edge77" class="edge">
<title>31&#45;&#45;33</title>
<path fill="none" stroke="black" d="M262.57,-174.46C247.92,-192.3 220.24,-226.01 205.64,-243.79"/>
</g>
<!-- 32&#45;&#45;33 -->
<g id="edge78" class="edge">
<title>32&#45;&#45;33</title>
<path fill="none" stroke="black" d="M186.44,-276.78C187.79,-273.79 189.24,-270.58 190.59,-267.59"/>
</g>
</g>
</svg>
</div>

Wait isn't this the same as the degree graph?  Nope, but they sure
do look similar! This is because at this small-scale PageRank
essentially degenerates into degree.  Both of these algorithms are
called *centralities* and LAGraph has several of them, for example
[Triangle Centrality]() uses the number of triangles a node is
connected to to compute its centrality score.  This goes on the
assumption that triangles form stronger bonds than individual
edges.
``` postgres-console
select draw(triu(graph),
            triangle_centrality(graph),
            false, false, true, 0.5, 'The Karate Graph')
    as draw_source from karate \gset
```
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="925pt" height="424pt"
 viewBox="0.00 0.00 925.18 423.83" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 419.83)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-419.83 921.18,-419.83 921.18,4 -4,4"/>
<text text-anchor="middle" x="458.59" y="-7.4" font-family="Times,serif" font-size="12.00">The Karate Graph</text>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="#f50000" fill-opacity="0.498039" stroke="black" cx="535.59" cy="-226.08" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="535.59" y="-224.18" font-family="Times,serif" font-size="8.00">0 : 0.67</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="#ffa200" fill-opacity="0.498039" stroke="black" cx="468.6" cy="-218.74" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="468.6" y="-216.84" font-family="Times,serif" font-size="8.00">1 : 0.55</text>
</g>
<!-- 0&#45;&#45;1 -->
<g id="edge1" class="edge">
<title>0&#45;&#45;1</title>
<path fill="none" stroke="black" d="M503.66,-222.58C502.59,-222.46 501.5,-222.34 500.43,-222.22"/>
</g>
<!-- 2 -->
<g id="node3" class="node">
<title>2</title>
<ellipse fill="#ff2100" fill-opacity="0.498039" stroke="black" cx="355.51" cy="-179.3" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="355.51" y="-177.4" font-family="Times,serif" font-size="8.00">2 : 0.64</text>
</g>
<!-- 0&#45;&#45;2 -->
<g id="edge2" class="edge">
<title>0&#45;&#45;2</title>
<path fill="none" stroke="black" d="M508.24,-218.97C474.19,-210.13 416.42,-195.12 382.53,-186.32"/>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="#e5ff19" fill-opacity="0.498039" stroke="black" cx="494.32" cy="-144.15" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="494.32" y="-142.25" font-family="Times,serif" font-size="8.00">3 : 0.47</text>
</g>
<!-- 0&#45;&#45;3 -->
<g id="edge3" class="edge">
<title>0&#45;&#45;3</title>
<path fill="none" stroke="black" d="M529.5,-213.99C521.65,-198.41 508.1,-171.5 500.3,-156.03"/>
</g>
<!-- 4 -->
<g id="node5" class="node">
<title>4</title>
<ellipse fill="#0041ff" fill-opacity="0.498039" stroke="black" cx="688.56" cy="-289.65" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="688.56" y="-287.75" font-family="Times,serif" font-size="8.00">4 : 0.18</text>
</g>
<!-- 0&#45;&#45;4 -->
<g id="edge4" class="edge">
<title>0&#45;&#45;4</title>
<path fill="none" stroke="black" d="M557.5,-235.19C586.58,-247.27 637.62,-268.48 666.68,-280.56"/>
</g>
<!-- 5 -->
<g id="node6" class="node">
<title>5</title>
<ellipse fill="#0057ff" fill-opacity="0.498039" stroke="black" cx="750.67" cy="-280.12" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="750.67" y="-278.22" font-family="Times,serif" font-size="8.00">5 : 0.20</text>
</g>
<!-- 0&#45;&#45;5 -->
<g id="edge5" class="edge">
<title>0&#45;&#45;5</title>
<path fill="none" stroke="black" d="M563.27,-233.03C604.56,-243.41 681.86,-262.83 723.1,-273.19"/>
</g>
<!-- 6 -->
<g id="node7" class="node">
<title>6</title>
<ellipse fill="#0057ff" fill-opacity="0.498039" stroke="black" cx="756.22" cy="-236.5" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="756.22" y="-234.6" font-family="Times,serif" font-size="8.00">6 : 0.20</text>
</g>
<!-- 0&#45;&#45;6 -->
<g id="edge6" class="edge">
<title>0&#45;&#45;6</title>
<path fill="none" stroke="black" d="M568.62,-227.64C610.21,-229.6 681.27,-232.96 722.97,-234.93"/>
</g>
<!-- 7 -->
<g id="node8" class="node">
<title>7</title>
<ellipse fill="#9aff64" fill-opacity="0.498039" stroke="black" cx="466.61" cy="-114.56" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="466.61" y="-112.66" font-family="Times,serif" font-size="8.00">7 : 0.42</text>
</g>
<!-- 0&#45;&#45;7 -->
<g id="edge7" class="edge">
<title>0&#45;&#45;7</title>
<path fill="none" stroke="black" d="M528.21,-214.14C514.93,-192.68 487.18,-147.81 473.94,-126.41"/>
</g>
<!-- 8 -->
<g id="node9" class="node">
<title>8</title>
<ellipse fill="#f0ff0e" fill-opacity="0.498039" stroke="black" cx="338.4" cy="-269.02" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="338.4" y="-267.12" font-family="Times,serif" font-size="8.00">8 : 0.48</text>
</g>
<!-- 0&#45;&#45;8 -->
<g id="edge8" class="edge">
<title>0&#45;&#45;8</title>
<path fill="none" stroke="black" d="M506.91,-232.32C469.53,-240.46 404.51,-254.63 367.11,-262.77"/>
</g>
<!-- 10 -->
<g id="node11" class="node">
<title>10</title>
<ellipse fill="#0041ff" fill-opacity="0.498039" stroke="black" cx="698.71" cy="-223.48" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="698.71" y="-221.58" font-family="Times,serif" font-size="8.00">10 : 0.18</text>
</g>
<!-- 0&#45;&#45;10 -->
<g id="edge9" class="edge">
<title>0&#45;&#45;10</title>
<path fill="none" stroke="black" d="M568.99,-225.54C595.74,-225.12 633.27,-224.52 661.08,-224.08"/>
</g>
<!-- 11 -->
<g id="node12" class="node">
<title>11</title>
<ellipse fill="#7aff84" fill-opacity="0.498039" stroke="black" cx="620.82" cy="-382.99" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="620.82" y="-381.09" font-family="Times,serif" font-size="8.00">11 : 0.40</text>
</g>
<!-- 0&#45;&#45;11 -->
<g id="edge10" class="edge">
<title>0&#45;&#45;11</title>
<path fill="none" stroke="black" d="M542.13,-238.13C558.04,-267.41 598.56,-342.01 614.37,-371.11"/>
</g>
<!-- 12 -->
<g id="node13" class="node">
<title>12</title>
<ellipse fill="#006cff" fill-opacity="0.498039" stroke="black" cx="596.31" cy="-77.9" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="596.31" y="-76" font-family="Times,serif" font-size="8.00">12 : 0.21</text>
</g>
<!-- 0&#45;&#45;12 -->
<g id="edge11" class="edge">
<title>0&#45;&#45;12</title>
<path fill="none" stroke="black" d="M540.56,-213.95C551.94,-186.17 579.82,-118.14 591.27,-90.19"/>
</g>
<!-- 13 -->
<g id="node14" class="node">
<title>13</title>
<ellipse fill="#7f0000" fill-opacity="0.498039" stroke="black" cx="399.6" cy="-212.99" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="399.6" y="-211.09" font-family="Times,serif" font-size="8.00">13 : 0.75</text>
</g>
<!-- 0&#45;&#45;13 -->
<g id="edge12" class="edge">
<title>0&#45;&#45;13</title>
<path fill="none" stroke="black" d="M503.36,-222.98C483.1,-221.03 456.91,-218.51 435.91,-216.48"/>
</g>
<!-- 17 -->
<g id="node18" class="node">
<title>17</title>
<ellipse fill="#0082ff" fill-opacity="0.498039" stroke="black" cx="602.56" cy="-233.99" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="602.56" y="-232.09" font-family="Times,serif" font-size="8.00">17 : 0.22</text>
</g>
<!-- 0&#45;&#45;17 -->
<g id="edge13" class="edge">
<title>0&#45;&#45;17</title>
<path fill="none" stroke="black" d="M567.5,-229.85C567.64,-229.86 567.77,-229.88 567.91,-229.9"/>
</g>
<!-- 19 -->
<g id="node20" class="node">
<title>19</title>
<ellipse fill="#ff9700" fill-opacity="0.498039" stroke="black" cx="399.61" cy="-298.51" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="399.61" y="-296.61" font-family="Times,serif" font-size="8.00">19 : 0.56</text>
</g>
<!-- 0&#45;&#45;19 -->
<g id="edge14" class="edge">
<title>0&#45;&#45;19</title>
<path fill="none" stroke="black" d="M516.68,-236.15C490.99,-249.83 445.29,-274.17 419.23,-288.05"/>
</g>
<!-- 21 -->
<g id="node22" class="node">
<title>21</title>
<ellipse fill="#0082ff" fill-opacity="0.498039" stroke="black" cx="597.79" cy="-152.03" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="597.79" y="-150.13" font-family="Times,serif" font-size="8.00">21 : 0.22</text>
</g>
<!-- 0&#45;&#45;21 -->
<g id="edge15" class="edge">
<title>0&#45;&#45;21</title>
<path fill="none" stroke="black" d="M545.31,-214.51C556.92,-200.68 576.26,-177.66 587.93,-163.76"/>
</g>
<!-- 31 -->
<g id="node32" class="node">
<title>31</title>
<ellipse fill="#ff1600" fill-opacity="0.498039" stroke="black" cx="272.21" cy="-162.72" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="272.21" y="-160.82" font-family="Times,serif" font-size="8.00">31 : 0.65</text>
</g>
<!-- 0&#45;&#45;31 -->
<g id="edge16" class="edge">
<title>0&#45;&#45;31</title>
<path fill="none" stroke="black" d="M507.9,-219.42C458.11,-207.44 353.86,-182.36 302.35,-169.97"/>
</g>
<!-- 1&#45;&#45;2 -->
<g id="edge17" class="edge">
<title>1&#45;&#45;2</title>
<path fill="none" stroke="black" d="M444.62,-210.37C425.43,-203.68 398.64,-194.34 379.46,-187.65"/>
</g>
<!-- 1&#45;&#45;3 -->
<g id="edge18" class="edge">
<title>1&#45;&#45;3</title>
<path fill="none" stroke="black" d="M472.73,-206.75C477.51,-192.92 485.31,-170.28 490.12,-156.36"/>
</g>
<!-- 1&#45;&#45;7 -->
<g id="edge19" class="edge">
<title>1&#45;&#45;7</title>
<path fill="none" stroke="black" d="M468.36,-206.38C467.98,-186.22 467.22,-146.52 466.84,-126.58"/>
</g>
<!-- 1&#45;&#45;13 -->
<g id="edge20" class="edge">
<title>1&#45;&#45;13</title>
<path fill="none" stroke="black" d="M436.12,-216.03C435.98,-216.02 435.84,-216.01 435.7,-216"/>
</g>
<!-- 1&#45;&#45;17 -->
<g id="edge21" class="edge">
<title>1&#45;&#45;17</title>
<path fill="none" stroke="black" d="M500.35,-222.35C520.31,-224.62 546.11,-227.56 566.79,-229.92"/>
</g>
<!-- 1&#45;&#45;19 -->
<g id="edge22" class="edge">
<title>1&#45;&#45;19</title>
<path fill="none" stroke="black" d="M458.42,-230.51C445.38,-245.59 422.89,-271.58 409.83,-286.69"/>
</g>
<!-- 1&#45;&#45;21 -->
<g id="edge23" class="edge">
<title>1&#45;&#45;21</title>
<path fill="none" stroke="black" d="M487.94,-208.75C512.16,-196.25 553.42,-174.94 577.89,-162.3"/>
</g>
<!-- 30 -->
<g id="node31" class="node">
<title>30</title>
<ellipse fill="#ffc200" fill-opacity="0.498039" stroke="black" cx="302.8" cy="-300.03" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="302.8" y="-298.13" font-family="Times,serif" font-size="8.00">30 : 0.53</text>
</g>
<!-- 1&#45;&#45;30 -->
<g id="edge24" class="edge">
<title>1&#45;&#45;30</title>
<path fill="none" stroke="black" d="M448.6,-228.54C416.91,-244.08 355.45,-274.22 323.39,-289.94"/>
</g>
<!-- 2&#45;&#45;3 -->
<g id="edge25" class="edge">
<title>2&#45;&#45;3</title>
<path fill="none" stroke="black" d="M382.94,-172.36C407.27,-166.19 442.64,-157.24 466.96,-151.08"/>
</g>
<!-- 2&#45;&#45;7 -->
<g id="edge26" class="edge">
<title>2&#45;&#45;7</title>
<path fill="none" stroke="black" d="M373.36,-168.9C394.09,-156.82 428.09,-137.01 448.8,-124.94"/>
</g>
<!-- 2&#45;&#45;8 -->
<g id="edge27" class="edge">
<title>2&#45;&#45;8</title>
<path fill="none" stroke="black" d="M353.2,-191.4C349.94,-208.52 344,-239.65 340.73,-256.83"/>
</g>
<!-- 9 -->
<g id="node10" class="node">
<title>9</title>
<ellipse fill="#ff8200" fill-opacity="0.498039" stroke="black" cx="260.71" cy="-100.3" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="260.71" y="-98.4" font-family="Times,serif" font-size="8.00">9 : 0.57</text>
</g>
<!-- 2&#45;&#45;9 -->
<g id="edge28" class="edge">
<title>2&#45;&#45;9</title>
<path fill="none" stroke="black" d="M341.93,-167.98C323.81,-152.88 291.92,-126.31 273.97,-111.34"/>
</g>
<!-- 2&#45;&#45;13 -->
<g id="edge29" class="edge">
<title>2&#45;&#45;13</title>
<path fill="none" stroke="black" d="M369.95,-190.33C374.77,-194.02 380.14,-198.12 384.98,-201.82"/>
</g>
<!-- 27 -->
<g id="node28" class="node">
<title>27</title>
<ellipse fill="#8fff6f" fill-opacity="0.498039" stroke="black" cx="194.97" cy="-126.97" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="194.97" y="-125.07" font-family="Times,serif" font-size="8.00">27 : 0.41</text>
</g>
<!-- 2&#45;&#45;27 -->
<g id="edge30" class="edge">
<title>2&#45;&#45;27</title>
<path fill="none" stroke="black" d="M330.78,-171.24C301.14,-161.57 251.68,-145.45 221.34,-135.56"/>
</g>
<!-- 28 -->
<g id="node29" class="node">
<title>28</title>
<ellipse fill="#64ff9a" fill-opacity="0.498039" stroke="black" cx="259.99" cy="-219.34" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="259.99" y="-217.44" font-family="Times,serif" font-size="8.00">28 : 0.38</text>
</g>
<!-- 2&#45;&#45;28 -->
<g id="edge31" class="edge">
<title>2&#45;&#45;28</title>
<path fill="none" stroke="black" d="M333.36,-188.59C318.26,-194.92 298.31,-203.28 283.02,-209.69"/>
</g>
<!-- 32 -->
<g id="node33" class="node">
<title>32</title>
<ellipse fill="#daff24" fill-opacity="0.498039" stroke="black" cx="181" cy="-288.83" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="181" y="-286.93" font-family="Times,serif" font-size="8.00">32 : 0.46</text>
</g>
<!-- 2&#45;&#45;32 -->
<g id="edge32" class="edge">
<title>2&#45;&#45;32</title>
<path fill="none" stroke="black" d="M338.78,-189.8C305.59,-210.63 231.76,-256.97 198.2,-278.04"/>
</g>
<!-- 3&#45;&#45;7 -->
<g id="edge33" class="edge">
<title>3&#45;&#45;7</title>
<path fill="none" stroke="black" d="M483.37,-132.46C481.43,-130.39 479.42,-128.24 477.48,-126.17"/>
</g>
<!-- 3&#45;&#45;12 -->
<g id="edge34" class="edge">
<title>3&#45;&#45;12</title>
<path fill="none" stroke="black" d="M510.71,-133.51C529.63,-121.22 560.58,-101.11 579.62,-88.74"/>
</g>
<!-- 3&#45;&#45;13 -->
<g id="edge35" class="edge">
<title>3&#45;&#45;13</title>
<path fill="none" stroke="black" d="M479.1,-155.21C461.43,-168.06 432.44,-189.12 414.78,-201.96"/>
</g>
<!-- 4&#45;&#45;6 -->
<g id="edge36" class="edge">
<title>4&#45;&#45;6</title>
<path fill="none" stroke="black" d="M702.58,-278.64C714.19,-269.52 730.61,-256.62 742.22,-247.5"/>
</g>
<!-- 4&#45;&#45;10 -->
<g id="edge37" class="edge">
<title>4&#45;&#45;10</title>
<path fill="none" stroke="black" d="M690.42,-277.51C692.25,-265.57 695.04,-247.45 696.86,-235.54"/>
</g>
<!-- 5&#45;&#45;6 -->
<g id="edge38" class="edge">
<title>5&#45;&#45;6</title>
<path fill="none" stroke="black" d="M752.25,-267.74C753.01,-261.79 753.91,-254.69 754.66,-248.77"/>
</g>
<!-- 5&#45;&#45;10 -->
<g id="edge39" class="edge">
<title>5&#45;&#45;10</title>
<path fill="none" stroke="black" d="M740.16,-268.66C731.26,-258.96 718.56,-245.11 709.57,-235.31"/>
</g>
<!-- 16 -->
<g id="node17" class="node">
<title>16</title>
<ellipse fill="#00007f" fill-opacity="0.498039" stroke="black" cx="879.71" cy="-278.15" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="879.71" y="-276.25" font-family="Times,serif" font-size="8.00">16 : 0.05</text>
</g>
<!-- 5&#45;&#45;16 -->
<g id="edge40" class="edge">
<title>5&#45;&#45;16</title>
<path fill="none" stroke="black" d="M783.9,-279.61C801.67,-279.34 823.63,-279.01 842.03,-278.73"/>
</g>
<!-- 6&#45;&#45;16 -->
<g id="edge41" class="edge">
<title>6&#45;&#45;16</title>
<path fill="none" stroke="black" d="M780.62,-244.73C801.71,-251.84 832.1,-262.1 853.66,-269.37"/>
</g>
<!-- 8&#45;&#45;30 -->
<g id="edge42" class="edge">
<title>8&#45;&#45;30</title>
<path fill="none" stroke="black" d="M325.55,-280.22C322.36,-282.99 318.95,-285.97 315.75,-288.75"/>
</g>
<!-- 8&#45;&#45;32 -->
<g id="edge43" class="edge">
<title>8&#45;&#45;32</title>
<path fill="none" stroke="black" d="M306.93,-272.98C280.64,-276.29 243.2,-281 215.98,-284.43"/>
</g>
<!-- 33 -->
<g id="node34" class="node">
<title>33</title>
<ellipse fill="#ff9700" fill-opacity="0.498039" stroke="black" cx="196.07" cy="-255.44" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="196.07" y="-253.54" font-family="Times,serif" font-size="8.00">33 : 0.56</text>
</g>
<!-- 8&#45;&#45;33 -->
<g id="edge44" class="edge">
<title>8&#45;&#45;33</title>
<path fill="none" stroke="black" d="M306.1,-265.94C284.12,-263.84 255.01,-261.07 232.25,-258.89"/>
</g>
<!-- 9&#45;&#45;33 -->
<g id="edge45" class="edge">
<title>9&#45;&#45;33</title>
<path fill="none" stroke="black" d="M255.75,-112.22C243.75,-141.02 213.25,-214.2 201.14,-243.27"/>
</g>
<!-- 13&#45;&#45;33 -->
<g id="edge46" class="edge">
<title>13&#45;&#45;33</title>
<path fill="none" stroke="black" d="M367.8,-219.62C329.56,-227.6 265.79,-240.9 227.65,-248.85"/>
</g>
<!-- 14 -->
<g id="node15" class="node">
<title>14</title>
<ellipse fill="#006cff" fill-opacity="0.498039" stroke="black" cx="61.83" cy="-340.53" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="61.83" y="-338.63" font-family="Times,serif" font-size="8.00">14 : 0.21</text>
</g>
<!-- 14&#45;&#45;32 -->
<g id="edge47" class="edge">
<title>14&#45;&#45;32</title>
<path fill="none" stroke="black" d="M84.24,-330.81C105.55,-321.56 137.41,-307.74 158.69,-298.51"/>
</g>
<!-- 14&#45;&#45;33 -->
<g id="edge48" class="edge">
<title>14&#45;&#45;33</title>
<path fill="none" stroke="black" d="M78.83,-329.75C104.51,-313.48 152.93,-282.78 178.78,-266.4"/>
</g>
<!-- 15 -->
<g id="node16" class="node">
<title>15</title>
<ellipse fill="#006cff" fill-opacity="0.498039" stroke="black" cx="190.23" cy="-403.81" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="190.23" y="-401.91" font-family="Times,serif" font-size="8.00">15 : 0.21</text>
</g>
<!-- 15&#45;&#45;32 -->
<g id="edge49" class="edge">
<title>15&#45;&#45;32</title>
<path fill="none" stroke="black" d="M189.24,-391.5C187.46,-369.37 183.75,-323.11 181.98,-301.05"/>
</g>
<!-- 15&#45;&#45;33 -->
<g id="edge50" class="edge">
<title>15&#45;&#45;33</title>
<path fill="none" stroke="black" d="M190.71,-391.67C191.8,-363.84 194.48,-295.73 195.59,-267.75"/>
</g>
<!-- 18 -->
<g id="node19" class="node">
<title>18</title>
<ellipse fill="#006cff" fill-opacity="0.498039" stroke="black" cx="94.91" cy="-375.22" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="94.91" y="-373.32" font-family="Times,serif" font-size="8.00">18 : 0.21</text>
</g>
<!-- 18&#45;&#45;32 -->
<g id="edge51" class="edge">
<title>18&#45;&#45;32</title>
<path fill="none" stroke="black" d="M106.52,-363.57C123.04,-346.99 153.15,-316.78 169.58,-300.3"/>
</g>
<!-- 18&#45;&#45;33 -->
<g id="edge52" class="edge">
<title>18&#45;&#45;33</title>
<path fill="none" stroke="black" d="M104.98,-363.3C124.37,-340.34 166.68,-290.24 186.04,-267.32"/>
</g>
<!-- 19&#45;&#45;33 -->
<g id="edge53" class="edge">
<title>19&#45;&#45;33</title>
<path fill="none" stroke="black" d="M368.25,-291.87C330,-283.78 265.73,-270.18 227.46,-262.08"/>
</g>
<!-- 20 -->
<g id="node21" class="node">
<title>20</title>
<ellipse fill="#006cff" fill-opacity="0.498039" stroke="black" cx="135.94" cy="-402.36" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="135.94" y="-400.46" font-family="Times,serif" font-size="8.00">20 : 0.21</text>
</g>
<!-- 20&#45;&#45;32 -->
<g id="edge54" class="edge">
<title>20&#45;&#45;32</title>
<path fill="none" stroke="black" d="M140.76,-390.21C149.43,-368.36 167.57,-322.68 176.21,-300.89"/>
</g>
<!-- 20&#45;&#45;33 -->
<g id="edge55" class="edge">
<title>20&#45;&#45;33</title>
<path fill="none" stroke="black" d="M140.86,-390.34C152.14,-362.79 179.74,-295.34 191.08,-267.63"/>
</g>
<!-- 22 -->
<g id="node23" class="node">
<title>22</title>
<ellipse fill="#006cff" fill-opacity="0.498039" stroke="black" cx="50.56" cy="-292.54" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="50.56" y="-290.64" font-family="Times,serif" font-size="8.00">22 : 0.21</text>
</g>
<!-- 22&#45;&#45;32 -->
<g id="edge56" class="edge">
<title>22&#45;&#45;32</title>
<path fill="none" stroke="black" d="M87.94,-291.48C105.41,-290.98 126.15,-290.39 143.62,-289.9"/>
</g>
<!-- 22&#45;&#45;33 -->
<g id="edge57" class="edge">
<title>22&#45;&#45;33</title>
<path fill="none" stroke="black" d="M80.01,-285.03C105.25,-278.6 141.44,-269.37 166.67,-262.94"/>
</g>
<!-- 23 -->
<g id="node24" class="node">
<title>23</title>
<ellipse fill="#00e2ff" fill-opacity="0.498039" stroke="black" cx="109.72" cy="-184.51" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="109.72" y="-182.61" font-family="Times,serif" font-size="8.00">23 : 0.29</text>
</g>
<!-- 25 -->
<g id="node26" class="node">
<title>25</title>
<ellipse fill="#0000ea" fill-opacity="0.498039" stroke="black" cx="112.63" cy="-94.92" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="112.63" y="-93.02" font-family="Times,serif" font-size="8.00">25 : 0.12</text>
</g>
<!-- 23&#45;&#45;25 -->
<g id="edge58" class="edge">
<title>23&#45;&#45;25</title>
<path fill="none" stroke="black" d="M110.11,-172.43C110.67,-155.33 111.68,-124.24 112.24,-107.09"/>
</g>
<!-- 23&#45;&#45;27 -->
<g id="edge59" class="edge">
<title>23&#45;&#45;27</title>
<path fill="none" stroke="black" d="M126.16,-173.41C141.35,-163.16 163.79,-148.01 178.88,-137.83"/>
</g>
<!-- 29 -->
<g id="node30" class="node">
<title>29</title>
<ellipse fill="#00c2ff" fill-opacity="0.498039" stroke="black" cx="58.1" cy="-225.51" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="58.1" y="-223.61" font-family="Times,serif" font-size="8.00">29 : 0.27</text>
</g>
<!-- 23&#45;&#45;29 -->
<g id="edge60" class="edge">
<title>23&#45;&#45;29</title>
<path fill="none" stroke="black" d="M95.61,-195.71C88.33,-201.5 79.47,-208.53 72.19,-214.32"/>
</g>
<!-- 23&#45;&#45;32 -->
<g id="edge61" class="edge">
<title>23&#45;&#45;32</title>
<path fill="none" stroke="black" d="M117.9,-196.48C131.65,-216.6 159.24,-256.98 172.92,-277"/>
</g>
<!-- 23&#45;&#45;33 -->
<g id="edge62" class="edge">
<title>23&#45;&#45;33</title>
<path fill="none" stroke="black" d="M123.59,-195.9C139.71,-209.14 166.13,-230.85 182.23,-244.07"/>
</g>
<!-- 24 -->
<g id="node25" class="node">
<title>24</title>
<ellipse fill="#00008a" fill-opacity="0.498039" stroke="black" cx="166.68" cy="-33.02" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="166.68" y="-31.12" font-family="Times,serif" font-size="8.00">24 : 0.05</text>
</g>
<!-- 24&#45;&#45;25 -->
<g id="edge63" class="edge">
<title>24&#45;&#45;25</title>
<path fill="none" stroke="black" d="M156.51,-44.66C146.85,-55.73 132.4,-72.28 122.75,-83.33"/>
</g>
<!-- 24&#45;&#45;27 -->
<g id="edge64" class="edge">
<title>24&#45;&#45;27</title>
<path fill="none" stroke="black" d="M170.38,-45.3C175.83,-63.41 185.96,-97.04 191.36,-114.98"/>
</g>
<!-- 24&#45;&#45;31 -->
<g id="edge65" class="edge">
<title>24&#45;&#45;31</title>
<path fill="none" stroke="black" d="M176.42,-44.99C196.52,-69.7 242.41,-126.09 262.5,-150.78"/>
</g>
<!-- 25&#45;&#45;31 -->
<g id="edge66" class="edge">
<title>25&#45;&#45;31</title>
<path fill="none" stroke="black" d="M135.5,-104.63C165.83,-117.52 219.08,-140.14 249.39,-153.02"/>
</g>
<!-- 26 -->
<g id="node27" class="node">
<title>26</title>
<ellipse fill="#000cff" fill-opacity="0.498039" stroke="black" cx="37.48" cy="-137.74" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="37.48" y="-135.84" font-family="Times,serif" font-size="8.00">26 : 0.14</text>
</g>
<!-- 26&#45;&#45;29 -->
<g id="edge67" class="edge">
<title>26&#45;&#45;29</title>
<path fill="none" stroke="black" d="M40.34,-149.95C44.28,-166.71 51.33,-196.69 55.26,-213.41"/>
</g>
<!-- 26&#45;&#45;33 -->
<g id="edge68" class="edge">
<title>26&#45;&#45;33</title>
<path fill="none" stroke="black" d="M52.4,-148.82C82.62,-171.25 150.79,-221.83 181.08,-244.31"/>
</g>
<!-- 27&#45;&#45;33 -->
<g id="edge69" class="edge">
<title>27&#45;&#45;33</title>
<path fill="none" stroke="black" d="M195.08,-139.29C195.29,-163.9 195.76,-218.91 195.97,-243.35"/>
</g>
<!-- 28&#45;&#45;31 -->
<g id="edge70" class="edge">
<title>28&#45;&#45;31</title>
<path fill="none" stroke="black" d="M262.58,-207.33C264.63,-197.85 267.47,-184.68 269.54,-175.1"/>
</g>
<!-- 28&#45;&#45;33 -->
<g id="edge71" class="edge">
<title>28&#45;&#45;33</title>
<path fill="none" stroke="black" d="M241.16,-229.98C232.91,-234.64 223.25,-240.09 214.99,-244.76"/>
</g>
<!-- 29&#45;&#45;32 -->
<g id="edge72" class="edge">
<title>29&#45;&#45;32</title>
<path fill="none" stroke="black" d="M78.12,-235.82C100.97,-247.6 138.14,-266.75 160.99,-278.52"/>
</g>
<!-- 29&#45;&#45;33 -->
<g id="edge73" class="edge">
<title>29&#45;&#45;33</title>
<path fill="none" stroke="black" d="M89.41,-232.3C111.99,-237.2 142.34,-243.78 164.89,-248.68"/>
</g>
<!-- 30&#45;&#45;32 -->
<g id="edge74" class="edge">
<title>30&#45;&#45;32</title>
<path fill="none" stroke="black" d="M266.59,-296.7C251.02,-295.27 232.86,-293.6 217.29,-292.17"/>
</g>
<!-- 30&#45;&#45;33 -->
<g id="edge75" class="edge">
<title>30&#45;&#45;33</title>
<path fill="none" stroke="black" d="M279.91,-290.47C261.9,-282.94 236.93,-272.51 218.93,-264.99"/>
</g>
<!-- 31&#45;&#45;32 -->
<g id="edge76" class="edge">
<title>31&#45;&#45;32</title>
<path fill="none" stroke="black" d="M263.47,-174.81C245.99,-198.98 206.94,-252.98 189.59,-276.96"/>
</g>
<!-- 31&#45;&#45;33 -->
<g id="edge77" class="edge">
<title>31&#45;&#45;33</title>
<path fill="none" stroke="black" d="M262.57,-174.46C247.92,-192.3 220.24,-226.01 205.64,-243.79"/>
</g>
<!-- 32&#45;&#45;33 -->
<g id="edge78" class="edge">
<title>32&#45;&#45;33</title>
<path fill="none" stroke="black" d="M186.44,-276.78C187.79,-273.79 189.24,-270.58 190.59,-267.59"/>
</g>
</g>
</svg>
</div>

This can be taken even further with the [Square Clustering]()
algorithm, which considers how many squares nodes connect to:
``` postgres-console
select draw(triu(graph),
            square_clustering(graph),
            false, false, true, 0.5, 'The Karate Graph')
    as draw_source from karate \gset
```
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="959pt" height="383pt"
 viewBox="0.00 0.00 959.38 383.29" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 379.29)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-379.29 955.38,-379.29 955.38,4 -4,4"/>
<text text-anchor="middle" x="475.69" y="-7.4" font-family="Times,serif" font-size="12.00">The Karate Graph</text>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="#00007f" fill-opacity="0.498039" stroke="black" cx="386.03" cy="-198.43" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="386.03" y="-196.53" font-family="Times,serif" font-size="8.00">0 : 0.09</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="#0030ff" fill-opacity="0.498039" stroke="black" cx="468.55" cy="-112.13" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="468.55" y="-110.23" font-family="Times,serif" font-size="8.00">1 : 0.17</text>
</g>
<!-- 0&#45;&#45;1 -->
<g id="edge1" class="edge">
<title>0&#45;&#45;1</title>
<path fill="none" stroke="black" d="M397.15,-186.79C412.99,-170.23 441.85,-140.04 457.59,-123.58"/>
</g>
<!-- 2 -->
<g id="node3" class="node">
<title>2</title>
<ellipse fill="#0000d2" fill-opacity="0.498039" stroke="black" cx="570.64" cy="-148.18" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="570.64" y="-146.28" font-family="Times,serif" font-size="8.00">2 : 0.12</text>
</g>
<!-- 0&#45;&#45;2 -->
<g id="edge2" class="edge">
<title>0&#45;&#45;2</title>
<path fill="none" stroke="black" d="M412.87,-191.12C447.97,-181.57 509.09,-164.93 544.06,-155.42"/>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="#0090ff" fill-opacity="0.498039" stroke="black" cx="413.16" cy="-128.78" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="413.16" y="-126.88" font-family="Times,serif" font-size="8.00">3 : 0.21</text>
</g>
<!-- 0&#45;&#45;3 -->
<g id="edge3" class="edge">
<title>0&#45;&#45;3</title>
<path fill="none" stroke="black" d="M390.75,-186.3C395.73,-173.51 403.53,-153.5 408.49,-140.77"/>
</g>
<!-- 4 -->
<g id="node5" class="node">
<title>4</title>
<ellipse fill="#0000ca" fill-opacity="0.498039" stroke="black" cx="241.83" cy="-297.15" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="241.83" y="-295.25" font-family="Times,serif" font-size="8.00">4 : 0.12</text>
</g>
<!-- 0&#45;&#45;4 -->
<g id="edge4" class="edge">
<title>0&#45;&#45;4</title>
<path fill="none" stroke="black" d="M370.32,-209.18C342.58,-228.17 285.24,-267.43 257.52,-286.41"/>
</g>
<!-- 5 -->
<g id="node6" class="node">
<title>5</title>
<ellipse fill="#00008b" fill-opacity="0.498039" stroke="black" cx="162.7" cy="-211.86" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="162.7" y="-209.96" font-family="Times,serif" font-size="8.00">5 : 0.09</text>
</g>
<!-- 0&#45;&#45;5 -->
<g id="edge5" class="edge">
<title>0&#45;&#45;5</title>
<path fill="none" stroke="black" d="M353.07,-200.41C310.85,-202.95 238.08,-207.33 195.79,-209.87"/>
</g>
<!-- 6 -->
<g id="node7" class="node">
<title>6</title>
<ellipse fill="#00008b" fill-opacity="0.498039" stroke="black" cx="171.14" cy="-269.39" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="171.14" y="-267.49" font-family="Times,serif" font-size="8.00">6 : 0.09</text>
</g>
<!-- 0&#45;&#45;6 -->
<g id="edge6" class="edge">
<title>0&#45;&#45;6</title>
<path fill="none" stroke="black" d="M361.38,-206.57C320.04,-220.22 237.2,-247.57 195.84,-261.23"/>
</g>
<!-- 7 -->
<g id="node8" class="node">
<title>7</title>
<ellipse fill="#4bffb3" fill-opacity="0.498039" stroke="black" cx="442.98" cy="-65.98" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="442.98" y="-64.08" font-family="Times,serif" font-size="8.00">7 : 0.30</text>
</g>
<!-- 0&#45;&#45;7 -->
<g id="edge7" class="edge">
<title>0&#45;&#45;7</title>
<path fill="none" stroke="black" d="M391.28,-186.2C402.13,-160.97 426.9,-103.38 437.73,-78.17"/>
</g>
<!-- 8 -->
<g id="node9" class="node">
<title>8</title>
<ellipse fill="#0010ff" fill-opacity="0.498039" stroke="black" cx="607.72" cy="-190.62" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="607.72" y="-188.72" font-family="Times,serif" font-size="8.00">8 : 0.15</text>
</g>
<!-- 0&#45;&#45;8 -->
<g id="edge8" class="edge">
<title>0&#45;&#45;8</title>
<path fill="none" stroke="black" d="M419.22,-197.26C461.01,-195.79 532.4,-193.27 574.31,-191.8"/>
</g>
<!-- 10 -->
<g id="node11" class="node">
<title>10</title>
<ellipse fill="#0000ca" fill-opacity="0.498039" stroke="black" cx="230.89" cy="-228.04" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="230.89" y="-226.14" font-family="Times,serif" font-size="8.00">10 : 0.12</text>
</g>
<!-- 0&#45;&#45;10 -->
<g id="edge9" class="edge">
<title>0&#45;&#45;10</title>
<path fill="none" stroke="black" d="M356.48,-204.07C329.84,-209.15 290.75,-216.61 263.27,-221.86"/>
</g>
<!-- 12 -->
<g id="node12" class="node">
<title>12</title>
<ellipse fill="#27ffd7" fill-opacity="0.498039" stroke="black" cx="275.9" cy="-122.89" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="275.9" y="-120.99" font-family="Times,serif" font-size="8.00">12 : 0.28</text>
</g>
<!-- 0&#45;&#45;12 -->
<g id="edge11" class="edge">
<title>0&#45;&#45;12</title>
<path fill="none" stroke="black" d="M370.25,-187.6C349.43,-173.33 312.98,-148.33 292,-133.93"/>
</g>
<!-- 13 -->
<g id="node13" class="node">
<title>13</title>
<ellipse fill="#0067ff" fill-opacity="0.498039" stroke="black" cx="523.89" cy="-159.51" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="523.89" y="-157.61" font-family="Times,serif" font-size="8.00">13 : 0.19</text>
</g>
<!-- 0&#45;&#45;13 -->
<g id="edge12" class="edge">
<title>0&#45;&#45;13</title>
<path fill="none" stroke="black" d="M412.28,-191.02C436.03,-184.31 470.93,-174.46 495.36,-167.57"/>
</g>
<!-- 17 -->
<g id="node17" class="node">
<title>17</title>
<ellipse fill="#ffdf00" fill-opacity="0.498039" stroke="black" cx="326.96" cy="-81.79" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="326.96" y="-79.89" font-family="Times,serif" font-size="8.00">17 : 0.40</text>
</g>
<!-- 0&#45;&#45;17 -->
<g id="edge13" class="edge">
<title>0&#45;&#45;17</title>
<path fill="none" stroke="black" d="M379.93,-186.38C368.61,-164.03 344.47,-116.38 333.11,-93.94"/>
</g>
<!-- 19 -->
<g id="node19" class="node">
<title>19</title>
<ellipse fill="#002aff" fill-opacity="0.498039" stroke="black" cx="532.79" cy="-221.17" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="532.79" y="-219.27" font-family="Times,serif" font-size="8.00">19 : 0.16</text>
</g>
<!-- 0&#45;&#45;19 -->
<g id="edge14" class="edge">
<title>0&#45;&#45;19</title>
<path fill="none" stroke="black" d="M416.79,-203.19C440.83,-206.92 474.21,-212.09 498.99,-215.93"/>
</g>
<!-- 21 -->
<g id="node21" class="node">
<title>21</title>
<ellipse fill="#ffdf00" fill-opacity="0.498039" stroke="black" cx="357.11" cy="-33.02" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="357.11" y="-31.12" font-family="Times,serif" font-size="8.00">21 : 0.40</text>
</g>
<!-- 0&#45;&#45;21 -->
<g id="edge15" class="edge">
<title>0&#45;&#45;21</title>
<path fill="none" stroke="black" d="M383.9,-186.26C378.55,-155.68 364.59,-75.81 359.24,-45.2"/>
</g>
<!-- 31 -->
<g id="node31" class="node">
<title>31</title>
<ellipse fill="#000085" fill-opacity="0.498039" stroke="black" cx="634.24" cy="-258.85" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="634.24" y="-256.95" font-family="Times,serif" font-size="8.00">31 : 0.09</text>
</g>
<!-- 0&#45;&#45;31 -->
<g id="edge16" class="edge">
<title>0&#45;&#45;31</title>
<path fill="none" stroke="black" d="M414.02,-205.24C461.05,-216.69 555.55,-239.7 604.04,-251.5"/>
</g>
<!-- 11 -->
<g id="node34" class="node">
<title>11</title>
<ellipse fill="none" stroke="black" cx="313.53" cy="-363.27" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="313.53" y="-361.37" font-family="Times,serif" font-size="8.00">11</text>
</g>
<!-- 0&#45;&#45;11 -->
<g id="edge10" class="edge">
<title>0&#45;&#45;11</title>
<path fill="none" stroke="black" d="M380.7,-210.55C367.22,-241.18 331.94,-321.4 318.68,-351.57"/>
</g>
<!-- 1&#45;&#45;2 -->
<g id="edge17" class="edge">
<title>1&#45;&#45;2</title>
<path fill="none" stroke="black" d="M492.48,-120.58C508.87,-126.37 530.5,-134.01 546.85,-139.78"/>
</g>
<!-- 1&#45;&#45;3 -->
<g id="edge18" class="edge">
<title>1&#45;&#45;3</title>
<path fill="none" stroke="black" d="M442.8,-119.87C441.44,-120.28 440.08,-120.68 438.73,-121.09"/>
</g>
<!-- 1&#45;&#45;7 -->
<g id="edge19" class="edge">
<title>1&#45;&#45;7</title>
<path fill="none" stroke="black" d="M461.83,-100C458.04,-93.17 453.33,-84.66 449.56,-77.86"/>
</g>
<!-- 1&#45;&#45;13 -->
<g id="edge20" class="edge">
<title>1&#45;&#45;13</title>
<path fill="none" stroke="black" d="M481.66,-123.36C490.36,-130.8 501.73,-140.54 510.49,-148.03"/>
</g>
<!-- 1&#45;&#45;17 -->
<g id="edge21" class="edge">
<title>1&#45;&#45;17</title>
<path fill="none" stroke="black" d="M439.89,-105.99C416.3,-100.93 382.88,-93.77 358.52,-88.55"/>
</g>
<!-- 1&#45;&#45;19 -->
<g id="edge22" class="edge">
<title>1&#45;&#45;19</title>
<path fill="none" stroke="black" d="M475.67,-124.21C488.02,-145.17 513.26,-188.03 525.63,-209.03"/>
</g>
<!-- 1&#45;&#45;21 -->
<g id="edge23" class="edge">
<title>1&#45;&#45;21</title>
<path fill="none" stroke="black" d="M453.05,-101.13C431.88,-86.1 394.12,-59.29 372.84,-44.18"/>
</g>
<!-- 30 -->
<g id="node30" class="node">
<title>30</title>
<ellipse fill="#0043ff" fill-opacity="0.498039" stroke="black" cx="650.06" cy="-118.78" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="650.06" y="-116.88" font-family="Times,serif" font-size="8.00">30 : 0.18</text>
</g>
<!-- 1&#45;&#45;30 -->
<g id="edge24" class="edge">
<title>1&#45;&#45;30</title>
<path fill="none" stroke="black" d="M501.84,-113.35C533.16,-114.5 579.98,-116.21 612.59,-117.41"/>
</g>
<!-- 2&#45;&#45;3 -->
<g id="edge25" class="edge">
<title>2&#45;&#45;3</title>
<path fill="none" stroke="black" d="M539.15,-144.3C511.77,-140.93 472.3,-136.07 444.87,-132.68"/>
</g>
<!-- 2&#45;&#45;7 -->
<g id="edge26" class="edge">
<title>2&#45;&#45;7</title>
<path fill="none" stroke="black" d="M554.21,-137.61C529.7,-121.82 483.82,-92.28 459.34,-76.52"/>
</g>
<!-- 2&#45;&#45;8 -->
<g id="edge27" class="edge">
<title>2&#45;&#45;8</title>
<path fill="none" stroke="black" d="M580.77,-159.78C586,-165.77 592.37,-173.05 597.6,-179.04"/>
</g>
<!-- 9 -->
<g id="node10" class="node">
<title>9</title>
<ellipse fill="#00d9ff" fill-opacity="0.498039" stroke="black" cx="690.19" cy="-42.85" rx="33.47" ry="12.04"/>
<text text-anchor="middle" x="690.19" y="-40.95" font-family="Times,serif" font-size="8.00">9 : 0.25</text>
</g>
<!-- 2&#45;&#45;9 -->
<g id="edge28" class="edge">
<title>2&#45;&#45;9</title>
<path fill="none" stroke="black" d="M583.43,-136.91C606.44,-116.64 654.54,-74.26 677.49,-54.04"/>
</g>
<!-- 2&#45;&#45;13 -->
<g id="edge29" class="edge">
<title>2&#45;&#45;13</title>
<path fill="none" stroke="black" d="M542.9,-154.9C542.75,-154.94 542.59,-154.98 542.43,-155.02"/>
</g>
<!-- 27 -->
<g id="node27" class="node">
<title>27</title>
<ellipse fill="#0000c6" fill-opacity="0.498039" stroke="black" cx="704.8" cy="-190.02" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="704.8" y="-188.12" font-family="Times,serif" font-size="8.00">27 : 0.12</text>
</g>
<!-- 2&#45;&#45;27 -->
<g id="edge30" class="edge">
<title>2&#45;&#45;27</title>
<path fill="none" stroke="black" d="M595.87,-156.05C619.25,-163.34 653.92,-174.16 677.84,-181.62"/>
</g>
<!-- 28 -->
<g id="node28" class="node">
<title>28</title>
<ellipse fill="#0016ff" fill-opacity="0.498039" stroke="black" cx="638.39" cy="-296.83" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="638.39" y="-294.93" font-family="Times,serif" font-size="8.00">28 : 0.16</text>
</g>
<!-- 2&#45;&#45;28 -->
<g id="edge31" class="edge">
<title>2&#45;&#45;28</title>
<path fill="none" stroke="black" d="M576.07,-160.1C588.75,-187.93 620.22,-256.97 632.93,-284.86"/>
</g>
<!-- 32 -->
<g id="node32" class="node">
<title>32</title>
<ellipse fill="#0000f5" fill-opacity="0.498039" stroke="black" cx="774.27" cy="-211.54" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="774.27" y="-209.64" font-family="Times,serif" font-size="8.00">32 : 0.14</text>
</g>
<!-- 2&#45;&#45;32 -->
<g id="edge32" class="edge">
<title>2&#45;&#45;32</title>
<path fill="none" stroke="black" d="M596.02,-156.08C634.61,-168.09 707.74,-190.84 747.28,-203.14"/>
</g>
<!-- 3&#45;&#45;7 -->
<g id="edge33" class="edge">
<title>3&#45;&#45;7</title>
<path fill="none" stroke="black" d="M418.91,-116.67C424.19,-105.55 431.97,-89.16 437.25,-78.05"/>
</g>
<!-- 3&#45;&#45;12 -->
<g id="edge34" class="edge">
<title>3&#45;&#45;12</title>
<path fill="none" stroke="black" d="M379.93,-127.35C359.78,-126.49 334.02,-125.38 313.17,-124.49"/>
</g>
<!-- 3&#45;&#45;13 -->
<g id="edge35" class="edge">
<title>3&#45;&#45;13</title>
<path fill="none" stroke="black" d="M439.68,-136.14C456.45,-140.79 478.1,-146.8 495.26,-151.56"/>
</g>
<!-- 4&#45;&#45;6 -->
<g id="edge36" class="edge">
<title>4&#45;&#45;6</title>
<path fill="none" stroke="black" d="M219.08,-288.21C211,-285.04 201.93,-281.48 193.86,-278.31"/>
</g>
<!-- 4&#45;&#45;10 -->
<g id="edge37" class="edge">
<title>4&#45;&#45;10</title>
<path fill="none" stroke="black" d="M239.93,-285.11C237.94,-272.58 234.85,-253.05 232.84,-240.4"/>
</g>
<!-- 5&#45;&#45;6 -->
<g id="edge38" class="edge">
<title>5&#45;&#45;6</title>
<path fill="none" stroke="black" d="M164.49,-224.06C165.93,-233.82 167.92,-247.45 169.36,-257.21"/>
</g>
<!-- 5&#45;&#45;10 -->
<g id="edge39" class="edge">
<title>5&#45;&#45;10</title>
<path fill="none" stroke="black" d="M190.83,-218.53C194.06,-219.3 197.36,-220.08 200.61,-220.86"/>
</g>
<!-- 16 -->
<g id="node16" class="node">
<title>16</title>
<ellipse fill="#8eff70" fill-opacity="0.498039" stroke="black" cx="37.48" cy="-262.02" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="37.48" y="-260.12" font-family="Times,serif" font-size="8.00">16 : 0.33</text>
</g>
<!-- 5&#45;&#45;16 -->
<g id="edge40" class="edge">
<title>5&#45;&#45;16</title>
<path fill="none" stroke="black" d="M140.31,-220.83C117.94,-229.79 83.69,-243.51 60.95,-252.62"/>
</g>
<!-- 6&#45;&#45;16 -->
<g id="edge41" class="edge">
<title>6&#45;&#45;16</title>
<path fill="none" stroke="black" d="M138.1,-267.57C118.93,-266.51 94.72,-265.18 74.85,-264.08"/>
</g>
<!-- 8&#45;&#45;30 -->
<g id="edge42" class="edge">
<title>8&#45;&#45;30</title>
<path fill="none" stroke="black" d="M614.71,-178.76C622.51,-165.53 635.06,-144.24 642.92,-130.9"/>
</g>
<!-- 8&#45;&#45;32 -->
<g id="edge43" class="edge">
<title>8&#45;&#45;32</title>
<path fill="none" stroke="black" d="M639.44,-194.6C667.95,-198.18 709.73,-203.43 739.22,-207.14"/>
</g>
<!-- 33 -->
<g id="node33" class="node">
<title>33</title>
<ellipse fill="#0000c2" fill-opacity="0.498039" stroke="black" cx="748.47" cy="-189.08" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="748.47" y="-187.18" font-family="Times,serif" font-size="8.00">33 : 0.12</text>
</g>
<!-- 8&#45;&#45;33 -->
<g id="edge44" class="edge">
<title>8&#45;&#45;33</title>
<path fill="none" stroke="black" d="M641.08,-190.26C662.05,-190.03 689.15,-189.73 710.89,-189.49"/>
</g>
<!-- 9&#45;&#45;33 -->
<g id="edge45" class="edge">
<title>9&#45;&#45;33</title>
<path fill="none" stroke="black" d="M694.96,-54.82C705.89,-82.24 732.64,-149.37 743.63,-176.95"/>
</g>
<!-- 13&#45;&#45;33 -->
<g id="edge46" class="edge">
<title>13&#45;&#45;33</title>
<path fill="none" stroke="black" d="M558.98,-164.13C601.17,-169.69 671.54,-178.95 713.62,-184.5"/>
</g>
<!-- 14 -->
<g id="node14" class="node">
<title>14</title>
<ellipse fill="#7f0000" fill-opacity="0.498039" stroke="black" cx="904.9" cy="-160.94" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="904.9" y="-159.04" font-family="Times,serif" font-size="8.00">14 : 0.56</text>
</g>
<!-- 14&#45;&#45;32 -->
<g id="edge47" class="edge">
<title>14&#45;&#45;32</title>
<path fill="none" stroke="black" d="M880.94,-170.22C857.52,-179.29 822.04,-193.03 798.52,-202.15"/>
</g>
<!-- 14&#45;&#45;33 -->
<g id="edge48" class="edge">
<title>14&#45;&#45;33</title>
<path fill="none" stroke="black" d="M872.11,-166.84C845.41,-171.64 807.9,-178.39 781.22,-183.19"/>
</g>
<!-- 15 -->
<g id="node15" class="node">
<title>15</title>
<ellipse fill="#7f0000" fill-opacity="0.498039" stroke="black" cx="867.61" cy="-312.99" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="867.61" y="-311.09" font-family="Times,serif" font-size="8.00">15 : 0.56</text>
</g>
<!-- 15&#45;&#45;32 -->
<g id="edge49" class="edge">
<title>15&#45;&#45;32</title>
<path fill="none" stroke="black" d="M856.91,-301.35C839,-281.89 803.16,-242.94 785.14,-223.35"/>
</g>
<!-- 15&#45;&#45;33 -->
<g id="edge50" class="edge">
<title>15&#45;&#45;33</title>
<path fill="none" stroke="black" d="M856.19,-301.11C833.36,-277.36 782.34,-224.31 759.68,-200.75"/>
</g>
<!-- 18 -->
<g id="node18" class="node">
<title>18</title>
<ellipse fill="#7f0000" fill-opacity="0.498039" stroke="black" cx="897.82" cy="-267.1" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="897.82" y="-265.2" font-family="Times,serif" font-size="8.00">18 : 0.56</text>
</g>
<!-- 18&#45;&#45;32 -->
<g id="edge51" class="edge">
<title>18&#45;&#45;32</title>
<path fill="none" stroke="black" d="M876.02,-257.3C853.57,-247.2 818.79,-231.56 796.28,-221.43"/>
</g>
<!-- 18&#45;&#45;33 -->
<g id="edge52" class="edge">
<title>18&#45;&#45;33</title>
<path fill="none" stroke="black" d="M877.99,-256.74C849.42,-241.82 796.99,-214.43 768.38,-199.49"/>
</g>
<!-- 19&#45;&#45;33 -->
<g id="edge53" class="edge">
<title>19&#45;&#45;33</title>
<path fill="none" stroke="black" d="M566.96,-216.08C607.35,-210.08 674.13,-200.14 714.45,-194.14"/>
</g>
<!-- 20 -->
<g id="node20" class="node">
<title>20</title>
<ellipse fill="#7f0000" fill-opacity="0.498039" stroke="black" cx="913.9" cy="-213.42" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="913.9" y="-211.52" font-family="Times,serif" font-size="8.00">20 : 0.56</text>
</g>
<!-- 20&#45;&#45;32 -->
<g id="edge54" class="edge">
<title>20&#45;&#45;32</title>
<path fill="none" stroke="black" d="M876.12,-212.91C856.25,-212.64 831.96,-212.31 812.09,-212.05"/>
</g>
<!-- 20&#45;&#45;33 -->
<g id="edge55" class="edge">
<title>20&#45;&#45;33</title>
<path fill="none" stroke="black" d="M879.62,-208.37C851.23,-204.2 811.08,-198.29 782.7,-194.12"/>
</g>
<!-- 22 -->
<g id="node22" class="node">
<title>22</title>
<ellipse fill="#7f0000" fill-opacity="0.498039" stroke="black" cx="819.15" cy="-343.87" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="819.15" y="-341.97" font-family="Times,serif" font-size="8.00">22 : 0.56</text>
</g>
<!-- 22&#45;&#45;32 -->
<g id="edge56" class="edge">
<title>22&#45;&#45;32</title>
<path fill="none" stroke="black" d="M815.01,-331.66C806.46,-306.45 786.95,-248.91 778.41,-223.72"/>
</g>
<!-- 22&#45;&#45;33 -->
<g id="edge57" class="edge">
<title>22&#45;&#45;33</title>
<path fill="none" stroke="black" d="M813.6,-331.72C800.39,-302.79 767.17,-230.05 753.99,-201.17"/>
</g>
<!-- 23 -->
<g id="node23" class="node">
<title>23</title>
<ellipse fill="#0012ff" fill-opacity="0.498039" stroke="black" cx="827.06" cy="-178.08" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="827.06" y="-176.18" font-family="Times,serif" font-size="8.00">23 : 0.15</text>
</g>
<!-- 25 -->
<g id="node25" class="node">
<title>25</title>
<ellipse fill="#003aff" fill-opacity="0.498039" stroke="black" cx="781.42" cy="-289.06" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="781.42" y="-287.16" font-family="Times,serif" font-size="8.00">25 : 0.17</text>
</g>
<!-- 23&#45;&#45;25 -->
<g id="edge58" class="edge">
<title>23&#45;&#45;25</title>
<path fill="none" stroke="black" d="M822,-190.38C813.18,-211.82 795.1,-255.79 786.36,-277.04"/>
</g>
<!-- 23&#45;&#45;27 -->
<g id="edge59" class="edge">
<title>23&#45;&#45;27</title>
<path fill="none" stroke="black" d="M791.04,-181.6C775.26,-183.14 756.78,-184.94 740.98,-186.49"/>
</g>
<!-- 29 -->
<g id="node29" class="node">
<title>29</title>
<ellipse fill="#0055ff" fill-opacity="0.498039" stroke="black" cx="854.49" cy="-103.13" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="854.49" y="-101.23" font-family="Times,serif" font-size="8.00">29 : 0.18</text>
</g>
<!-- 23&#45;&#45;29 -->
<g id="edge60" class="edge">
<title>23&#45;&#45;29</title>
<path fill="none" stroke="black" d="M831.46,-166.04C836.55,-152.13 844.88,-129.39 850,-115.39"/>
</g>
<!-- 23&#45;&#45;32 -->
<g id="edge61" class="edge">
<title>23&#45;&#45;32</title>
<path fill="none" stroke="black" d="M810.07,-188.85C804.04,-192.67 797.26,-196.97 791.23,-200.79"/>
</g>
<!-- 23&#45;&#45;33 -->
<g id="edge62" class="edge">
<title>23&#45;&#45;33</title>
<path fill="none" stroke="black" d="M792.36,-182.94C789.23,-183.37 786.07,-183.82 782.94,-184.26"/>
</g>
<!-- 24 -->
<g id="node24" class="node">
<title>24</title>
<ellipse fill="#0000ca" fill-opacity="0.498039" stroke="black" cx="715.66" cy="-341.64" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="715.66" y="-339.74" font-family="Times,serif" font-size="8.00">24 : 0.12</text>
</g>
<!-- 24&#45;&#45;25 -->
<g id="edge63" class="edge">
<title>24&#45;&#45;25</title>
<path fill="none" stroke="black" d="M729.93,-330.23C740.96,-321.41 756.15,-309.26 767.18,-300.44"/>
</g>
<!-- 24&#45;&#45;27 -->
<g id="edge64" class="edge">
<title>24&#45;&#45;27</title>
<path fill="none" stroke="black" d="M714.79,-329.48C712.76,-301.1 707.71,-230.68 705.67,-202.23"/>
</g>
<!-- 24&#45;&#45;31 -->
<g id="edge65" class="edge">
<title>24&#45;&#45;31</title>
<path fill="none" stroke="black" d="M704.34,-330.12C688.87,-314.4 661.28,-286.35 645.73,-270.53"/>
</g>
<!-- 25&#45;&#45;31 -->
<g id="edge66" class="edge">
<title>25&#45;&#45;31</title>
<path fill="none" stroke="black" d="M749.84,-282.58C725.1,-277.5 690.81,-270.46 666.01,-265.37"/>
</g>
<!-- 26 -->
<g id="node26" class="node">
<title>26</title>
<ellipse fill="#0000dc" fill-opacity="0.498039" stroke="black" cx="893.82" cy="-46.86" rx="37.45" ry="12.04"/>
<text text-anchor="middle" x="893.82" y="-44.96" font-family="Times,serif" font-size="8.00">26 : 0.13</text>
</g>
<!-- 26&#45;&#45;29 -->
<g id="edge67" class="edge">
<title>26&#45;&#45;29</title>
<path fill="none" stroke="black" d="M885.48,-58.8C878.81,-68.34 869.49,-81.67 862.82,-91.21"/>
</g>
<!-- 26&#45;&#45;33 -->
<g id="edge68" class="edge">
<title>26&#45;&#45;33</title>
<path fill="none" stroke="black" d="M881.93,-58.5C854.67,-85.17 787.94,-150.46 760.53,-177.28"/>
</g>
<!-- 27&#45;&#45;33 -->
<g id="edge69" class="edge">
<title>27&#45;&#45;33</title>
<path fill="none" stroke="black" d="M742.39,-189.21C742.51,-189.21 742.62,-189.21 742.73,-189.21"/>
</g>
<!-- 28&#45;&#45;31 -->
<g id="edge70" class="edge">
<title>28&#45;&#45;31</title>
<path fill="none" stroke="black" d="M637.07,-284.81C636.59,-280.38 636.04,-275.35 635.56,-270.92"/>
</g>
<!-- 28&#45;&#45;33 -->
<g id="edge71" class="edge">
<title>28&#45;&#45;33</title>
<path fill="none" stroke="black" d="M650.17,-285.3C671.35,-264.56 715.64,-221.21 736.77,-200.53"/>
</g>
<!-- 29&#45;&#45;32 -->
<g id="edge72" class="edge">
<title>29&#45;&#45;32</title>
<path fill="none" stroke="black" d="M845.6,-115.14C830.1,-136.09 798.32,-179.04 782.96,-199.8"/>
</g>
<!-- 29&#45;&#45;33 -->
<g id="edge73" class="edge">
<title>29&#45;&#45;33</title>
<path fill="none" stroke="black" d="M840.63,-114.37C820.31,-130.84 782.65,-161.37 762.33,-177.84"/>
</g>
<!-- 30&#45;&#45;32 -->
<g id="edge74" class="edge">
<title>30&#45;&#45;32</title>
<path fill="none" stroke="black" d="M665.29,-130.16C689.26,-148.05 735.5,-182.58 759.31,-200.36"/>
</g>
<!-- 30&#45;&#45;33 -->
<g id="edge75" class="edge">
<title>30&#45;&#45;33</title>
<path fill="none" stroke="black" d="M665.44,-129.77C683.81,-142.9 714.4,-164.75 732.87,-177.94"/>
</g>
<!-- 31&#45;&#45;32 -->
<g id="edge76" class="edge">
<title>31&#45;&#45;32</title>
<path fill="none" stroke="black" d="M660.25,-250.06C685.4,-241.56 723.3,-228.76 748.4,-220.28"/>
</g>
<!-- 31&#45;&#45;33 -->
<g id="edge77" class="edge">
<title>31&#45;&#45;33</title>
<path fill="none" stroke="black" d="M652.09,-247.95C673.55,-234.84 709.34,-212.98 730.74,-199.91"/>
</g>
<!-- 32&#45;&#45;33 -->
<g id="edge78" class="edge">
<title>32&#45;&#45;33</title>
<path fill="none" stroke="black" d="M761.07,-200.05C760.97,-199.96 760.87,-199.88 760.77,-199.79"/>
</g>
</g>
</svg>
</div>

Which centrality is the "best"?  Well there is no answer to that,
they are all tools in the analyst toolbox.

## High Performance Parallel Breadth First Search (BFS)

BFS is the core operation of graph alalytics.  Like a pepple thrown
into a pond, you start from a point in the graph, and traverse all
the edges you find, as you find them.  As you traverse the graphs
you can accumulate some interesting information as you go, like how
many edges away from the starting point you are, the "level" and
the row index of the node you got here from, the "parent".  In the
GraphBLAS, this information is accumulated in vectors.

## Level BFS
Level BFS exposes the depth of each vertex starting from a
given source vertex using the breadth-first search algorithm.
See [https://en.wikipedia.org/wiki/Breadth-first_search](https://en.wikipedia.org/wiki/Breadth-first_search) for details.

``` postgres-console
select draw(triu(graph), (select level from bfs(graph, 1)), false, false, true, 0.5) as draw_source from karate \gset
```
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="772pt" height="344pt"
 viewBox="0.00 0.00 772.28 343.60" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 339.6)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-339.6 768.28,-339.6 768.28,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="#00d4ff" fill-opacity="0.498039" stroke="black" cx="446.83" cy="-174.24" rx="25" ry="12.04"/>
<text text-anchor="middle" x="446.83" y="-172.34" font-family="Times,serif" font-size="8.00">0 : 1</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="#00007f" fill-opacity="0.498039" stroke="black" cx="390.55" cy="-168.07" rx="25" ry="12.04"/>
<text text-anchor="middle" x="390.55" y="-166.17" font-family="Times,serif" font-size="8.00">1 : 0</text>
</g>
<!-- 0&#45;&#45;1 -->
<g id="edge1" class="edge">
<title>0&#45;&#45;1</title>
<path fill="none" stroke="black" d="M422.64,-171.59C420.02,-171.3 417.35,-171.01 414.72,-170.72"/>
</g>
<!-- 2 -->
<g id="node3" class="node">
<title>2</title>
<ellipse fill="#00d4ff" fill-opacity="0.498039" stroke="black" cx="295.52" cy="-134.94" rx="25" ry="12.04"/>
<text text-anchor="middle" x="295.52" y="-133.04" font-family="Times,serif" font-size="8.00">2 : 1</text>
</g>
<!-- 0&#45;&#45;2 -->
<g id="edge2" class="edge">
<title>0&#45;&#45;2</title>
<path fill="none" stroke="black" d="M424.83,-168.53C396.14,-161.07 346.25,-148.11 317.55,-140.66"/>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="#00d4ff" fill-opacity="0.498039" stroke="black" cx="412.16" cy="-105.4" rx="25" ry="12.04"/>
<text text-anchor="middle" x="412.16" y="-103.5" font-family="Times,serif" font-size="8.00">3 : 1</text>
</g>
<!-- 0&#45;&#45;3 -->
<g id="edge3" class="edge">
<title>0&#45;&#45;3</title>
<path fill="none" stroke="black" d="M440.95,-162.56C434.62,-149.98 424.58,-130.06 418.19,-117.37"/>
</g>
<!-- 4 -->
<g id="node5" class="node">
<title>4</title>
<ellipse fill="#ffd400" fill-opacity="0.498039" stroke="black" cx="575.37" cy="-227.66" rx="25" ry="12.04"/>
<text text-anchor="middle" x="575.37" y="-225.76" font-family="Times,serif" font-size="8.00">4 : 2</text>
</g>
<!-- 0&#45;&#45;4 -->
<g id="edge4" class="edge">
<title>0&#45;&#45;4</title>
<path fill="none" stroke="black" d="M465.8,-182.12C490.1,-192.22 531.99,-209.63 556.33,-219.75"/>
</g>
<!-- 5 -->
<g id="node6" class="node">
<title>5</title>
<ellipse fill="#ffd400" fill-opacity="0.498039" stroke="black" cx="627.57" cy="-219.65" rx="25" ry="12.04"/>
<text text-anchor="middle" x="627.57" y="-217.75" font-family="Times,serif" font-size="8.00">5 : 2</text>
</g>
<!-- 0&#45;&#45;5 -->
<g id="edge5" class="edge">
<title>0&#45;&#45;5</title>
<path fill="none" stroke="black" d="M469,-179.81C503.78,-188.55 570.79,-205.39 605.51,-214.11"/>
</g>
<!-- 6 -->
<g id="node7" class="node">
<title>6</title>
<ellipse fill="#ffd400" fill-opacity="0.498039" stroke="black" cx="632.23" cy="-183" rx="25" ry="12.04"/>
<text text-anchor="middle" x="632.23" y="-181.1" font-family="Times,serif" font-size="8.00">6 : 2</text>
</g>
<!-- 0&#45;&#45;6 -->
<g id="edge6" class="edge">
<title>0&#45;&#45;6</title>
<path fill="none" stroke="black" d="M471.84,-175.42C507.32,-177.1 571.9,-180.15 607.33,-181.83"/>
</g>
<!-- 7 -->
<g id="node8" class="node">
<title>7</title>
<ellipse fill="#00d4ff" fill-opacity="0.498039" stroke="black" cx="388.87" cy="-80.54" rx="25" ry="12.04"/>
<text text-anchor="middle" x="388.87" y="-78.64" font-family="Times,serif" font-size="8.00">7 : 1</text>
</g>
<!-- 0&#45;&#45;7 -->
<g id="edge7" class="edge">
<title>0&#45;&#45;7</title>
<path fill="none" stroke="black" d="M439.49,-162.37C428.35,-144.35 407.27,-110.27 396.16,-92.31"/>
</g>
<!-- 8 -->
<g id="node9" class="node">
<title>8</title>
<ellipse fill="#ffd400" fill-opacity="0.498039" stroke="black" cx="281.14" cy="-210.33" rx="25" ry="12.04"/>
<text text-anchor="middle" x="281.14" y="-208.43" font-family="Times,serif" font-size="8.00">8 : 2</text>
</g>
<!-- 0&#45;&#45;8 -->
<g id="edge8" class="edge">
<title>0&#45;&#45;8</title>
<path fill="none" stroke="black" d="M424.14,-179.18C392.5,-186.08 335.46,-198.5 303.83,-205.39"/>
</g>
<!-- 10 -->
<g id="node11" class="node">
<title>10</title>
<ellipse fill="#ffd400" fill-opacity="0.498039" stroke="black" cx="583.91" cy="-172.06" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="583.91" y="-170.16" font-family="Times,serif" font-size="8.00">10 : 2</text>
</g>
<!-- 0&#45;&#45;10 -->
<g id="edge9" class="edge">
<title>0&#45;&#45;10</title>
<path fill="none" stroke="black" d="M471.66,-173.85C495.27,-173.47 530.75,-172.9 555.48,-172.51"/>
</g>
<!-- 11 -->
<g id="node12" class="node">
<title>11</title>
<ellipse fill="#ffd400" fill-opacity="0.498039" stroke="black" cx="518.46" cy="-306.09" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="518.46" y="-304.19" font-family="Times,serif" font-size="8.00">11 : 2</text>
</g>
<!-- 0&#45;&#45;11 -->
<g id="edge10" class="edge">
<title>0&#45;&#45;11</title>
<path fill="none" stroke="black" d="M453.19,-185.94C466.75,-210.9 498.38,-269.12 512.01,-294.23"/>
</g>
<!-- 12 -->
<g id="node13" class="node">
<title>12</title>
<ellipse fill="#ffd400" fill-opacity="0.498039" stroke="black" cx="497.86" cy="-49.73" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="497.86" y="-47.83" font-family="Times,serif" font-size="8.00">12 : 2</text>
</g>
<!-- 0&#45;&#45;12 -->
<g id="edge11" class="edge">
<title>0&#45;&#45;12</title>
<path fill="none" stroke="black" d="M451.73,-162.3C461.45,-138.56 483.13,-85.67 492.9,-61.82"/>
</g>
<!-- 13 -->
<g id="node14" class="node">
<title>13</title>
<ellipse fill="#00d4ff" fill-opacity="0.498039" stroke="black" cx="332.57" cy="-163.24" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="332.57" y="-161.34" font-family="Times,serif" font-size="8.00">13 : 1</text>
</g>
<!-- 0&#45;&#45;13 -->
<g id="edge12" class="edge">
<title>0&#45;&#45;13</title>
<path fill="none" stroke="black" d="M422.32,-171.88C404.09,-170.13 379.2,-167.73 360.25,-165.91"/>
</g>
<!-- 17 -->
<g id="node18" class="node">
<title>17</title>
<ellipse fill="#00d4ff" fill-opacity="0.498039" stroke="black" cx="503.11" cy="-180.89" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="503.11" y="-178.99" font-family="Times,serif" font-size="8.00">17 : 1</text>
</g>
<!-- 0&#45;&#45;17 -->
<g id="edge13" class="edge">
<title>0&#45;&#45;17</title>
<path fill="none" stroke="black" d="M471.03,-177.1C472.58,-177.28 474.14,-177.47 475.7,-177.65"/>
</g>
<!-- 19 -->
<g id="node20" class="node">
<title>19</title>
<ellipse fill="#00d4ff" fill-opacity="0.498039" stroke="black" cx="332.57" cy="-235.1" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="332.57" y="-233.2" font-family="Times,serif" font-size="8.00">19 : 1</text>
</g>
<!-- 0&#45;&#45;19 -->
<g id="edge14" class="edge">
<title>0&#45;&#45;19</title>
<path fill="none" stroke="black" d="M429.98,-183.22C408.62,-194.6 371.99,-214.11 350.25,-225.69"/>
</g>
<!-- 21 -->
<g id="node22" class="node">
<title>21</title>
<ellipse fill="#00d4ff" fill-opacity="0.498039" stroke="black" cx="499.1" cy="-112.02" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="499.1" y="-110.12" font-family="Times,serif" font-size="8.00">21 : 1</text>
</g>
<!-- 0&#45;&#45;21 -->
<g id="edge15" class="edge">
<title>0&#45;&#45;21</title>
<path fill="none" stroke="black" d="M456.42,-162.83C465.85,-151.6 480.17,-134.56 489.58,-123.36"/>
</g>
<!-- 31 -->
<g id="node32" class="node">
<title>31</title>
<ellipse fill="#ffd400" fill-opacity="0.498039" stroke="black" cx="225.53" cy="-121" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="225.53" y="-119.1" font-family="Times,serif" font-size="8.00">31 : 2</text>
</g>
<!-- 0&#45;&#45;31 -->
<g id="edge16" class="edge">
<title>0&#45;&#45;31</title>
<path fill="none" stroke="black" d="M424.4,-168.84C382.75,-158.82 293.79,-137.42 250.38,-126.98"/>
</g>
<!-- 1&#45;&#45;2 -->
<g id="edge17" class="edge">
<title>1&#45;&#45;2</title>
<path fill="none" stroke="black" d="M370.39,-161.05C354.27,-155.42 331.76,-147.57 315.64,-141.95"/>
</g>
<!-- 1&#45;&#45;3 -->
<g id="edge18" class="edge">
<title>1&#45;&#45;3</title>
<path fill="none" stroke="black" d="M394.71,-155.99C398.54,-144.89 404.18,-128.54 408.01,-117.45"/>
</g>
<!-- 1&#45;&#45;7 -->
<g id="edge19" class="edge">
<title>1&#45;&#45;7</title>
<path fill="none" stroke="black" d="M390.31,-155.9C390,-139.18 389.42,-109.27 389.11,-92.6"/>
</g>
<!-- 1&#45;&#45;13 -->
<g id="edge20" class="edge">
<title>1&#45;&#45;13</title>
<path fill="none" stroke="black" d="M365.96,-166.03C364.11,-165.87 362.23,-165.71 360.36,-165.56"/>
</g>
<!-- 1&#45;&#45;17 -->
<g id="edge21" class="edge">
<title>1&#45;&#45;17</title>
<path fill="none" stroke="black" d="M414.69,-170.82C432.52,-172.85 456.82,-175.62 475.44,-177.74"/>
</g>
<!-- 1&#45;&#45;19 -->
<g id="edge22" class="edge">
<title>1&#45;&#45;19</title>
<path fill="none" stroke="black" d="M380.71,-179.44C370.12,-191.69 353.34,-211.09 342.65,-223.45"/>
</g>
<!-- 1&#45;&#45;21 -->
<g id="edge23" class="edge">
<title>1&#45;&#45;21</title>
<path fill="none" stroke="black" d="M407.51,-159.31C427.43,-149.03 460.34,-132.04 480.81,-121.46"/>
</g>
<!-- 30 -->
<g id="node31" class="node">
<title>30</title>
<ellipse fill="#00d4ff" fill-opacity="0.498039" stroke="black" cx="251.23" cy="-236.38" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="251.23" y="-234.48" font-family="Times,serif" font-size="8.00">30 : 1</text>
</g>
<!-- 1&#45;&#45;30 -->
<g id="edge24" class="edge">
<title>1&#45;&#45;30</title>
<path fill="none" stroke="black" d="M372.9,-176.72C346.54,-189.65 297.1,-213.89 270.06,-227.15"/>
</g>
<!-- 2&#45;&#45;3 -->
<g id="edge25" class="edge">
<title>2&#45;&#45;3</title>
<path fill="none" stroke="black" d="M317.73,-129.31C338.49,-124.06 369.3,-116.26 390.03,-111.01"/>
</g>
<!-- 2&#45;&#45;7 -->
<g id="edge26" class="edge">
<title>2&#45;&#45;7</title>
<path fill="none" stroke="black" d="M311.78,-125.46C328.92,-115.47 355.74,-99.85 372.8,-89.9"/>
</g>
<!-- 2&#45;&#45;8 -->
<g id="edge27" class="edge">
<title>2&#45;&#45;8</title>
<path fill="none" stroke="black" d="M293.21,-147.05C290.54,-161.03 286.18,-183.92 283.5,-197.99"/>
</g>
<!-- 9 -->
<g id="node10" class="node">
<title>9</title>
<ellipse fill="#ffd400" fill-opacity="0.498039" stroke="black" cx="215.87" cy="-68.55" rx="25" ry="12.04"/>
<text text-anchor="middle" x="215.87" y="-66.65" font-family="Times,serif" font-size="8.00">9 : 2</text>
</g>
<!-- 2&#45;&#45;9 -->
<g id="edge28" class="edge">
<title>2&#45;&#45;9</title>
<path fill="none" stroke="black" d="M282.72,-124.27C267.77,-111.81 243.19,-91.33 228.37,-78.97"/>
</g>
<!-- 2&#45;&#45;13 -->
<g id="edge29" class="edge">
<title>2&#45;&#45;13</title>
<path fill="none" stroke="black" d="M308.9,-145.16C312.05,-147.56 315.41,-150.13 318.59,-152.56"/>
</g>
<!-- 27 -->
<g id="node28" class="node">
<title>27</title>
<ellipse fill="#ffd400" fill-opacity="0.498039" stroke="black" cx="160.63" cy="-90.96" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="160.63" y="-89.06" font-family="Times,serif" font-size="8.00">27 : 2</text>
</g>
<!-- 2&#45;&#45;27 -->
<g id="edge30" class="edge">
<title>2&#45;&#45;27</title>
<path fill="none" stroke="black" d="M274.74,-128.16C250.06,-120.11 209.02,-106.74 183.46,-98.41"/>
</g>
<!-- 28 -->
<g id="node29" class="node">
<title>28</title>
<ellipse fill="#ffd400" fill-opacity="0.498039" stroke="black" cx="215.26" cy="-168.58" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="215.26" y="-166.68" font-family="Times,serif" font-size="8.00">28 : 2</text>
</g>
<!-- 2&#45;&#45;28 -->
<g id="edge31" class="edge">
<title>2&#45;&#45;28</title>
<path fill="none" stroke="black" d="M276.5,-142.91C264.17,-148.08 248.12,-154.81 235.54,-160.08"/>
</g>
<!-- 32 -->
<g id="node33" class="node">
<title>32</title>
<ellipse fill="#ffd400" fill-opacity="0.498039" stroke="black" cx="148.89" cy="-226.97" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="148.89" y="-225.07" font-family="Times,serif" font-size="8.00">32 : 2</text>
</g>
<!-- 2&#45;&#45;32 -->
<g id="edge32" class="edge">
<title>2&#45;&#45;32</title>
<path fill="none" stroke="black" d="M280.11,-144.61C252.16,-162.15 193.42,-199.02 164.98,-216.87"/>
</g>
<!-- 3&#45;&#45;7 -->
<g id="edge33" class="edge">
<title>3&#45;&#45;7</title>
<path fill="none" stroke="black" d="M401.88,-94.42C401,-93.49 400.11,-92.54 399.24,-91.6"/>
</g>
<!-- 3&#45;&#45;12 -->
<g id="edge34" class="edge">
<title>3&#45;&#45;12</title>
<path fill="none" stroke="black" d="M427.09,-95.71C442.53,-85.67 466.54,-70.08 482.23,-59.88"/>
</g>
<!-- 3&#45;&#45;13 -->
<g id="edge35" class="edge">
<title>3&#45;&#45;13</title>
<path fill="none" stroke="black" d="M398.3,-115.48C383.96,-125.9 361.65,-142.11 347.08,-152.7"/>
</g>
<!-- 4&#45;&#45;6 -->
<g id="edge36" class="edge">
<title>4&#45;&#45;6</title>
<path fill="none" stroke="black" d="M588.56,-217.3C597.75,-210.09 609.95,-200.51 619.12,-193.3"/>
</g>
<!-- 4&#45;&#45;10 -->
<g id="edge37" class="edge">
<title>4&#45;&#45;10</title>
<path fill="none" stroke="black" d="M577.22,-215.59C578.66,-206.27 580.63,-193.42 582.06,-184.1"/>
</g>
<!-- 5&#45;&#45;6 -->
<g id="edge38" class="edge">
<title>5&#45;&#45;6</title>
<path fill="none" stroke="black" d="M629.09,-207.65C629.6,-203.64 630.17,-199.18 630.68,-195.16"/>
</g>
<!-- 5&#45;&#45;10 -->
<g id="edge39" class="edge">
<title>5&#45;&#45;10</title>
<path fill="none" stroke="black" d="M617.44,-208.61C610.49,-201.03 601.29,-191 594.28,-183.37"/>
</g>
<!-- 16 -->
<g id="node17" class="node">
<title>16</title>
<ellipse fill="#7f0000" fill-opacity="0.498039" stroke="black" cx="735.99" cy="-218" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="735.99" y="-216.1" font-family="Times,serif" font-size="8.00">16 : 3</text>
</g>
<!-- 5&#45;&#45;16 -->
<g id="edge40" class="edge">
<title>5&#45;&#45;16</title>
<path fill="none" stroke="black" d="M652.44,-219.27C668.82,-219.02 690.32,-218.7 707.4,-218.44"/>
</g>
<!-- 6&#45;&#45;16 -->
<g id="edge41" class="edge">
<title>6&#45;&#45;16</title>
<path fill="none" stroke="black" d="M652.73,-189.92C670.33,-195.85 695.65,-204.39 713.74,-210.49"/>
</g>
<!-- 8&#45;&#45;30 -->
<g id="edge42" class="edge">
<title>8&#45;&#45;30</title>
<path fill="none" stroke="black" d="M268.98,-220.93C267.29,-222.39 265.56,-223.9 263.87,-225.37"/>
</g>
<!-- 8&#45;&#45;32 -->
<g id="edge43" class="edge">
<title>8&#45;&#45;32</title>
<path fill="none" stroke="black" d="M256.89,-213.38C234.07,-216.25 199.95,-220.55 176.18,-223.54"/>
</g>
<!-- 33 -->
<g id="node34" class="node">
<title>33</title>
<ellipse fill="#ffd400" fill-opacity="0.498039" stroke="black" cx="161.55" cy="-198.92" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="161.55" y="-197.02" font-family="Times,serif" font-size="8.00">33 : 2</text>
</g>
<!-- 8&#45;&#45;33 -->
<g id="edge44" class="edge">
<title>8&#45;&#45;33</title>
<path fill="none" stroke="black" d="M256.65,-207.99C237.11,-206.13 209.72,-203.51 189.36,-201.57"/>
</g>
<!-- 9&#45;&#45;33 -->
<g id="edge45" class="edge">
<title>9&#45;&#45;33</title>
<path fill="none" stroke="black" d="M210.85,-80.58C200.51,-105.42 176.89,-162.1 166.55,-186.91"/>
</g>
<!-- 13&#45;&#45;33 -->
<g id="edge46" class="edge">
<title>13&#45;&#45;33</title>
<path fill="none" stroke="black" d="M306.96,-168.58C274.63,-175.33 219.33,-186.86 187.05,-193.6"/>
</g>
<!-- 14 -->
<g id="node15" class="node">
<title>14</title>
<ellipse fill="#7f0000" fill-opacity="0.498039" stroke="black" cx="48.75" cy="-270.41" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="48.75" y="-268.51" font-family="Times,serif" font-size="8.00">14 : 3</text>
</g>
<!-- 14&#45;&#45;32 -->
<g id="edge47" class="edge">
<title>14&#45;&#45;32</title>
<path fill="none" stroke="black" d="M69.01,-261.62C86.5,-254.04 111.61,-243.14 128.99,-235.6"/>
</g>
<!-- 14&#45;&#45;33 -->
<g id="edge48" class="edge">
<title>14&#45;&#45;33</title>
<path fill="none" stroke="black" d="M64.91,-260.17C86.47,-246.5 124.42,-222.45 145.78,-208.91"/>
</g>
<!-- 15 -->
<g id="node16" class="node">
<title>15</title>
<ellipse fill="#7f0000" fill-opacity="0.498039" stroke="black" cx="156.64" cy="-323.58" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="156.64" y="-321.68" font-family="Times,serif" font-size="8.00">15 : 3</text>
</g>
<!-- 15&#45;&#45;32 -->
<g id="edge49" class="edge">
<title>15&#45;&#45;32</title>
<path fill="none" stroke="black" d="M155.66,-311.35C154.16,-292.76 151.35,-257.63 149.86,-239.11"/>
</g>
<!-- 15&#45;&#45;33 -->
<g id="edge50" class="edge">
<title>15&#45;&#45;33</title>
<path fill="none" stroke="black" d="M157.13,-311.17C158.07,-287.28 160.12,-235.13 161.06,-211.27"/>
</g>
<!-- 18 -->
<g id="node19" class="node">
<title>18</title>
<ellipse fill="#7f0000" fill-opacity="0.498039" stroke="black" cx="76.54" cy="-299.56" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="76.54" y="-297.66" font-family="Times,serif" font-size="8.00">18 : 3</text>
</g>
<!-- 18&#45;&#45;32 -->
<g id="edge51" class="edge">
<title>18&#45;&#45;32</title>
<path fill="none" stroke="black" d="M87.85,-288.22C101.36,-274.66 123.84,-252.1 137.42,-238.48"/>
</g>
<!-- 18&#45;&#45;33 -->
<g id="edge52" class="edge">
<title>18&#45;&#45;33</title>
<path fill="none" stroke="black" d="M86.29,-288.02C102.69,-268.6 135.59,-229.65 151.91,-210.33"/>
</g>
<!-- 19&#45;&#45;33 -->
<g id="edge53" class="edge">
<title>19&#45;&#45;33</title>
<path fill="none" stroke="black" d="M306.97,-229.69C274.63,-222.84 219.33,-211.14 187.05,-204.31"/>
</g>
<!-- 20 -->
<g id="node21" class="node">
<title>20</title>
<ellipse fill="#7f0000" fill-opacity="0.498039" stroke="black" cx="111.02" cy="-322.37" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="111.02" y="-320.47" font-family="Times,serif" font-size="8.00">20 : 3</text>
</g>
<!-- 20&#45;&#45;32 -->
<g id="edge54" class="edge">
<title>20&#45;&#45;32</title>
<path fill="none" stroke="black" d="M115.82,-310.29C123.1,-291.94 136.87,-257.25 144.13,-238.96"/>
</g>
<!-- 20&#45;&#45;33 -->
<g id="edge55" class="edge">
<title>20&#45;&#45;33</title>
<path fill="none" stroke="black" d="M116.05,-310.08C125.78,-286.3 147.09,-234.25 156.69,-210.79"/>
</g>
<!-- 22 -->
<g id="node23" class="node">
<title>22</title>
<ellipse fill="#7f0000" fill-opacity="0.498039" stroke="black" cx="39.28" cy="-230.09" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="39.28" y="-228.19" font-family="Times,serif" font-size="8.00">22 : 3</text>
</g>
<!-- 22&#45;&#45;32 -->
<g id="edge56" class="edge">
<title>22&#45;&#45;32</title>
<path fill="none" stroke="black" d="M67.51,-229.29C83.74,-228.82 104.09,-228.25 120.37,-227.78"/>
</g>
<!-- 22&#45;&#45;33 -->
<g id="edge57" class="edge">
<title>22&#45;&#45;33</title>
<path fill="none" stroke="black" d="M63.73,-223.85C85.05,-218.42 115.83,-210.57 137.14,-205.14"/>
</g>
<!-- 23 -->
<g id="node24" class="node">
<title>23</title>
<ellipse fill="#7f0000" fill-opacity="0.498039" stroke="black" cx="88.99" cy="-139.31" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="88.99" y="-137.41" font-family="Times,serif" font-size="8.00">23 : 3</text>
</g>
<!-- 25 -->
<g id="node26" class="node">
<title>25</title>
<ellipse fill="#7f0000" fill-opacity="0.498039" stroke="black" cx="91.43" cy="-64.03" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="91.43" y="-62.13" font-family="Times,serif" font-size="8.00">25 : 3</text>
</g>
<!-- 23&#45;&#45;25 -->
<g id="edge58" class="edge">
<title>23&#45;&#45;25</title>
<path fill="none" stroke="black" d="M89.38,-127.22C89.84,-113.25 90.58,-90.4 91.03,-76.35"/>
</g>
<!-- 23&#45;&#45;27 -->
<g id="edge59" class="edge">
<title>23&#45;&#45;27</title>
<path fill="none" stroke="black" d="M104.18,-129.06C116.34,-120.85 133.3,-109.4 145.46,-101.2"/>
</g>
<!-- 29 -->
<g id="node30" class="node">
<title>29</title>
<ellipse fill="#7f0000" fill-opacity="0.498039" stroke="black" cx="45.61" cy="-173.76" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="45.61" y="-171.86" font-family="Times,serif" font-size="8.00">29 : 3</text>
</g>
<!-- 23&#45;&#45;29 -->
<g id="edge60" class="edge">
<title>23&#45;&#45;29</title>
<path fill="none" stroke="black" d="M75.26,-150.21C70.1,-154.31 64.24,-158.97 59.1,-163.05"/>
</g>
<!-- 23&#45;&#45;32 -->
<g id="edge61" class="edge">
<title>23&#45;&#45;32</title>
<path fill="none" stroke="black" d="M97.07,-151.13C108.56,-167.96 129.51,-198.62 140.94,-215.34"/>
</g>
<!-- 23&#45;&#45;33 -->
<g id="edge62" class="edge">
<title>23&#45;&#45;33</title>
<path fill="none" stroke="black" d="M102.3,-150.24C115.39,-161 135.26,-177.32 148.32,-188.05"/>
</g>
<!-- 24 -->
<g id="node25" class="node">
<title>24</title>
<ellipse fill="#7f0000" fill-opacity="0.498039" stroke="black" cx="136.85" cy="-12.02" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="136.85" y="-10.12" font-family="Times,serif" font-size="8.00">24 : 3</text>
</g>
<!-- 24&#45;&#45;25 -->
<g id="edge63" class="edge">
<title>24&#45;&#45;25</title>
<path fill="none" stroke="black" d="M127,-23.31C119.48,-31.91 109.17,-43.72 101.58,-52.41"/>
</g>
<!-- 24&#45;&#45;27 -->
<g id="edge64" class="edge">
<title>24&#45;&#45;27</title>
<path fill="none" stroke="black" d="M140.46,-24.01C144.93,-38.85 152.5,-63.97 156.98,-78.86"/>
</g>
<!-- 24&#45;&#45;31 -->
<g id="edge65" class="edge">
<title>24&#45;&#45;31</title>
<path fill="none" stroke="black" d="M146.34,-23.68C163.41,-44.66 199.09,-88.51 216.11,-109.42"/>
</g>
<!-- 25&#45;&#45;31 -->
<g id="edge66" class="edge">
<title>25&#45;&#45;31</title>
<path fill="none" stroke="black" d="M111.51,-72.56C136.79,-83.3 179.97,-101.65 205.32,-112.41"/>
</g>
<!-- 26 -->
<g id="node27" class="node">
<title>26</title>
<ellipse fill="#7f0000" fill-opacity="0.498039" stroke="black" cx="28.28" cy="-100.02" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="28.28" y="-98.12" font-family="Times,serif" font-size="8.00">26 : 3</text>
</g>
<!-- 26&#45;&#45;29 -->
<g id="edge67" class="edge">
<title>26&#45;&#45;29</title>
<path fill="none" stroke="black" d="M31.15,-112.19C34.34,-125.78 39.47,-147.63 42.69,-161.32"/>
</g>
<!-- 26&#45;&#45;33 -->
<g id="edge68" class="edge">
<title>26&#45;&#45;33</title>
<path fill="none" stroke="black" d="M42.55,-110.6C68.2,-129.64 121.81,-169.43 147.39,-188.41"/>
</g>
<!-- 27&#45;&#45;33 -->
<g id="edge69" class="edge">
<title>27&#45;&#45;33</title>
<path fill="none" stroke="black" d="M160.73,-103.35C160.91,-124.17 161.27,-165.95 161.44,-186.68"/>
</g>
<!-- 28&#45;&#45;31 -->
<g id="edge70" class="edge">
<title>28&#45;&#45;31</title>
<path fill="none" stroke="black" d="M217.85,-156.58C219.39,-149.42 221.35,-140.36 222.9,-133.17"/>
</g>
<!-- 28&#45;&#45;33 -->
<g id="edge71" class="edge">
<title>28&#45;&#45;33</title>
<path fill="none" stroke="black" d="M197.97,-178.35C191.83,-181.81 184.94,-185.71 178.8,-189.17"/>
</g>
<!-- 29&#45;&#45;32 -->
<g id="edge72" class="edge">
<title>29&#45;&#45;32</title>
<path fill="none" stroke="black" d="M64.08,-183.28C82.87,-192.96 111.84,-207.89 130.58,-217.54"/>
</g>
<!-- 29&#45;&#45;33 -->
<g id="edge73" class="edge">
<title>29&#45;&#45;33</title>
<path fill="none" stroke="black" d="M71.06,-179.28C90.39,-183.48 116.85,-189.22 136.17,-193.41"/>
</g>
<!-- 30&#45;&#45;32 -->
<g id="edge74" class="edge">
<title>30&#45;&#45;32</title>
<path fill="none" stroke="black" d="M223.53,-233.84C208.97,-232.5 191.17,-230.86 176.6,-229.52"/>
</g>
<!-- 30&#45;&#45;33 -->
<g id="edge75" class="edge">
<title>30&#45;&#45;33</title>
<path fill="none" stroke="black" d="M230.88,-227.88C216.2,-221.75 196.48,-213.51 181.82,-207.38"/>
</g>
<!-- 31&#45;&#45;32 -->
<g id="edge76" class="edge">
<title>31&#45;&#45;32</title>
<path fill="none" stroke="black" d="M217.03,-132.75C202.3,-153.12 172.18,-194.77 157.42,-215.17"/>
</g>
<!-- 31&#45;&#45;33 -->
<g id="edge77" class="edge">
<title>31&#45;&#45;33</title>
<path fill="none" stroke="black" d="M216.09,-132.5C203.99,-147.23 183.14,-172.62 171.03,-187.37"/>
</g>
<!-- 32&#45;&#45;33 -->
<g id="edge78" class="edge">
<title>32&#45;&#45;33</title>
<path fill="none" stroke="black" d="M154.33,-214.91C154.92,-213.6 155.52,-212.27 156.11,-210.97"/>
</g>
</g>
</svg>
</div>



## Parent BFS
Parent BFS returns the predecessor of each vertex in the
BFS tree rooted at the chosen source. It is also based on
[https://en.wikipedia.org/wiki/Breadth-first_search](https://en.wikipedia.org/wiki/Breadth-first_search).

``` postgres-console
select draw(triu(graph), (select parent from bfs(graph, 1)), false, false, true, 0.5) as draw_source from karate \gset
```
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="793pt" height="351pt"
 viewBox="0.00 0.00 792.94 351.14" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 347.14)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-347.14 788.94,-347.14 788.94,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="#00009e" fill-opacity="0.498039" stroke="black" cx="460.5" cy="-178.17" rx="25" ry="12.04"/>
<text text-anchor="middle" x="460.5" y="-176.27" font-family="Times,serif" font-size="8.00">0 : 1</text>
</g>
<!-- 1 -->
<g id="node2" class="node">
<title>1</title>
<ellipse fill="#00009e" fill-opacity="0.498039" stroke="black" cx="402.85" cy="-171.85" rx="25" ry="12.04"/>
<text text-anchor="middle" x="402.85" y="-169.95" font-family="Times,serif" font-size="8.00">1 : 1</text>
</g>
<!-- 0&#45;&#45;1 -->
<g id="edge1" class="edge">
<title>0&#45;&#45;1</title>
<path fill="none" stroke="black" d="M436.05,-175.49C433.1,-175.17 430.1,-174.84 427.15,-174.51"/>
</g>
<!-- 2 -->
<g id="node3" class="node">
<title>2</title>
<ellipse fill="#00009e" fill-opacity="0.498039" stroke="black" cx="305.52" cy="-137.91" rx="25" ry="12.04"/>
<text text-anchor="middle" x="305.52" y="-136.01" font-family="Times,serif" font-size="8.00">2 : 1</text>
</g>
<!-- 0&#45;&#45;2 -->
<g id="edge2" class="edge">
<title>0&#45;&#45;2</title>
<path fill="none" stroke="black" d="M438.62,-172.49C409.25,-164.86 357.36,-151.37 327.79,-143.69"/>
</g>
<!-- 3 -->
<g id="node4" class="node">
<title>3</title>
<ellipse fill="#00009e" fill-opacity="0.498039" stroke="black" cx="424.99" cy="-107.66" rx="25" ry="12.04"/>
<text text-anchor="middle" x="424.99" y="-105.76" font-family="Times,serif" font-size="8.00">3 : 1</text>
</g>
<!-- 0&#45;&#45;3 -->
<g id="edge3" class="edge">
<title>0&#45;&#45;3</title>
<path fill="none" stroke="black" d="M454.48,-166.21C447.91,-153.16 437.46,-132.42 430.92,-119.45"/>
</g>
<!-- 4 -->
<g id="node5" class="node">
<title>4</title>
<ellipse fill="#00007f" fill-opacity="0.498039" stroke="black" cx="592.15" cy="-232.88" rx="25" ry="12.04"/>
<text text-anchor="middle" x="592.15" y="-230.98" font-family="Times,serif" font-size="8.00">4 : 0</text>
</g>
<!-- 0&#45;&#45;4 -->
<g id="edge4" class="edge">
<title>0&#45;&#45;4</title>
<path fill="none" stroke="black" d="M479.36,-186.01C504.39,-196.41 548.31,-214.66 573.32,-225.05"/>
</g>
<!-- 5 -->
<g id="node6" class="node">
<title>5</title>
<ellipse fill="#00007f" fill-opacity="0.498039" stroke="black" cx="645.6" cy="-224.68" rx="25" ry="12.04"/>
<text text-anchor="middle" x="645.6" y="-222.78" font-family="Times,serif" font-size="8.00">5 : 0</text>
</g>
<!-- 0&#45;&#45;5 -->
<g id="edge5" class="edge">
<title>0&#45;&#45;5</title>
<path fill="none" stroke="black" d="M482.83,-183.78C518.5,-192.74 587.94,-210.19 623.49,-219.12"/>
</g>
<!-- 6 -->
<g id="node7" class="node">
<title>6</title>
<ellipse fill="#00007f" fill-opacity="0.498039" stroke="black" cx="650.38" cy="-187.14" rx="25" ry="12.04"/>
<text text-anchor="middle" x="650.38" y="-185.24" font-family="Times,serif" font-size="8.00">6 : 0</text>
</g>
<!-- 0&#45;&#45;6 -->
<g id="edge6" class="edge">
<title>0&#45;&#45;6</title>
<path fill="none" stroke="black" d="M485.32,-179.34C521.71,-181.06 589.16,-184.25 625.55,-185.97"/>
</g>
<!-- 7 -->
<g id="node8" class="node">
<title>7</title>
<ellipse fill="#00009e" fill-opacity="0.498039" stroke="black" cx="401.14" cy="-82.19" rx="25" ry="12.04"/>
<text text-anchor="middle" x="401.14" y="-80.29" font-family="Times,serif" font-size="8.00">7 : 1</text>
</g>
<!-- 0&#45;&#45;7 -->
<g id="edge7" class="edge">
<title>0&#45;&#45;7</title>
<path fill="none" stroke="black" d="M453.22,-166.4C441.77,-147.88 419.67,-112.15 408.29,-93.76"/>
</g>
<!-- 8 -->
<g id="node9" class="node">
<title>8</title>
<ellipse fill="#00007f" fill-opacity="0.498039" stroke="black" cx="290.8" cy="-215.13" rx="25" ry="12.04"/>
<text text-anchor="middle" x="290.8" y="-213.23" font-family="Times,serif" font-size="8.00">8 : 0</text>
</g>
<!-- 0&#45;&#45;8 -->
<g id="edge8" class="edge">
<title>0&#45;&#45;8</title>
<path fill="none" stroke="black" d="M437.61,-183.15C405.13,-190.23 346.02,-203.1 313.59,-210.16"/>
</g>
<!-- 10 -->
<g id="node11" class="node">
<title>10</title>
<ellipse fill="#00007f" fill-opacity="0.498039" stroke="black" cx="600.89" cy="-175.93" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="600.89" y="-174.03" font-family="Times,serif" font-size="8.00">10 : 0</text>
</g>
<!-- 0&#45;&#45;10 -->
<g id="edge9" class="edge">
<title>0&#45;&#45;10</title>
<path fill="none" stroke="black" d="M485.28,-177.77C509.68,-177.38 546.91,-176.79 572.5,-176.38"/>
</g>
<!-- 11 -->
<g id="node12" class="node">
<title>11</title>
<ellipse fill="#00007f" fill-opacity="0.498039" stroke="black" cx="533.86" cy="-313.21" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="533.86" y="-311.31" font-family="Times,serif" font-size="8.00">11 : 0</text>
</g>
<!-- 0&#45;&#45;11 -->
<g id="edge10" class="edge">
<title>0&#45;&#45;11</title>
<path fill="none" stroke="black" d="M467.01,-190.15C480.96,-215.84 513.62,-275.95 527.46,-301.44"/>
</g>
<!-- 12 -->
<g id="node13" class="node">
<title>12</title>
<ellipse fill="#00007f" fill-opacity="0.498039" stroke="black" cx="512.75" cy="-50.64" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="512.75" y="-48.74" font-family="Times,serif" font-size="8.00">12 : 0</text>
</g>
<!-- 0&#45;&#45;12 -->
<g id="edge11" class="edge">
<title>0&#45;&#45;12</title>
<path fill="none" stroke="black" d="M465.51,-165.94C475.52,-141.5 497.9,-86.9 507.84,-62.65"/>
</g>
<!-- 13 -->
<g id="node14" class="node">
<title>13</title>
<ellipse fill="#00009e" fill-opacity="0.498039" stroke="black" cx="343.47" cy="-166.9" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="343.47" y="-165" font-family="Times,serif" font-size="8.00">13 : 1</text>
</g>
<!-- 0&#45;&#45;13 -->
<g id="edge12" class="edge">
<title>0&#45;&#45;13</title>
<path fill="none" stroke="black" d="M435.97,-175.81C417.06,-173.99 390.92,-171.47 371.25,-169.58"/>
</g>
<!-- 17 -->
<g id="node18" class="node">
<title>17</title>
<ellipse fill="#00009e" fill-opacity="0.498039" stroke="black" cx="518.14" cy="-184.98" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="518.14" y="-183.08" font-family="Times,serif" font-size="8.00">17 : 1</text>
</g>
<!-- 0&#45;&#45;17 -->
<g id="edge13" class="edge">
<title>0&#45;&#45;17</title>
<path fill="none" stroke="black" d="M484.61,-181.02C486.58,-181.25 488.59,-181.49 490.58,-181.72"/>
</g>
<!-- 19 -->
<g id="node20" class="node">
<title>19</title>
<ellipse fill="#00009e" fill-opacity="0.498039" stroke="black" cx="343.47" cy="-240.5" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="343.47" y="-238.6" font-family="Times,serif" font-size="8.00">19 : 1</text>
</g>
<!-- 0&#45;&#45;19 -->
<g id="edge14" class="edge">
<title>0&#45;&#45;19</title>
<path fill="none" stroke="black" d="M443.73,-187.1C421.86,-198.75 383.77,-219.04 361.36,-230.97"/>
</g>
<!-- 21 -->
<g id="node22" class="node">
<title>21</title>
<ellipse fill="#00009e" fill-opacity="0.498039" stroke="black" cx="514.03" cy="-114.44" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="514.03" y="-112.54" font-family="Times,serif" font-size="8.00">21 : 1</text>
</g>
<!-- 0&#45;&#45;21 -->
<g id="edge15" class="edge">
<title>0&#45;&#45;21</title>
<path fill="none" stroke="black" d="M470.07,-166.77C479.69,-155.32 494.46,-137.74 504.18,-126.16"/>
</g>
<!-- 31 -->
<g id="node32" class="node">
<title>31</title>
<ellipse fill="#00007f" fill-opacity="0.498039" stroke="black" cx="233.84" cy="-123.64" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="233.84" y="-121.74" font-family="Times,serif" font-size="8.00">31 : 0</text>
</g>
<!-- 0&#45;&#45;31 -->
<g id="edge16" class="edge">
<title>0&#45;&#45;31</title>
<path fill="none" stroke="black" d="M437.94,-172.74C395.28,-162.48 303.09,-140.3 258.71,-129.62"/>
</g>
<!-- 1&#45;&#45;2 -->
<g id="edge17" class="edge">
<title>1&#45;&#45;2</title>
<path fill="none" stroke="black" d="M382.68,-164.82C365.98,-158.99 342.36,-150.76 325.66,-144.93"/>
</g>
<!-- 1&#45;&#45;3 -->
<g id="edge18" class="edge">
<title>1&#45;&#45;3</title>
<path fill="none" stroke="black" d="M407.01,-159.78C410.97,-148.3 416.89,-131.14 420.84,-119.68"/>
</g>
<!-- 1&#45;&#45;7 -->
<g id="edge19" class="edge">
<title>1&#45;&#45;7</title>
<path fill="none" stroke="black" d="M402.62,-159.76C402.29,-142.65 401.7,-111.54 401.37,-94.38"/>
</g>
<!-- 1&#45;&#45;13 -->
<g id="edge20" class="edge">
<title>1&#45;&#45;13</title>
<path fill="none" stroke="black" d="M378.35,-169.81C376.05,-169.62 373.71,-169.42 371.38,-169.23"/>
</g>
<!-- 1&#45;&#45;17 -->
<g id="edge21" class="edge">
<title>1&#45;&#45;17</title>
<path fill="none" stroke="black" d="M427.02,-174.6C445.64,-176.72 471.39,-179.65 490.77,-181.86"/>
</g>
<!-- 1&#45;&#45;19 -->
<g id="edge22" class="edge">
<title>1&#45;&#45;19</title>
<path fill="none" stroke="black" d="M393.04,-183.19C382.11,-195.83 364.51,-216.18 353.49,-228.92"/>
</g>
<!-- 1&#45;&#45;21 -->
<g id="edge23" class="edge">
<title>1&#45;&#45;21</title>
<path fill="none" stroke="black" d="M420.22,-162.88C440.87,-152.22 475.14,-134.52 496.05,-123.73"/>
</g>
<!-- 30 -->
<g id="node31" class="node">
<title>30</title>
<ellipse fill="#00009e" fill-opacity="0.498039" stroke="black" cx="260.16" cy="-241.81" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="260.16" y="-239.91" font-family="Times,serif" font-size="8.00">30 : 1</text>
</g>
<!-- 1&#45;&#45;30 -->
<g id="edge24" class="edge">
<title>1&#45;&#45;30</title>
<path fill="none" stroke="black" d="M385.35,-180.43C358.27,-193.71 306.42,-219.13 278.71,-232.72"/>
</g>
<!-- 2&#45;&#45;3 -->
<g id="edge25" class="edge">
<title>2&#45;&#45;3</title>
<path fill="none" stroke="black" d="M327.71,-132.29C349.17,-126.86 381.5,-118.67 402.92,-113.25"/>
</g>
<!-- 2&#45;&#45;7 -->
<g id="edge26" class="edge">
<title>2&#45;&#45;7</title>
<path fill="none" stroke="black" d="M321.74,-128.46C339.43,-118.15 367.56,-101.76 385.16,-91.51"/>
</g>
<!-- 2&#45;&#45;8 -->
<g id="edge27" class="edge">
<title>2&#45;&#45;8</title>
<path fill="none" stroke="black" d="M303.22,-149.98C300.46,-164.48 295.84,-188.68 293.08,-203.15"/>
</g>
<!-- 9 -->
<g id="node10" class="node">
<title>9</title>
<ellipse fill="#0000bd" fill-opacity="0.498039" stroke="black" cx="223.94" cy="-69.92" rx="25" ry="12.04"/>
<text text-anchor="middle" x="223.94" y="-68.02" font-family="Times,serif" font-size="8.00">9 : 2</text>
</g>
<!-- 2&#45;&#45;9 -->
<g id="edge28" class="edge">
<title>2&#45;&#45;9</title>
<path fill="none" stroke="black" d="M292.78,-127.29C277.45,-114.51 251.89,-93.21 236.6,-80.47"/>
</g>
<!-- 2&#45;&#45;13 -->
<g id="edge29" class="edge">
<title>2&#45;&#45;13</title>
<path fill="none" stroke="black" d="M319.22,-148.38C322.62,-150.97 326.26,-153.76 329.66,-156.35"/>
</g>
<!-- 27 -->
<g id="node28" class="node">
<title>27</title>
<ellipse fill="#0000bd" fill-opacity="0.498039" stroke="black" cx="167.36" cy="-92.87" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="167.36" y="-90.97" font-family="Times,serif" font-size="8.00">27 : 2</text>
</g>
<!-- 2&#45;&#45;27 -->
<g id="edge30" class="edge">
<title>2&#45;&#45;27</title>
<path fill="none" stroke="black" d="M284.84,-131.17C259.4,-122.87 216.35,-108.84 190.03,-100.26"/>
</g>
<!-- 28 -->
<g id="node29" class="node">
<title>28</title>
<ellipse fill="#0000bd" fill-opacity="0.498039" stroke="black" cx="223.32" cy="-172.37" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="223.32" y="-170.47" font-family="Times,serif" font-size="8.00">28 : 2</text>
</g>
<!-- 2&#45;&#45;28 -->
<g id="edge31" class="edge">
<title>2&#45;&#45;28</title>
<path fill="none" stroke="black" d="M286.46,-145.9C273.65,-151.27 256.79,-158.34 243.72,-163.82"/>
</g>
<!-- 32 -->
<g id="node33" class="node">
<title>32</title>
<ellipse fill="#0000bd" fill-opacity="0.498039" stroke="black" cx="155.34" cy="-232.18" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="155.34" y="-230.28" font-family="Times,serif" font-size="8.00">32 : 2</text>
</g>
<!-- 2&#45;&#45;32 -->
<g id="edge32" class="edge">
<title>2&#45;&#45;32</title>
<path fill="none" stroke="black" d="M290.3,-147.47C261.81,-165.35 200.71,-203.7 171.51,-222.02"/>
</g>
<!-- 3&#45;&#45;7 -->
<g id="edge33" class="edge">
<title>3&#45;&#45;7</title>
<path fill="none" stroke="black" d="M414.73,-96.71C413.72,-95.64 412.7,-94.54 411.68,-93.46"/>
</g>
<!-- 3&#45;&#45;12 -->
<g id="edge34" class="edge">
<title>3&#45;&#45;12</title>
<path fill="none" stroke="black" d="M439.88,-97.99C455.82,-87.63 481.01,-71.27 497.2,-60.75"/>
</g>
<!-- 3&#45;&#45;13 -->
<g id="edge35" class="edge">
<title>3&#45;&#45;13</title>
<path fill="none" stroke="black" d="M411.16,-117.71C396.35,-128.47 372.96,-145.47 357.91,-156.41"/>
</g>
<!-- 4&#45;&#45;6 -->
<g id="edge36" class="edge">
<title>4&#45;&#45;6</title>
<path fill="none" stroke="black" d="M605.36,-222.5C614.89,-215.01 627.7,-204.95 637.22,-197.48"/>
</g>
<!-- 4&#45;&#45;10 -->
<g id="edge37" class="edge">
<title>4&#45;&#45;10</title>
<path fill="none" stroke="black" d="M594,-220.8C595.49,-211.14 597.56,-197.65 599.04,-187.99"/>
</g>
<!-- 5&#45;&#45;6 -->
<g id="edge38" class="edge">
<title>5&#45;&#45;6</title>
<path fill="none" stroke="black" d="M647.17,-212.39C647.7,-208.2 648.3,-203.51 648.83,-199.33"/>
</g>
<!-- 5&#45;&#45;10 -->
<g id="edge39" class="edge">
<title>5&#45;&#45;10</title>
<path fill="none" stroke="black" d="M635.23,-213.37C628.01,-205.5 618.42,-195.04 611.2,-187.17"/>
</g>
<!-- 16 -->
<g id="node17" class="node">
<title>16</title>
<ellipse fill="#001bff" fill-opacity="0.498039" stroke="black" cx="756.65" cy="-222.98" rx="28.07" ry="12.04"/>
<text text-anchor="middle" x="756.65" y="-221.08" font-family="Times,serif" font-size="8.00">16 : 5</text>
</g>
<!-- 5&#45;&#45;16 -->
<g id="edge40" class="edge">
<title>5&#45;&#45;16</title>
<path fill="none" stroke="black" d="M670.53,-224.3C687.66,-224.04 710.46,-223.69 728.29,-223.42"/>
</g>
<!-- 6&#45;&#45;16 -->
<g id="edge41" class="edge">
<title>6&#45;&#45;16</title>
<path fill="none" stroke="black" d="M670.87,-194.05C689.09,-200.2 715.66,-209.16 734.39,-215.48"/>
</g>
<!-- 8&#45;&#45;30 -->
<g id="edge42" class="edge">
<title>8&#45;&#45;30</title>
<path fill="none" stroke="black" d="M278.69,-225.67C276.68,-227.42 274.6,-229.24 272.58,-230.99"/>
</g>
<!-- 8&#45;&#45;32 -->
<g id="edge43" class="edge">
<title>8&#45;&#45;32</title>
<path fill="none" stroke="black" d="M266.58,-218.18C242.99,-221.14 207.19,-225.65 182.61,-228.74"/>
</g>
<!-- 33 -->
<g id="node34" class="node">
<title>33</title>
<ellipse fill="#13ffeb" fill-opacity="0.498039" stroke="black" cx="168.31" cy="-203.44" rx="31.64" ry="12.04"/>
<text text-anchor="middle" x="168.31" y="-201.54" font-family="Times,serif" font-size="8.00">33 : 13</text>
</g>
<!-- 8&#45;&#45;33 -->
<g id="edge44" class="edge">
<title>8&#45;&#45;33</title>
<path fill="none" stroke="black" d="M266.31,-212.79C247.04,-210.95 220.07,-208.38 199.3,-206.4"/>
</g>
<!-- 9&#45;&#45;33 -->
<g id="edge45" class="edge">
<title>9&#45;&#45;33</title>
<path fill="none" stroke="black" d="M219,-81.77C208.47,-107.04 183.91,-166 173.31,-191.42"/>
</g>
<!-- 13&#45;&#45;33 -->
<g id="edge46" class="edge">
<title>13&#45;&#45;33</title>
<path fill="none" stroke="black" d="M318,-172.22C285.64,-178.96 229.99,-190.57 196.42,-197.57"/>
</g>
<!-- 14 -->
<g id="node15" class="node">
<title>14</title>
<ellipse fill="#9e0000" fill-opacity="0.498039" stroke="black" cx="52.78" cy="-276.67" rx="31.64" ry="12.04"/>
<text text-anchor="middle" x="52.78" y="-274.77" font-family="Times,serif" font-size="8.00">14 : 32</text>
</g>
<!-- 14&#45;&#45;32 -->
<g id="edge47" class="edge">
<title>14&#45;&#45;32</title>
<path fill="none" stroke="black" d="M74.03,-267.45C92.1,-259.61 117.85,-248.44 135.51,-240.78"/>
</g>
<!-- 14&#45;&#45;33 -->
<g id="edge48" class="edge">
<title>14&#45;&#45;33</title>
<path fill="none" stroke="black" d="M69.33,-266.17C91.29,-252.25 129.84,-227.82 151.79,-213.91"/>
</g>
<!-- 15 -->
<g id="node16" class="node">
<title>15</title>
<ellipse fill="#9e0000" fill-opacity="0.498039" stroke="black" cx="163.28" cy="-331.12" rx="31.64" ry="12.04"/>
<text text-anchor="middle" x="163.28" y="-329.22" font-family="Times,serif" font-size="8.00">15 : 32</text>
</g>
<!-- 15&#45;&#45;32 -->
<g id="edge49" class="edge">
<title>15&#45;&#45;32</title>
<path fill="none" stroke="black" d="M162.31,-318.99C160.78,-300 157.85,-263.46 156.32,-244.41"/>
</g>
<!-- 15&#45;&#45;33 -->
<g id="edge50" class="edge">
<title>15&#45;&#45;33</title>
<path fill="none" stroke="black" d="M163.76,-318.88C164.73,-294.41 166.88,-239.74 167.84,-215.46"/>
</g>
<!-- 18 -->
<g id="node19" class="node">
<title>18</title>
<ellipse fill="#9e0000" fill-opacity="0.498039" stroke="black" cx="81.25" cy="-306.52" rx="31.64" ry="12.04"/>
<text text-anchor="middle" x="81.25" y="-304.62" font-family="Times,serif" font-size="8.00">18 : 32</text>
</g>
<!-- 18&#45;&#45;32 -->
<g id="edge51" class="edge">
<title>18&#45;&#45;32</title>
<path fill="none" stroke="black" d="M92.5,-295.23C106.43,-281.26 130.01,-257.6 143.98,-243.57"/>
</g>
<!-- 18&#45;&#45;33 -->
<g id="edge52" class="edge">
<title>18&#45;&#45;33</title>
<path fill="none" stroke="black" d="M91.23,-294.7C108.03,-274.81 141.72,-234.92 158.44,-215.13"/>
</g>
<!-- 19&#45;&#45;33 -->
<g id="edge53" class="edge">
<title>19&#45;&#45;33</title>
<path fill="none" stroke="black" d="M318,-235.11C285.55,-228.25 229.66,-216.42 196.13,-209.32"/>
</g>
<!-- 20 -->
<g id="node21" class="node">
<title>20</title>
<ellipse fill="#9e0000" fill-opacity="0.498039" stroke="black" cx="116.56" cy="-329.88" rx="31.64" ry="12.04"/>
<text text-anchor="middle" x="116.56" y="-327.98" font-family="Times,serif" font-size="8.00">20 : 32</text>
</g>
<!-- 20&#45;&#45;32 -->
<g id="edge54" class="edge">
<title>20&#45;&#45;32</title>
<path fill="none" stroke="black" d="M121.31,-317.9C128.76,-299.15 143.08,-263.07 150.55,-244.25"/>
</g>
<!-- 20&#45;&#45;33 -->
<g id="edge55" class="edge">
<title>20&#45;&#45;33</title>
<path fill="none" stroke="black" d="M121.52,-317.76C131.44,-293.53 153.6,-239.39 163.44,-215.34"/>
</g>
<!-- 22 -->
<g id="node23" class="node">
<title>22</title>
<ellipse fill="#9e0000" fill-opacity="0.498039" stroke="black" cx="43.08" cy="-235.37" rx="31.64" ry="12.04"/>
<text text-anchor="middle" x="43.08" y="-233.47" font-family="Times,serif" font-size="8.00">22 : 32</text>
</g>
<!-- 22&#45;&#45;32 -->
<g id="edge56" class="edge">
<title>22&#45;&#45;32</title>
<path fill="none" stroke="black" d="M74.95,-234.46C91.28,-234 111.01,-233.44 126.84,-232.99"/>
</g>
<!-- 22&#45;&#45;33 -->
<g id="edge57" class="edge">
<title>22&#45;&#45;33</title>
<path fill="none" stroke="black" d="M69.64,-228.59C90.89,-223.18 120.55,-215.62 141.79,-210.2"/>
</g>
<!-- 23 -->
<g id="node24" class="node">
<title>23</title>
<ellipse fill="#ff3900" fill-opacity="0.498039" stroke="black" cx="93.99" cy="-142.39" rx="31.64" ry="12.04"/>
<text text-anchor="middle" x="93.99" y="-140.49" font-family="Times,serif" font-size="8.00">23 : 27</text>
</g>
<!-- 25 -->
<g id="node26" class="node">
<title>25</title>
<ellipse fill="#bd0000" fill-opacity="0.498039" stroke="black" cx="96.5" cy="-65.29" rx="31.64" ry="12.04"/>
<text text-anchor="middle" x="96.5" y="-63.39" font-family="Times,serif" font-size="8.00">25 : 31</text>
</g>
<!-- 23&#45;&#45;25 -->
<g id="edge58" class="edge">
<title>23&#45;&#45;25</title>
<path fill="none" stroke="black" d="M94.38,-130.35C94.85,-115.94 95.63,-91.98 96.1,-77.51"/>
</g>
<!-- 23&#45;&#45;27 -->
<g id="edge59" class="edge">
<title>23&#45;&#45;27</title>
<path fill="none" stroke="black" d="M109.55,-131.89C122.09,-123.43 139.62,-111.6 152.08,-103.19"/>
</g>
<!-- 29 -->
<g id="node30" class="node">
<title>29</title>
<ellipse fill="#9e0000" fill-opacity="0.498039" stroke="black" cx="49.57" cy="-177.68" rx="31.64" ry="12.04"/>
<text text-anchor="middle" x="49.57" y="-175.78" font-family="Times,serif" font-size="8.00">29 : 32</text>
</g>
<!-- 23&#45;&#45;29 -->
<g id="edge60" class="edge">
<title>23&#45;&#45;29</title>
<path fill="none" stroke="black" d="M79.94,-153.56C74.65,-157.76 68.65,-162.52 63.38,-166.71"/>
</g>
<!-- 23&#45;&#45;32 -->
<g id="edge61" class="edge">
<title>23&#45;&#45;32</title>
<path fill="none" stroke="black" d="M102.01,-154.13C113.77,-171.34 135.56,-203.23 147.32,-220.43"/>
</g>
<!-- 23&#45;&#45;33 -->
<g id="edge62" class="edge">
<title>23&#45;&#45;33</title>
<path fill="none" stroke="black" d="M107.62,-153.59C121.03,-164.6 141.39,-181.32 154.76,-192.31"/>
</g>
<!-- 24 -->
<g id="node25" class="node">
<title>24</title>
<ellipse fill="#ff3900" fill-opacity="0.498039" stroke="black" cx="143.01" cy="-12.02" rx="31.64" ry="12.04"/>
<text text-anchor="middle" x="143.01" y="-10.12" font-family="Times,serif" font-size="8.00">24 : 27</text>
</g>
<!-- 24&#45;&#45;25 -->
<g id="edge63" class="edge">
<title>24&#45;&#45;25</title>
<path fill="none" stroke="black" d="M132.92,-23.58C125.12,-32.51 114.37,-44.82 106.57,-53.75"/>
</g>
<!-- 24&#45;&#45;27 -->
<g id="edge64" class="edge">
<title>24&#45;&#45;27</title>
<path fill="none" stroke="black" d="M146.71,-24.3C151.32,-39.59 159.13,-65.53 163.71,-80.74"/>
</g>
<!-- 24&#45;&#45;31 -->
<g id="edge65" class="edge">
<title>24&#45;&#45;31</title>
<path fill="none" stroke="black" d="M152.39,-23.55C169.8,-44.93 206.91,-90.54 224.38,-112.01"/>
</g>
<!-- 25&#45;&#45;31 -->
<g id="edge66" class="edge">
<title>25&#45;&#45;31</title>
<path fill="none" stroke="black" d="M117.66,-74.28C143.84,-85.4 188.1,-104.21 213.77,-115.11"/>
</g>
<!-- 26 -->
<g id="node27" class="node">
<title>26</title>
<ellipse fill="#7f0000" fill-opacity="0.498039" stroke="black" cx="31.82" cy="-102.15" rx="31.64" ry="12.04"/>
<text text-anchor="middle" x="31.82" y="-100.25" font-family="Times,serif" font-size="8.00">26 : 33</text>
</g>
<!-- 26&#45;&#45;29 -->
<g id="edge67" class="edge">
<title>26&#45;&#45;29</title>
<path fill="none" stroke="black" d="M34.67,-114.28C37.96,-128.29 43.35,-151.22 46.66,-165.32"/>
</g>
<!-- 26&#45;&#45;33 -->
<g id="edge68" class="edge">
<title>26&#45;&#45;33</title>
<path fill="none" stroke="black" d="M46.43,-112.99C72.7,-132.48 127.61,-173.24 153.81,-192.68"/>
</g>
<!-- 27&#45;&#45;33 -->
<g id="edge69" class="edge">
<title>27&#45;&#45;33</title>
<path fill="none" stroke="black" d="M167.47,-105.13C167.65,-126.38 168.02,-169.84 168.2,-191.13"/>
</g>
<!-- 28&#45;&#45;31 -->
<g id="edge70" class="edge">
<title>28&#45;&#45;31</title>
<path fill="none" stroke="black" d="M225.92,-160.33C227.52,-152.88 229.58,-143.36 231.19,-135.89"/>
</g>
<!-- 28&#45;&#45;33 -->
<g id="edge71" class="edge">
<title>28&#45;&#45;33</title>
<path fill="none" stroke="black" d="M206.21,-182.03C199.91,-185.59 192.74,-189.64 186.35,-193.25"/>
</g>
<!-- 29&#45;&#45;32 -->
<g id="edge72" class="edge">
<title>29&#45;&#45;32</title>
<path fill="none" stroke="black" d="M68.48,-187.42C87.97,-197.46 118.13,-213.01 137.3,-222.88"/>
</g>
<!-- 29&#45;&#45;33 -->
<g id="edge73" class="edge">
<title>29&#45;&#45;33</title>
<path fill="none" stroke="black" d="M77.41,-183.72C96.46,-187.85 121.63,-193.31 140.64,-197.44"/>
</g>
<!-- 30&#45;&#45;32 -->
<g id="edge74" class="edge">
<title>30&#45;&#45;32</title>
<path fill="none" stroke="black" d="M232.35,-239.26C217.18,-237.86 198.47,-236.14 183.29,-234.75"/>
</g>
<!-- 30&#45;&#45;33 -->
<g id="edge75" class="edge">
<title>30&#45;&#45;33</title>
<path fill="none" stroke="black" d="M239.78,-233.3C224.98,-227.11 204.99,-218.76 189.92,-212.47"/>
</g>
<!-- 31&#45;&#45;32 -->
<g id="edge76" class="edge">
<title>31&#45;&#45;32</title>
<path fill="none" stroke="black" d="M225.44,-135.25C210.33,-156.14 178.74,-199.81 163.68,-220.64"/>
</g>
<!-- 31&#45;&#45;33 -->
<g id="edge77" class="edge">
<title>31&#45;&#45;33</title>
<path fill="none" stroke="black" d="M224.45,-135.07C212.06,-150.16 190.38,-176.56 177.89,-191.77"/>
</g>
<!-- 32&#45;&#45;33 -->
<g id="edge78" class="edge">
<title>32&#45;&#45;33</title>
<path fill="none" stroke="black" d="M160.76,-220.15C161.49,-218.56 162.23,-216.92 162.95,-215.32"/>
</g>
</g>
</svg>
</div>


It's pretty easy to write a function in just about any language
that will do a BFS across a graph, but as graphs get bigger these
approaches do not scale.  In order to BFS efficiently, multiple
cores must be used and coordinate their parallel work.

Different kinds of processor architectures, CPUs, GPUs, FPGAs etc,
have extremely different ways of doing concurrent work.  Reaching
the scale of billions of nodes and hundreds of cores requires the
kind of sparse computing expertise that's been used to develop the
GraphBLAS API.

For example, we've benchmarked OneSparse using the industry
standard [GAP Benchmark Suite]().  The GAP is a group of publicly
available [graph data sets]() and algorithms that graph libraries
can use to benchmark their performance.

### Benchmarks
![BFS Benchmarks](/images/BFS.svg)

This chart displays BFS performance on several GAP graphs.  The
smallest graph, `road`, has 57M edges.  The largest graph `urand`
has 4.3B edges.  The values show the number of "Edges Per Second"
OneSparse can traverse with BFS by dividing the number of edges in
the graph by the run time.

## OneSparse Beta

## Preview Table Access Method

