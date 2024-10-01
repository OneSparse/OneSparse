# Scalar 'integer' Operators

This documentation is also tests for the code, the examples below
show the literal output of these statements from Postgres.

Some setup to make sure warnings are shown, and that the extension
is installed.
``` postgres-console
set client_min_messages = 'WARNING';
create extension if not exists onesparse;
```
Test various scalar math operations with native Postgres types
``` postgres-console
select (1::integer)::scalar + -1::integer;
 ?column? 
----------
 i4:0
(1 row)

select 1::integer + (-1::integer)::scalar;
 ?column? 
----------
        0
(1 row)

select (1::integer)::scalar - 1::integer;
 ?column? 
----------
 i4:0
(1 row)

select 1::integer - (1::integer)::scalar;
 ?column? 
----------
        0
(1 row)

select (1::integer)::scalar * -1::integer;
 ?column? 
----------
 i4:-1
(1 row)

select 1::integer * (-1::integer)::scalar;
 ?column? 
----------
       -1
(1 row)

select (1::integer)::scalar / 1::integer;
 ?column? 
----------
 i4:1
(1 row)

select 1::integer / (1::integer)::scalar;
 ?column? 
----------
        1
(1 row)

```