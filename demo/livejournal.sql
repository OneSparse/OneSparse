
drop table if exists livejournal;

create unlogged table livejournal (
    i integer,
    j integer
    );

\copy livejournal (i, j) FROM 'demo/Matrix/com-LiveJournal/com-LiveJournal2.mtx' DELIMITER ' ';

ALTER TABLE livejournal ADD CONSTRAINT livejournal_i_j_pk PRIMARY KEY (i, j);

delete from test_graphs where name in ('lj', 'ljs');

insert into test_graphs (name, graph)
    values ('lj', (select resize(matrix_agg(i::bigint, j::bigint, 1),
                    greatest(max(i)+1, max(j)+1), greatest(max(i)+1, max(j)+1)) from livejournal));

insert into test_graphs (name, graph)
    values ('ljs', (select eadd(graph, graph, descr=>'t1') from test_graphs where name = 'lj'));

create or replace view vlivejournal as select graph from test_graphs where name = 'lj';
create or replace view vlivejournals as select graph from test_graphs where name = 'ljs';

create or replace function demo() returns text as $$
    declare
        g matrix;
        nv bigint;
        result text = '';
        tc bigint;
        tt bigint;
        pr bigint;
        start_time timestamp;
        tc_time interval;
        tt_time interval;
        pr_time interval;
    begin
        select graph into g from vlivejournal;
        g = wait(g);
        nv = nvals(g);
        start_time = clock_timestamp();
        tc = tcount_sll(g);
        tc_time = clock_timestamp() - start_time;
        raise notice 'tcount %', tc;

        select graph into g from vlivejournal;
        g = wait(g);
        start_time = clock_timestamp();
        tt = nvals(tcentrality(g));
        tt_time = clock_timestamp() - start_time;
        raise notice 'tcentrality %', tt;

        select graph into g from vlivejournal;
        g = wait(g);
        start_time = clock_timestamp();
        pr = nvals(pagerank(g));
        pr_time = clock_timestamp() - start_time;
        raise notice 'page rank %', pr;

        result = format($f$
        LiveJournal Nvals: %s
        Triangle Count: %s (time: %s eps %s)
        Triangle Centrality: %s (time: %s eps %s)
        Page Rank: %s (time: %s eps %s)$f$,
        nv,
        tc, tc_time, (nv / extract(epoch from tc_time)),
        tt, tt_time, (nv / extract(epoch from tt_time)),
        pr, pr_time, (nv / extract(epoch from pr_time))
    );
    return result;
    end;
    $$ language plpgsql;


create view ljtc as
    WITH
        setup AS (SELECT graph as a from vlivejournal),
        triangulate AS (SELECT mxm(a, a, mask=>a, descr=>'st1') AS t FROM setup),
        reduce_graph AS (SELECT reduce_rows(t) as y FROM triangulate),
        compute_thresh AS (SELECT reduce_scalar(y) as k FROM reduce_graph)
    SELECT 3 * ((a @ y) |- 2 * (t @ y) |+ y) / k
    FROM setup, triangulate, reduce_graph, compute_thresh;
