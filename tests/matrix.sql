begin;
select plan(5);

create table edge (
    i integer,
    j integer,
    v integer
    );

insert into edge (i, j, v) values 
    (1, 4, 1),
    (1, 2, 2),
    (2, 7, 3),
    (2, 5, 4),
    (3, 6, 5),
    (4, 3, 6),
    (4, 1, 7),
    (5, 6, 8),
    (6, 3, 9),
    (7, 3, 0);

select is(
    ncols(matrix(
        array[1,1,2,2,3,4,4,5,6,7],
        array[2,4,5,7,6,1,3,6,3,3],
        array[2,1,4,3,5,7,6,8,9,0])),
    10::bigint,
    'matrix ncols');
        
select is(
    nrows(matrix(
        array[1,1,2,2,3,4,4,5,6,7],
        array[2,4,5,7,6,1,3,6,3,3],
        array[2,1,4,3,5,7,6,8,9,0])),
    10::bigint,
    'matrix nrows');
    
select is(
    nvals(matrix(
        array[1,1,2,2,3,4,4,5,6,7],
        array[2,4,5,7,6,1,3,6,3,3],
        array[2,1,4,3,5,7,6,8,9,0])),
    10::bigint,
    'matrix nvals');

select is(
    matrix(array[0,1,2],
            array[1,2,0],
            array[4,5,6]) &&
    matrix(array[0,1,2],
            array[1,2,0],
            array[4,5,6]),
    matrix(array[0,1,2],
    array[1,2,0],
    array[16,25,36]),
    'matrix ewise mult');
    
select is(
    matrix(array[0,1,2],
            array[1,2,0],
            array[4,5,6]) ||
    matrix(array[0,1,2],
            array[1,2,0],
            array[4,5,6]),
    matrix(array[0,1,2],
    array[1,2,0],
    array[8,10,12]),
    'matrix ewise add');
    
select * from finish();
rollback;
