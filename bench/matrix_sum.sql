-- Experimental matrix_sum aggregate.
--
-- Requires OneSparse built with WITH_MATRIX_SUM=1 against the LAGraph
-- "matrix-sum" branch (https://github.com/GraphBLAS/LAGraph/pull/407), which
-- provides LAGraph_Matrix_Sum. Kept out of the generated extension SQL so stock
-- builds and CREATE EXTENSION are unaffected; load it manually after
-- CREATE EXTENSION onesparse:
--
--     \i bench/matrix_sum.sql
--
-- matrix_sum collects the per-row matrix handles during the scan and performs a
-- single LAGraph_Matrix_Sum (extract-tuples + one GrB_Matrix_build) at finalize,
-- combining duplicate (i,j) entries with a dup operator that defaults to PLUS.
-- An optional binaryop argument overrides the dup operator.

CREATE FUNCTION matrix_sum_matrix(state internal, a matrix)
RETURNS internal
AS '$libdir/onesparse', 'matrix_sum_matrix'
LANGUAGE C;

CREATE FUNCTION matrix_sum_matrix(state internal, a matrix, op binaryop)
RETURNS internal
AS '$libdir/onesparse', 'matrix_sum_matrix'
LANGUAGE C;

CREATE FUNCTION matrix_sum_final(state internal)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_sum_final'
LANGUAGE C STRICT;

CREATE AGGREGATE matrix_sum (a matrix)
    (SFUNC=matrix_sum_matrix, STYPE=internal, FINALFUNC=matrix_sum_final);

CREATE AGGREGATE matrix_sum (a matrix, op binaryop)
    (SFUNC=matrix_sum_matrix, STYPE=internal, FINALFUNC=matrix_sum_final);

-- matrix_binary_sum: same interface and result as matrix_sum, but uses
-- LAGraph_Matrix_Binary_Sum (a different summing technique inside LAGraph) for
-- a different parallel/time/space tradeoff.
CREATE FUNCTION matrix_binary_sum_matrix(state internal, a matrix)
RETURNS internal
AS '$libdir/onesparse', 'matrix_binary_sum_matrix'
LANGUAGE C;

CREATE FUNCTION matrix_binary_sum_matrix(state internal, a matrix, op binaryop)
RETURNS internal
AS '$libdir/onesparse', 'matrix_binary_sum_matrix'
LANGUAGE C;

CREATE FUNCTION matrix_binary_sum_final(state internal)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_binary_sum_final'
LANGUAGE C STRICT;

CREATE AGGREGATE matrix_binary_sum (a matrix)
    (SFUNC=matrix_binary_sum_matrix, STYPE=internal, FINALFUNC=matrix_binary_sum_final);

CREATE AGGREGATE matrix_binary_sum (a matrix, op binaryop)
    (SFUNC=matrix_binary_sum_matrix, STYPE=internal, FINALFUNC=matrix_binary_sum_final);

-- matrix_agg(a matrix) and matrix_agg(a matrix, op binaryop) now ship in the
-- stock extension (binary-counter merge, default dup PLUS), so no bench-only
-- definition is needed here.
