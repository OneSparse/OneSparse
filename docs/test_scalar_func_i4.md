# Scalar 'integer' Functions

This documentation is also tests for the code, the examples below
show the literal output of these statements from Postgres.

Some setup to make sure warnings are shown, and that the extension
is installed.
```
set client_min_messages = 'WARNING';
create extension if not exists onesparse;

```
Test construction of min, zero and max values:
```
select '-1'::integer::scalar;
select '0'::integer::scalar;
select '1'::integer::scalar;

```
Test setting a scalar value from max to 2
```
select set('1'::integer::scalar, 2);

```
Test various casting functions used by the CREATE CAST machinery:
```
select scalar_integer((-1)::integer);
select scalar_integer((0)::integer);
select scalar_integer((1)::integer);
select integer_scalar((-1)::integer::scalar);
select integer_scalar((0)::integer::scalar);
select integer_scalar((1)::integer::scalar);
```