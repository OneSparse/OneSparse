
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
## Scalar `smallint`

Test various scalar math operations with native Postgres types
```
select (1::smallint)::scalar + -1::smallint;
select 1::smallint + (-1::smallint)::scalar;
select (1::smallint)::scalar - 1::smallint;
select 1::smallint - (1::smallint)::scalar;
select (1::smallint)::scalar * -1::smallint;
select 1::smallint * (-1::smallint)::scalar;
select (1::smallint)::scalar / 1::smallint;
select 1::smallint / (1::smallint)::scalar;

```
Test construction of min, zero and max values:
```
select '-1'::smallint::scalar;
select '0'::smallint::scalar;
select '1'::smallint::scalar;

```
Test setting a scalar value from max to 2
```
select set('1'::smallint::scalar, 2);

```
Test various casting functions used by the CREATE CAST machinery:
```
select scalar_smallint((-1)::smallint);
select scalar_smallint((0)::smallint);
select scalar_smallint((1)::smallint);
select smallint_scalar((-1)::smallint::scalar);
select smallint_scalar((0)::smallint::scalar);
select smallint_scalar((1)::smallint::scalar);

```
These casting functions cast the Postgres type `smallint` to the
GraphBLAS scalar type `GrB_INT16`.
```

select cast(-1::smallint as scalar);
select cast(0::smallint as scalar);
select cast(1::smallint as scalar);

```
These tests cast back from the scalar type `GrB_INT16` to the
Postgres type `smallint`
```

select cast((-1::smallint)::scalar as smallint);
select cast((0::smallint)::scalar as  smallint);
select cast((1::smallint)::scalar as scalar);
```
## Scalar `float4`

Test various scalar math operations with native Postgres types
```
select (1::float4)::scalar + -1::float4;
select 1::float4 + (-1::float4)::scalar;
select (1::float4)::scalar - 1::float4;
select 1::float4 - (1::float4)::scalar;
select (1::float4)::scalar * -1::float4;
select 1::float4 * (-1::float4)::scalar;
select (1::float4)::scalar / 1::float4;
select 1::float4 / (1::float4)::scalar;

```
Test construction of min, zero and max values:
```
select '-1'::float4::scalar;
select '0'::float4::scalar;
select '1'::float4::scalar;

```
Test setting a scalar value from max to 2
```
select set('1'::float4::scalar, 2);

```
Test various casting functions used by the CREATE CAST machinery:
```
select scalar_float4((-1)::float4);
select scalar_float4((0)::float4);
select scalar_float4((1)::float4);
select float4_scalar((-1)::float4::scalar);
select float4_scalar((0)::float4::scalar);
select float4_scalar((1)::float4::scalar);

```
These casting functions cast the Postgres type `float4` to the
GraphBLAS scalar type `GrB_INT32`.
```

select cast(-1::float4 as scalar);
select cast(0::float4 as scalar);
select cast(1::float4 as scalar);

```
These tests cast back from the scalar type `GrB_INT32` to the
Postgres type `float4`
```

select cast((-1::float4)::scalar as float4);
select cast((0::float4)::scalar as  float4);
select cast((1::float4)::scalar as scalar);
```
## Scalar `float8`

Test various scalar math operations with native Postgres types
```
select (1::float8)::scalar + -1::float8;
select 1::float8 + (-1::float8)::scalar;
select (1::float8)::scalar - 1::float8;
select 1::float8 - (1::float8)::scalar;
select (1::float8)::scalar * -1::float8;
select 1::float8 * (-1::float8)::scalar;
select (1::float8)::scalar / 1::float8;
select 1::float8 / (1::float8)::scalar;

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
These casting functions cast the Postgres type `float8` to the
GraphBLAS scalar type `GrB_INT64`.
```

select cast(-1::float8 as scalar);
select cast(0::float8 as scalar);
select cast(1::float8 as scalar);

```
These tests cast back from the scalar type `GrB_INT64` to the
Postgres type `float8`
```

select cast((-1::float8)::scalar as float8);
select cast((0::float8)::scalar as  float8);
select cast((1::float8)::scalar as scalar);
```