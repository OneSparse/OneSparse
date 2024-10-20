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
                                                      List of data types
  Schema   |  Name  | Internal name | Size | Elements |  Owner   | Access privileges |               Description               
-----------+--------+---------------+------+----------+----------+-------------------+-----------------------------------------
 onesparse | scalar | scalar        | var  |          | postgres |                   | Scalars hold individual element values.
(1 row)

```
print a scalar, this renders the value with no prefix
``` postgres-console
select print('int32:42'::scalar);
 print 
-------
 42
(1 row)

```
Duplicate a scalar
``` postgres-console
select dup('int32:42'::scalar);
   dup    
----------
 int32:42
(1 row)

```
Wait for a scalar to complete in non-blocking mode
``` postgres-console
select wait('int32:42'::scalar);
   wait   
----------
 int32:42
(1 row)

```
Clear a scalar, deleting its stored element.
``` postgres-console
select clear('int32:42'::scalar);
 clear 
-------
 int
(1 row)

```