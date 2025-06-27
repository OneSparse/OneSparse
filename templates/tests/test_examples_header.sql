\pset linestyle unicode
\pset border 2
-- # Algorithms
--
-- OneSparse leverages the LAGraph suite of graph algorithms
-- implemented with the GraphBLAS API.
--
create materialized view karate as select mmread('/home/postgres/onesparse/demo/karate.mtx') as graph;
--
-- ## Level BFS
--
select draw(tril(graph), (select level from bfs(graph, 1)), false, false, true, 0.5)::text as draw_source from karate \gset
\i sql/draw_sfdp.sql
--
--
-- ## Parent BFS
--
select draw(tril(graph), (select parent from bfs(graph, 1)), false, false, true, 0.5)::text as draw_source from karate \gset
\i sql/draw_sfdp.sql
--
--
-- ## Page Rank
--
select draw(tril(graph), pagerank(graph), false, false, true, 0.5)::text as draw_source from karate \gset
\i sql/draw_sfdp.sql
--
--
-- ## Triangle Centrality
--
select draw(tril(graph), triangle_centrality(graph), false, false, true, 0.5)::text as draw_source from karate \gset
\i sql/draw_sfdp.sql
--
-- ## Betweeness Centrality
--
select draw(tril(graph), betweenness(graph, ARRAY[1,32]::bigint[]), false, false, true, 0.5)::text as draw_source from karate \gset
\i sql/draw_sfdp.sql
--
--
-- ## Square Clustering
--
select draw(tril(graph), square_clustering(graph), false, false, true, 0.5)::text as draw_source from karate \gset
\i sql/draw_sfdp.sql
