#!/bin/bash
# Three-way scaling benchmark over the graphchallenge .grb corpus:
# matrix_agg (serial binary-counter merge) vs matrix_sum (single concat+build)
# vs matrix_binary_sum (binary reduction tree). matrix_agg is the speedup
# baseline; AGG_MAX caps how high it is run (it is single-threaded, so slower
# than the parallel LAGraph engines). Assumes a table allm(idx, a) is loaded and the
# matrix_sum.sql aggregates are registered. Each query runs ONCE; both its
# result (nvals) and elapsed time are captured from that single run.
set -u
DB=onesparse-bench-db
# matrix_agg is now an O(N log N) binary-counter merge, so it can run across the
# whole sweep; AGG_MAX caps it only if you want to bound wall time. Override from
# the environment, e.g. AGG_MAX=4096 ./run_three_way.sh
AGG_MAX="${AGG_MAX:-4096}"
KS="${KS:-64 128 256 512 1024 2048 4096}"

# run one timed query; print "<nvals>|<ms>", or "CRASH|CRASH" if the backend dies
run() {
  local sql="$1" out nv ms
  out=$(docker exec -i "$DB" psql -U postgres -t -A -c "\timing on" -c "$sql" 2>&1)
  if echo "$out" | grep -q "server closed the connection\|terminating connection\|could not connect\|out of memory"; then
    until docker exec "$DB" psql -o /dev/null -t -q -U postgres -c 'select 1' 2>/dev/null; do sleep 1; done
    echo "CRASH|CRASH"; return
  fi
  nv=$(echo "$out" | grep -vE "Time:|Timing is on\.|^$" | head -1)
  ms=$(echo "$out" | grep -oE "Time: [0-9.]+ ms" | grep -oE "[0-9.]+" | head -1)
  echo "${nv:-?}|${ms:-?}"
}

echo "K|agg_ms|sum_ms|binsum_ms|agg_nvals|sum_nvals|binsum_nvals"
for K in $KS; do
  W="FROM allm WHERE idx < $K"

  IFS='|' read sum_n sum_t <<< "$(run "SELECT nvals(matrix_sum(a)) $W")"
  IFS='|' read bin_n bin_t <<< "$(run "SELECT nvals(matrix_binary_sum(a)) $W")"
  if [ "$K" -le "$AGG_MAX" ]; then
    IFS='|' read agg_n agg_t <<< "$(run "SELECT nvals(matrix_agg(a,'plus_uint32'::binaryop)) $W")"
  else
    agg_n="—"; agg_t="—"
  fi

  echo "$K|$agg_t|$sum_t|$bin_t|$agg_n|$sum_n|$bin_n"
done
