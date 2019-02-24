
select is(
    vector(cast(array[:VALS] as :TYPE [])),
    cast(array[:VALS] as :TYPE [])::vector,
    'literal vector casting ' || :'TYPE');

select is(
    size(vector_:TYPE(10)),
    10::bigint,
    'empty vector size ' || :'TYPE');
    
select is(
    nvals(vector_:TYPE(10)),
    0::bigint,
    'empty vector nvals ' || :'TYPE');
    
select is(
    size(vector(cast(array[:VALS] as :TYPE []))),
    3::bigint,
    'vector size ' || :'TYPE');

select is(
    nvals(vector(cast(array[:VALS] as :TYPE []))),
    3::bigint,
    'vector nvals ' || :'TYPE');

select is(
    size(vector(cast(array[:VALS] as :TYPE []))),
    nvals(vector(cast(array[:VALS] as :TYPE []))),
    'vector size equals vals ' || :'TYPE');

select is(
    vector(cast(array[:VALS] as :TYPE [])) +
    vector(cast(array[:VALS] as :TYPE [])),
    cast(array[:EAX] as :TYPE [])::vector,
    'vector ewise add ' || :'TYPE');

select is(
    vector(cast(array[:VALS] as :TYPE [])) *
    vector(cast(array[:VALS] as :TYPE [])),
    cast(array[:EMX] as :TYPE [])::vector,
    'vector ewise mul ' || :'TYPE');

select is(
    vector(cast(array[:VALS] as :TYPE [])) =
    vector(cast(array[:VALS] as :TYPE [])),
    true,
    'vector equal ' || :'TYPE');

select is(
    vector(cast(array[:VALS] as :TYPE [])) <>
    vector(cast(array[:VALS] as :TYPE [])),
    false,
    'vector not equal ' || :'TYPE');

-- sparse construction, second array defines indices

select is(
    size(vector(cast(array[:VALS] as :TYPE []), array[:IDXS])),
    10::bigint,
    'sparse size ' || :'TYPE');

select is(
    size(vector(cast(array[:VALS] as :TYPE []), array[:IDXS], 20)),
    20::bigint,
    'sparse explicit size ' || :'TYPE');

select is(
    nvals(vector(cast(array[:VALS] as :TYPE []), array[:IDXS])),
    3::bigint,
    'sparse nvals ' || :'TYPE');

select is(
    vector(cast(array[:VALS] as :TYPE []), array[:IDXS]) +
    vector(cast(array[:VALS] as :TYPE []), array[:IDXS]),
    vector(cast(array[:EAX] as :TYPE []), array[:IDXS]),
    'sparse vector add ' || :'TYPE');

select is(
    ewise_add(vector(cast(array[:VALS] as :TYPE []), array[:IDXS]),
    vector(cast(array[:VALS] as :TYPE []), array[:IDXS]),
    mask=>vector(array[:MASK], array[:IDXS])),
    vector(cast(array[:M_EAX] as :TYPE []), array[:M_IDXS]),
    'sparse vector add masked ' || :'TYPE');
    
select is(
    vector(cast(array[:VALS] as :TYPE []), array[:IDXS]) *
    vector(cast(array[:VALS] as :TYPE []), array[:IDXS]),
    vector(cast(array[:EMX] as :TYPE []), array[:IDXS]),
    'sparse vector mult ' || :'TYPE');

select is(
    ewise_mult(vector(cast(array[:VALS] as :TYPE []), array[:IDXS]),
    vector(cast(array[:VALS] as :TYPE []), array[:IDXS]),
    mask=>vector(array[:MASK], array[:IDXS])),
    vector(cast(array[:M_EMX] as :TYPE []), array[:M_IDXS]),
    'sparse vector mult masked ' || :'TYPE');
    
select is(
    vector(cast(array[:VALS] as :TYPE []), array[:IDXS]) =
    vector(cast(array[:VALS] as :TYPE []), array[:IDXS]),
    true,
    'sparse vector eq ' || :'TYPE');

select is(
    vector(cast(array[:VALS] as :TYPE []), array[:IDXS]) <>
    vector(cast(array[:VALS] as :TYPE []), array[:IDXS]),
    false,
    'sparse vector neq ' || :'TYPE');

