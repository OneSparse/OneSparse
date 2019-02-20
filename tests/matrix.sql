begin;
select plan(1);

create table edge (
    i bigint,
    j bigint,
    v bigint
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
    
select * from finish();
rollback;
