begin;
select plan(84);

\set IDXS 1,4,9
\set VALS 1,2,3
    
\set EAX 2,4,6
\set EMX 1,4,9

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
    
\ir vector_template.sql    
    
\set TYPE float
\ir vector_template.sql    
    
\set TYPE bool
\set VALS true,false,true
\set EAX true,false,true
\set EMX true,false,true
    
\ir vector_template.sql
    
select * from finish();
rollback;
