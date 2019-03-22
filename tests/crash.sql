create extension if not exists pggraphblas;
    
\ir gdb.sql
\i /demo/fixtures.sql

select * from matrix_pagerank((select matrix(array_agg(i), array_agg(j), array_agg(true)) from test));
    
