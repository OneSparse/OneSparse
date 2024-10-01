# Scalar Functions

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
select '-1'::float8::scalar;
select '0'::float8::scalar;
select '1'::float8::scalar;

```
Test setting a scalar value from max to 2
```
select set('1'::float8::scalar, 2);

```
Test various casting functions used by the CREATE CAST machinery:
```
select scalar_float8((-1)::float8);
select scalar_float8((0)::float8);
select scalar_float8((1)::float8);
select float8_scalar((-1)::float8::scalar);
select float8_scalar((0)::float8::scalar);
select float8_scalar((1)::float8::scalar);
```