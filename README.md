# pggraphblas
High Performance Graph Processing with Postgres using Linear Algebra

GraphBLAS is a sparse linear algebra API optimized for processing
graphs encoded as sparse matrices and vectors.  In addition to common
real/integer matrix algebras, GraphBLAS supports up to 960 different
"semiring" algebras, that can be used to sparsely encode and solve
graph problems.

[GraphBLAS Mathematics](http://www.mit.edu/~kepner/GraphBLAS/GraphBLAS-Math-release.pdf)

[Lower Latency Graph Queries in Cypher with Redis GraphRoi Lipman, Redis LabsTim Davis, Texas A&M U](https://www.youtube.com/watch?v=xnez6tloNSQ)

[A good introduction to semirings Part I](https://www.youtube.com/watch?v=Gd_VT_Nj8Xw) [Part II](https://www.youtube.com/watch?v=dluPFbuq6zs)

[A good introduction to abstract algebra](https://www.youtube.com/playlist?list=PLi01XoE8jYoi3SgnnGorR_XOW3IcK-TP6)

[Graph algorithms via SuiteSparse:GraphBLAS: triangle counting and K-truss](http://faculty.cse.tamu.edu/davis/GraphBLAS_files/Davis_HPEC18.pdf)

# development

If you have docker installed, run `./test.sh` to build a docker
container with postgres:11 and GraphBLAS compiled with debug symbols
on.  This will automatically run test.sql.  If you provide any
argument to test.sh the container will not be immediately cleaned up,
and you can access the database directly with `./psql`

DONE-ish:

* Matrix type for INT64 (float4)
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
