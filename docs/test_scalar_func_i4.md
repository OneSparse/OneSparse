# Scalar `integer` Functions

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
select '-1'::integer::scalar;
 scalar 
--------
 i4:-1
(1 row)

select '0'::integer::scalar;
 scalar 
--------
 i4:0
(1 row)

select '1'::integer::scalar;
 scalar 
--------
 i4:1
(1 row)

```
Test setting a scalar value from max to 2
``` postgres-console
select set('1'::integer::scalar, 2);
 set  
------
 i4:2
(1 row)

```
Test various casting functions used by the CREATE CAST machinery:
``` postgres-console
select scalar_integer((-1)::integer);
 scalar_integer 
----------------
 i4:-1
(1 row)

select scalar_integer((0)::integer);
 scalar_integer 
----------------
 i4:0
(1 row)

select scalar_integer((1)::integer);
 scalar_integer 
----------------
 i4:1
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