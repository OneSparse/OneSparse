begin;
select plan(7);

-- "dense" vectors constructed from 1-d arrays, indices go from 0...n

select is(
    vector(array[1,2,3]),
    array[1,2,3]::vector,
    'literal vector casting');
    
select is(
    size(vector(array[1,2,3])),
    3::bigint,
    'vector size');
    
select is(
    nvals(vector(array[1,2,3])),
    3::bigint,
    'vector nvals');
    
select is(
    vector(array[1,2,3]) + vector(array[1,2,3]),
    array[2,4,6]::vector,
    'vector ewise add');
    
select is(
    vector(array[1,2,3]) * vector(array[1,2,3]),
    array[1,4,9]::vector,
    'vector ewise mul');
    
select is(
    vector(array[1,2,3]) = vector(array[1,2,3]),
    true,
    'vector equal');

select is(
    vector(array[1,2,3]) <> vector(array[1,2,3]),
    false,
    'vector not equal');
    
select * from finish();
rollback;
