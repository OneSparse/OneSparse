# Monoid

In [abstract
algebra](https://en.wikipedia.org/wiki/Abstract_algebra "Abstract
algebra"), a **monoid** is a set equipped with an
[associative](https://en.wikipedia.org/wiki/Associative
"Associative") [binary
operation](https://en.wikipedia.org/wiki/Binary_operation "Binary
operation") and an [identity
element](https://en.wikipedia.org/wiki/Identity_element "Identity
element"). For example, the nonnegative
[integers](https://en.wikipedia.org/wiki/Integer "Integer") with
addition form a monoid, the identity element being 0.

## Monoid identity

``` postgres-console
select ident('plus_monoid_int8');
┌────────┐
│ ident  │
├────────┤
│ int8:0 │
└────────┘
(1 row)

select ident('plus_monoid_int16');
┌─────────┐
│  ident  │
├─────────┤
│ int16:0 │
└─────────┘
(1 row)

select ident('plus_monoid_fp32');
┌───────────────┐
│     ident     │
├───────────────┤
│ fp32:0.000000 │
└───────────────┘
(1 row)

select ident('min_monoid_int8');
┌──────────┐
│  ident   │
├──────────┤
│ int8:127 │
└──────────┘
(1 row)

select ident('min_monoid_int16');
┌─────────────┐
│    ident    │
├─────────────┤
│ int16:32767 │
└─────────────┘
(1 row)

select ident('min_monoid_fp32');
┌───────────────┐
│     ident     │
├───────────────┤
│ fp32:Infinity │
└───────────────┘
(1 row)

select ident('max_monoid_int8');
┌───────────┐
│   ident   │
├───────────┤
│ int8:-128 │
└───────────┘
(1 row)

select ident('max_monoid_int16');
┌──────────────┐
│    ident     │
├──────────────┤
│ int16:-32768 │
└──────────────┘
(1 row)

select ident('max_monoid_fp32');
┌────────────────┐
│     ident      │
├────────────────┤
│ fp32:-Infinity │
└────────────────┘
(1 row)

```
## Monoid terminal

Monoid terminals tell SuiteSparse when it can terminate a large
array operation because the terminal value has been found.

``` postgres-console
select terminal('plus_monoid_int8');
┌──────────┐
│ terminal │
├──────────┤
│ int      │
└──────────┘
(1 row)

select terminal('plus_monoid_int16');
┌──────────┐
│ terminal │
├──────────┤
│ int      │
└──────────┘
(1 row)

select terminal('min_monoid_int8');
┌───────────┐
│ terminal  │
├───────────┤
│ int8:-128 │
└───────────┘
(1 row)

select terminal('min_monoid_int16');
┌──────────────┐
│   terminal   │
├──────────────┤
│ int16:-32768 │
└──────────────┘
(1 row)

select terminal('max_monoid_int8');
┌──────────┐
│ terminal │
├──────────┤
│ int8:127 │
└──────────┘
(1 row)

select terminal('max_monoid_int16');
┌─────────────┐
│  terminal   │
├─────────────┤
│ int16:32767 │
└─────────────┘
(1 row)

select terminal('land_monoid_bool');
┌──────────┐
│ terminal │
├──────────┤
│ bool:f   │
└──────────┘
(1 row)

select terminal('lor_monoid_bool');
┌──────────┐
│ terminal │
├──────────┤
│ bool:t   │
└──────────┘
(1 row)

```
## Builtin SuiteSparse Monoids

| OneSparse Name | SuiteSparse Name |
|----------------|------------------|
| min_monoid_int8 | GrB_MIN_MONOID_INT8 |
| min_monoid_int16 | GrB_MIN_MONOID_INT16 |
| min_monoid_int32 | GrB_MIN_MONOID_INT32 |
| min_monoid_int64 | GrB_MIN_MONOID_INT64 |
| min_monoid_uint8 | GrB_MIN_MONOID_UINT8 |
| min_monoid_uint16 | GrB_MIN_MONOID_UINT16 |
| min_monoid_uint32 | GrB_MIN_MONOID_UINT32 |
| min_monoid_uint64 | GrB_MIN_MONOID_UINT64 |
| min_monoid_fp32 | GrB_MIN_MONOID_FP32 |
| min_monoid_fp64 | GrB_MIN_MONOID_FP64 |
| max_monoid_int8 | GrB_MAX_MONOID_INT8 |
| max_monoid_int16 | GrB_MAX_MONOID_INT16 |
| max_monoid_int32 | GrB_MAX_MONOID_INT32 |
| max_monoid_int64 | GrB_MAX_MONOID_INT64 |
| max_monoid_uint8 | GrB_MAX_MONOID_UINT8 |
| max_monoid_uint16 | GrB_MAX_MONOID_UINT16 |
| max_monoid_uint32 | GrB_MAX_MONOID_UINT32 |
| max_monoid_uint64 | GrB_MAX_MONOID_UINT64 |
| max_monoid_fp32 | GrB_MAX_MONOID_FP32 |
| max_monoid_fp64 | GrB_MAX_MONOID_FP64 |
| plus_monoid_int8 | GrB_PLUS_MONOID_INT8 |
| plus_monoid_int16 | GrB_PLUS_MONOID_INT16 |
| plus_monoid_int32 | GrB_PLUS_MONOID_INT32 |
| plus_monoid_int64 | GrB_PLUS_MONOID_INT64 |
| plus_monoid_uint8 | GrB_PLUS_MONOID_UINT8 |
| plus_monoid_uint16 | GrB_PLUS_MONOID_UINT16 |
| plus_monoid_uint32 | GrB_PLUS_MONOID_UINT32 |
| plus_monoid_uint64 | GrB_PLUS_MONOID_UINT64 |
| plus_monoid_fp32 | GrB_PLUS_MONOID_FP32 |
| plus_monoid_fp64 | GrB_PLUS_MONOID_FP64 |
| plus_fc32_monoid | GxB_PLUS_FC32_MONOID |
| plus_fc64_monoid | GxB_PLUS_FC64_MONOID |
| times_monoid_int8 | GrB_TIMES_MONOID_INT8 |
| times_monoid_int16 | GrB_TIMES_MONOID_INT16 |
| times_monoid_int32 | GrB_TIMES_MONOID_INT32 |
| times_monoid_int64 | GrB_TIMES_MONOID_INT64 |
| times_monoid_uint8 | GrB_TIMES_MONOID_UINT8 |
| times_monoid_uint16 | GrB_TIMES_MONOID_UINT16 |
| times_monoid_uint32 | GrB_TIMES_MONOID_UINT32 |
| times_monoid_uint64 | GrB_TIMES_MONOID_UINT64 |
| times_monoid_fp32 | GrB_TIMES_MONOID_FP32 |
| times_monoid_fp64 | GrB_TIMES_MONOID_FP64 |
| times_fc32_monoid | GxB_TIMES_FC32_MONOID |
| times_fc64_monoid | GxB_TIMES_FC64_MONOID |
| any_bool_monoid | GxB_ANY_BOOL_MONOID |
| any_int8_monoid | GxB_ANY_INT8_MONOID |
| any_int16_monoid | GxB_ANY_INT16_MONOID |
| any_int32_monoid | GxB_ANY_INT32_MONOID |
| any_int64_monoid | GxB_ANY_INT64_MONOID |
| any_uint8_monoid | GxB_ANY_UINT8_MONOID |
| any_uint16_monoid | GxB_ANY_UINT16_MONOID |
| any_uint32_monoid | GxB_ANY_UINT32_MONOID |
| any_uint64_monoid | GxB_ANY_UINT64_MONOID |
| any_fp32_monoid | GxB_ANY_FP32_MONOID |
| any_fp64_monoid | GxB_ANY_FP64_MONOID |
| any_fc32_monoid | GxB_ANY_FC32_MONOID |
| any_fc64_monoid | GxB_ANY_FC64_MONOID |
| bor_uint8_monoid | GxB_BOR_UINT8_MONOID |
| bor_uint16_monoid | GxB_BOR_UINT16_MONOID |
| bor_uint32_monoid | GxB_BOR_UINT32_MONOID |
| bor_uint64_monoid | GxB_BOR_UINT64_MONOID |
| band_uint8_monoid | GxB_BAND_UINT8_MONOID |
| band_uint16_monoid | GxB_BAND_UINT16_MONOID |
| band_uint32_monoid | GxB_BAND_UINT32_MONOID |
| band_uint64_monoid | GxB_BAND_UINT64_MONOID |
| bxor_uint8_monoid | GxB_BXOR_UINT8_MONOID |
| bxor_uint16_monoid | GxB_BXOR_UINT16_MONOID |
| bxor_uint32_monoid | GxB_BXOR_UINT32_MONOID |
| bxor_uint64_monoid | GxB_BXOR_UINT64_MONOID |
| bxnor_uint8_monoid | GxB_BXNOR_UINT8_MONOID |
| bxnor_uint16_monoid | GxB_BXNOR_UINT16_MONOID |
| bxnor_uint32_monoid | GxB_BXNOR_UINT32_MONOID |
| bxnor_uint64_monoid | GxB_BXNOR_UINT64_MONOID |
| lor_monoid_bool | GrB_LOR_MONOID_BOOL |
| land_monoid_bool | GrB_LAND_MONOID_BOOL |
| lxor_monoid_bool | GrB_LXOR_MONOID_BOOL |
| lxnor_monoid_bool | GrB_LXNOR_MONOID_BOOL |
| min_int8_monoid | GxB_MIN_INT8_MONOID |
| min_int16_monoid | GxB_MIN_INT16_MONOID |
| min_int32_monoid | GxB_MIN_INT32_MONOID |
| min_int64_monoid | GxB_MIN_INT64_MONOID |
| min_uint8_monoid | GxB_MIN_UINT8_MONOID |
| min_uint16_monoid | GxB_MIN_UINT16_MONOID |
| min_uint32_monoid | GxB_MIN_UINT32_MONOID |
| min_uint64_monoid | GxB_MIN_UINT64_MONOID |
| min_fp32_monoid | GxB_MIN_FP32_MONOID |
| min_fp64_monoid | GxB_MIN_FP64_MONOID |
| max_int8_monoid | GxB_MAX_INT8_MONOID |
| max_int16_monoid | GxB_MAX_INT16_MONOID |
| max_int32_monoid | GxB_MAX_INT32_MONOID |
| max_int64_monoid | GxB_MAX_INT64_MONOID |
| max_uint8_monoid | GxB_MAX_UINT8_MONOID |
| max_uint16_monoid | GxB_MAX_UINT16_MONOID |
| max_uint32_monoid | GxB_MAX_UINT32_MONOID |
| max_uint64_monoid | GxB_MAX_UINT64_MONOID |
| max_fp32_monoid | GxB_MAX_FP32_MONOID |
| max_fp64_monoid | GxB_MAX_FP64_MONOID |
| plus_int8_monoid | GxB_PLUS_INT8_MONOID |
| plus_int16_monoid | GxB_PLUS_INT16_MONOID |
| plus_int32_monoid | GxB_PLUS_INT32_MONOID |
| plus_int64_monoid | GxB_PLUS_INT64_MONOID |
| plus_uint8_monoid | GxB_PLUS_UINT8_MONOID |
| plus_uint16_monoid | GxB_PLUS_UINT16_MONOID |
| plus_uint32_monoid | GxB_PLUS_UINT32_MONOID |
| plus_uint64_monoid | GxB_PLUS_UINT64_MONOID |
| plus_fp32_monoid | GxB_PLUS_FP32_MONOID |
| plus_fp64_monoid | GxB_PLUS_FP64_MONOID |
| times_int8_monoid | GxB_TIMES_INT8_MONOID |
| times_int16_monoid | GxB_TIMES_INT16_MONOID |
| times_int32_monoid | GxB_TIMES_INT32_MONOID |
| times_int64_monoid | GxB_TIMES_INT64_MONOID |
| times_uint8_monoid | GxB_TIMES_UINT8_MONOID |
| times_uint16_monoid | GxB_TIMES_UINT16_MONOID |
| times_uint32_monoid | GxB_TIMES_UINT32_MONOID |
| times_uint64_monoid | GxB_TIMES_UINT64_MONOID |
| times_fp32_monoid | GxB_TIMES_FP32_MONOID |
| times_fp64_monoid | GxB_TIMES_FP64_MONOID |
| lor_bool_monoid | GxB_LOR_BOOL_MONOID |
| land_bool_monoid | GxB_LAND_BOOL_MONOID |
| lxor_bool_monoid | GxB_LXOR_BOOL_MONOID |
| lxnor_bool_monoid | GxB_LXNOR_BOOL_MONOID |
| eq_bool_monoid | GxB_EQ_BOOL_MONOID |