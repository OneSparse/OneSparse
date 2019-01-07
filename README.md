# pggraphblas
Postgres extension wrapping GraphBLAS

If you have docker installed, run `./test.sh` to build a docker with
postgres:11 and GraphBLAS compiled with debug symbols on.  This will
automatically run test.sql.  If you provide any argument to test.sh
the container will not be immediately cleaned up, and you can access
the database directly with `./psql`

DONE-ish:
    - Matrix type for FP32 (float4)
    - matrix_agg to GrB_Matrix_build
    - matrix_extract from GrB_Matrix_extractTuples

TODO:
    - Generalize matrix type
    - polymorphic matrix_agg support all types
    - overload all the operators
    - everything above but for vectors

UNKNOWNS:
    - operation descriptors, how to mask, invert, clear destination?
    - storage options?  CRS? CCS?        
