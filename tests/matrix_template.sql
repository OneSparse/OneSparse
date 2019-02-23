
-- This template is included from matrix.sql multiple times per type
-- each test is expanded several times per type

select is(
    nrows(matrix_:TYPE(10, 10)),
    10::bigint,
    'empty construction nrows. ' || :'TYPE');

select is(
    ncols(matrix_:TYPE(10, 10)),
    10::bigint,
    'empty construction ncols. ' || :'TYPE');
    
select is(
    nvals(matrix_:TYPE(10, 10)),
    0::bigint,
    'empty construction nvals. ' || :'TYPE');

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
  'matrix ewise add ' || :'TYPE');


select is(
  ewise_add(matrix(
      array[:ROWS],
      array[:COLS],
      cast(array[:VALS] as :TYPE [])),
    matrix(
      array[:ROWS],
      array[:COLS],
      cast(array[:VALS2] as :TYPE [])),
    mask=>matrix(
      array[:ROWS],
      array[:COLS],
      array[:M_VALS])
  ),
  matrix(array[:M_ROWS],
          array[:M_COLS],
          cast(array[:M_EAX] as :TYPE []), 3, 3),
  'matrix ewise add masked ' || :'TYPE');

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
  'matrix ewise mul ' || :'TYPE');

select is(
  ewise_mult(matrix(
      array[:ROWS],
      array[:COLS],
      cast(array[:VALS] as :TYPE [])),
    matrix(
      array[:ROWS],
      array[:COLS],
      cast(array[:VALS2] as :TYPE [])),
    mask=>matrix(
      array[:ROWS],
      array[:COLS],
      array[:M_VALS])
  ),
  matrix(array[:M_ROWS],
      array[:M_COLS],
      cast(array[:M_EMX] as :TYPE []), 3, 3),
  'matrix ewise mult masked ' || :'TYPE');

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
  'matrix mxm ' || :'TYPE');

select is(
  mxm(matrix(
    array[:ROWS],
    array[:COLS],
    cast(array[:VALS] as :TYPE [])),
  matrix(
    array[:ROWS],
    array[:COLS],
    cast(array[:VALS2] as :TYPE [])),

  mask=>matrix(
    array[:ROWS],
    array[:ROLS],
    array[:M_VALS]),
    
  doutp=>'default',
  dmask=>'default',
  dinp0=>'default',
  dinp1=>'default'
  ),
  matrix(array[:M_ROWS],
    array[:M_ROLS],
    cast(array[:M_MMX] as :TYPE []), 3, 3),
  'matrix mxm masked ' || :'TYPE');

select is(
  mxm(
    matrix(
      array[:ROWS],
      array[:COLS],
      cast(array[:VALS] as :TYPE [])),
    matrix(
      array[:ROWS],
      array[:COLS],
      cast(array[:VALS] as :TYPE [])),
    matrix(
      array[:ROWS],
      array[:ROLS],
      cast(array[:VALS2] as :TYPE [])),
    accum=>'TIMES_' || :'C_TYPE'),
    matrix(
      array[:ROWS],
      array[:ROLS],
      cast(array[:A_MMX] as :TYPE [])),
    'matrix mxm accum ' || :'TYPE');

select is(
  matrix(array[:ROWS],
    array[:COLS],
    cast(array[:VALS] as :TYPE [])) *
  vector(cast(array[:VALS2] as :TYPE [])),

  vector(cast(array[:MMX] as :TYPE [])),
  'matrix mxv ' || :'TYPE');

select is(
  vector(cast(array[:VALS2] as :TYPE [])) *
  matrix(array[:ROWS],
    array[:COLS],
    cast(array[:VALS] as :TYPE [])),

  vector(cast(array[:VMX] as :TYPE [])),
  'matrix vxm ' || :'TYPE');

