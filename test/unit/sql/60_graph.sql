BEGIN;

CREATE EXTENSION IF NOT EXISTS onesparse;
CREATE EXTENSION IF NOT EXISTS pgtap;
SET LOCAL search_path = public, onesparse;

SELECT plan(1);
SELECT pass('graph test skeleton is available');

-- BFS, SSSP, PageRank, components, and centrality algorithms.

SELECT * FROM finish();

ROLLBACK;
