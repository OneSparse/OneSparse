begin;
select plan(132);

\set IDXS 1,4,9
\set MASK false,false,true
\set M_IDXS 9
\set VALS 1,2,3
    
\set EAX 2,4,6
\set EMX 1,4,9
\set M_EAX 6
\set M_EMX 9
\set VRS 6
    
\set TYPE integer
\set SRING PLUS_TIMES_INT32
\ir vector_template.sql    
    
\set TYPE bigint
\set SRING PLUS_TIMES_INT64
\ir vector_template.sql    
    
\set TYPE smallint
\set SRING PLUS_TIMES_INT16
\ir vector_template.sql    
    
\set TYPE real
\set SRING PLUS_TIMES_FP32
\set VALS 1.25,2.5,5.0
\set EAX 2.500000,5.000000,10.000000
\set EMX 1.562500,6.250000,25.000000
\set M_EAX 10.0
\set M_EMX 25.0
\set VRS 8.75
    
\ir vector_template.sql    
    
\set TYPE float
\set SRING PLUS_TIMES_FP64
\ir vector_template.sql    
    
\set TYPE bool
\set SRING LOR_LAND_BOOL
\set VALS true,false,true
\set EAX true,false,true
\set EMX true,false,true
\set M_EAX true
\set M_EMX true
\set VRS true

\ir vector_template.sql
    
select * from finish();
rollback;
