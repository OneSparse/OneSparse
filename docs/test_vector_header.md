# Vector

This documentation is also tests for the code, the examples below
show the literal output of these statements from Postgres.

Some setup to make sure warnings are shown, and that the extension
is installed.
``` postgres-console
set client_min_messages = 'WARNING';
create extension if not exists onesparse;
```
## Construction

Examples of creating vectors and inspecting their properties.
An empty vector can be constructed many ways, but one of the
simplest is casting a type code to the matrix type.  In this case
`int32` means GrB_INT32.  The type codes are intentionally compressed
to be as short as possible for smaller pg_dumps.

Much of these functions are basically vector versions of the same
functions for matrix.  See those docs for details:
Cast a type name to make an empty vector
``` postgres-console
select 'int32'::vector;
┌────────┐
│ vector │
├────────┤
│ int32  │
└────────┘
(1 row)

```
Use the constructor function
``` postgres-console
select vector('int32');
┌────────┐
│ vector │
├────────┤
│ int32  │
└────────┘
(1 row)

```
Count the stored elements
``` postgres-console
select nvals('int32'::vector);
┌───────┐
│ nvals │
├───────┤
│     0 │
└───────┘
(1 row)

```
Inspect the possible size of the vector
``` postgres-console
select size('int32'::vector);
┌─────────────────────┐
│        size         │
├─────────────────────┤
│ 1152921504606846976 │
└─────────────────────┘
(1 row)

```
Cast from an empty array
``` postgres-console
select 'int32[]'::vector;
┌────────┐
│ vector │
├────────┤
│ int32  │
└────────┘
(1 row)

```
Stored elements in the empty array
``` postgres-console
select nvals('int32[]'::vector);
┌───────┐
│ nvals │
├───────┤
│     0 │
└───────┘
(1 row)

```
Size of the unbounded empty array
``` postgres-console
select size('int32[]'::vector);
┌─────────────────────┐
│        size         │
├─────────────────────┤
│ 1152921504606846976 │
└─────────────────────┘
(1 row)

```
Create a bounded but empty vector
``` postgres-console
select 'int32(10)'::vector;
┌───────────┐
│  vector   │
├───────────┤
│ int32(10) │
└───────────┘
(1 row)

```
Bounded vectors start with zero elements
``` postgres-console
select nvals('int32(10)'::vector);
┌───────┐
│ nvals │
├───────┤
│     0 │
└───────┘
(1 row)

```
Its size reflects the bound
``` postgres-console
select size('int32(10)'::vector);
┌──────┐
│ size │
├──────┤
│   10 │
└──────┘
(1 row)

```
Alternate syntax for a bounded empty vector
``` postgres-console
select 'int32(10)[]'::vector;
┌───────────┐
│  vector   │
├───────────┤
│ int32(10) │
└───────────┘
(1 row)

```
It also has zero stored elements
``` postgres-console
select nvals('int32(10)[]'::vector);
┌───────┐
│ nvals │
├───────┤
│     0 │
└───────┘
(1 row)

```
And a fixed size
``` postgres-console
select size('int32(10)[]'::vector);
┌──────┐
│ size │
├──────┤
│   10 │
└──────┘
(1 row)

```
Create a vector with some values
``` postgres-console
select 'int32[0:1 1:2 2:3]'::vector;
┌────────────────────┐
│       vector       │
├────────────────────┤
│ int32[0:1 1:2 2:3] │
└────────────────────┘
(1 row)

```
Count those values
``` postgres-console
select nvals('int32[0:1 1:2 2:3]'::vector);
┌───────┐
│ nvals │
├───────┤
│     3 │
└───────┘
(1 row)

```
Iterate over the index/value pairs
``` postgres-console
select * from elements('int32[0:1 1:2 2:3]'::vector);
┌───┬─────────┐
│ i │    v    │
├───┼─────────┤
│ 0 │ int32:1 │
│ 1 │ int32:2 │
│ 2 │ int32:3 │
└───┴─────────┘
(3 rows)

```
Combine bounds and elements
``` postgres-console
select 'int32(10)[0:1 1:2 2:3]'::vector;
┌────────────────────────┐
│         vector         │
├────────────────────────┤
│ int32(10)[0:1 1:2 2:3] │
└────────────────────────┘
(1 row)

```
Bounded size is retained
``` postgres-console
select size('int32(10)[0:1 1:2 2:3]'::vector);
┌──────┐
│ size │
├──────┤
│   10 │
└──────┘
(1 row)

```
Attempting to store outside the bounds raises an error
``` postgres-console
select size('int32(2)[0:1 1:2 2:3]'::vector);
ERROR:  INVALID_INDEX GraphBLAS error: GrB_INVALID_INDEX
function: GrB_Vector_setElement_INT64 (w, x, row)
Row index 2 out of range; must be < 2: Error setting Vector Element
LINE 1: select size('int32(2)[0:1 1:2 2:3]'::vector);
                    ^
```
## Equality

Two matrices can be compared for equality with the '=' and '!=' operators:
``` postgres-console
select u != v as "u != v", u = v as "u = v", v = u as "v = u", v = u as "v = u" from test_fixture;
┌────────┬───────┬───────┬───────┐
│ u != v │ u = v │ v = u │ v = u │
├────────┼───────┼───────┼───────┤
│ t      │ f     │ f     │ f     │
└────────┴───────┴───────┴───────┘
(1 row)

```
## Elementwise Addition

