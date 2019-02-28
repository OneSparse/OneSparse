drop table if exists test;

create table test (
    i integer,
    j integer
    );

insert into test (i, j) values
    (1, 4),
    (1, 2),
    (2, 7),
    (2, 5),
    (3, 6),
    (4, 3),
    (4, 1),
    (5, 6),
    (6, 3),
    (7, 3);

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
