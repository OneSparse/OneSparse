
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
    vector(cast(array[:VALS] as :TYPE [])) =
    vector(cast(array[:VALS] as :TYPE [])),
    true,
    'vector eq ' || :'TYPE');

select is(
    vector(cast(array[:VALS] as :TYPE [])) <>
    vector(cast(array[:VALS] as :TYPE [])),
    false,
    'vector neq ' || :'TYPE');

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
    size(vector(array[:IDXS], cast(array[:VALS] as :TYPE []))),
    10::bigint,
    'sparse size ' || :'TYPE');

select is(
    size(vector(array[:IDXS], cast(array[:VALS] as :TYPE []), 20)),
    20::bigint,
    'sparse explicit size ' || :'TYPE');

select is(
    nvals(vector(array[:IDXS], cast(array[:VALS] as :TYPE []))),
    3::bigint,
    'sparse nvals ' || :'TYPE');

select is(
    vector(array[:IDXS], cast(array[:VALS] as :TYPE [])) +
    vector(array[:IDXS], cast(array[:VALS] as :TYPE [])),
    vector(array[:IDXS], cast(array[:EAX] as :TYPE [])),
    'sparse vector add ' || :'TYPE');

select is(
    ewise_add(vector(array[:IDXS], cast(array[:VALS] as :TYPE [])),
    vector(array[:IDXS], cast(array[:VALS] as :TYPE [])),
    mask=>vector(array[:IDXS], array[:MASK])),
    vector(array[:M_IDXS], cast(array[:M_EAX] as :TYPE [])),
    'sparse vector add masked ' || :'TYPE');

select is(
    vector(array[:IDXS], cast(array[:VALS] as :TYPE [])) *
    vector(array[:IDXS], cast(array[:VALS] as :TYPE [])),
    vector(array[:IDXS], cast(array[:EMX] as :TYPE [])),
    'sparse vector mult ' || :'TYPE');

select is(
    ewise_mult(vector(array[:IDXS], cast(array[:VALS] as :TYPE [])),
    vector(array[:IDXS], cast(array[:VALS] as :TYPE [])),
    mask=>vector(array[:IDXS], array[:MASK])),
    vector(array[:M_IDXS], cast(array[:M_EMX] as :TYPE [])),
    'sparse vector mult masked ' || :'TYPE');

select is(
    vector(array[:IDXS], cast(array[:VALS] as :TYPE [])) =
    vector(array[:IDXS], cast(array[:VALS] as :TYPE [])),
    true,
    'sparse vector eq ' || :'TYPE');

select is(
    vector(array[:IDXS], cast(array[:VALS] as :TYPE [])) <>
    vector(array[:IDXS], cast(array[:VALS] as :TYPE [])),
    false,
    'sparse vector neq ' || :'TYPE');

select is(
    reduce_:TYPE(vector(cast(array[:VALS] as :TYPE [])), :'SRING'),
    cast(:VRS as :TYPE),
    'vector reduce scalar ' || :'TYPE');

select is(
    reduce_:TYPE(vector(cast(array[:VALS] as :TYPE [])), :'SRING'),
    cast(:VRS as :TYPE),
    'vector reduce scalar ' || :'TYPE');

select is(
    assign(vector_:TYPE(2), :VAL),
    vector(cast(array[:VAL, :VAL] AS :TYPE [])),
    'vector assign ' || :'TYPE');

select is(
    nvals(assign(vector_:TYPE(2), cast(:VAL as :TYPE),
                  mask=>vector(array[true,false]))),
    1::bigint,
    'vector assign mask ' || :'TYPE');

select is(
    set_element(assign(vector_:TYPE(2), :VAL), 1, :VAL2),
    vector(cast(array[:VAL, :VAL2] as :TYPE [])),
    'vector set_element ' || :'TYPE');

create table foo_:TYPE (v vector);
insert into foo_:TYPE (v) values (vector(cast(array[:VALS] as :TYPE [])));

select results_eq(
    'select nvals(v) from foo_' || :'TYPE',
    'select 3::bigint',
    'select vector from table ' || :'TYPE');
    
