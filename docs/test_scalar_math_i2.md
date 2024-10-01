# Scalar 'smallint' Math

This documentation is also tests for the code, the examples below
show the literal output of these statements from Postgres.

Some setup to make sure warnings are shown, and that the extension
is installed.
```
set client_min_messages = 'WARNING';
create extension if not exists onesparse;

```
Add mixed scalar objects and postgres scalar values
```
select (-1::smallint)::scalar + 1::smallint;
select -1::smallint + (1::smallint)::scalar;
```