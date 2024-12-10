
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
select 0.0 seed \gset
insert into onesparse.test_fixture (T, a, b, d, s, u, v, unaryop, indexunaryop, binaryop, monoid, semiring) values
    ('int32',
        random_matrix(4, 4, 10, 4, :seed + 0.3),
        random_matrix(4, 4, 10, 4, :seed + 0.4),
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


create or replace function tcount_burkhardt(graph matrix) returns bigint language plpgsql as
    $$
    begin
        graph = wait(graph);
        graph = mxm(graph, graph, 'plus_pair_int32', mask=>graph, descr=>'s');
        return reduce_scalar(graph) / 6;
    end;
    $$;

CREATE OR REPLACE FUNCTION tc2(a matrix)
    RETURNS vector LANGUAGE plpgsql AS
    $$
    DECLARE
    m matrix;
    t matrix;
    y vector;
    k scalar;
    plus_pair semiring = 'plus_pair_int32';
    st1 descriptor = 'st1';
    BEGIN
        raise notice 'begin';
        a = wait(a);
        m = tril(a, -1);
        t = mxm(a, a, plus_pair, mask=>m, descr=>st1);
        y = eadd(reduce_cols(t), reduce_rows(t));
        k = reduce_scalar(y);
        RETURN 3 * ((a @ y) |- 2 * (one(t) @ y) |+ y) / k;
    END;
    $$;


create or replace function te(a matrix)
    returns matrix language plpgsql as
    $$
    begin
        return a @ a;
    end;
    $$;

create or replace function pagerank(
    a matrix,
    damping float default 0.85,
    itermax integer default 100,
    tol float default 1e-4
    )
    returns vector language plpgsql as
    $$
    declare
        rtype type = 'fp32';
        n bigint;
        teleport float;
        rdiff float = 1.0;
        d_out vector;
        t vector;
        w vector;
        r vector;
        plus_second semiring = 'plus_second_fp32';
        abs unaryop = 'abs_fp32';
        plus binaryop = 'plus_fp32';
        div binaryop = 'div_fp32';
        t0 descriptor = 't0';
    begin
        a = wait(a);
        n = nrows(a);
        teleport = (1 - damping) / n;
        d_out = cast_to(reduce_rows(one(a)), rtype);
        t = vector(rtype, n);
        w = vector(rtype, n);
        r = vector(rtype, n);
        r = assign(r, 1::float / n);
        d_out = assign(d_out, damping::scalar, accum=>div);
        for i in 0..itermax loop
            select r, t into t, r;
            r = wait(r);
            t = wait(t);
            w = eadd(t, d_out, div, c=>w);
            r = assign(r, teleport);
            r = mxv(a, w, plus_second, c=>r, accum=>plus, descr=>t0);
            t = apply(t, abs, c=>t);
            rdiff = reduce_scalar(t);
            exit when rdiff < tol;
        end loop;
        return r;
    end;
    $$;
