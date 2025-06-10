create extension if not exists onesparse;
drop table if exists ostest;

create table ostest (
    i integer,
    j integer
    );

insert into ostest (i, j) values
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

drop table if exists karate;
create table karate (
    i integer,
    j integer
    );

\copy karate from 'demo/Matrix/karate/karate.mtx' with (delimiter ' ');

drop table if exists mbeacxc;

create table mbeacxc (
    i bigint,
    j bigint,
    v numeric
    );

create index on mbeacxc (i) include (j);

\copy mbeacxc from 'demo/Matrix/mbeacxc' with (delimiter ' ');

drop table if exists bcsstk01;
    
create table bcsstk01 (
    i bigint,
    j bigint,
    v numeric
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

drop table if exists test_graphs cascade;
create table test_graphs (name text primary key, graph matrix not null);

insert into test_graphs values
    ('ostest', (select matrix_agg(i, j, 1) |+ matrix_agg(j, i, 1) from ostest)), -- make symmetric
    ('karate', (select matrix_agg(i, j, 1) |+ matrix_agg(j, i, 1) from karate)), -- make symmetric
    ('ash219', (select matrix_agg(i, j, v) from ash219)),
    ('mbeacxc', (select matrix_agg(i, j, v) from mbeacxc));

create view karateg as select graph from test_graphs where name = 'karate';


CREATE OR REPLACE FUNCTION sssp(graph matrix, start_node bigint)
    RETURNS vector LANGUAGE plpgsql AS
    $$
    DECLARE
    sssp_vector vector = vector(type(graph));
    next_vector vector = vector(type(graph));
    BEGIN
        sssp_vector = set_element(sssp_vector, start_node, 0);
        WHILE sssp_vector != next_vector LOOP
            next_vector = dup(sssp_vector);
            sssp_vector = sssp_vector @<+< graph;
        END LOOP;
    RETURN sssp_vector;
    END;
    $$;

CREATE OR REPLACE FUNCTION bcentrality(
    A matrix,
    sources bigint[]
)
RETURNS vector
LANGUAGE plpgsql
AS $$
DECLARE
    i int;
    n bigint = nrows(A);
    ns bigint = array_length(sources, 1);

    centrality vector = vector('fp64', n);
    A_T matrix = transpose(A);
    paths matrix = matrix('fp64', ns, n);
    frontier matrix = matrix('fp64', ns, n);
BEGIN
    FOR i IN 0..array_upper(sources, 1) LOOP
        paths = set_element(paths, i-1, sources[i], 1);
        frontier = set_element(frontier, i-1, sources[i], 1);
    END LOOP;
    RETURN centrality;
END;
$$;
