
## Scalar `float8`

Test various scalar math operations with native Postgres types
Scalar-native arithmetic (scalar op native)
``` postgres-console
select (1::float8)::scalar + -1::float8;
   ?column?    
---------------
 fp64:0.000000
(1 row)

select (1::float8)::scalar - 1::float8;
   ?column?    
---------------
 fp64:0.000000
(1 row)

select (1::float8)::scalar * -1::float8;
    ?column?    
----------------
 fp64:-1.000000
(1 row)

select (1::float8)::scalar / 1::float8;
   ?column?    
---------------
 fp64:1.000000
(1 row)

```
Native-scalar arithmetic (native op scalar)
``` postgres-console
select 1::float8 + (-1::float8)::scalar;
 ?column? 
----------
        0
(1 row)

select 1::float8 - (1::float8)::scalar;
 ?column? 
----------
        0
(1 row)

select 1::float8 * (-1::float8)::scalar;
 ?column? 
----------
       -1
(1 row)

select 1::float8 / (1::float8)::scalar;
 ?column? 
----------
        1
(1 row)

```
Scalar-scalar arithmetic (via native extraction)
``` postgres-console
select ((1::float8)::scalar)::float8 + ((-1::float8)::scalar)::float8;
 ?column? 
----------
        0
(1 row)

select ((1::float8)::scalar)::float8 - ((0::float8)::scalar)::float8;
 ?column? 
----------
        1
(1 row)

select ((1::float8)::scalar)::float8 * ((-1::float8)::scalar)::float8;
 ?column? 
----------
       -1
(1 row)

select ((1::float8)::scalar)::float8 / ((1::float8)::scalar)::float8;
 ?column? 
----------
        1
(1 row)

```
Test construction of min, zero and max values:
``` postgres-console
select '-1'::float8::scalar;
     scalar     
----------------
 fp64:-1.000000
(1 row)

select '0'::float8::scalar;
    scalar     
---------------
 fp64:0.000000
(1 row)

select '1'::float8::scalar;
    scalar     
---------------
 fp64:1.000000
(1 row)

```
Test setting a scalar value from max to 2
``` postgres-console
select set('1'::float8::scalar, 2);
      set      
---------------
 fp64:2.000000
(1 row)

```
Test setting to min, zero, max
``` postgres-console
select set('0'::float8::scalar, -1::float8);
      set       
----------------
 fp64:-1.000000
(1 row)

select set('1'::float8::scalar, 0::float8);
      set      
---------------
 fp64:0.000000
(1 row)

select set('-1'::float8::scalar, 1::float8);
      set      
---------------
 fp64:1.000000
(1 row)

```
Comparison operations (via extraction)
``` postgres-console
select ((1::float8)::scalar)::float8 = 1::float8;
 ?column? 
----------
 t
(1 row)

select ((-1::float8)::scalar)::float8 = -1::float8;
 ?column? 
----------
 t
(1 row)

select ((0::float8)::scalar)::float8 = 0::float8;
 ?column? 
----------
 t
(1 row)

select ((1::float8)::scalar)::float8 > -1::float8;
 ?column? 
----------
 t
(1 row)

select ((-1::float8)::scalar)::float8 < 1::float8;
 ?column? 
----------
 t
(1 row)

select ((1::float8)::scalar)::float8 >= 1::float8;
 ?column? 
----------
 t
(1 row)

select ((-1::float8)::scalar)::float8 <= -1::float8;
 ?column? 
----------
 t
(1 row)

select ((1::float8)::scalar)::float8 != -1::float8;
 ?column? 
----------
 t
(1 row)

```
Compare two scalar values (via extraction)
``` postgres-console
select ((1::float8)::scalar)::float8 = ((1::float8)::scalar)::float8;
 ?column? 
----------
 t
(1 row)

select ((1::float8)::scalar)::float8 > ((-1::float8)::scalar)::float8;
 ?column? 
----------
 t
(1 row)

select ((-1::float8)::scalar)::float8 < ((1::float8)::scalar)::float8;
 ?column? 
----------
 t
(1 row)

```
Edge case: operations with zero
``` postgres-console
select (0::float8)::scalar + 1::float8;
   ?column?    
---------------
 fp64:1.000000
(1 row)

select (1::float8)::scalar - 0::float8;
   ?column?    
---------------
 fp64:1.000000
(1 row)

select (0::float8)::scalar * 1::float8;
   ?column?    
---------------
 fp64:0.000000
(1 row)

select (1::float8)::scalar * 0::float8;
   ?column?    
---------------
 fp64:0.000000
(1 row)

```
Edge case: operations at boundaries
``` postgres-console
select (1::float8)::scalar + 0::float8;
   ?column?    
---------------
 fp64:1.000000
(1 row)

select (-1::float8)::scalar - 0::float8;
    ?column?    
----------------
 fp64:-1.000000
(1 row)

select (1::float8)::scalar / 1::float8;
   ?column?    
---------------
 fp64:1.000000
(1 row)

select (-1::float8)::scalar * 1::float8;
    ?column?    
----------------
 fp64:-1.000000
(1 row)

```
Test various casting functions used by the CREATE CAST machinery:
``` postgres-console
select scalar_float8((-1)::float8);
 scalar_float8  
----------------
 fp64:-1.000000
(1 row)

select scalar_float8((0)::float8);
 scalar_float8 
---------------
 fp64:0.000000
(1 row)

select scalar_float8((1)::float8);
 scalar_float8 
---------------
 fp64:1.000000
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
----------------
 fp64:-1.000000
(1 row)

select cast(0::float8 as scalar);
    scalar     
---------------
 fp64:0.000000
(1 row)

select cast(1::float8 as scalar);
    scalar     
---------------
 fp64:1.000000
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
---------------
 fp64:1.000000
(1 row)

```
Round-trip conversion tests (value should be preserved)
``` postgres-console
select (-1::float8)::scalar::float8 = -1::float8;
 ?column? 
----------
 t
(1 row)

select (0::float8)::scalar::float8 = 0::float8;
 ?column? 
----------
 t
(1 row)

select (1::float8)::scalar::float8 = 1::float8;
 ?column? 
----------
 t
(1 row)

```
Multiple round-trips should preserve value
``` postgres-console
select ((-1::float8)::scalar::float8)::scalar::float8 = -1::float8;
 ?column? 
----------
 t
(1 row)

select ((1::float8)::scalar::float8)::scalar::float8 = 1::float8;
 ?column? 
----------
 t
(1 row)

```
NULL handling
``` postgres-console
select NULL::float8::scalar;
 scalar 
--------
 fp64
(1 row)

select (1::float8)::scalar + NULL::float8;
ERROR:  Cannot pass NULL to scalar_plus_fp64
select NULL::float8 + (1::float8)::scalar;
ERROR:  Cannot pass NULL to plus_scalar_fp64
select NULL::float8 * (0::float8)::scalar;
ERROR:  Cannot pass NULL to mult_scalar_fp64
```
Utility functions
``` postgres-console
select nvals((1::float8)::scalar) as has_value;
 has_value 
-----------
         1
(1 row)

select nvals(('fp64')::scalar) as empty_scalar;
 empty_scalar 
--------------
            0
(1 row)

select type((1::float8)::scalar);
 type 
------
 fp64
(1 row)

select print((1::float8)::scalar);
  print   
----------
 1.000000
(1 row)

select dup((1::float8)::scalar)::float8;
 dup 
-----
   1
(1 row)

select wait((1::float8)::scalar)::float8;
 wait 
------
    1
(1 row)

select nvals(clear((1::float8)::scalar)) as cleared;
 cleared 
---------
       0
(1 row)

```