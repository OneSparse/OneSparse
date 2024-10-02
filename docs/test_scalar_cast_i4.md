# Scalar `integer` Casting

This documentation is also tests for the code, the examples below
show the literal output of these statements from Postgres.

Some setup to make sure warnings are shown, and that the extension
is installed.
``` postgres-console
set client_min_messages = 'WARNING';
create extension if not exists onesparse;
```
These casting functions cast the Postgres type `integer` to the
GraphBLAS scalar type `GrB_INT32`.
``` postgres-console
select cast(-1::integer as scalar);
 scalar 
--------
 i4:-1
(1 row)

select cast(0::integer as scalar);
 scalar 
--------
 i4:0
(1 row)

select cast(1::integer as scalar);
 scalar 
--------
 i4:1
(1 row)

```
These tests cast back from the scalar type `GrB_INT32` to the
Postgres type `integer`
``` postgres-console
select cast(-1::integer::scalar as integer);
ERROR:  operator does not exist: - scalar
LINE 1: select cast(-1::integer::scalar as integer);
                    ^
HINT:  No operator matches the given name and argument type. You might need to add an explicit type cast.
select cast(0::integer::scalar as  integer);
 int4 
------
    0
(1 row)

select cast(1::integer::scalar as scalar);
 scalar 
--------
 i4:1
(1 row)

```