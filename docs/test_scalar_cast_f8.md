# Scalar `float8` Casting

This documentation is also tests for the code, the examples below
show the literal output of these statements from Postgres.

Some setup to make sure warnings are shown, and that the extension
is installed.
``` postgres-console
set client_min_messages = 'WARNING';
create extension if not exists onesparse;
```
These casting functions cast the Postgres type `float8` to the
GraphBLAS scalar type `GrB_INT64`.
``` postgres-console
select cast(-1::float8 as scalar);
    scalar    
--------------
 f8:-1.000000
(1 row)

select cast(0::float8 as scalar);
   scalar    
-------------
 f8:0.000000
(1 row)

select cast(1::float8 as scalar);
   scalar    
-------------
 f8:1.000000
(1 row)

```
These tests cast back from the scalar type `GrB_INT64` to the
Postgres type `float8`
``` postgres-console
select cast(-1::float8::scalar as float8);
ERROR:  operator does not exist: - scalar
LINE 1: select cast(-1::float8::scalar as float8);
                    ^
HINT:  No operator matches the given name and argument type. You might need to add an explicit type cast.
select cast(0::float8::scalar as  float8);
 float8 
--------
      0
(1 row)

select cast(1::float8::scalar as scalar);
   scalar    
-------------
 f8:1.000000
(1 row)

```