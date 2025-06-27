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
-- Level BFS exposes the depth of each vertex starting from a
-- given source vertex using the breadth-first search algorithm.
-- See [https://en.wikipedia.org/wiki/Breadth-first_search](https://en.wikipedia.org/wiki/Breadth-first_search) for details.
--
select draw(tril(graph), (select level from bfs(graph, 1)), false, false, true, 0.5)::text as draw_source from karate \gset
\i sql/draw_sfdp.sql
--
--
-- ## Parent BFS
-- Parent BFS returns the predecessor of each vertex in the
-- BFS tree rooted at the chosen source. It is also based on
-- [https://en.wikipedia.org/wiki/Breadth-first_search](https://en.wikipedia.org/wiki/Breadth-first_search).
--
select draw(tril(graph), (select parent from bfs(graph, 1)), false, false, true, 0.5)::text as draw_source from karate \gset
\i sql/draw_sfdp.sql
--
-- ### BFS Benchmarks
-- ![BFS Benchmarks](images/BFS.svg)
--
-- ## Page Rank
-- PageRank assigns an importance score to each vertex based on link structure.
-- For details see [https://en.wikipedia.org/wiki/PageRank](https://en.wikipedia.org/wiki/PageRank).
--
select draw(tril(graph), pagerank(graph)*100, false, false, true, 0.5)::text as draw_source from karate \gset
\i sql/draw_sfdp.sql
--
-- ### PageRank Benchmarks
-- ![PageRank Benchmarks](images/PageRank.svg)
--
-- ## Triangle Centrality
-- Triangle centrality counts the number of triangles incident to each vertex.
-- This measure relates to the clustering coefficient
-- [https://en.wikipedia.org/wiki/Clustering_coefficient](https://en.wikipedia.org/wiki/Clustering_coefficient).
--
select draw(tril(graph), triangle_centrality(graph)*10, false, false, true, 0.5)::text as draw_source from karate \gset
\i sql/draw_sfdp.sql
--
-- ## Betweeness Centrality
-- Betweenness centrality measures how often a vertex appears on shortest paths between others.
-- [https://en.wikipedia.org/wiki/Betweenness_centrality](https://en.wikipedia.org/wiki/Betweenness_centrality).
--
select draw(tril(graph), betweenness(graph, ARRAY[1,32]::bigint[]), false, false, true, 0.5)::text as draw_source from karate \gset
\i sql/draw_sfdp.sql
--
-- ## Square Clustering
-- Calculates the square clustering coefficient for each vertex.
-- [https://en.wikipedia.org/wiki/Clustering_coefficient](https://en.wikipedia.org/wiki/Clustering_coefficient).
--
select draw(tril(graph), square_clustering(graph), false, false, true, 0.5)::text as draw_source from karate \gset
\i sql/draw_sfdp.sql
