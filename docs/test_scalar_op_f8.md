# Scalar `float8` Operations

Test various scalar math operations with native Postgres types
``` postgres-console
select (1::float8)::scalar + -1::float8;
  ?column?   
-------------
 f8:0.000000
(1 row)

select 1::float8 + (-1::float8)::scalar;
 ?column? 
----------
        0
(1 row)

select (1::float8)::scalar - 1::float8;
  ?column?   
-------------
 f8:0.000000
(1 row)

select 1::float8 - (1::float8)::scalar;
 ?column? 
----------
        0
(1 row)

select (1::float8)::scalar * -1::float8;
   ?column?   
--------------
 f8:-1.000000
(1 row)

select 1::float8 * (-1::float8)::scalar;
 ?column? 
----------
       -1
(1 row)

select (1::float8)::scalar / 1::float8;
  ?column?   
-------------
 f8:1.000000
(1 row)

select 1::float8 / (1::float8)::scalar;
 ?column? 
----------
        1
(1 row)

```
Test construction of min, zero and max values:
``` postgres-console
select '-1'::float8::scalar;
    scalar    
--------------
 f8:-1.000000
(1 row)

select '0'::float8::scalar;
   scalar    
-------------
 f8:0.000000
(1 row)

select '1'::float8::scalar;
   scalar    
-------------
 f8:1.000000
(1 row)

```
Test setting a scalar value from max to 2
``` postgres-console
select set('1'::float8::scalar, 2);
     set     
-------------
 f8:2.000000
(1 row)

```
Test various casting functions used by the CREATE CAST machinery:
``` postgres-console
select scalar_float8((-1)::float8);
 scalar_float8 
---------------
 f8:-1.000000
(1 row)

select scalar_float8((0)::float8);
 scalar_float8 
---------------
 f8:0.000000
(1 row)

select scalar_float8((1)::float8);
 scalar_float8 
---------------
 f8:1.000000
(1 row)

select float8_scalar((-1)::float8::scalar);
 float8_scalar 
---------------
            -1
(1 row)

select float8_scalar((0)::float8::scalar);
 float8_scalar 
---------------
             0
(1 row)

select float8_scalar((1)::float8::scalar);
 float8_scalar 
---------------
             1
(1 row)

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
select cast((-1::float8)::scalar as float8);
 float8 
--------
     -1
(1 row)

select cast((0::float8)::scalar as  float8);
 float8 
--------
      0
(1 row)

select cast((1::float8)::scalar as scalar);
   scalar    
-------------
 f8:1.000000
(1 row)

```