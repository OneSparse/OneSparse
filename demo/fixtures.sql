create extension if not exists onesparse;
drop table if exists test;

create table test (
    i integer,
    j integer,
    v boolean default true
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

create index on mbeacxc (i) include (j);

\copy mbeacxc from 'demo/Matrix/mbeacxc' with (delimiter ' ');

drop table if exists bcsstk01;
    
create table bcsstk01 (
    i bigint,
    j bigint,
    v float
    );

\copy bcsstk01 from 'demo/Matrix/bcsstk01' with (delimiter ' ');

drop table if exists ash219;

create table ash219 (
    i integer,
    j integer,
    v integer
    );

\copy ash219 from 'demo/Matrix/ash219' with (delimiter ' ');
    

drop table if exists fs_183_1;

create table fs_183_1 (
    i integer,
    j integer,
    v real
    );

\copy fs_183_1 from 'demo/Matrix/fs_183_1' with (delimiter ' ');

-- create table livejournal (
--     i integer,
--     j integer
--     );

-- \copy livejournal from 'demo/Matrix/com-LiveJournal.mtx' with (delimiter ' ');

create table test_graphs (name text primary key, graph matrix not null);

insert into test_graphs values ('mbeacxc', (select matrix_agg(i, j, v) from mbeacxc));
