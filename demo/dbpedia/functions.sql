
create or replace function build_dbpedia_matrix() returns matrix language plpgsql as $$
declare
    node_count integer := (select count(*) from article_x_article);
    A matrix := matrix_bool(node_count, node_count);
    s bigint;
    d bigint;
begin
    for s, d in select source, dest from article_x_article loop
        A := set_element(A, s, d, true);
    end loop;
    return A;
end;
$$;
