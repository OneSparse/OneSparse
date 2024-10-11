-- # Scalar `{type.pgtype}`
--
-- Test various scalar math operations with native Postgres types
select ({type.max}::{type.pgtype})::scalar + {type.min}::{type.pgtype};
select {type.max}::{type.pgtype} + ({type.min}::{type.pgtype})::scalar;
select ({type.max}::{type.pgtype})::scalar - {type.max}::{type.pgtype};
select {type.max}::{type.pgtype} - ({type.max}::{type.pgtype})::scalar;
select ({type.max}::{type.pgtype})::scalar * {type.min}::{type.pgtype};
select {type.max}::{type.pgtype} * ({type.min}::{type.pgtype})::scalar;
select ({type.max}::{type.pgtype})::scalar / {type.max}::{type.pgtype};
select {type.max}::{type.pgtype} / ({type.max}::{type.pgtype})::scalar;

-- Test construction of min, zero and max values:
select '{type.min}'::{type.pgtype}::scalar;
select '{type.zero}'::{type.pgtype}::scalar;
select '{type.max}'::{type.pgtype}::scalar;

-- Test setting a scalar value from max to 2
select set('{type.max}'::{type.pgtype}::scalar, 2);

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
