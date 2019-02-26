drop table if exists test;

create table test (
    i integer,
    j integer,
    v integer
    );

insert into test (i, j, v) values
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

drop table if exists mbeacxc;

create table mbeacxc (
    i bigint,
    j bigint,
    v float
    );

\copy mbeacxc from 'GraphBLAS/Demo/Matrix/mbeacxc' with (delimiter ' ');

drop table if exists bcsstk16;
    
create table bcsstk16 (
    i bigint,
    j bigint,
    v float
    );

\copy bcsstk16 from 'GraphBLAS/Demo/Matrix/bcsstk16' with (delimiter ' ');
