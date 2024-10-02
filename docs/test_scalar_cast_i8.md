# Scalar `bigint` Casting

This documentation is also tests for the code, the examples below
show the literal output of these statements from Postgres.

Some setup to make sure warnings are shown, and that the extension
is installed.
``` postgres-console
set client_min_messages = 'WARNING';
create extension if not exists onesparse;
```
These casting functions cast the Postgres type `bigint` to the
GraphBLAS scalar type `GrB_INT64`.
``` postgres-console
select cast(-1::bigint as scalar);
 scalar 
--------
 i8:-1
(1 row)

select cast(0::bigint as scalar);
 scalar 
--------
 i8:0
(1 row)

select cast(1::bigint as scalar);
 scalar 
--------
 i8:1
(1 row)

```
These tests cast back from the scalar type `GrB_INT64` to the
Postgres type `bigint`
``` postgres-console
select cast(-1::bigint::scalar as bigint);
ERROR:  operator does not exist: - scalar
LINE 1: select cast(-1::bigint::scalar as bigint);
                    ^
HINT:  No operator matches the given name and argument type. You might need to add an explicit type cast.
select cast(0::bigint::scalar as  bigint);
 int8 
------
    0
(1 row)

select cast(1::bigint::scalar as scalar);
 scalar 
--------
 i8:1
(1 row)

```