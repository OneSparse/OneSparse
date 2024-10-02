# Scalar `bigint` Functions

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
select '-1'::bigint::scalar;
 scalar 
--------
 i8:-1
(1 row)

select '0'::bigint::scalar;
 scalar 
--------
 i8:0
(1 row)

select '1'::bigint::scalar;
 scalar 
--------
 i8:1
(1 row)

```
Test setting a scalar value from max to 2
``` postgres-console
select set('1'::bigint::scalar, 2);
 set  
------
 i8:2
(1 row)

```
Test various casting functions used by the CREATE CAST machinery:
``` postgres-console
select scalar_bigint((-1)::bigint);
 scalar_bigint 
---------------
 i8:-1
(1 row)

select scalar_bigint((0)::bigint);
 scalar_bigint 
---------------
 i8:0
(1 row)

select scalar_bigint((1)::bigint);
 scalar_bigint 
---------------
 i8:1
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