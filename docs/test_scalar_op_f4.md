
## Scalar `float4`

Test various scalar math operations with native Postgres types
Scalar-native arithmetic (scalar op native)
``` postgres-console
select (1::float4)::scalar + -1::float4;
   ?column?    
---------------
 fp32:0.000000
(1 row)

select (1::float4)::scalar - 1::float4;
   ?column?    
---------------
 fp32:0.000000
(1 row)

select (1::float4)::scalar * -1::float4;
    ?column?    
----------------
 fp32:-1.000000
(1 row)

select (1::float4)::scalar / 1::float4;
   ?column?    
---------------
 fp32:1.000000
(1 row)

```
Native-scalar arithmetic (native op scalar)
``` postgres-console
select 1::float4 + (-1::float4)::scalar;
 ?column? 
----------
        0
(1 row)

select 1::float4 - (1::float4)::scalar;
 ?column? 
----------
        0
(1 row)

select 1::float4 * (-1::float4)::scalar;
 ?column? 
----------
       -1
(1 row)

select 1::float4 / (1::float4)::scalar;
 ?column? 
----------
        1
(1 row)

```
Scalar-scalar arithmetic (via native extraction)
``` postgres-console
select ((1::float4)::scalar)::float4 + ((-1::float4)::scalar)::float4;
 ?column? 
----------
        0
(1 row)

select ((1::float4)::scalar)::float4 - ((0::float4)::scalar)::float4;
 ?column? 
----------
        1
(1 row)

select ((1::float4)::scalar)::float4 * ((-1::float4)::scalar)::float4;
 ?column? 
----------
       -1
(1 row)

select ((1::float4)::scalar)::float4 / ((1::float4)::scalar)::float4;
 ?column? 
----------
        1
(1 row)

```
Test construction of min, zero and max values:
``` postgres-console
select '-1'::float4::scalar;
     scalar     
----------------
 fp32:-1.000000
(1 row)

select '0'::float4::scalar;
    scalar     
---------------
 fp32:0.000000
(1 row)

select '1'::float4::scalar;
    scalar     
---------------
 fp32:1.000000
(1 row)

```
Test setting a scalar value from max to 2
``` postgres-console
select set('1'::float4::scalar, 2);
      set      
---------------
 fp32:2.000000
(1 row)

```
Test setting to min, zero, max
``` postgres-console
select set('0'::float4::scalar, -1::float4);
      set       
----------------
 fp32:-1.000000
(1 row)

select set('1'::float4::scalar, 0::float4);
      set      
---------------
 fp32:0.000000
(1 row)

select set('-1'::float4::scalar, 1::float4);
      set      
---------------
 fp32:1.000000
(1 row)

```
Comparison operations (via extraction)
``` postgres-console
select ((1::float4)::scalar)::float4 = 1::float4;
 ?column? 
----------
 t
(1 row)

select ((-1::float4)::scalar)::float4 = -1::float4;
 ?column? 
----------
 t
(1 row)

select ((0::float4)::scalar)::float4 = 0::float4;
 ?column? 
----------
 t
(1 row)

select ((1::float4)::scalar)::float4 > -1::float4;
 ?column? 
----------
 t
(1 row)

select ((-1::float4)::scalar)::float4 < 1::float4;
 ?column? 
----------
 t
(1 row)

select ((1::float4)::scalar)::float4 >= 1::float4;
 ?column? 
----------
 t
(1 row)

select ((-1::float4)::scalar)::float4 <= -1::float4;
 ?column? 
----------
 t
(1 row)

select ((1::float4)::scalar)::float4 != -1::float4;
 ?column? 
----------
 t
(1 row)

```
Compare two scalar values (via extraction)
``` postgres-console
select ((1::float4)::scalar)::float4 = ((1::float4)::scalar)::float4;
 ?column? 
----------
 t
(1 row)

select ((1::float4)::scalar)::float4 > ((-1::float4)::scalar)::float4;
 ?column? 
----------
 t
(1 row)

select ((-1::float4)::scalar)::float4 < ((1::float4)::scalar)::float4;
 ?column? 
----------
 t
(1 row)

```
Edge case: operations with zero
``` postgres-console
select (0::float4)::scalar + 1::float4;
   ?column?    
---------------
 fp32:1.000000
(1 row)

select (1::float4)::scalar - 0::float4;
   ?column?    
---------------
 fp32:1.000000
(1 row)

select (0::float4)::scalar * 1::float4;
   ?column?    
---------------
 fp32:0.000000
(1 row)

select (1::float4)::scalar * 0::float4;
   ?column?    
---------------
 fp32:0.000000
(1 row)

```
Edge case: operations at boundaries
``` postgres-console
select (1::float4)::scalar + 0::float4;
   ?column?    
---------------
 fp32:1.000000
(1 row)

select (-1::float4)::scalar - 0::float4;
    ?column?    
----------------
 fp32:-1.000000
(1 row)

select (1::float4)::scalar / 1::float4;
   ?column?    
---------------
 fp32:1.000000
(1 row)

select (-1::float4)::scalar * 1::float4;
    ?column?    
----------------
 fp32:-1.000000
(1 row)

```
Test various casting functions used by the CREATE CAST machinery:
``` postgres-console
select scalar_float4((-1)::float4);
 scalar_float4  
----------------
 fp32:-1.000000
(1 row)

select scalar_float4((0)::float4);
 scalar_float4 
---------------
 fp32:0.000000
(1 row)

select scalar_float4((1)::float4);
 scalar_float4 
---------------
 fp32:1.000000
(1 row)

select float4_scalar((-1)::float4::scalar);
 float4_scalar 
---------------
            -1
(1 row)

select float4_scalar((0)::float4::scalar);
 float4_scalar 
---------------
             0
(1 row)

select float4_scalar((1)::float4::scalar);
 float4_scalar 
---------------
             1
(1 row)

```
These casting functions cast the Postgres type `float4` to the
GraphBLAS scalar type `GrB_INT32`.
``` postgres-console
select cast(-1::float4 as scalar);
     scalar     
----------------
 fp32:-1.000000
(1 row)

select cast(0::float4 as scalar);
    scalar     
---------------
 fp32:0.000000
(1 row)

select cast(1::float4 as scalar);
    scalar     
---------------
 fp32:1.000000
(1 row)

```
These tests cast back from the scalar type `GrB_INT32` to the
Postgres type `float4`
``` postgres-console
select cast((-1::float4)::scalar as float4);
 float4 
--------
     -1
(1 row)

select cast((0::float4)::scalar as  float4);
 float4 
--------
      0
(1 row)

select cast((1::float4)::scalar as scalar);
    scalar     
---------------
 fp32:1.000000
(1 row)

```
Round-trip conversion tests (value should be preserved)
``` postgres-console
select (-1::float4)::scalar::float4 = -1::float4;
 ?column? 
----------
 t
(1 row)

select (0::float4)::scalar::float4 = 0::float4;
 ?column? 
----------
 t
(1 row)

select (1::float4)::scalar::float4 = 1::float4;
 ?column? 
----------
 t
(1 row)

```
Multiple round-trips should preserve value
``` postgres-console
select ((-1::float4)::scalar::float4)::scalar::float4 = -1::float4;
 ?column? 
----------
 t
(1 row)

select ((1::float4)::scalar::float4)::scalar::float4 = 1::float4;
 ?column? 
----------
 t
(1 row)

```
NULL handling
``` postgres-console
select NULL::float4::scalar;
 scalar 
--------
 fp32
(1 row)

select (1::float4)::scalar + NULL::float4;
ERROR:  Cannot pass NULL to scalar_plus_fp32
select NULL::float4 + (1::float4)::scalar;
ERROR:  Cannot pass NULL to plus_scalar_fp32
select NULL::float4 * (0::float4)::scalar;
ERROR:  Cannot pass NULL to mult_scalar_fp32
```
Utility functions
``` postgres-console
select nvals((1::float4)::scalar) as has_value;
 has_value 
-----------
         1
(1 row)

select nvals(('fp32')::scalar) as empty_scalar;
 empty_scalar 
--------------
            0
(1 row)

select type((1::float4)::scalar);
 type 
------
 fp32
(1 row)

select print((1::float4)::scalar);
  print   
----------
 1.000000
(1 row)

select dup((1::float4)::scalar)::float4;
 dup 
-----
   1
(1 row)

select wait((1::float4)::scalar)::float4;
 wait 
------
    1
(1 row)

select nvals(clear((1::float4)::scalar)) as cleared;
 cleared 
---------
       0
(1 row)

```