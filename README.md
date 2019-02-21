# pggraphblas
High Performance Graph Processing with Postgres using Linear Algebra

# summary

GraphBLAS is a sparse linear algebra API optimized for processing
graphs encoded as sparse matrices and vectors.  In addition to common
real/integer matrix algebras, GraphBLAS supports up to 960 different
"semiring" algebras, that can be used as basic building blocks to
implement graph algorithms.

pggraphblas leverages the expertise in the field of sparse matrix
programming by [The GraphBLAS Forum](http://graphblas.org) and uses
the
[SuiteSparse:GraphBLAS](http://faculty.cse.tamu.edu/davis/suitesparse.html)
API implementation. SuiteSparse:GraphBLAS is brought to us by the work
of Dr. Tim Davis, professor in the Department of Computer Science and
Engineering at Texas A&M University.  [News and
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
stored and is interpreted as an "algebraic zero" value, which might
not be the actual number zero, but other values like positive or
negative infinity depending on the particular algebraic operations
applied.

pggraphblas is a postgres extension that provides access to two new
types: matrix and vector, as well as the GraphBLAS api to manipulate
these types.  Aggregate functions are provided to build matrices from
SQL queries, and set-returning functions are also provided to turn
graphs back into relational sets.  From a PostgreSQL point of view,
matrices look a little bit like arrays, being stored as variable
length column values.

pggraphblas is not a "graph database" using postgres. Postgres is
*already* an excellent database for graph storage and retrieval. Many
real-world foreign key relationships, common structures in most
postgres databases, describe sparse graphs.  pggraphblas provides
building blocks for solving graph algorithms over with those graphs.

Graph traversal and manipulation can be acheived in PostgreSQL using
recursive common table expressions ("WITH" queries) in a very flexible
and general way, but this approach has a drawback: sparse relational
graphs are scattered across indexes and table blocks, having poor
locality.  Interpreted sql code works by considering row based
expressions one at a time, vertex by vertex so to speak.

Using pggraphblas brings high density memory encoding and optimized
numerical computing methods to solving sparse graph problems.
GraphBLAS is designed to be optimized for storing values where dense
matrices are not, and is optimized for accessing sparse values where
database tables are not.

GraphBLAS is also an actively developed project with future plans such
as GPU/TPU integration, bringing higher density numeric computing to
the problem with no change code that uses the API.

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

# types

GraphBLAS has an extremely rich type system that encompases not only
common "artithmetic" algegbra, but also many other algebras using an
algebraic structures called semirings.  The combinations of 7 matrix
types and 960 semiring operations offer a huge number of building
blocks for solving graph problems.

PostgreSQL is a strongly typed language that comes with many built-in
data types, the following types map from GraphBLAS to PostgreSQL:

    - GrB_BOOL maps to bool
    - GrB_INT8 maps to char(1)
    - GrB_INT16 maps to smallint
    - GrB_INT32 maps to integer
    - GrB_INT64 maps to bigint
    - GrB_FP32 maps to real
    - GrB_FP64 maps to float aka "double precision"

Postgres does not support unsigned integers like GraphBLAS (is there a
workaround?  support the uint extension?)

pggraphblas matrix and vector objects have an intrinsic type of data
that they store.  So, a matrix can store booleans, and various sized
integers and floats.  Once a matrix is created with a specific type it
cannot be changed.

When combining matrices (and vectors) it is possible to not only use
the common arithmetic plus and multiply operators on any given type,
it is also possible to mix various semiring operations on any
supported type.  Put example here of {R,min,+,0,+inf}.

# API

Pggraphblas tries to adhere closely to the spirit of the GraphBLAS C
API.  It provides sixe new types: vector, matrix descriptor, binop,
monoid and semiring.  It also exposes a number of functions that
operate with or on those types.

## vector

Vectors can be constructed from arrays by calling `vector(array[])` or
casting from an array:

    postgres=# select vector(array[1,2,3]) = array[1,2,3]::vector;
     t

This constructs a "dense" integer vector, because it's size and it's
number of values are the same:

    postgres=# select size(vector(array[1,2,3])) = nvals(array[1,2,3]::vector);
    t

The supported types are bigint (64 bit), integer (32 bit), smallint
(16 bit), bool (t/f), real (32 bit single precision) and float (64 bit
double precision).  While GraphBLAS supports unsigned types, all
currently supported types are signed but unsigned may be supported in
the future.

Sparse vectors can be constructed by calling vector with two array
arguments , the first array are the indexes, and must be coercible to
bigint, and the second array are the values, and can be any supported
type:

  XXX

Both arrays must be the exact same length.  The size of the vector
will be the maximum element from the first list, but the vector will
only store the values that are defined.  This is the "sparse" nature
of the object, it can be efficiently used in matrix math involving
millions of elements, when only a few actual values are in play.

Vectors can be added and multiplied in element-wise fashion using
either operator or functional notation:

    postgres=# select vector(array[1,2,3]) + vector(array[1,2,3]) = vector(array[2,4,6]);
     t

    postgres=# select vector(array[1,2,3]) * vector(array[1,2,3]) = vector(array[1,4,9]);
     t

    postgres=# select ewise_add(vector(array[1,2,3]), vector(array[1,2,3])) = vector(array[2,4,6]);
     t

    postgres=# select ewise_mult(vector(array[1,2,3]) * vector(array[1,2,3])) = vector(array[1,4,9]);
     t

The "functional" form is provided so that other graphblas parameters
and features can be used during the operation.  For example, a
different binaryop can be selected for the operation:

XXX

## matrix

Matrices can be constructed with `matrix(bigint[], bigint[], array[])`
where the first array are the row indexes, the second array the column
indexes, and the third array the values.  Matrices support all the
same types as vectors.

## mxm

Matrices can be multiplied using a default semiring via the '*'
operator, or called explicity using functional notation so that
different semirings can be used to carry out the operation.
Functional notation also allows passing other graphblas options like a
masking matrix, an accumulator operation, and a graphblas descriptor
object which can specify replacement and transposition rules for
inputs and outputs.

## mxv

## vxm

## ewise add

## ewise mul

## extract

## assign

## apply

## select

## reduce

## descriptors

## semirings
    
## binops

# 0.1 goals

To run common graph algorithms on matrices, for example, breadth first search:
```
create function bfs(A matrix, source index) returns vector as $$
declare
    n index := matrix_nrows(A);          -- The number of result rows
    v vector := vector_int(n)            -- int32 result vector of vertex levels
    q vector := vector_bool(n);          -- bool mask of completed vertices
    desc descriptor := descriptor(       -- MxM descriptor:
        'mask', 'scmp',                  -- negate the mask
        'outp', 'replace');              -- clear results first
    level integer := 1;                  -- start at level 1
    not_done bool := true;               -- flag to indicate still work to do
    
begin
    perform matrix_set_element(q, true, source);  -- set the source element to done
    
    while not_done and level <= n loop            -- while still work to do
        assign(level, 'all', v, mask=q);     -- assign the current level to all
                                                  -- results not masked as done by q
    
        perform mxv(A, q, q                       -- multiply q<mask> = Aq
            semiring='lor_land_bool',             -- using lor_land_bool semiring
            mask=v,                               -- only those not masked
            desc=desc);                           -- clearing results first
    
        not_done := reduce_bool(q,                -- are there more neighbors
            operator='lor_bool_monoid');          -- to consider?
    
        level := level + 1;                       -- increment the level
    end loop;
    return v;
end;
$$ language plpgsql;
```        

# TODO
    
* Generalize matrix type (DONE)
* polymorphic matrix_agg support all types (DONE)
* overload all the operators (DONE)
* Semiring (DONE)
* Binary Ops (DONE)

* Unary Ops (WIP)
* Assignment (WIP)
* Reduction (WIP)
* Subgraphing (WIP)
* parallel aggregates of subgraphs with final merging (WIP)
* parsable vector/matrix formats for vaild db dumps and literals (WIP)
