# Scalar 'float8' Casting

This documentation is also tests for the code, the examples below
show the literal output of these statements from Postgres.

Some setup to make sure warnings are shown, and that the extension
is installed.
```
set client_min_messages = 'WARNING';
create extension if not exists onesparse;

```
These casting functions cast the Postgres type float8 to the
GraphBLAS scalar type GrB_INT64.
```

select cast(-1::float8 as scalar);
select cast(0::float8 as scalar);
select cast(1::float8 as scalar);

```
These tests cast back from the scalar type GrB_INT64 to the
Postgres type float8
```

select cast(-1::float8::scalar as float8);
select cast(0::float8::scalar as  float8);
select cast(1::float8::scalar as scalar);
```