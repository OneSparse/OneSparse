# Scalar `float4` Functions

This documentation is also tests for the code, the examples below
show the literal output of these statements from Postgres.

Some setup to make sure warnings are shown, and that the extension
is installed.
``` postgres-console
set client_min_messages = 'WARNING';
create extension if not exists onesparse;
```
Test construction of min, zero and max values:
``` postgres-console
select '-1'::float4::scalar;
    scalar    
--------------
 f4:-1.000000
(1 row)

select '0'::float4::scalar;
   scalar    
-------------
 f4:0.000000
(1 row)

select '1'::float4::scalar;
   scalar    
-------------
 f4:1.000000
(1 row)

```
Test setting a scalar value from max to 2
``` postgres-console
select set('1'::float4::scalar, 2);
     set     
-------------
 f4:2.000000
(1 row)

```
Test various casting functions used by the CREATE CAST machinery:
``` postgres-console
select scalar_float4((-1)::float4);
 scalar_float4 
---------------
 f4:-1.000000
(1 row)

select scalar_float4((0)::float4);
 scalar_float4 
---------------
 f4:0.000000
(1 row)

select scalar_float4((1)::float4);
 scalar_float4 
---------------
 f4:1.000000
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