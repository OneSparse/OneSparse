begin;
select plan(108);

\set IDXS 1,4,9
\set MASK false,false,true
\set M_IDXS 9
\set VALS 1,2,3
    
\set EAX 2,4,6
\set EMX 1,4,9
\set M_EAX 6
\set M_EMX 9

\set TYPE integer
\ir vector_template.sql    
    
\set TYPE bigint
\ir vector_template.sql    
    
\set TYPE smallint
\ir vector_template.sql    
    
\set TYPE real
\set VALS 1.25,2.5,5.0
\set EAX 2.500000,5.000000,10.000000
\set EMX 1.562500,6.250000,25.000000
\set M_EAX 10.0
\set M_EMX 25.0
    
\ir vector_template.sql    
    
\set TYPE float
\ir vector_template.sql    
    
\set TYPE bool
\set VALS true,false,true
\set EAX true,false,true
\set EMX true,false,true
\set M_EAX true
\set M_EMX true

\ir vector_template.sql
    
select * from finish();
rollback;
