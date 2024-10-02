# Scalar `float8` Math

This documentation is also tests for the code, the examples below
show the literal output of these statements from Postgres.

Some setup to make sure warnings are shown, and that the extension
is installed.
``` postgres-console
set client_min_messages = 'WARNING';
create extension if not exists onesparse;
```
Add mixed scalar objects and postgres scalar values
``` postgres-console
select (-1::float8)::scalar + 1::float8;
  ?column?   
-------------
 f8:0.000000
(1 row)

select -1::float8 + (1::float8)::scalar;
 ?column? 
----------
        0
(1 row)

```