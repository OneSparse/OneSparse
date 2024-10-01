# Scalar `smallint` Casting

This documentation is also tests for the code, the examples below
show the literal output of these statements from Postgres.

Some setup to make sure warnings are shown, and that the extension
is installed.
``` postgres-console
set client_min_messages = 'WARNING';
create extension if not exists onesparse;
```
These casting functions cast the Postgres type `smallint` to the
GraphBLAS scalar type `GrB_INT16`.
``` postgres-console
select cast(-1::smallint as scalar);
 scalar 
--------
 i2:-1
(1 row)

select cast(0::smallint as scalar);
 scalar 
--------
 i2:0
(1 row)

select cast(1::smallint as scalar);
 scalar 
--------
 i2:1
(1 row)

```
These tests cast back from the scalar type `GrB_INT16` to the
Postgres type `smallint`
``` postgres-console
select cast(-1::smallint::scalar as smallint);
ERROR:  operator does not exist: - scalar
LINE 1: select cast(-1::smallint::scalar as smallint);
                    ^
HINT:  No operator matches the given name and argument type. You might need to add an explicit type cast.
select cast(0::smallint::scalar as  smallint);
 int2 
------
    0
(1 row)

select cast(1::smallint::scalar as scalar);
 scalar 
--------
 i2:1
(1 row)

```