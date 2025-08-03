# Types

``` postgres-console
create extension if not exists onesparse;
```
OneSparse exposes SuiteSparse:GraphBLAS objects as native Postgres
types.  These types are:
``` postgres-console
                                                   List of data types
┌───────────┬──────────────────┬───────────────────────────────────────────────────────────────────────────────────────┐
│  Schema   │       Name       │                                      Description                                      │
├───────────┼──────────────────┼───────────────────────────────────────────────────────────────────────────────────────┤
│ onesparse │ argminmax_x_p    │ Return type for argminmax with value and index vectors.                               │
│ onesparse │ bfs_level_parent │ Return type for bfs with level and parent vectors.                                    │
│ onesparse │ binaryop         │ BinaryOps apply a function to two elements and returning an element.                  │
│ onesparse │ descriptor       │ Descriptors control specific details of GraphBLAS operations.                         │
│ onesparse │ graph            │ Graphs wrap the LAGraph library functions.                                            │
│ onesparse │ indexunaryop     │ IndexUnaryOps apply a function to a positional element, returning an element.         │
│ onesparse │ matrix           │ Matrices hold sparse rows and columns of elements.                                    │
│ onesparse │ monoid           │ Monoids apply a BinaryOp and an identity value to two elements, returning an element. │
│ onesparse │ scalar           │ Scalars hold individual element values.                                               │
│ onesparse │ semiring         │ Semirings associate additive and multiplicative operators for matrix multiplication.  │
│ onesparse │ type             │ Types define the structure and behavior of elements.                                  │
│ onesparse │ unaryop          │ UnaryOps apply a function to an element, returning an element.                        │
│ onesparse │ vector           │ Vectors hold a sparse array of elements.                                              │
└───────────┴──────────────────┴───────────────────────────────────────────────────────────────────────────────────────┘
(13 rows)

```
## Element Types

The GraphBLAS has both core types and user defined types that
define the value and structure of matrix elements.  Currently
onesparse maps these Postgres types to GraphBLAS types.
SuiteSparse provides a wider variety of numeric types than Postgres
(unsigned ints, complex, user defined).

| Postgres Type Name | SuiteSparse Type Name |
|--------------------|-----------------------|
| smallint           | int16                 |
| integer            | int32                 |
| bigint             | int64                 |
| float              | fp32                  |
| double precision   | fp64                  |
| bool               | bool                  |

Types can stored in tables in the database, they can be created by
casting the SuiteSparse type name to the `type` type:
``` postgres-console
select 'int32'::type;
┌───────┐
│ type  │
├───────┤
│ int32 │
└───────┘
(1 row)

```
Matrix, Vector and Scalar constructors take a type argument.  The
name is implicity cast to type in this case.  The two examples:
``` postgres-console
select matrix('int32');
┌────────┐
│ matrix │
├────────┤
│ int32  │
└────────┘
(1 row)

select matrix('int32'::type);
┌────────┐
│ matrix │
├────────┤
│ int32  │
└────────┘
(1 row)

```
Are equivalent.

| OneSparse Name | SuiteSparse Name |
|----------------|------------------|
| bool | GrB_BOOL |
| int8 | GrB_INT8 |
| int16 | GrB_INT16 |
| int32 | GrB_INT32 |
| int64 | GrB_INT64 |
| uint8 | GrB_UINT8 |
| uint16 | GrB_UINT16 |
| uint32 | GrB_UINT32 |
| uint64 | GrB_UINT64 |
| fp32 | GrB_FP32 |
| fp64 | GrB_FP64 |
| fc32 | GxB_FC32 |
| fc64 | GxB_FC64 |