
create or replace function build_dbpedia_matrix() returns matrix language plpgsql as $$
declare
    node_count integer := (select count(*) from article);
    A matrix := matrix_bool(node_count, node_count);
    s integer;
    o integer;
begin
    for s in select id from article loop
        for o in select dest from article_x_article where source = s loop
	    A := set_element(A, s, o, true);
	end loop;
	if s % 10000 = 0 then
	    raise notice '%', s;
	end if;
    end loop;
    return A;
end;
$$;
