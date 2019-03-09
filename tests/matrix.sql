
begin;
select plan(138);

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

\set VAL 1    
\set VALS 1,2,3
\set VALS2 2,3,4
\set TVALS 2,3,1
    
\set EAX 3,5,7
\set EMX 2,6,12
\set MMX 3,8,6
\set VMX 12,2,6
\set VRS 6
    
\set M_EAX 3,5
\set M_EMX 2,6
\set M_MMX 3,8
    
\set A_MMX 4,18,12
    
\set TYPE bigint
\set C_TYPE INT64
\set SRING PLUS_TIMES_INT64
\ir matrix_template.sql    
    
\set TYPE integer
\set C_TYPE INT32
\set SRING PLUS_TIMES_INT32
\ir matrix_template.sql    
    
\set TYPE smallint
\set C_TYPE INT16
\set SRING PLUS_TIMES_INT16
\ir matrix_template.sql

-- real/float expected values
    
\set VAL 1.25    
\set VALS 1.25,2.5,5.0
\set VALS2 2.5,5.0,10.0
\set TVALS 2.5,5.0,1.25
    
\set EAX 3.750000,7.500000,15.000000
\set EMX 3.125000,12.500000,50.000000
\set MMX 6.250000,25.000000,12.500000
\set VMX 50.000000,3.125000,12.500000
\set VRS 8.75
    
\set M_EAX 3.750000,7.500000
\set M_EMX 3.125000,12.500000
\set M_MMX 6.250000,25.000000
        
\set A_MMX 7.812500,62.500000,62.500000
    
\set TYPE real
\set C_TYPE FP32
\set SRING PLUS_TIMES_FP32
\ir matrix_template.sql
    
\set TYPE float
\set C_TYPE FP64
\set SRING PLUS_TIMES_FP64
\ir matrix_template.sql

-- bool
    
\set VAL true
\set VALS true,false,true
\set VALS2 true,true,false
\set TVALS false,true,true
    
\set EAX true,true,true
\set EMX true,false,false
\set MMX true,false,true
\set VMX false,true,false
\set VRS true
    
\set M_EAX true,true
\set M_EMX true,false
\set M_MMX true,false
    
\set A_MMX false,false,false
    
\set TYPE bool
\set C_TYPE BOOL
\set SRING LOR_LAND_BOOL
\ir matrix_template.sql
    
select * from finish();
rollback;
