-- include test table fixtures from SuiteSparse Demo/Matrix

\ir fixtures.sql
        
create or replace function bfs(A matrix, source bigint) returns vector as $$
declare
    n bigint := nrows(A);                          -- The number of result rows.
    v vector := vector_integer(n);                 -- int32 result vector of vertex levels.
    q vector := assign(vector_bool(n), false);     -- bool mask of completed vertices.
    level integer := 0;                            -- Start at level 1.
    not_done bool := true;                         -- Flag to indicate still work to do.
    
begin
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

create or replace function bfscte(start integer) returns table(id integer, level integer) as $$
  with recursive test_cte (i, j, level, path) 
     as
     ( 
       select i, j, 0 as level, array[i] as path
       from test
       where i = start
       union all
       select nxt.i, nxt.j, level + 1, array_append(prv.path, nxt.i)
       from test nxt, test_cte prv
       where nxt.i = prv.j
       and nxt.i != ALL(prv.path)
     )
     select distinct on (i) i, level
from test_cte order by i, array_length(path, 1);
    $$ language sql;
