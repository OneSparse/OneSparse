\set ECHO none
\set QUIET 1

\pset format unaligned
\pset tuples_only true
\pset pager

\set ON_ERROR_ROLLBACK 1
\set ON_ERROR_STOP true
\set QUIET 1

create extension if not exists pgtap;
create extension if not exists pggraphblas;

begin;
select plan(15);

-- vectors
        
select is(
    '{1,2,3}'::vector,
    '{1,2,3}'::vector,
    'literal vector');
    
select is(
    '{1,2,3}'::vector + '{2,3,4}'::vector,
    '{3,5,7}'::vector,
    'literal vector add');

select is(
    '{1,2,3}'::vector * '{2,3,4}'::vector,
    '{2,6,12}'::vector,
    'literal vector mul');

select is(
    vector_size('{1,2,3}'::vector), 3::bigint,
    'literal vector size');

select is(
    vector_nvals('{1,2,3}'::vector), 3::bigint,
    'literal vector nvals');

-- sparse vectors
    
select is(
    '{{1,4,9},{1,2,3}}'::vector,
    '{{1,4,9},{1,2,3}}'::vector,
    'sparse literal vector');
    
select is(
    '{{1,4,9},{1,2,3}}'::vector + '{{1,4,9},{2,3,4}}'::vector,
    '{{1,4,9},{3,5,7}}'::vector,
    'sparse literal vector add');

select is(
    '{{1,4,9},{1,2,3}}'::vector * '{{1,4,9},{2,3,4}}'::vector,
    '{{1,4,9},{2,6,12}}'::vector,
    'sparse literal vector mul');

select is(
    vector_size('{{1,4,9},{1,2,3}}'::vector), 10::bigint,
    'sparse literal vector size');

select is(
    vector_nvals('{{1,4,9},{1,2,3}}'::vector), 3::bigint,
    'sparse literal vector nvals');
    
-- matrices
        
select is(
    '{{0,1,2},{1,2,0},{4,5,6}}'::matrix,
    '{{0,1,2},{1,2,0},{4,5,6}}'::matrix,
    'literal matrix');

select is(
    '{{0,1,2},{1,2,0},{4,5,6}}'::matrix || '{{0,1,2},{1,2,0},{4,5,6}}'::matrix,
    '{{0,1,2},{1,2,0},{8,10,12}}'::matrix,
    'literal matrix ewise add');

select is(
    '{{0,1,2},{1,2,0},{4,5,6}}'::matrix && '{{0,1,2},{1,2,0},{4,5,6}}'::matrix,
    '{{0,1,2},{1,2,0},{16,25,36}}'::matrix,
    'literal matrix ewise mul');
    
select is(
    '{{0,1,2},{1,2,0},{4,5,6}}'::matrix * '{{0,1,2},{1,2,0},{4,5,6}}'::matrix,
    '{{0,1,2},{2,0,1},{20,30,24}}'::matrix,
    'literal matrix mul');
    
create table graph (
    i bigint,
    j bigint,
    v bigint
    );

insert into graph (i, j, v) values 
    (1, 4, 1),
    (1, 2, 2),
    (2, 7, 3),
    (2, 5, 4),
    (3, 6, 5),
    (4, 3, 6),
    (4, 1, 7),
    (5, 6, 8),
    (6, 3, 9),
    (7, 3, 10);

create function matrix_from_table() returns matrix as $$
    declare m matrix;
    begin
        select matrix_agg(i, j, v) from graph into m;
        return m;
    end;
$$ language plpgsql;

select is(
    matrix_from_table(),
    '{{1,1,2,2,3,4,4,5,6,7},{2,4,5,7,6,1,3,6,3,3},{2,1,4,3,5,7,6,8,9,10}}'::matrix,
    'matrix from table.');
        
select * from finish();
rollback;

