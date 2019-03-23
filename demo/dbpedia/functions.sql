
create or replace function build_dbpedia_matrix() returns matrix language plpgsql as $$
declare
    node_count integer := (select count(*) from article_x_article);
    A matrix := matrix_bool(node_count, node_count);
begin
    perform set_element(A, source, dest, true) from article_x_article;
    return A;
end;
$$;
