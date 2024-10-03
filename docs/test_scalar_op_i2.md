# Scalar `smallint` Operations

Test various scalar math operations with native Postgres types
``` postgres-console
select (1::smallint)::scalar + -1::smallint;
 ?column? 
----------
 int16:0
(1 row)

select 1::smallint + (-1::smallint)::scalar;
 ?column? 
----------
        0
(1 row)

select (1::smallint)::scalar - 1::smallint;
 ?column? 
----------
 int16:0
(1 row)

select 1::smallint - (1::smallint)::scalar;
 ?column? 
----------
        0
(1 row)

select (1::smallint)::scalar * -1::smallint;
 ?column? 
----------
 int16:-1
(1 row)

select 1::smallint * (-1::smallint)::scalar;
 ?column? 
----------
       -1
(1 row)

select (1::smallint)::scalar / 1::smallint;
 ?column? 
----------
 int16:1
(1 row)

select 1::smallint / (1::smallint)::scalar;
 ?column? 
----------
        1
(1 row)

```
Test construction of min, zero and max values:
``` postgres-console
select '-1'::smallint::scalar;
  scalar  
----------
 int16:-1
(1 row)

select '0'::smallint::scalar;
 scalar  
---------
 int16:0
(1 row)

select '1'::smallint::scalar;
 scalar  
---------
 int16:1
(1 row)

```
Test setting a scalar value from max to 2
``` postgres-console
select set('1'::smallint::scalar, 2);
   set   
---------
 int16:2
(1 row)

```
Test various casting functions used by the CREATE CAST machinery:
``` postgres-console
select scalar_smallint((-1)::smallint);
 scalar_smallint 
-----------------
 int16:-1
(1 row)

select scalar_smallint((0)::smallint);
 scalar_smallint 
-----------------
 int16:0
(1 row)

select scalar_smallint((1)::smallint);
 scalar_smallint 
-----------------
 int16:1
(1 row)

select smallint_scalar((-1)::smallint::scalar);
 smallint_scalar 
-----------------
              -1
(1 row)

select smallint_scalar((0)::smallint::scalar);
 smallint_scalar 
-----------------
               0
(1 row)

select smallint_scalar((1)::smallint::scalar);
 smallint_scalar 
-----------------
               1
(1 row)

```
These casting functions cast the Postgres type `smallint` to the
GraphBLAS scalar type `GrB_INT16`.
``` postgres-console
select cast(-1::smallint as scalar);
  scalar  
----------
 int16:-1
(1 row)

select cast(0::smallint as scalar);
 scalar  
---------
 int16:0
(1 row)

select cast(1::smallint as scalar);
 scalar  
---------
 int16:1
(1 row)

```
These tests cast back from the scalar type `GrB_INT16` to the
Postgres type `smallint`
``` postgres-console
select cast((-1::smallint)::scalar as smallint);
 int2 
------
   -1
(1 row)

select cast((0::smallint)::scalar as  smallint);
 int2 
------
    0
(1 row)

select cast((1::smallint)::scalar as scalar);
 scalar  
---------
 int16:1
(1 row)

```