create extension if not exists pggraphblas;
    
\ir gdb.sql

select bfs_matrix(matrix(array_agg(i), array_agg(j), array_agg(true)), 4) from test;
    
