begin;
select plan(36);

\set ROWS 0,1,2
\set COLS 1,2,0
\set ROLS 2,0,1
\set VALS 1,2,3
\set VALS2 2,3,4

\set EAX 3,5,7
\set EMX 2,6,12
\set MMX 3,8,6
    
\set TYPE integer
\ir matrix_template.sql    
    
\set TYPE bigint
\ir matrix_template.sql    
    
\set TYPE smallint
\ir matrix_template.sql    
    
\set TYPE real
\set VALS 1.25,2.5,5.0
\set VALS2 2.5,5.0,10.0
    
\set EAX 3.750000,7.500000,15.000000
\set EMX 3.125000,12.500000,50.000000
\set MMX 6.250000,25.000000,12.500000
    
\ir matrix_template.sql    
    
\set TYPE float
\ir matrix_template.sql    
    
\set TYPE bool
\set VALS true,false,true
\set VALS2 true,true,false
    
\set EAX true,true,true
\set EMX true,false,false
\set MMX true,false,true
    
\ir matrix_template.sql
    
select * from finish();
rollback;
