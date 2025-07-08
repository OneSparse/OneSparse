\pset linestyle unicode
\pset border 2
-- ---
-- draft: true
-- authors:
--   - michelp
-- date: 2025-07-01
-- categories:
--   - onesparse
--   - postgres
--   - graphblas
--   - suitesparse
--   - graphs
--   - linear algebra
-- ---

-- # Billions of Edges Per Second with Postgres

-- For years now those of us over at [The GraphBLAS Forum](graphblas.org)
-- have been working hard to bring a standardized, state-of-the-art
-- parallel graph traversal API to all possible architectures.  This API
-- work has led to the evolution of the high performance graph linear
-- algebra library [SuiteSparse
-- GraphBLAS](https://people.engr.tamu.edu/davis/GraphBLAS.html) by
-- [Dr. Timothy Davis et al]() at at [Texas A&M University]().
-- SuiteSparse contains powerful optimized kernels for sparse graph
-- computation, the ability for users to define their own edge types and
-- operators, and has a built-in JIT compiler that can target many
-- computing architectures including [Nvidia CUDA]().

-- <!-- more -->

-- OneSparse is more than just fast graphs, its graph algorithms are
-- expressed using [Linear
-- Algebra](https://en.wikipedia.org/wiki/Linear_algebra) operations
-- to traverse and process graphs that are represented as matrices.
-- In this powerful approach inspired by the [mathematical roots of
-- graph theory](https://en.wikipedia.org/wiki/Adjacency_matrix), sets
-- of nodes are sparse Vectors and graphs of nodes are sparse
-- Matrices.

-- This differs quite a bit from most graph processing libraries that
-- focus on individual nodes and edges, which are inherently serial
-- concepts.  Writing efficient bulk parallel decisions at such a low
-- level is complex, and hard to retarget to new architectures due to
-- close-to-the-metal complexities that all new and coming families of
-- parallel processors possess.
--
-- Other libraries use an "Actor" model where the algorithm defines
-- the behavior of a node when it receives or sends messages to its
-- adjacent actors.  This approach has good parallelism opportunities,
-- but reasoning about complex algorithms and their data hazzards can
-- be difficult.  Seeing the forest for the trees is hard from an
-- all-tree perspective.
--
-- By basing itself on the abstractions of math, the GraphBLAS
-- leverages the rules and objects of [Linear
-- Algebra](https://en.wikipedia.org/wiki/Linear_algebra), and in
-- particular Sparse [Matrix
-- Multiplication](https://en.wikipedia.org/wiki/Matrix_multiplication),
-- over [Semirings](https://en.wikipedia.org/wiki/Semiring) to
-- efficiently schedule graph operations in bulk across many cores, or
-- whatever other processing unit the particular architecture employs.
--
-- The key idea is that graphs and matricies are conceptuals
-- reflections of each other, an idea understood now for hundreds of
-- years.  Creating a Graph in OneSparse is the same as creating a
-- matrix.
--

create extension if not exists onesparse;

with example as (select 'bool(7:7)[0:1:t 0:3:t 1:4:t 1:6:t 2:5:t 3:0:t 3:2:t 4:5:t 5:2:t 6:2:t 6:3:t 6:4:t]'::matrix as graph)
    select draw(graph) as twocol_a_source, print(graph) as twocol_b_source from example \gset
\i sql/twocol_drawtext.sql

with a as (select random_matrix(4,8,48,sym:=false,seed:=0.41) as m),
     b as (select random_matrix(4,8,48,sym:=false,seed:=0.42) as m)
select hyperdraw(triu((select m from a)), triu((select m from b))) as draw_source \gset
\i sql/draw.sql

-- By specifying operations in a high level algebraic form, the
-- library implementor can use [advanced sparse compression
-- techniques](https://en.wikipedia.org/wiki/Sparse_matrix),
-- statistical and structural knowledge of the data, and other
-- optimizations to target the best approach for a given architecture
-- on a per-operation basis.
--
-- ## OneSparse
--
-- OneSparse is a Postgres extension that brings the power of SuiteSparse
-- to Postgres. Have you found yourself spending too much time
-- extracting, transforming, and loading graphs into side databases that
-- need constant money, maintanance and impedence matching with SQL.
-- With OneSparse, it's easy to turn SQL data into graphs and back again
-- in pure SQL.

-- OneSparse can serialize and deserialize graphs into Postgres
-- objects to and from an on-disk state.  This state can come from
-- various sources, but the simplest is the ["TOAST" storage]() of
-- large variable length data arrays in rows.  However, the one major
-- limitation of this approach is that Postgres is limited by design
-- to a maximum TOAST size of about 1 gigabyte which typically limits
-- TOASTed graphs to a few hundred million edges at most.

-- TOAST storage is very useful for small graphs, or sub-graphs of a
-- large graph, but graphs with many billions of edges blow right past
-- this size limit, so OneSparse also provides functions for loading
-- much larger graphs from either SQL queries, Large Object storage,
-- or most optimally, from compressed files on the server filesystem
-- itself.

-- For the purposes of a quick demo, we will use a very small graph
-- that is very commonly studied in graph theory called the
-- [Newman/karate]() graph, which can be downloaded from the
-- [SuiteSparse Matrix Collection]().
--
-- Now we can create graphs, a simple approach for small graphs that
-- fit into the TOAST limit is to make a materialized view that loads
-- the graph data from disk using the standard [Matrix Market]()
-- format:

create materialized view if not exists karate as
    select mmread('/home/postgres/onesparse/demo/karate.mtx') as graph;

select graph from karate;

--
-- Like all SQL types in Postgres, an object must have a literal
-- input/output format that can recreate the object.  The default
-- format shown here shows a list of *edge coordinates*.  To see the
-- matrix like representation, OneSparse provides a `print()` function:

select print(graph) from karate;

-- This graph can now be accessed from SQL using the powerful
-- GraphBLAS API exposed by OneSparse.  For example, we can query the
-- number of rows, columns and values in the graph:

select nrows(graph), ncols(graph), nvals(graph) from karate;

-- At first a matrix might seem like an odd way to store a graph, but
-- now we bring the power of Linear Algebra into play.  For example,
-- let's suppose you want to know the "degree" of every node in the
-- graph, that is, how many edges connect to the node.
--
-- This can be done by reducing the Matrix to a Vector, which sums all
-- the column vectors of the matrix into a vector which contains the
-- degree for each node:

select reduce_cols(cast_to(graph, 'int32')) from karate;

-- A vector maps a node's row index to a value.  Now look at the graph
-- where nodes are labled and color mapped according to the degree of
-- each node:

select draw(triu(graph),
            reduce_cols(cast_to(graph, 'int32')),
            false, false, true, 0.5, 'Degree of Karate Graph')
    as draw_source from karate \gset
\i sql/draw_sfdp.sql
--
-- !!! note
--     The draw() function is part of the OneSparse doctest
--     infrastructure, it's used to generate these doctest diagrams
--     in-line with this documentation, which is also a test.
--
-- We saw the core idea about how graphs and matrices can be one and
-- the same, but now we see the relationship to vectors and graphs.
-- Vectors represent sets of nodes in the graph.  They map the node to
-- some useful piece of information, like the nodes degree.  Vector
-- reduction is a core operation of linear algebra, and the GraphBLAS
-- fully embaces this.  Vectors often end up being the result of an
-- algebraic graph algorithm.
--
-- Degree was simple enough, but what about more advanced algorithms?
-- This is where the GraphBLAS community really bring the power of
-- deep graph research with the [LAGraph]() library.  LAGraph is a
-- collection of optimized graph algorithms using the GraphBLAS API.
-- OneSparse also wraps LAgraph functions and exposes them to SQL.
-- For example, let's [PageRank]() the karate graph:

select draw(triu(graph),
            pagerank(graph),
            false, false, true, 0.5, 'PageRank The Karate Graph')
    as draw_source from karate \gset
\i sql/draw_sfdp.sql

-- Wait isn't this the same as the degree graph?  Nope, but they sure
-- do look similar! This is because at this small-scale PageRank
-- essentially degenerates into degree.  Both of these algorithms are
-- called *centralities* and LAGraph has several of them, for example
-- [Triangle Centrality]() uses the number of triangles a node is
-- connected to to compute its centrality score.  This goes on the
-- assumption that triangles form stronger bonds than individual
-- edges.

select draw(triu(graph),
            triangle_centrality(graph),
            false, false, true, 0.5, 'The Karate Graph')
    as draw_source from karate \gset
\i sql/draw_sfdp.sql

-- This can be taken even further with the [Square Clustering]()
-- algorithm, which considers how many squares nodes connect to:

select draw(triu(graph),
            square_clustering(graph),
            false, false, true, 0.5, 'The Karate Graph')
    as draw_source from karate \gset
\i sql/draw_sfdp.sql

-- Which centrality is the "best"?  Well there is no answer to that,
-- they are all tools in the analyst toolbox.
--
-- ## High Performance Parallel Breadth First Search (BFS)
--
-- BFS is the core operation of graph alalytics.  Like a pepple thrown
-- into a pond, you start from a point in the graph, and traverse all
-- the edges you find, as you find them.  As you traverse the graphs
-- you can accumulate some interesting information as you go, like how
-- many edges away from the starting point you are, the "level" and
-- the row index of the node you got here from, the "parent".  In the
-- GraphBLAS, this information is accumulated in vectors.
--
-- ## Level BFS
-- Level BFS exposes the depth of each vertex starting from a
-- given source vertex using the breadth-first search algorithm.
-- See [https://en.wikipedia.org/wiki/Breadth-first_search](https://en.wikipedia.org/wiki/Breadth-first_search) for details.

select draw(triu(graph), (select level from bfs(graph, 1)), false, false, true, 0.5) as draw_source from karate \gset
\i sql/draw_sfdp.sql

--
-- ## Parent BFS
-- Parent BFS returns the predecessor of each vertex in the
-- BFS tree rooted at the chosen source. It is also based on
-- [https://en.wikipedia.org/wiki/Breadth-first_search](https://en.wikipedia.org/wiki/Breadth-first_search).
--
select draw(triu(graph), (select parent from bfs(graph, 1)), false, false, true, 0.5) as draw_source from karate \gset
\i sql/draw_sfdp.sql
--
-- It's pretty easy to write a function in just about any language
-- that will do a BFS across a graph, but as graphs get bigger these
-- approaches do not scale.  In order to BFS efficiently, multiple
-- cores must be used and coordinate their parallel work.
--
-- Different kinds of processor architectures, CPUs, GPUs, FPGAs etc,
-- have extremely different ways of doing concurrent work.  Reaching
-- the scale of billions of nodes and hundreds of cores requires the
-- kind of sparse computing expertise that's been used to develop the
-- GraphBLAS API.
--
-- For example, we've benchmarked OneSparse using the industry
-- standard [GAP Benchmark Suite]().  The GAP is a group of publicly
-- available [graph data sets]() and algorithms that graph libraries
-- can use to benchmark their performance.
--
-- ### Benchmarks
--
-- <div style="text-align: center;">
--   <img src="/images/BFS.svg" alt="BFS GAP Benchmarks with OneSparse"/>
-- </div>
--
-- This chart displays BFS performance on several GAP graphs.  The
-- smallest graph, `road`, has 57M edges.  The largest graph `urand`
-- has 4.3B edges.  The values show the number of "Edges Per Second"
-- OneSparse can traverse with BFS by dividing the number of edges in
-- the graph by the run time.
--
-- ## OneSparse Beta
--
-- ## Preview Table Access Method
--
--
