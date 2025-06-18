
drop table if exists orkut;

create unlogged table orkut (
    i integer,
    j integer
    );

\copy orkut (i, j) FROM 'demo/Matrix/com-Orkut/com-Orkut2.mtx' DELIMITER ' ';

ALTER TABLE orkut ADD CONSTRAINT orkut_i_j_pk PRIMARY KEY (i, j);

delete from test_graphs where name in ('orkut', 'orkuts');

insert into test_graphs (name, graph)
    values ('orkut', (select resize(matrix_agg(i::bigint, j::bigint, 1),
                    greatest(max(i), max(j)), greatest(max(i), max(j))) from orkut));

insert into test_graphs (name, graph)
    values ('orkuts', (select eadd(graph, graph, descr=>'t1') from test_graphs where name = 'orkut'));

create or replace view vorkut as select graph from test_graphs where name = 'orkut';
create or replace view vorkuts as select graph from test_graphs where name = 'orkuts';