Add two vectors element by element using a binary operator
``` postgres-console
select eadd('int32[0:1 1:2 2:3]'::vector, 'int32[0:1 1:2 2:3]'::vector, 'plus_int32');
┌────────────────────┐
│        eadd        │
├────────────────────┤
│ int32[0:2 1:4 2:6] │
└────────────────────┘
(1 row)

```
Eadd can also be accomplished with binary operators specific to
OneSparse.  Different binaryops are passed to eadd to do different
elementwise operations:
``` postgres-console
select print(u |+ v) as "u |+ v", print(u |- v) as "u |- v", print(u |* v) as "u |* v", print(u |/ v) as "u |/ v" from test_fixture;
┌───────────┬───────────┬───────────┬───────────┐
│  u |+ v   │  u |- v   │  u |* v   │  u |/ v   │
├───────────┼───────────┼───────────┼───────────┤
│           │           │           │           │
│    ───    │    ───    │    ───    │    ───    │
│  0│       │  0│       │  0│       │  0│       │
│  1│  5    │  1│ -1    │  1│  6    │  1│  0    │
│  2│  3    │  2│  3    │  2│  3    │  2│  3    │
│  3│       │  3│       │  3│       │  3│       │
│           │           │           │           │
└───────────┴───────────┴───────────┴───────────┘
(1 row)

```
## Elementwise Multiplication

Multiply corresponding elements of two vectors
``` postgres-console
select emult('int32[0:1 1:2 2:3]'::vector, 'int32[0:1 1:2 2:3]'::vector, 'times_int32');
┌────────────────────┐
│       emult        │
├────────────────────┤
│ int32[0:1 1:4 2:9] │
└────────────────────┘
(1 row)

```
Emult can also be accomplished with binary operators specific to
OneSparse.  Different binaryops are passed to emult to do different
elementwise operations:
``` postgres-console
select print(u &+ v) as "u &+ v", print(u &- v) as "u &- v", print(u &* v) as "u &* v", print(u &/ v) as "u &/ v" from test_fixture;
┌───────────┬───────────┬───────────┬───────────┐
│  u &+ v   │  u &- v   │  u &* v   │  u &/ v   │
├───────────┼───────────┼───────────┼───────────┤
│           │           │           │           │
│    ───    │    ───    │    ───    │    ───    │
│  0│       │  0│       │  0│       │  0│       │
│  1│  5    │  1│ -1    │  1│  6    │  1│  0    │
│  2│       │  2│       │  2│       │  2│       │
│  3│       │  3│       │  3│       │  3│       │
│           │           │           │           │
└───────────┴───────────┴───────────┴───────────┘
(1 row)

```
## Elementwise Union

Union of two vectors with scalars for missing values
``` postgres-console
select eunion('int32[0:1 1:2 2:3]'::vector, 42, 'int32[0:1 1:2 2:3]'::vector, 84, 'plus_int32');
┌────────────────────┐
│       eunion       │
├────────────────────┤
│ int32[0:2 1:4 2:6] │
└────────────────────┘
(1 row)

```
## Reduction

Reduce a vector to a scalar value using a monoid
``` postgres-console
select reduce_scalar('int32[0:1 1:2 2:3]'::vector, 'plus_monoid_int32');
┌───────────────┐
│ reduce_scalar │
├───────────────┤
│ int32:6       │
└───────────────┘
(1 row)

```
## Selection and Apply

Filter a vector and apply unary operators
``` postgres-console
select choose('int32[0:1 1:2 2:3]'::vector, 'valuegt_int32', 1);
┌────────────────┐
│     choose     │
├────────────────┤
│ int32[1:2 2:3] │
└────────────────┘
(1 row)

select apply('int32[1:1 2:2 3:3]'::vector, 'ainv_int32'::unaryop);
┌───────────────────────┐
│         apply         │
├───────────────────────┤
│ int32[1:-1 2:-2 3:-3] │
└───────────────────────┘
(1 row)

```
## Setting and Getting Elements
Elements can be set individually with `set_element`, the modified
input is returned:
``` postgres-console
select set_element('int32[1:1 2:2 3:3]'::vector, 4, 4);
┌────────────────────────┐
│      set_element       │
├────────────────────────┤
│ int32[1:1 2:2 3:3 4:4] │
└────────────────────────┘
(1 row)

```
Scalar elements can be extracted individually with `get_element`
``` postgres-console
select get_element('int32[1:1 2:2 3:3]'::vector, 3);
┌─────────────┐
│ get_element │
├─────────────┤
│ int32:3     │
└─────────────┘
(1 row)

```
## Utility Operations

Miscellaneous helpers for vectors
``` postgres-console
select print('int32(4)[1:1 2:2 3:3]'::vector);
┌───────────┐
│   print   │
├───────────┤
│           │
│    ───    │
│  0│       │
│  1│  1    │
│  2│  2    │
│  3│  3    │
│           │
└───────────┘
(1 row)

select wait('int32[0:1 1:2 2:3]'::vector);
┌────────────────────┐
│        wait        │
├────────────────────┤
│ int32[0:1 1:2 2:3] │
└────────────────────┘
(1 row)

select dup('int32[0:1 1:2 2:3]'::vector);
┌────────────────────┐
│        dup         │
├────────────────────┤
│ int32[0:1 1:2 2:3] │
└────────────────────┘
(1 row)

select clear('int32[0:1 1:2 2:3]'::vector);
┌───────┐
│ clear │
├───────┤
│ int32 │
└───────┘
(1 row)

```