begin;
select plan(5);

\set ROWS 0,1,2
\set COLS 0,1,2
    
\set A 1
\set B 2
\set C 3
\set D 1
\set E 2
\set F 3

\set EAX 2
\set EAY 4
\set EAZ 6
    
\set TYPE integer
\ir matrix_template.sql    
    
\set TYPE bigint
\ir matrix_template.sql    
    
\set TYPE smallint
\ir matrix_template.sql    
    
\set TYPE real
\set A 1.1
\set B 2.2
\set C 3.3
\set A 1.1
\set B 2.2
\set C 3.3
    
\set EAX 2.1
\set EAY 4.2
\set EAZ 6.3

\ir matrix_template.sql    
    
\set TYPE float
\ir matrix_template.sql    
    
\set TYPE bool
\set A true
\set B false
\set C true
\set D true
\set E true
\set F false
    
\set EAX true
\set EAY true
\set EAZ true
    
\ir matrix_template.sql
    
select * from finish();
rollback;
