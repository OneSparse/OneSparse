# Scalar `bigint`

Test various scalar math operations with native Postgres types
``` postgres-console
select (1::bigint)::scalar + -1::bigint;
 ?column? 
----------
 int64:0
(1 row)

select 1::bigint + (-1::bigint)::scalar;
 ?column? 
----------
        0
(1 row)

select (1::bigint)::scalar - 1::bigint;
 ?column? 
----------
 int64:0
(1 row)

select 1::bigint - (1::bigint)::scalar;
 ?column? 
----------
        0
(1 row)

select (1::bigint)::scalar * -1::bigint;
 ?column? 
----------
 int64:-1
(1 row)

select 1::bigint * (-1::bigint)::scalar;
 ?column? 
----------
       -1
(1 row)

select (1::bigint)::scalar / 1::bigint;
 ?column? 
----------
 int64:1
(1 row)

select 1::bigint / (1::bigint)::scalar;
 ?column? 
----------
        1
(1 row)

```
Test construction of min, zero and max values:
``` postgres-console
select '-1'::bigint::scalar;
  scalar  
----------
 int64:-1
(1 row)

select '0'::bigint::scalar;
 scalar  
---------
 int64:0
(1 row)

select '1'::bigint::scalar;
 scalar  
---------
 int64:1
(1 row)

```
Test setting a scalar value from max to 2
``` postgres-console
select set('1'::bigint::scalar, 2);
   set   
---------
 int64:2
(1 row)

```
Test various casting functions used by the CREATE CAST machinery:
``` postgres-console
select scalar_bigint((-1)::bigint);
 scalar_bigint 
---------------
 int64:-1
(1 row)

select scalar_bigint((0)::bigint);
 scalar_bigint 
---------------
 int64:0
(1 row)

select scalar_bigint((1)::bigint);
 scalar_bigint 
---------------
 int64:1
(1 row)

select bigint_scalar((-1)::bigint::scalar);
 bigint_scalar 
---------------
            -1
(1 row)

select bigint_scalar((0)::bigint::scalar);
 bigint_scalar 
---------------
             0
(1 row)

select bigint_scalar((1)::bigint::scalar);
 bigint_scalar 
---------------
             1
(1 row)

```
These casting functions cast the Postgres type `bigint` to the
GraphBLAS scalar type `GrB_INT64`.
``` postgres-console
select cast(-1::bigint as scalar);
  scalar  
----------
 int64:-1
(1 row)

select cast(0::bigint as scalar);
 scalar  
---------
 int64:0
(1 row)

select cast(1::bigint as scalar);
 scalar  
---------
 int64:1
(1 row)

```
These tests cast back from the scalar type `GrB_INT64` to the
Postgres type `bigint`
``` postgres-console
select cast((-1::bigint)::scalar as bigint);
 int8 
------
   -1
(1 row)

select cast((0::bigint)::scalar as  bigint);
 int8 
------
    0
(1 row)

select cast((1::bigint)::scalar as scalar);
 scalar  
---------
 int64:1
(1 row)

```