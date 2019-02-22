select is(
    ncols(matrix(
        array[:ROWS],
        array[:COLS],
        cast(array[:VALS] as :TYPE []))),
    3::bigint,
    'matrix ncols ' || :'TYPE');
        
select is(
    nrows(matrix(
        array[:ROWS],
        array[:COLS],
        cast(array[:VALS] as :TYPE []))),
    3::bigint,
    'matrix nrows ' || :'TYPE');

select is(
    nvals(matrix(
        array[:ROWS],
        array[:COLS],
        cast(array[:VALS] as :TYPE []))),
     3::bigint,
    'matrix nvals ' || :'TYPE');

select is(
    matrix(array[:ROWS],
            array[:COLS],
            cast(array[:VALS] as :TYPE [])) ||
    matrix(array[:ROWS],
            array[:COLS],
            cast(array[:VALS2] as :TYPE [])),
    matrix(array[:ROWS],
            array[:COLS],
            cast(array[:EAX] as :TYPE [])),
    'matrix ewise add');


select is(
    matrix(array[:ROWS],
            array[:COLS],
            cast(array[:VALS] as :TYPE [])) &&
    matrix(array[:ROWS],
            array[:COLS],
            cast(array[:VALS2] as :TYPE [])),
    matrix(array[:ROWS],
            array[:COLS],
            cast(array[:EMX] as :TYPE [])),
    'matrix ewise mul');

select is(
    matrix(array[:ROWS],
            array[:COLS],
            cast(array[:VALS] as :TYPE [])) *
    matrix(array[:ROWS],
            array[:COLS],
            cast(array[:VALS2] as :TYPE [])),
    matrix(array[:ROWS],
            array[:ROLS],
            cast(array[:MMX] as :TYPE [])),
    'matrix  mul');

