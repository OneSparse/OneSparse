select is(
    ncols(matrix(
        array[:ROWS],
        array[:COLS],
        cast(array[:A,:B,:C] as :TYPE []))),
    3::bigint,
    'matrix ncols ' || :'TYPE');
        
select is(
    nrows(matrix(
        array[:ROWS],
        array[:COLS],
        cast(array[:A,:B,:C] as :TYPE []))),
    3::bigint,
    'matrix nrows ' || :'TYPE');

select is(
    nvals(matrix(
        array[:ROWS],
        array[:COLS],
        cast(array[:A,:B,:C] as :TYPE []))),
     3::bigint,
    'matrix nvals ' || :'TYPE');

select is(
    matrix(array[:ROWS],
            array[:COLS],
            cast(array[:A,:B,:C] as :TYPE [])) ||
    matrix(array[:ROWS],
            array[:COLS],
            cast(array[:D,:E,:F] as :TYPE [])),
    matrix(array[:ROWS],
            array[:COLS],
            cast(array[:EAX,:EAY,:EAZ] as :TYPE [])),
    'matrix ewise add');


