

create function test_m_ewise_mult() returns setof matrix_tuple as $$
    declare m matrix;
    begin
        select matrix_agg(i, j, v) from graph into m;
        return query select * from matrix_tuples(m && m);
    end;
$$ language plpgsql;
    
create function test_m_ewise_add() returns setof matrix_tuple as $$
    declare m matrix;
    begin
        select matrix_agg(i, j, v) from graph into m;
        return query select * from matrix_tuples(m || m);
    end;
$$ language plpgsql;


create function test_v_ewise_mult() returns setof vector_tuple as $$
    declare m vector;
    begin
        select vector_agg(i, v) from graph into m;
        return query select * from vector_tuples(m && m);
    end;
$$ language plpgsql;
    
create function test_v_ewise_add() returns setof vector_tuple as $$
    declare m vector;
    begin
        select vector_agg(i, v) from graph into m;
        return query select * from vector_tuples(m || m);
    end;
$$ language plpgsql;
