
begin;
select plan(72);

-- The following variables are substituted into matrix_template.sql.
-- Each support type is defined and the expected values, the template
-- is then expanded once for each type.

-- common values
    
\set ROWS 0,1,2
\set ROLS 2,0,1
\set COLS 1,2,0
    
\set M_ROWS 0,1
\set M_ROLS 2,0
\set M_COLS 1,2
\set M_VALS true,true,false

-- Integer expected values
    
\set VALS 1,2,3
\set VALS2 2,3,4
    
\set EAX 3,5,7
\set EMX 2,6,12
\set MMX 3,8,6
    
\set M_EAX 3,5
\set M_EMX 2,6
\set M_MMX 3,8
    
\set TYPE bigint
\ir matrix_template.sql    
    
\set TYPE integer
\ir matrix_template.sql    
    
\set TYPE smallint
\ir matrix_template.sql

-- real/float expected values
    
\set VALS 1.25,2.5,5.0
\set VALS2 2.5,5.0,10.0
    
\set EAX 3.750000,7.500000,15.000000
\set EMX 3.125000,12.500000,50.000000
\set MMX 6.250000,25.000000,12.500000
    
\set M_EAX 3.750000,7.500000
\set M_EMX 3.125000,12.500000
\set M_MMX 6.250000,25.000000
        
\set TYPE real
\ir matrix_template.sql
    
\set TYPE float
\ir matrix_template.sql

-- bool
    
\set VALS true,false,true
\set VALS2 true,true,false
    
\set EAX true,true,true
\set EMX true,false,false
\set MMX true,false,true
    
\set M_EAX true,true
\set M_EMX true,false
\set M_MMX true,false
    
\set TYPE bool
\ir matrix_template.sql
    
select * from finish();
rollback;
