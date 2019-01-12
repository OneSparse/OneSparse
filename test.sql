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
select plan(1);

select lives_ok($$select '{1,2,3}'::vector$$, 'lives_ok');

select * from finish();
rollback;
