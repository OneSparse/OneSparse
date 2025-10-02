
## Scalar `bigint`

Test various scalar math operations with native Postgres types
```
select (1::bigint)::scalar + -1::bigint;
select 1::bigint + (-1::bigint)::scalar;
select (1::bigint)::scalar - 1::bigint;
select 1::bigint - (1::bigint)::scalar;
select (1::bigint)::scalar * -1::bigint;
select 1::bigint * (-1::bigint)::scalar;
select (1::bigint)::scalar / 1::bigint;
select 1::bigint / (1::bigint)::scalar;

```
Test construction of min, zero and max values:
```
select '-1'::bigint::scalar;
select '0'::bigint::scalar;
select '1'::bigint::scalar;

```
Test setting a scalar value from max to 2
```
select set('1'::bigint::scalar, 2);

```
Test various casting functions used by the CREATE CAST machinery:
```
select scalar_bigint((-1)::bigint);
select scalar_bigint((0)::bigint);
select scalar_bigint((1)::bigint);
select bigint_scalar((-1)::bigint::scalar);
select bigint_scalar((0)::bigint::scalar);
select bigint_scalar((1)::bigint::scalar);

```
These casting functions cast the Postgres type `bigint` to the
GraphBLAS scalar type `GrB_INT64`.
```

select cast(-1::bigint as scalar);
select cast(0::bigint as scalar);
select cast(1::bigint as scalar);

```
These tests cast back from the scalar type `GrB_INT64` to the
Postgres type `bigint`
```

select cast((-1::bigint)::scalar as bigint);
select cast((0::bigint)::scalar as  bigint);
select cast((1::bigint)::scalar as scalar);
```