
## Scalar `integer`

Test various scalar math operations with native Postgres types
```
select (1::integer)::scalar + -1::integer;
select 1::integer + (-1::integer)::scalar;
select (1::integer)::scalar - 1::integer;
select 1::integer - (1::integer)::scalar;
select (1::integer)::scalar * -1::integer;
select 1::integer * (-1::integer)::scalar;
select (1::integer)::scalar / 1::integer;
select 1::integer / (1::integer)::scalar;

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
These casting functions cast the Postgres type `integer` to the
GraphBLAS scalar type `GrB_INT32`.
```

select cast(-1::integer as scalar);
select cast(0::integer as scalar);
select cast(1::integer as scalar);

```
These tests cast back from the scalar type `GrB_INT32` to the
Postgres type `integer`
```

select cast((-1::integer)::scalar as integer);
select cast((0::integer)::scalar as  integer);
select cast((1::integer)::scalar as scalar);
```