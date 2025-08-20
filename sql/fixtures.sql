
create extension if not exists onesparse;
create table if not exists onesparse.test_fixture (
    t type,
    a matrix,
    b matrix,
    d matrix,
    s matrix,
    u vector,
    v vector,
    unaryop unaryop,
    indexunaryop indexunaryop,
    binaryop binaryop,
    monoid monoid,
    semiring semiring
    );

delete from onesparse.test_fixture;
select 42 seed \gset
insert into onesparse.test_fixture (T, a, b, d, s, u, v, unaryop, indexunaryop, binaryop, monoid, semiring) values
    ('int32',
        random_matrix('int8', 4, 4, 0.5, :seed + 1) % 42,
        random_matrix('int8', 4, 4, 0.5, :seed + 3) % 42,
        dense_matrix(4, 4, 1),
        'int32(2:2)[0:0:1 0:1:1 1:1:1]'::matrix,
        'int32(4)[1:2]'::vector,
        'int32(4)[1:3 2:3]'::vector,
        'ainv_int32',
        'valuegt_int32',
        'times_int32',
        'plus_monoid_int32',
        'plus_times_int32');


create or replace function test_expand(graph matrix) returns matrix language plpgsql as
    $$
    declare
        nvals bigint = nvals(graph);
    begin
        raise notice 'expand';
        return graph;
    end;
    $$;

create or replace function test_expand_expand(graph matrix) returns matrix language plpgsql as
    $$
    declare
        nvals bigint = nvals(graph);
    begin
        raise notice 'expand expand';
        graph = test_expand(graph);
        return test_expand(graph);
    end;
    $$;

create or replace function test_se(graph matrix) returns matrix language plpgsql as
    $$
    declare
        nvals bigint;
    begin
        graph = wait(graph);
        nvals = nvals(graph);
        raise notice 'nvals: %', nvals;
        graph = set_element(graph, 4, 2, 42);
        graph = set_element(graph, 4, 3, 43);
        graph = set_element(graph, 4, 4, 44);
        return graph;
    end;
    $$;

create or replace function test_arr(count int default 1) returns matrix[] language plpgsql as
    $$
    declare
        r matrix[];
    begin
        r = array['int32', 'int32'];
        r[1] = wait(r[1]);
        return r;
    end;
    $$;

create or replace function tcount_b(graph matrix) returns bigint language plpgsql as
    $$
    begin
        graph = wait(graph);
        graph = mxm(graph, graph, 'plus_pair_int32', mask=>graph, descr=>'s');
        return reduce_scalar(graph) / 6;
    end;
    $$;

create or replace function tcount_sll(graph matrix) returns bigint language plpgsql as
    $$
    begin
        graph = tril(graph);
        graph = mxm(graph, graph, 'plus_pair_int32', mask=>graph, descr=>'s');
        return reduce_scalar(graph);
    end;
    $$;

create or replace function tcount_slu(graph matrix) returns bigint language plpgsql as
    $$
    declare
        low matrix;
        upp matrix;
    begin
        graph = wait(graph);
        low = tril(graph);
        upp = triu(graph);
        graph = mxm(low, upp, 'plus_pair_int32', mask=>low, descr=>'st1');
        return reduce_scalar(graph);
    end;
    $$;

create or replace function te(a matrix)
    returns vector language plpgsql as
    $$
    declare
        v vector;
        w vector;
        t matrix;
    begin
        v = wait('int32'::vector);
        t = wait('int32'::matrix);
        w = (one(v) @ t) |+ v;
        v = apply(2, w, 'times_int32');
        return v;
    end;
    $$;


