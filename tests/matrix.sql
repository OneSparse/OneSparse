begin;
select plan(30);

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
    
\set EMX 1
\set EMY 4
\set EMZ 9
    
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

\set EMX 1.1
\set EMY 4.4
\set EMZ 9.9
    
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
    
\set EMX true
\set EMY false
\set EMZ false
    
\ir matrix_template.sql
    
select * from finish();
rollback;
