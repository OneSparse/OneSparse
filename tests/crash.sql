create extension if not exists pggraphblas;
    
\ir gdb.sql

select  ewise_add(matrix(
      array[0,1,2],
      array[1,2,0],
      cast(array[1,2,3] as bigint [])),
    matrix(
      array[0,1,2],
      array[1,2,0],
      cast(array[2,3,4] as bigint [])),
    mask=>matrix(
      array[0,1,2],
      array[1,2,0],
      array[true,true,false])
  );

