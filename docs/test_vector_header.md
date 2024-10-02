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
`i4` means GrB_INT32.  The type codes are intentionally compressed
to be as short as possible for smaller pg_dumps.

Much of these functions are basically vector versions of the same
functions for matrix.  See those docs for details:
``` postgres-console
select 'i4'::vector;
 vector 
--------
 i4[]
(1 row)

select nvals('i4'::vector);
 nvals 
-------
     0
(1 row)

select size('i4'::vector);
        size         
---------------------
 1152921504606846976
(1 row)

select 'i4[]'::vector;
 vector 
--------
 i4[]
(1 row)

select nvals('i4[]'::vector);
 nvals 
-------
     0
(1 row)

select size('i4[]'::vector);
        size         
---------------------
 1152921504606846976
(1 row)

select 'i4(10)'::vector;
 vector 
--------
 i4[]
(1 row)

select nvals('i4(10)'::vector);
 nvals 
-------
     0
(1 row)

select size('i4(10)'::vector);
 size 
------
   10
(1 row)

select 'i4(10)[]'::vector;
 vector 
--------
 i4[]
(1 row)

select nvals('i4(10)[]'::vector);
 nvals 
-------
     0
(1 row)

select size('i4(10)[]'::vector);
 size 
------
   10
(1 row)

select 'i4[0:1 1:2 2:3]'::vector;
     vector      
-----------------
 i4[0:1 1:2 2:3]
(1 row)

select nvals('i4[0:1 1:2 2:3]'::vector);
 nvals 
-------
     3
(1 row)

select 'i4(10)[0:1 1:2 2:3]'::vector;
     vector      
-----------------
 i4[0:1 1:2 2:3]
(1 row)

select size('i4(10)[0:1 1:2 2:3]'::vector);
 size 
------
   10
(1 row)

select size('i4(2)[0:1 1:2 2:3]'::vector);
ERROR:  INVALID_INDEX GraphBLAS error: GrB_INVALID_INDEX
function: GrB_Vector_setElement_INT64 (w, x, row)
Row index 2 out of range; must be < 2: Error setting Vector Element
LINE 1: select size('i4(2)[0:1 1:2 2:3]'::vector);
                    ^
select ewise_add('i4[0:1 1:2 2:3]'::vector, 'i4[0:1 1:2 2:3]'::vector, 'plus_int32');
    ewise_add    
-----------------
 i4[0:2 1:4 2:6]
(1 row)

select ewise_mult('i4[0:1 1:2 2:3]'::vector, 'i4[0:1 1:2 2:3]'::vector, 'times_int32');
   ewise_mult    
-----------------
 i4[0:1 1:4 2:9]
(1 row)

select ewise_union('i4[0:1 1:2 2:3]'::vector, 42, 'i4[0:1 1:2 2:3]'::vector, 84, 'plus_int32');
   ewise_union   
-----------------
 i4[0:2 1:4 2:6]
(1 row)

select reduce_scalar('i4[0:1 1:2 2:3]'::vector, 'plus_monoid_int32');
 reduce_scalar 
---------------
 i4:6
(1 row)

select selection('i4[0:1 1:2 2:3]'::vector, 'valuegt_int32', 1);
  selection  
-------------
 i4[1:2 2:3]
(1 row)

select apply('i4[1:1 2:2 3:3]'::vector, 'ainv_int32'::unaryop);
       apply        
--------------------
 i4[1:-1 2:-2 3:-3]
(1 row)

```
Elements can be set individually with `set_element`, the modified
input is returned:
``` postgres-console
select set_element('i4[1:1 2:2 3:3]'::vector, 4, 4);
     set_element     
---------------------
 i4[1:1 2:2 3:3 4:4]
(1 row)

```
Scalar elements can be extracted individually with `get_element`
``` postgres-console
select get_element('i4[1:1 2:2 3:3]'::vector, 3);
 get_element 
-------------
 i4:3
(1 row)

select print('i4[1:1 2:2 3:3]'::vector);
                              print                              
-----------------------------------------------------------------
                                                                +
   1152921504606846976x1 GraphBLAS int32_t vector, sparse by col+
   A->vector, 3 entries, memory: 284 bytes                      +
                                                                +
 
(1 row)

select print('i4[1:1 2:2 3:3]'::vector, 5);
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

select wait('i4[0:1 1:2 2:3]'::vector);
 wait 
------
 
(1 row)

select dup('i4[0:1 1:2 2:3]'::vector);
       dup       
-----------------
 i4[0:1 1:2 2:3]
(1 row)

select clear('i4[0:1 1:2 2:3]'::vector);
 clear 
-------
 
(1 row)

```