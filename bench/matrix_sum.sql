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

-- A PLUS-capable matrix_agg for an apples-to-apples baseline. The shipped
-- 1-arg matrix_agg defaults to TIMES; matrix_agg_matrix already accepts an
-- optional binary operator, so we just expose a 2-arg aggregate form here.
CREATE FUNCTION matrix_agg_matrix(state matrix, a matrix, op binaryop)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_agg_matrix'
LANGUAGE C STABLE;

CREATE AGGREGATE matrix_agg (a matrix, op binaryop)
    (SFUNC=matrix_agg_matrix, STYPE=matrix, FINALFUNC=matrix_agg_final);
