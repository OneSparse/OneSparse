# pggraphblas
High Performance Graph Processing with Postgres using Linear Algebra

# summary

GraphBLAS is a sparse linear algebra API optimized for processing
graphs encoded as sparse matrices and vectors.  In addition to common
real/integer matrix algebras, GraphBLAS supports up to 960 different
"semiring" algebras, that can be used to sparsely encode and solve
graph problems.  pggraphblas is a postgres extension that exposes the
matrix and vector types and their API to SQL.

pggraphblas leverages the expertise in the field of sparse matrix
programming by [The GraphBLAS](http://graphblas.org) and wraps the
[SuiteSparse
GraphBLAS](http://faculty.cse.tamu.edu/davis/suitesparse.html)
implementation of the GraphBLAS API which provides access to sparse
matrix and vector types and functions.  The SuiteSparse:GraphBLAS
implementation is brought to us by the work of Dr. Tim Davis,
professor in the Department of Computer Science and Engineering at
Texas A&M University, and others.  [News and
information](http://faculty.cse.tamu.edu/davis/news.html) can provide
you with a lot more background information, in addition to the
references below.

This is a currently very alpha quality integration, much of the API is
not supported yet but is an active work in progress.  See TODO below.

# manifesto

For a long time, mathematicians have known that [matrices are powerful
representations of
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

By using a sparse matrix instead of dense, only the elements used are
actually stored in the matrix. The "empty" part of the matrix is not
stored and assumes a default "algebraic zero" value.  GraphBLAS brings
a powerful, abstract algebra to solving graph problems with various
combinations of math operations on matrix elements. The combinations
of operations are called "semirings".  Different semirings can
accomplish different common steps in graph problems, such as finding
shortest or most optimal paths.

pggraphblas is a postgres extension that provides access to two new
types: matrix and vector, as well as the GraphBLAS api to manipulate
these types.  Aggregate functions are provided to build matrices from
SQL queries, and set-returning functions are also provided to turn
graphs back into relational sets.  From a SQL point of view, matrices
look a certain bit like arrays, being stored as variable length column
values.

Postgres is already an excellent database for graph storage, many
real-world foreign key relationships, common structures in most
postgres databases, describe sparse graphs.  Graph traversal and
manipulation can be acheived in SQL using recursive common table
expressions in a very flexible and general way, but this approach has
a drawback: sparse relational graphs are scattered across indexes and
table blocks, having poor locality.  Interpreted sql code works by
considering rows one at a time, vertex by vertex.

Using pggraphblas bring high memory density encoding and optimized
numerical computing methods to solving sparse graph problems.
GraphBLAS is designed to be optimized for this situation where dense
matrices are not.  GraphBLAS is also an actively developed project
with future plans such as GPU/TPU integration, bringing high numeric
computing density to the problem with no change code that uses the
API.


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
    n index := matrix_nrows(A);          -- The number of result rows
    v vector := vector_new('int32', n)   -- int32 result vector of vertex levels
    q vector := vector_new('bool', n);   -- bool mask of completed vertices
    desc descriptor := descriptor_new(   -- MxM descriptor:
        'mask', 'scmp',                  -- negate the mask
        'outp', 'replace');              -- clear results first
    level integer := 1;                  -- start at level 1
    not_done bool := true;               -- flag to indicate still work to do
    
begin
    perform matrix_set_element(q, true, source);  -- set the source element to done
    
    while not_done and level <= n loop            -- while still work to do
        v := assign(level, 'all', v, mask=q);     -- assign the current level to all
                                                  -- results not masked as done by q
    
        q := mxv(A, q,                            -- multiply A by q
            semiring='lor_land_bool',             -- using lor_land_bool semiring
            mask=v,                               -- only those not masked
            desc=desc);                           -- clearing results first
    
        not_done := vector_reduce_bool(q,         -- are there more neighbors
            operator='lor_bool_monoid');          -- to consider?
    
        level := level + 1;                       -- increment the level
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
* composite user defined element types, semirings, and monoid UDFs
