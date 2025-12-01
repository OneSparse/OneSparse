
## Scalar `float8`

Test various scalar math operations with native Postgres types
```

```
Scalar-native arithmetic (scalar op native)
```
select (1::float8)::scalar + -1::float8;
select (1::float8)::scalar - 1::float8;
select (1::float8)::scalar * -1::float8;
select (1::float8)::scalar / 1::float8;

```
Native-scalar arithmetic (native op scalar)
```
select 1::float8 + (-1::float8)::scalar;
select 1::float8 - (1::float8)::scalar;
select 1::float8 * (-1::float8)::scalar;
select 1::float8 / (1::float8)::scalar;

```
Scalar-scalar arithmetic (via native extraction)
```
select ((1::float8)::scalar)::float8 + ((-1::float8)::scalar)::float8;
select ((1::float8)::scalar)::float8 - ((0::float8)::scalar)::float8;
select ((1::float8)::scalar)::float8 * ((-1::float8)::scalar)::float8;
select ((1::float8)::scalar)::float8 / ((1::float8)::scalar)::float8;

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
Test setting to min, zero, max
```
select set('0'::float8::scalar, -1::float8);
select set('1'::float8::scalar, 0::float8);
select set('-1'::float8::scalar, 1::float8);

```
Comparison operations (via extraction)
```
select ((1::float8)::scalar)::float8 = 1::float8;
select ((-1::float8)::scalar)::float8 = -1::float8;
select ((0::float8)::scalar)::float8 = 0::float8;
select ((1::float8)::scalar)::float8 > -1::float8;
select ((-1::float8)::scalar)::float8 < 1::float8;
select ((1::float8)::scalar)::float8 >= 1::float8;
select ((-1::float8)::scalar)::float8 <= -1::float8;
select ((1::float8)::scalar)::float8 != -1::float8;

```
Compare two scalar values (via extraction)
```
select ((1::float8)::scalar)::float8 = ((1::float8)::scalar)::float8;
select ((1::float8)::scalar)::float8 > ((-1::float8)::scalar)::float8;
select ((-1::float8)::scalar)::float8 < ((1::float8)::scalar)::float8;

```
Edge case: operations with zero
```
select (0::float8)::scalar + 1::float8;
select (1::float8)::scalar - 0::float8;
select (0::float8)::scalar * 1::float8;
select (1::float8)::scalar * 0::float8;

```
Edge case: operations at boundaries
```
select (1::float8)::scalar + 0::float8;
select (-1::float8)::scalar - 0::float8;
select (1::float8)::scalar / 1::float8;
select (-1::float8)::scalar * 1::float8;

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
Round-trip conversion tests (value should be preserved)
```
select (-1::float8)::scalar::float8 = -1::float8;
select (0::float8)::scalar::float8 = 0::float8;
select (1::float8)::scalar::float8 = 1::float8;

```
Multiple round-trips should preserve value
```
select ((-1::float8)::scalar::float8)::scalar::float8 = -1::float8;
select ((1::float8)::scalar::float8)::scalar::float8 = 1::float8;

```
NULL handling
```
select NULL::float8::scalar;
select (1::float8)::scalar + NULL::float8;
select NULL::float8 + (1::float8)::scalar;
select NULL::float8 * (0::float8)::scalar;

```
Utility functions
```
select nvals((1::float8)::scalar) as has_value;
select nvals(('fp64')::scalar) as empty_scalar;
select type((1::float8)::scalar);
select print((1::float8)::scalar);
select dup((1::float8)::scalar)::float8;
select wait((1::float8)::scalar)::float8;
select nvals(clear((1::float8)::scalar)) as cleared;
```