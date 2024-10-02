# Scalar

This documentation is also tests for the code, the examples below
show the literal output of these statements from Postgres.

Some setup to make sure warnings are shown, and that the extension
is installed.
``` postgres-console
set client_min_messages = 'WARNING';
create extension if not exists onesparse;
```
Describe the scalar type
``` postgres-console
\dT+ scalar
                                        List of data types
  Schema   |  Name  | Internal name | Size | Elements |  Owner   | Access privileges | Description 
-----------+--------+---------------+------+----------+----------+-------------------+-------------
 onesparse | scalar | scalar        | var  |          | postgres |                   | 
(1 row)

```
print a scalar
``` postgres-console
select print('i4:42'::scalar);
                    print                    
---------------------------------------------
                                            +
   1x1 GraphBLAS int32_t scalar, full by col+
   A->scalar, 1 entry, memory: 240 bytes    +
   iso value:   42                          +
                                            +
 
(1 row)

```
print a scalar
``` postgres-console
select print('i4:42'::scalar, 5);
                    print                    
---------------------------------------------
                                            +
   1x1 GraphBLAS int32_t scalar, full by col+
   A->scalar, 1 entry, memory: 240 bytes    +
   iso value:   42                          +
                                            +
     (0,0)   42                             +
 
(1 row)

```
Duplicate a scalar
``` postgres-console
select dup('i4:42'::scalar);
  dup  
-------
 i4:42
(1 row)

```
Wait for a scalar to complete in non-blocking mode
``` postgres-console
select wait('i4:42'::scalar);
 wait  
-------
 i4:42
(1 row)

```
Clear a scalar, deleting its stored element.
``` postgres-console
select clear('i4:42'::scalar);
 clear 
-------
 i4:
(1 row)

```