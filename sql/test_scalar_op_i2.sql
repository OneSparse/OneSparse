-- # Scalar `smallint`
--
-- Test various scalar math operations with native Postgres types
select (1::smallint)::scalar + -1::smallint;
select 1::smallint + (-1::smallint)::scalar;
select (1::smallint)::scalar - 1::smallint;
select 1::smallint - (1::smallint)::scalar;
select (1::smallint)::scalar * -1::smallint;
select 1::smallint * (-1::smallint)::scalar;
select (1::smallint)::scalar / 1::smallint;
select 1::smallint / (1::smallint)::scalar;

-- Test construction of min, zero and max values:
select '-1'::smallint::scalar;
select '0'::smallint::scalar;
select '1'::smallint::scalar;

-- Test setting a scalar value from max to 2
select set('1'::smallint::scalar, 2);

-- Test various casting functions used by the CREATE CAST machinery:
select scalar_smallint((-1)::smallint);
select scalar_smallint((0)::smallint);
select scalar_smallint((1)::smallint);
select smallint_scalar((-1)::smallint::scalar);
select smallint_scalar((0)::smallint::scalar);
select smallint_scalar((1)::smallint::scalar);

-- These casting functions cast the Postgres type `smallint` to the
-- GraphBLAS scalar type `GrB_INT16`.

select cast(-1::smallint as scalar);
select cast(0::smallint as scalar);
select cast(1::smallint as scalar);

-- These tests cast back from the scalar type `GrB_INT16` to the
-- Postgres type `smallint`

select cast((-1::smallint)::scalar as smallint);
select cast((0::smallint)::scalar as  smallint);
select cast((1::smallint)::scalar as scalar);
