
drop table if exists friendster;

create unlogged table friendster (
    i integer,
    j integer
    );

\copy friendster (i, j) FROM 'demo/Matrix/com-Friendster/com-Friendster2.mtx' DELIMITER ' ';

-- ALTER TABLE friendster ADD CONSTRAINT friendster_i_j_pk PRIMARY KEY (i, j);

-- delete from test_graphs where name in ('friendster', 'friendsters');

-- insert into test_graphs (name, graph)
--     values ('friendster', (select resize(matrix_agg(i::bigint, j::bigint, 1),
--                     greatest(max(i), max(j)), greatest(max(i), max(j))) from friendster));

-- insert into test_graphs (name, graph)
--     values ('friendsters', (select eadd(graph, graph, descr=>'t1') from test_graphs where name = 'friendster'));

-- create or replace view vfriendster as select graph from test_graphs where name = 'friendster';
-- create or replace view vfriendsters as select graph from test_graphs where name = 'friendsters';
