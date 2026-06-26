-- Low-overlap benchmark: matrix_sum (LAGraph single build) vs matrix_agg
-- (binary-counter merge) over a corpus of serialized GraphBLAS matrices (.grb
-- files produced by GxB_Matrix_serialize), such as ~/dev/NSC/data/grb.
--
-- Requires OneSparse built with WITH_MATRIX_SUM=1 against the LAGraph
-- "matrix-sum" branch, and bench/matrix_sum.sql loaded first. The .grb corpus
-- must be readable by the server; mount/point it at /data (so /data/manifest.csv
-- and /data/<inner_tar>/<n>.grb exist), then:
--
--     CREATE EXTENSION onesparse;
--     \i bench/matrix_sum.sql
--     \i bench/matrix_sum_grb_bench.sql
--
-- Why resize(..., -1, -1): all engines require the inputs to share dimensions
-- (a GrB_eWiseAdd / build requirement). Resizing every matrix to unbounded
-- (GxB_INDEX_MAX) makes the whole corpus uniform regardless of its stored dims.
-- The corpus is uint32, so the PLUS dup operator is plus_uint32; every engine
-- defaults to PLUS.

\timing on

-- The manifest lists every matrix: global_index, inner_tar, n, rel_path, bytes.
DROP TABLE IF EXISTS manifest;
CREATE TABLE manifest(global_index int, inner_tar text, n int, rel_path text, bytes bigint);
COPY manifest FROM '/data/manifest.csv' CSV HEADER;

-- Load the first :k matrices into a table, resized to unbounded. Set :k on the
-- psql command line, e.g.  psql -v k=512 -f bench/matrix_sum_grb_bench.sql
\if :{?k}
\else
  \set k 256
\endif
\echo loading :k matrices

DROP TABLE IF EXISTS bench;
CREATE TABLE bench AS
  SELECT resize(deserialize_file('/data/' || rel_path), -1, -1) AS a
  FROM manifest WHERE global_index < :k;

-- Overlap check: with a low/no-overlap corpus, the union nvals approaches the
-- sum of the per-matrix nvals (so the result is large), while matrix_sum
-- sorts/builds once.
SELECT sum(nvals(a)) AS total_input_nvals, nvals(matrix_sum(a)) AS union_nvals FROM bench;

-- matrix_sum: one internally-parallel LAGraph build.
-- PEAK MEMORY: LAGraph_Matrix_Sum materializes every input tuple at once
-- (peak ~= sum of all input nvals) before the dedup build. On a zero-overlap
-- corpus this grows linearly with :k; measured on a 39 GB box, k=2048
-- (268M tuples) succeeded in ~44s but k=4096 (536M tuples) OOM-crashed the
-- backend. Watch RSS and cap :k accordingly, or use the memory-bounded
-- per-worker-build hybrid (see the companion design plan).
\echo -- matrix_sum (LAGraph build) --
SELECT nvals(matrix_sum(a)) AS sum_nvals FROM bench;

-- matrix_binary_sum: same result, different LAGraph summing technique
-- (binary reduction tree of eWiseAdds rather than a single concatenate+build).
-- Different parallel/time/space tradeoff; compare its wall time against
-- matrix_sum's above.
\echo -- matrix_binary_sum (LAGraph binary reduction) --
SELECT nvals(matrix_binary_sum(a)) AS binary_sum_nvals FROM bench;

-- matrix_agg: serial binary-counter merge (O(N log N), no LAGraph dependency).
-- Single-threaded so slower than the parallel LAGraph engines above, but frees
-- each input as it is absorbed, so it is memory-frugal and reaches high :k.
\echo -- matrix_agg (binary-counter merge) --
SELECT nvals(matrix_agg(a, 'plus_uint32'::binaryop)) AS agg_nvals FROM bench;
