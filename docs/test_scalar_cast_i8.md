# Scalar 'bigint' Casting

This documentation is also tests for the code, the examples below
show the literal output of these statements from Postgres.

Some setup to make sure warnings are shown, and that the extension
is installed.
```
set client_min_messages = 'WARNING';
create extension if not exists onesparse;

```
These casting functions cast the Postgres type bigint to the
GraphBLAS scalar type GrB_INT64.
```

select cast(-1::bigint as scalar);
select cast(0::bigint as scalar);
select cast(1::bigint as scalar);

```
These tests cast back from the scalar type GrB_INT64 to the
Postgres type bigint
```

select cast(-1::bigint::scalar as bigint);
select cast(0::bigint::scalar as  bigint);
select cast(1::bigint::scalar as scalar);
```