begin;
select plan(1);

-- "dense" vectors constructed from 1-d arrays, indices go from 0...n

select is(
    vector(array[1,2,3]::bigint[]),
    array[1,2,3]::bigint[]::vector,
    'literal vector casting');
    
select * from finish();
rollback;
