# pggraphblas
Postgres extension wrapping GraphBLAS

If you have docker installed, run `./test.sh` to build a docker with
postgres:11 and GraphBLAS compiled with debug symbols on.  This will
automatically run test.sql.  If you provide any argument to test.sh
the container will not be immediately cleaned up, and you can access
the database directly with `./psql`

DONE-ish:
    
* Matrix type for INT64 (float4)
* matrix_agg/matrix_tuples to GrB_Matrix_build
* matrix_tuples from GrB_Matrix_extractTuples
* vector type for INT64 (float4)
* vector_agg to GrB_Matrix_build
* matrix_tuples from GrB_Vector_extractTuples
* most of the GrB_PLUS_TIMES_INT64 semi-ring.
* overloaded operators for plus,times,set-union/intersection

TODO:
    
* Generalize matrix type
* polymorphic matrix_agg support all types
* overload all the operators
* parallel aggregates of subgraphs with final merging

UNKNOWNS:
    
* operation descriptors, how to mask, invert, clear destination?
* storage options?  CRS? CCS? Tim Davis recommends just storing
  extracted tuples for now
