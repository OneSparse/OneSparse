```
\pset linestyle unicode
\pset border 2
```
# Types

```
set client_min_messages = 'WARNING'; -- pragma:hide
create extension if not exists onesparse;

```
OneSparse exposes SuiteSparse:GraphBLAS objects as native Postgres
types.  These types are:
```

\dT onesparse.*

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
```

select 'int32'::type;

```
Matrix, Vector and Scalar constructors take a type argument.  The
name is implicity cast to type in this case.  The two examples:
```

select matrix('int32');
select matrix('int32'::type);

```
## User Defined Types
```

insert into user_defined_type (name, type_def) values
    ('tuple_fp64', 'typedef struct {int64_t k; double v;} tuple_fp64;');

```

```

select 'tuple_fp64'::type;

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