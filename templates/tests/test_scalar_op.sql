--
-- ## Scalar `{type.pgtype}`
--
-- Test various scalar math operations with native Postgres types

-- Scalar-native arithmetic (scalar op native)
select ({type.max}::{type.pgtype})::scalar + {type.min}::{type.pgtype};
select ({type.max}::{type.pgtype})::scalar - {type.max}::{type.pgtype};
select ({type.max}::{type.pgtype})::scalar * {type.min}::{type.pgtype};
select ({type.max}::{type.pgtype})::scalar / {type.max}::{type.pgtype};

-- Native-scalar arithmetic (native op scalar)
select {type.max}::{type.pgtype} + ({type.min}::{type.pgtype})::scalar;
select {type.max}::{type.pgtype} - ({type.max}::{type.pgtype})::scalar;
select {type.max}::{type.pgtype} * ({type.min}::{type.pgtype})::scalar;
select {type.max}::{type.pgtype} / ({type.max}::{type.pgtype})::scalar;

-- Scalar-scalar arithmetic (via native extraction)
select (({type.max}::{type.pgtype})::scalar)::{type.pgtype} + (({type.min}::{type.pgtype})::scalar)::{type.pgtype};
select (({type.max}::{type.pgtype})::scalar)::{type.pgtype} - (({type.zero}::{type.pgtype})::scalar)::{type.pgtype};
select (({type.max}::{type.pgtype})::scalar)::{type.pgtype} * (({type.min}::{type.pgtype})::scalar)::{type.pgtype};
select (({type.max}::{type.pgtype})::scalar)::{type.pgtype} / (({type.max}::{type.pgtype})::scalar)::{type.pgtype};

-- Test construction of min, zero and max values:
select '{type.min}'::{type.pgtype}::scalar;
select '{type.zero}'::{type.pgtype}::scalar;
select '{type.max}'::{type.pgtype}::scalar;

-- Test setting a scalar value from max to 2
select set('{type.max}'::{type.pgtype}::scalar, 2);

-- Test setting to min, zero, max
select set('{type.zero}'::{type.pgtype}::scalar, {type.min}::{type.pgtype});
select set('{type.max}'::{type.pgtype}::scalar, {type.zero}::{type.pgtype});
select set('{type.min}'::{type.pgtype}::scalar, {type.max}::{type.pgtype});

-- Comparison operations (via extraction)
select (({type.max}::{type.pgtype})::scalar)::{type.pgtype} = {type.max}::{type.pgtype};
select (({type.min}::{type.pgtype})::scalar)::{type.pgtype} = {type.min}::{type.pgtype};
select (({type.zero}::{type.pgtype})::scalar)::{type.pgtype} = {type.zero}::{type.pgtype};
select (({type.max}::{type.pgtype})::scalar)::{type.pgtype} > {type.min}::{type.pgtype};
select (({type.min}::{type.pgtype})::scalar)::{type.pgtype} < {type.max}::{type.pgtype};
select (({type.max}::{type.pgtype})::scalar)::{type.pgtype} >= {type.max}::{type.pgtype};
select (({type.min}::{type.pgtype})::scalar)::{type.pgtype} <= {type.min}::{type.pgtype};
select (({type.max}::{type.pgtype})::scalar)::{type.pgtype} != {type.min}::{type.pgtype};

-- Compare two scalar values (via extraction)
select (({type.max}::{type.pgtype})::scalar)::{type.pgtype} = (({type.max}::{type.pgtype})::scalar)::{type.pgtype};
select (({type.max}::{type.pgtype})::scalar)::{type.pgtype} > (({type.min}::{type.pgtype})::scalar)::{type.pgtype};
select (({type.min}::{type.pgtype})::scalar)::{type.pgtype} < (({type.max}::{type.pgtype})::scalar)::{type.pgtype};

-- Edge case: operations with zero
select ({type.zero}::{type.pgtype})::scalar + {type.max}::{type.pgtype};
select ({type.max}::{type.pgtype})::scalar - {type.zero}::{type.pgtype};
select ({type.zero}::{type.pgtype})::scalar * {type.max}::{type.pgtype};
select ({type.max}::{type.pgtype})::scalar * {type.zero}::{type.pgtype};

-- Edge case: operations at boundaries
select ({type.max}::{type.pgtype})::scalar + {type.zero}::{type.pgtype};
select ({type.min}::{type.pgtype})::scalar - {type.zero}::{type.pgtype};
select ({type.max}::{type.pgtype})::scalar / 1::{type.pgtype};
select ({type.min}::{type.pgtype})::scalar * 1::{type.pgtype};

-- Test various casting functions used by the CREATE CAST machinery:
select scalar_{type.pgtype}(({type.min})::{type.pgtype});
select scalar_{type.pgtype}(({type.zero})::{type.pgtype});
select scalar_{type.pgtype}(({type.max})::{type.pgtype});
select {type.pgtype}_scalar(({type.min})::{type.pgtype}::scalar);
select {type.pgtype}_scalar(({type.zero})::{type.pgtype}::scalar);
select {type.pgtype}_scalar(({type.max})::{type.pgtype}::scalar);

-- These casting functions cast the Postgres type `{type.pgtype}` to the
-- GraphBLAS scalar type `{type.gbtype}`.

select cast({type.min}::{type.pgtype} as scalar);
select cast({type.zero}::{type.pgtype} as scalar);
select cast({type.max}::{type.pgtype} as scalar);

-- These tests cast back from the scalar type `{type.gbtype}` to the
-- Postgres type `{type.pgtype}`

select cast(({type.min}::{type.pgtype})::scalar as {type.pgtype});
select cast(({type.zero}::{type.pgtype})::scalar as  {type.pgtype});
select cast(({type.max}::{type.pgtype})::scalar as scalar);

-- Round-trip conversion tests (value should be preserved)
select ({type.min}::{type.pgtype})::scalar::{type.pgtype} = {type.min}::{type.pgtype};
select ({type.zero}::{type.pgtype})::scalar::{type.pgtype} = {type.zero}::{type.pgtype};
select ({type.max}::{type.pgtype})::scalar::{type.pgtype} = {type.max}::{type.pgtype};

-- Multiple round-trips should preserve value
select (({type.min}::{type.pgtype})::scalar::{type.pgtype})::scalar::{type.pgtype} = {type.min}::{type.pgtype};
select (({type.max}::{type.pgtype})::scalar::{type.pgtype})::scalar::{type.pgtype} = {type.max}::{type.pgtype};

-- NULL handling
select NULL::{type.pgtype}::scalar;
select ({type.max}::{type.pgtype})::scalar + NULL::{type.pgtype};
select NULL::{type.pgtype} + ({type.max}::{type.pgtype})::scalar;
select NULL::{type.pgtype} * ({type.zero}::{type.pgtype})::scalar;

-- Utility functions
select nvals(({type.max}::{type.pgtype})::scalar) as has_value;
select nvals(('{type.name}')::scalar) as empty_scalar;
select type(({type.max}::{type.pgtype})::scalar);
select print(({type.max}::{type.pgtype})::scalar);
select dup(({type.max}::{type.pgtype})::scalar)::{type.pgtype};
select wait(({type.max}::{type.pgtype})::scalar)::{type.pgtype};
select nvals(clear(({type.max}::{type.pgtype})::scalar)) as cleared;
