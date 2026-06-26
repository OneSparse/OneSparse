-- Benchmark + correctness: matrix_sum (LAGraph single build) vs matrix_binary_sum
-- (LAGraph binary reduction) vs matrix_agg (binary-counter merge). Run manually;
-- not auto-discovered by the regression suite. Requires bench/matrix_sum.sql
-- loaded first (matrix_agg itself ships in the stock extension):
--
--     CREATE EXTENSION onesparse;
--     \i bench/matrix_sum.sql
--     \i bench/matrix_sum_bench.sql
--
-- Notes on the inputs:
--   * All engines must see the SAME matrices and require the inputs to share
--     dimensions (a GrB_eWiseAdd / build requirement). We resize the bounded
--     random_matrix output to unbounded with resize(m, -1, -1) so the whole set
--     is uniform.
--   * Use an integer type so PLUS is exactly associative and the results are
--     bit-identical (FP reassociation under build/reduction can differ).
--   * Compare with the matrix = operator, NOT print(): print() cannot render an
--     unbounded matrix (its logical dimension is 2^60).

\timing on

-- Correctness on a modest set: the build-based sum and the pairwise PLUS
-- aggregate must agree exactly.
DROP TABLE IF EXISTS bench_small;
CREATE TABLE bench_small AS
  SELECT g AS id, resize(random_matrix('int64', 200, 200, 0.05, g), -1, -1) AS a
  FROM generate_series(1, 50) g;

-- All three summing engines must agree exactly (matrix_agg now defaults to PLUS).
SELECT (matrix_agg(a) = matrix_sum(a))         AS agg_eq_sum,
       (matrix_sum(a) = matrix_binary_sum(a))  AS sum_eq_binary_sum
FROM bench_small;

-- Larger fixture for timing. Tune the count / dimensions / density (overlap)
-- to explore where the single internally-parallel build wins.
DROP TABLE IF EXISTS bench_big;
CREATE TABLE bench_big AS
  SELECT g AS id, resize(random_matrix('int64', 1000, 1000, 0.005, g), -1, -1) AS a
  FROM generate_series(1, 1000) g;

-- Warm up, then compare wall-clock (\timing). nvals() forces full evaluation
-- while keeping the printed output tiny.
SELECT nvals(matrix_agg(a))                          AS agg_nvals        FROM bench_big;
SELECT nvals(matrix_sum(a))                          AS sum_nvals        FROM bench_big;
SELECT nvals(matrix_binary_sum(a))                   AS binary_sum_nvals FROM bench_big;

-- GraphBLAS thread count affects the internally-parallel build. There is no
-- SQL GUC for it; set OMP_NUM_THREADS in the server's environment before
-- starting PostgreSQL and re-run, sweeping e.g. 1, 2, 4, 8 to find the knee.
