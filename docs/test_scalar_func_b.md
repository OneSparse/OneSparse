# Scalar 'bool' Functions

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
select '0'::bool::scalar;
 scalar 
--------
 b:f
(1 row)

select '0'::bool::scalar;
 scalar 
--------
 b:f
(1 row)

select '1'::bool::scalar;
 scalar 
--------
 b:t
(1 row)

```
Test setting a scalar value from max to 2
``` postgres-console
select set('1'::bool::scalar, 2);
 set 
-----
 b:t
(1 row)

```
Test various casting functions used by the CREATE CAST machinery:
``` postgres-console
select scalar_bool((0)::bool);
 scalar_bool 
-------------
 b:f
(1 row)

select scalar_bool((0)::bool);
 scalar_bool 
-------------
 b:f
(1 row)

select scalar_bool((1)::bool);
 scalar_bool 
-------------
 b:t
(1 row)

select bool_scalar((0)::bool::scalar);
 bool_scalar 
-------------
 f
(1 row)

select bool_scalar((0)::bool::scalar);
 bool_scalar 
-------------
 f
(1 row)

select bool_scalar((1)::bool::scalar);
 bool_scalar 
-------------
 t
(1 row)

```