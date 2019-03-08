create extension if not exists pggraphblas;
    
\ir gdb.sql
    
-- two matrix multiplies, the second fails

create table t (v vector);
insert into t (v) values (assign(vector_bool(10), true));

create table f (m matrix);
insert into f (m) values (matrix(array[0,1,2], array[2,1,0], array[1,2,3]));
