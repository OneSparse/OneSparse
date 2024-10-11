# Vector

This documentation is also tests for the code, the examples below
show the literal output of these statements from Postgres.

Some setup to make sure warnings are shown, and that the extension
is installed.
```

set client_min_messages = 'WARNING';
create extension if not exists onesparse;

\pset linestyle unicode
\pset border 2

```
An empty vector can be constructed many ways, but one of the
simplest is casting a type code to the matrix type.  In this case
`int32` means GrB_INT32.  The type codes are intentionally compressed
to be as short as possible for smaller pg_dumps.

Much of these functions are basically vector versions of the same
functions for matrix.  See those docs for details:
```

select 'int32'::vector;

select nvals('int32'::vector);

select size('int32'::vector);

select 'int32[]'::vector;

select nvals('int32[]'::vector);

select size('int32[]'::vector);

select 'int32(10)'::vector;

select nvals('int32(10)'::vector);

select size('int32(10)'::vector);

select 'int32(10)[]'::vector;

select nvals('int32(10)[]'::vector);

select size('int32(10)[]'::vector);

select 'int32[0:1 1:2 2:3]'::vector;

select nvals('int32[0:1 1:2 2:3]'::vector);

select * from elements('int32[0:1 1:2 2:3]'::vector);

select 'int32(10)[0:1 1:2 2:3]'::vector;

select size('int32(10)[0:1 1:2 2:3]'::vector);

select size('int32(2)[0:1 1:2 2:3]'::vector);

select eadd('int32[0:1 1:2 2:3]'::vector, 'int32[0:1 1:2 2:3]'::vector, 'plus_int32');

select emult('int32[0:1 1:2 2:3]'::vector, 'int32[0:1 1:2 2:3]'::vector, 'times_int32');

select eunion('int32[0:1 1:2 2:3]'::vector, 42, 'int32[0:1 1:2 2:3]'::vector, 84, 'plus_int32');

select reduce_scalar('int32[0:1 1:2 2:3]'::vector, 'plus_monoid_int32');

select selection('int32[0:1 1:2 2:3]'::vector, 'valuegt_int32', 1);

select apply('int32[1:1 2:2 3:3]'::vector, 'ainv_int32'::unaryop);

```
Elements can be set individually with `set_element`, the modified
input is returned:
```

select set_element('int32[1:1 2:2 3:3]'::vector, 4, 4);

```
Scalar elements can be extracted individually with `get_element`
```

select get_element('int32[1:1 2:2 3:3]'::vector, 3);

select print('int32(4)[1:1 2:2 3:3]'::vector);

select wait('int32[0:1 1:2 2:3]'::vector);

select dup('int32[0:1 1:2 2:3]'::vector);

select clear('int32[0:1 1:2 2:3]'::vector);
```