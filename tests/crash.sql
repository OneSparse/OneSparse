create extension if not exists pggraphblas;
    
\ir gdb.sql

create table t (m matrix);

insert into t (m) values (assign(matrix_bool(100,100), true));
