drop table if exists orkut;

create table orkut (
    i integer,
    j integer
    );

\copy orkut (i, j) FROM 'demo/Matrix/com-Orkut/com-Orkut2.mtx' DELIMITER ' ';

delete from test_graphs where name in ('orkut', 'orkuts');

insert into test_graphs (name, graph)
    values ('orkut', (select matrix_query('select (i::bigint - 1), (j::bigint - 1), 1 from orkut',
                    greatest(max(i), max(j)), greatest(max(i), max(j))) from orkut));

insert into test_graphs (name, graph)
    values ('orkuts', (select eadd(graph, graph, descr=>'t1') from test_graphs where name = 'orkut'));

create or replace view vorkut as select graph from test_graphs where name = 'orkut';
create or replace view vorkuts as select graph from test_graphs where name = 'orkuts';
