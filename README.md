# pggraphblas
High Performance Graph Processing with Postgres using Linear Algebra

# summary

GraphBLAS is a sparse linear algebra API optimized for processing
graphs encoded as sparse matrices and vectors.  In addition to common
real/integer matrix algebras, GraphBLAS supports up to 960 different
"semiring" algebras, that can be used to sparsely encode and solve
graph problems.  pggraphblas is a postgres extension that exposes the
matrix and vector types and their API to SQL.

This is a currently very alpha quality integration, much of the API is
not supported yet but is an active work in progress.  See TODO below.

# manifesto

For a long time, mathematicians have known that [matrices are a
powerful representation of
graphs](http://www.mit.edu/~kepner/GraphBLAS/GraphBLAS-Math-release.pdf).
One practical problem with matrix-encoding graphs is that most
real-world graphs tend to be sparse, so dense linear algebra libraries
like BLAS or numpy do not efficiently encode them in memory or operate
on them efficiently, as most matrix elements are unused.

For example, suppose a fictional social network has 1 billion users,
and each user has about 100 friends, which means there are about 100
billion (1e+11) connections in the graph.  A dense matrix large enough
to hold this graph would need (1 billion)^2 or
(1,000,000,000,000,000,000), a "quintillion" elements, but only 1e+11
of them would have meaningful values, leaving only 0.0000001 of the
graph being utilized.

By using a sparse matrix instead of dense, problems of this size are
encodable on modern computing hardware.  pggraphblas leverages the
expertise in the field of sparse matrix programming by [The
GraphBLAS](http://graphblas.org) organization. GraphBLAS brings a
powerful, abstract algebraic approach to solving graph problems with
various combinations of matrix operations, or "semirings".

pggraphblas is a postgres extension that provides access to two new
types: matrix and vector, as well as the GraphBLAS api to manipulate
these types.  Aggregate functions are provided to build matrices from
SQL queries, and set-returning functions are also provided to turn
graphs back into relational sets.  From a SQL point of view, matrices
look a certain bit like arrays, being stored as variable length column
values.

pggraphblas wraps the [SuiteSparse
GraphBLAS](http://faculty.cse.tamu.edu/davis/suitesparse.html)
implementation of the GraphBLAS API and provides access to sparse
matrix and vector types, as well much of the APIs functionality.  The
SuiteSparse:GraphBLAS implementation is brought to us by Dr. Tim
Davis, professor in the Department of Computer Science and Engineering
at Texas A&M University.  [News and
information](http://faculty.cse.tamu.edu/davis/news.html) can provide
you with a lot more background information, in addition to the
references below.

# references

[Lower Latency Graph Queries in Cypher with Redis GraphRoi Lipman, Redis LabsTim Davis, Texas A&M U](https://www.youtube.com/watch?v=xnez6tloNSQ)

[A good introduction to semirings Part I](https://www.youtube.com/watch?v=Gd_VT_Nj8Xw) [Part II](https://www.youtube.com/watch?v=dluPFbuq6zs)

[A good introduction to abstract algebra](https://www.youtube.com/playlist?list=PLi01XoE8jYoi3SgnnGorR_XOW3IcK-TP6)

[Graph algorithms via SuiteSparse:GraphBLAS: triangle counting and K-truss](http://faculty.cse.tamu.edu/davis/GraphBLAS_files/Davis_HPEC18.pdf)

# development

If you have docker installed, run `./test.sh` to build a docker
container with postgres:11 and GraphBLAS compiled with debug symbols
on.  This will automatically run test.sql.  If you provide any
argument to test.sh the container will not be immediately cleaned up,
and the argument will be run in a tmux in the container, so to launch
a psql, do `./test.sh psql`.  GDB is installed so create another pane
and attach to the current database pid to debug the C code.

# 0.1 goals

To run common graph algorithms on matrices, for example, breadth first search:
```
create function bfs(A matrix, source index) returns vector as $$
declare
    n index := matrix_nrows(A);
    q vector :vector_new('int32', n)
    v vector := vector_new('bool', n);
    desc descriptor := descriptor_new('mask', 'scmp', 'outp', 'replace');
    level integer := 1;
    not_done bool := true;
begin
    perform matrix_set_element(q, true, source);
    while not_done and level <= n loop
        v := assign(level, 'all', n, mask=q);
        q := mxv(A, q, semiring='lor_land_bool', mask=v, desc=desc);
        not_done := vector_reduce_bool(q, operator='lor_bool_monoid');
        level := level + 1;
    end loop;
    return v;
end;
$$ language plpgsql;
```        

DONE-ish:

* Matrix type for INT64 (bigint)
* matrix_agg/matrix_tuples to GrB_Matrix_build
* matrix_tuples from GrB_Matrix_extractTuples
* vector type for INT64 (float4)
* vector_agg to GrB_Matrix_build
* matrix_tuples from GrB_Vector_extractTuples
* most of the GrB_PLUS_TIMES_INT64 semiring.
* overloaded operators for plus,times,set-union/intersection
* "expanded" object API

TODO:

* Generalize matrix type
* polymorphic matrix_agg support all types
* overload all the operators
* parallel aggregates of subgraphs with final merging

UNKNOWNS:

* operation descriptors, how to mask, invert, clear destination?
