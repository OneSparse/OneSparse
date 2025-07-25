
drop table if exists road;

create table road (
    id serial primary key,
    source integer,
    target integer,
    cost integer
    );

\copy road (source, target, cost) FROM 'demo/Matrix/GAP-road/GAP-road-clean.mtx' DELIMITER ' ';

delete from test_graphs where name in ('road', 'roads');

insert into test_graphs (name, graph)
    values ('road', (select matrix_query('select source::bigint, target::bigint, cost from road',
                    greatest(max(source)+1, max(target)+1), greatest(max(source)+1, max(target)+1)) from road));

create or replace view vroad as select graph from test_graphs where name = 'road';

select serialize_file((select graph from vroad), '/home/postgres/onesparse/demo/road.grb');

insert into test_graphs (name, graph)
    values ('roads', (select eadd(graph, graph, descr=>'t1') from vroad));

create or replace view vroads as select graph from test_graphs where name = 'roads';

select serialize_file((select graph from vroads), '/home/postgres/onesparse/demo/roads.grb');
