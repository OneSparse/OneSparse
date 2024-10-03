# Vectors

This documentation is also tests for the code, the examples below
show the literal output of these statements from Postgres.

Some setup to make sure warnings are shown, and that the extension
is installed.
``` postgres-console
set client_min_messages = 'WARNING';
create extension if not exists onesparse;
```
Describe the vector type:
``` postgres-console
\dT+ vector
                                        List of data types
  Schema   |  Name  | Internal name | Size | Elements |  Owner   | Access privileges | Description 
-----------+--------+---------------+------+----------+----------+-------------------+-------------
 onesparse | vector | vector        | var  |          | postgres |                   | 
(1 row)

```
An empty vector can be constructed many ways, but one of the
simplest is casting a type code to the matrix type.  In this case
`int32` means GrB_INT32.  The type codes are intentionally compressed
to be as short as possible for smaller pg_dumps.

Much of these functions are basically vector versions of the same
functions for matrix.  See those docs for details:
``` postgres-console
select 'int32'::vector;
 vector 
--------
 int32
(1 row)

select nvals('int32'::vector);
 nvals 
-------
     0
(1 row)

select size('int32'::vector);
        size         
---------------------
 1152921504606846976
(1 row)

select 'int32[]'::vector;
 vector 
--------
 int32
(1 row)

select nvals('int32[]'::vector);
 nvals 
-------
     0
(1 row)

select size('int32[]'::vector);
        size         
---------------------
 1152921504606846976
(1 row)

select 'int32(10)'::vector;
  vector   
-----------
 int32(10)
(1 row)

select nvals('int32(10)'::vector);
 nvals 
-------
     0
(1 row)

select size('int32(10)'::vector);
 size 
------
   10
(1 row)

select 'int32(10)[]'::vector;
  vector   
-----------
 int32(10)
(1 row)

select nvals('int32(10)[]'::vector);
 nvals 
-------
     0
(1 row)

select size('int32(10)[]'::vector);
 size 
------
   10
(1 row)

select 'int32[0:1 1:2 2:3]'::vector;
       vector       
--------------------
 int32[0:1 1:2 2:3]
(1 row)

select nvals('int32[0:1 1:2 2:3]'::vector);
 nvals 
-------
     3
(1 row)

select 'int32(10)[0:1 1:2 2:3]'::vector;
         vector         
------------------------
 int32(10)[0:1 1:2 2:3]
(1 row)

select size('int32(10)[0:1 1:2 2:3]'::vector);
 size 
------
   10
(1 row)

select size('int32(2)[0:1 1:2 2:3]'::vector);
ERROR:  INVALID_INDEX GraphBLAS error: GrB_INVALID_INDEX
function: GrB_Vector_setElement_INT64 (w, x, row)
Row index 2 out of range; must be < 2: Error setting Vector Element
LINE 1: select size('int32(2)[0:1 1:2 2:3]'::vector);
                    ^
select ewise_add('int32[0:1 1:2 2:3]'::vector, 'int32[0:1 1:2 2:3]'::vector, 'plus_int32');
     ewise_add      
--------------------
 int32[0:2 1:4 2:6]
(1 row)

select ewise_mult('int32[0:1 1:2 2:3]'::vector, 'int32[0:1 1:2 2:3]'::vector, 'times_int32');
     ewise_mult     
--------------------
 int32[0:1 1:4 2:9]
(1 row)

select ewise_union('int32[0:1 1:2 2:3]'::vector, 42, 'int32[0:1 1:2 2:3]'::vector, 84, 'plus_int32');
    ewise_union     
--------------------
 int32[0:2 1:4 2:6]
(1 row)

select reduce_scalar('int32[0:1 1:2 2:3]'::vector, 'plus_monoid_int32');
 reduce_scalar 
---------------
 int32:6
(1 row)

select selection('int32[0:1 1:2 2:3]'::vector, 'valuegt_int32', 1);
   selection    
----------------
 int32[1:2 2:3]
(1 row)

select apply('int32[1:1 2:2 3:3]'::vector, 'ainv_int32'::unaryop);
         apply         
-----------------------
 int32[1:-1 2:-2 3:-3]
(1 row)

```
Elements can be set individually with `set_element`, the modified
input is returned:
``` postgres-console
select set_element('int32[1:1 2:2 3:3]'::vector, 4, 4);
      set_element       
------------------------
 int32[1:1 2:2 3:3 4:4]
(1 row)

```
Scalar elements can be extracted individually with `get_element`
``` postgres-console
select get_element('int32[1:1 2:2 3:3]'::vector, 3);
 get_element 
-------------
 int32:3
(1 row)

select print('int32[1:1 2:2 3:3]'::vector);
                              print                              
-----------------------------------------------------------------
                                                                +
   1152921504606846976x1 GraphBLAS int32_t vector, sparse by col+
   A->vector, 3 entries, memory: 284 bytes                      +
                                                                +
 
(1 row)

select print('int32[1:1 2:2 3:3]'::vector, 5);
                              print                              
-----------------------------------------------------------------
                                                                +
   1152921504606846976x1 GraphBLAS int32_t vector, sparse by col+
   A->vector, 3 entries, memory: 284 bytes                      +
                                                                +
     (1,0)   1                                                  +
     (2,0)   2                                                  +
     (3,0)   3                                                  +
                                                                +
 
(1 row)

select wait('int32[0:1 1:2 2:3]'::vector);
 wait 
------
 
(1 row)

select dup('int32[0:1 1:2 2:3]'::vector);
        dup         
--------------------
 int32[0:1 1:2 2:3]
(1 row)

select clear('int32[0:1 1:2 2:3]'::vector);
 clear 
-------
 
(1 row)

```