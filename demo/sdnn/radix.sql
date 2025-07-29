
CREATE OR REPLACE FUNCTION permutation_matrix(t type, n integer)
RETURNS matrix AS $$
DECLARE
    p matrix = matrix(t, n, n);
BEGIN
    p = set_element(p, n - 1, 0, 1.0::float4);

    FOR i IN 0 .. n - 2 LOOP
        p = set_element(p, i, i + 1, 1.0::float4);
    END LOOP;
    return p;
END;
$$ LANGUAGE plpgsql;

create or replace function radix_net(topos integer[])
returns setof matrix as $$

$$ language plpgsql;
