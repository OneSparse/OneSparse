# Scalar `smallint` Functions

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
select '-1'::smallint::scalar;
 scalar 
--------
 i2:-1
(1 row)

select '0'::smallint::scalar;
 scalar 
--------
 i2:0
(1 row)

select '1'::smallint::scalar;
 scalar 
--------
 i2:1
(1 row)

```
Test setting a scalar value from max to 2
``` postgres-console
select set('1'::smallint::scalar, 2);
 set  
------
 i2:2
(1 row)

```
Test various casting functions used by the CREATE CAST machinery:
``` postgres-console
select scalar_smallint((-1)::smallint);
 scalar_smallint 
-----------------
 i2:-1
(1 row)

select scalar_smallint((0)::smallint);
 scalar_smallint 
-----------------
 i2:0
(1 row)

select scalar_smallint((1)::smallint);
 scalar_smallint 
-----------------
 i2:1
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