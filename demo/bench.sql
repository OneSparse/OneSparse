\timing on
    
create extension if not exists pggraphblas;
    
create table a (m matrix);

create table b (i bigint, j bigint);

create index on b (i) include (j);

insert into a select matrix_random_bool(8000,8000,32000000);

insert into b select row, col from matrix_elements_bool((select m from a));
