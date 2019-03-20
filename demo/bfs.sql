-- include test table fixtures from SuiteSparse Demo/Matrix

\ir fixtures.sql

drop function if exists bfs2;
        
create function bfs2(tablename text, source bigint) returns vector as $$
declare
    A matrix;
    n bigint;
    v vector;
    q vector;
    level integer := 0;                            -- Start at level 1.
    not_done bool := true;                         -- Flag to indicate still work to do.
    
begin
    execute format('select matrix(array_agg(i), array_agg(j), array_agg(true)) from %I', tablename) into A;
    n := nrows(A);                          -- The number of result rows.
    v := vector_integer(n);                 -- int32 result vector of vertex levels.
    q := assign(vector_bool(n), false);     -- bool mask of completed vertices.

    q := set_element(q, source, true);             -- Set the source element to done.
    
    while not_done and level <= n loop             -- While still work to do.
        v := assign(v, level, mask=>q);            -- Assign the current level to all

        q := mxv(transpose(A), q, q,               -- Multiply q<mask> = Aq,
            semiring=>'LOR_LAND_BOOL',             -- using LOR_LAND_BOOL semiring
            mask=>v,                               -- only those *not* masked
            dmask=>'scmp',                         -- by complementing the mask
            doutp=>'replace');                     -- clearing results in q first

        not_done := reduce_bool(q);                -- are there more neighbors?

        level := level + 1;                        -- increment the level
    end loop;
    return v;
end;
$$ language plpgsql;

drop function if exists bfs;
        
create function bfs3(A matrix, source bigint) returns vector as $$
declare
    n bigint := nrows(A);                          -- The number of result rows.
    v vector := vector_integer(n);                 -- int32 result vector of vertex levels.
    q vector := assign(vector_bool(n), false);     -- bool mask of completed vertices.
    level integer := 0;                            -- Start at level 1.
    not_done bool := true;                         -- Flag to indicate still work to do.
    t timestamptz;
    
begin
    q := set_element(q, source, true);             -- Set the source element to done.
    
    while not_done and level <= n loop             -- While still work to do.
        v := assign(v, level, mask=>q);            -- Assign the current level to all

        q := mxv(transpose(A), q, q,               -- Multiply q<mask> = T(A)q,
            semiring=>'LOR_LAND_BOOL',             -- using LOR_LAND_BOOL semiring
            mask=>v,                               -- only those *not* masked
            dmask=>'scmp',                         -- by complementing the mask
            doutp=>'replace');                     -- clearing results in q first

        not_done := reduce_bool(q);                -- are there more neighbors?
    
        level := level + 1;                        -- increment the level
    end loop;
    return v;
end;
$$ language plpgsql;
    
drop function if exists bfscte;
            
create function bfscte(tablename text, source integer)
returns table(id integer, level integer) as
$$
begin
return query execute format($body$
  with recursive test_cte (i, j, level, path) 
     as
     ( 
       select i, j, 0 as level, array[i] as path
       from %1$I
       where i = %2$s
       union all
       select nxt.i, nxt.j, level + 1, array_append(prv.path, nxt.i)
       from %1$I nxt, test_cte prv
       where nxt.i = prv.j
       and nxt.i != ALL(prv.path)
     )
     select distinct on (i) i, level
     from test_cte order by i, array_length(path, 1)
$body$, tablename, source);
end;
$$ language plpgsql;

CREATE OR REPLACE FUNCTION plbfs(s_node integer)
RETURNS table (node integer, distance integer) AS $$
	DECLARE
		parent integer[];
		level integer[];
		frontier integer[];
		next integer[];
		i integer;
		u integer;
		v integer;
	BEGIN
		i := 1;
		frontier[0] := s_node;
		level[s_node] := 0;
		parent[s_node] = -1;
		WHILE frontier IS NOT NULL 
		LOOP
			next := NULL;
			FOREACH u IN ARRAY frontier 
			LOOP
				FOR v IN (SELECT bcsstk16.j FROM bcsstk16 WHERE bcsstk16.i=u)
				LOOP
					IF level[v] IS NULL THEN
						parent[v] := u;
						level[v] := i;
                        return query select v, i;
						next = array_append(next, v);
					END IF;
				END LOOP;
    		END LOOP;
			i := i + 1;
    		frontier := next;
		END LOOP;
    return;
END; $$
LANGUAGE PLPGSQL;    

create or replace function go(tablename text, start integer) returns vector as $$
    declare
      m matrix;
      v vector;
    begin
    execute format('select matrix(array_agg(i), array_agg(j), array_agg(v)) from %1$I', tablename) into m;
    return bfs(m, start);
    end;
    $$ language plpgsql;


-- select bfs(kron(matrix(array_agg(i), array_agg(j), array_agg(v)), matrix(array_agg(i), array_agg(j), array_agg(v)), mulop=>'plus_int32'), 1) from fs_183_1;


-- select * from (select matrix_elements_float(kron(matrix(array_agg(i), array_agg(j), array_agg(v)), matrix(array_agg(i), array_agg(j), array_agg(v)), mulop=>'times_fp64')) from fs_183_1) i limit 10;

-- select i.* from (select matrix(array_agg(t.i), array_agg(t.j), array_agg(t.v)) as mat from test t) m cross join lateral (select * from  matrix_elements_float(mat)) i ;


-- postgres=# insert into test2 select i.* from (select kron(matrix(array_agg(t.i), array_agg(t.j), array_agg(t.v)), matrix(array_agg(t.i), array_agg(t.j), array_agg(t.v)), mulop=>'times_fp64') as mat from fs_183_1 t) m cross join lateral (select * from  matrix_elements_float(mat)) i ;
    
