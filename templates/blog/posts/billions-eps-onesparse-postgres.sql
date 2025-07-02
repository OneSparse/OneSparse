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

-- # Billions of Edges Per Second on Postgres Graphs

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

-- SuiteSparse graph algorithms are expressed using [Linear
-- Algebra](https://en.wikipedia.org/wiki/Linear_algebra) operations to
-- traverse and process graphs.  In this powerful approach inspired by
-- the [mathematical roots of graph
-- theory](https://en.wikipedia.org/wiki/Adjacency_matrix), sets of nodes
-- are sparse Vectors and graphs of nodes are sparse Matrices.  This
-- differs quite a bit from most graph processing libraries that focus on
-- individual nodes and edges.  Writing efficient bulk parallel decisions
-- at that scale is complex, and hard to retarget to new architectures
-- due to close-to-the-metal complexities.

-- The GraphBLAS leverages the rules of linear algebra, and in particular
-- [Matrix
-- Multiplication](https://en.wikipedia.org/wiki/Matrix_multiplication),
-- to efficiently schedule graph operations in bulk across many cores, or
-- whatever other processing unit the particular architecture employs.
-- By specifying operations in a high level algebraic form, the library
-- implementor can use [advanced sparse compression
-- techniques](https://en.wikipedia.org/wiki/Sparse_matrix), statistical
-- and structural knowledge of the data, and other optimizations to
-- target the best approach for a given architecture on a per-operation
-- basis.

-- ## OneSparse

-- OneSparse is a Postgres extension that brings the power of SuiteSparse
-- to Postgres. Have you found yourself spending too much time
-- extracting, transforming, and loading graphs into side databases that
-- need constant money, maintanance and impedence matching with SQL.
-- With OneSparse, it's easy to turn SQL data into graphs and back again
-- in pure SQL.
