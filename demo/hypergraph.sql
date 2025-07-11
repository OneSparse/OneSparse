
with r as (select random_matrix(4, 4, 4, 1, 8) g)
    select print(g),
    one(eadd(reduce_cols(g, 'any_int32_monoid'),
             reduce_cols(g,'any_int32_monoid'),
        'any_int32')) from r;

CREATE OR REPLACE FUNCTION hypergraph_to_dot(a matrix, b matrix)
RETURNS text LANGUAGE plpgsql AS $$
DECLARE
    dot text := 'digraph G {' || E'\n' ||
                '  node [shape=circle];' || E'\n';
    i bigint;
    rec record;
BEGIN
    -- Emit edge nodes as boxes
    FOR i IN (select * from irows(a) union select * from irows(transpose(b)))
    LOOP
        dot := dot || format('  e%s [shape=box];%s', i::text, E'\n');
    END LOOP;

    -- Emit arcs from source nodes to edge nodes
    FOR rec IN SELECT * FROM elements(a)
    LOOP
        dot := dot || format('  n%s -> e%s;%s', rec.i::text, rec.j::text, E'\n');
    END LOOP;

    -- Emit arcs from edge nodes to destination nodes
    FOR rec IN SELECT * FROM elements(transpose(b))
    LOOP
        dot := dot || format('  e%s -> n%s;%s', rec.i::text, rec.j::text, E'\n');
    END LOOP;

    dot := dot || '}';
    RETURN dot;
END;
$$;
