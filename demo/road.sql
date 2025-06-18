
drop table if exists road;

create unlogged table road (
    id serial primary key,
    source integer,
    target integer,
    cost integer
    );

\copy road (source, target, cost) FROM 'demo/Matrix/GAP-road/GAP-road-clean.mtx' DELIMITER ' ';

-- CREATE INDEX road_source_target_idx on road (source, target);
-- CREATE INDEX road_source_target_idx on road (target);

delete from test_graphs where name in ('road', 'roads');

insert into test_graphs (name, graph)
    values ('road', (select resize(matrix_agg(source::bigint, target::bigint, cost),
                    greatest(max(source), max(target)), greatest(max(source), max(target))) from road));

insert into test_graphs (name, graph)
    values ('roads', (select eadd(graph, graph, descr=>'t1') from test_graphs where name = 'road'));

create or replace view vroad as select graph from test_graphs where name = 'road';
create or replace view vroads as select graph from test_graphs where name = 'roads';
