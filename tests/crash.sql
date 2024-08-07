create extension if not exists onesparse;
    
\ir gdb.sql
\i /demo/fixtures.sql

select * from sssp_bf((select matrix(array_agg(i), array_agg(j), array_agg(true)) from test), 2);

