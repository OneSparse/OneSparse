-- \set ECHO none
-- \set QUIET 1

-- \pset format unaligned
-- \pset tuples_only true
-- \pset pager

-- \set ON_ERROR_ROLLBACK 1
-- \set ON_ERROR_STOP true
-- \set QUIET 1

create extension pgtap;
create extension pggraphblas;

\i fixture.sql
begin;
select plan(3);

select is(
    '{1,2,3}'::vector
,
    '{1,2,3}'::vector,
    'literal vector');
    
select is(
    '{1,2,3}'::vector + '{2,3,4}'::vector
,
    '{3,5,7}'::vector,
    'literal vector add');

select is(
    '{1,2,3}'::vector * '{2,3,4}'::vector
,
    '{2,6,12}'::vector,
    'literal vector mul');


select * from finish();
rollback;
