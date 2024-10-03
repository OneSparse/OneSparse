# Scalar `integer` Operations

Test various scalar math operations with native Postgres types
``` postgres-console
select (1::integer)::scalar + -1::integer;
 ?column? 
----------
 int32:0
(1 row)

select 1::integer + (-1::integer)::scalar;
 ?column? 
----------
        0
(1 row)

select (1::integer)::scalar - 1::integer;
 ?column? 
----------
 int32:0
(1 row)

select 1::integer - (1::integer)::scalar;
 ?column? 
----------
        0
(1 row)

select (1::integer)::scalar * -1::integer;
 ?column? 
----------
 int32:-1
(1 row)

select 1::integer * (-1::integer)::scalar;
 ?column? 
----------
       -1
(1 row)

select (1::integer)::scalar / 1::integer;
 ?column? 
----------
 int32:1
(1 row)

select 1::integer / (1::integer)::scalar;
 ?column? 
----------
        1
(1 row)

```
Test construction of min, zero and max values:
``` postgres-console
select '-1'::integer::scalar;
  scalar  
----------
 int32:-1
(1 row)

select '0'::integer::scalar;
 scalar  
---------
 int32:0
(1 row)

select '1'::integer::scalar;
 scalar  
---------
 int32:1
(1 row)

```
Test setting a scalar value from max to 2
``` postgres-console
select set('1'::integer::scalar, 2);
   set   
---------
 int32:2
(1 row)

```
Test various casting functions used by the CREATE CAST machinery:
``` postgres-console
select scalar_integer((-1)::integer);
 scalar_integer 
----------------
 int32:-1
(1 row)

select scalar_integer((0)::integer);
 scalar_integer 
----------------
 int32:0
(1 row)

select scalar_integer((1)::integer);
 scalar_integer 
----------------
 int32:1
(1 row)

select integer_scalar((-1)::integer::scalar);
 integer_scalar 
----------------
             -1
(1 row)

select integer_scalar((0)::integer::scalar);
 integer_scalar 
----------------
              0
(1 row)

select integer_scalar((1)::integer::scalar);
 integer_scalar 
----------------
              1
(1 row)

```
These casting functions cast the Postgres type `integer` to the
GraphBLAS scalar type `GrB_INT32`.
``` postgres-console
select cast(-1::integer as scalar);
  scalar  
----------
 int32:-1
(1 row)

select cast(0::integer as scalar);
 scalar  
---------
 int32:0
(1 row)

select cast(1::integer as scalar);
 scalar  
---------
 int32:1
(1 row)

```
These tests cast back from the scalar type `GrB_INT32` to the
Postgres type `integer`
``` postgres-console
select cast((-1::integer)::scalar as integer);
 int4 
------
   -1
(1 row)

select cast((0::integer)::scalar as  integer);
 int4 
------
    0
(1 row)

select cast((1::integer)::scalar as scalar);
 scalar  
---------
 int32:1
(1 row)

```