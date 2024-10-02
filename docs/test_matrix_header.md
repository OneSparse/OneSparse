# Matrix

This documentation is also tests for the code, the examples below
show the literal output of these statements from Postgres.

Some setup to make sure warnings are shown, and that the extension
is installed.
``` postgres-console
set client_min_messages = 'WARNING';
create extension if not exists onesparse;
```
The `matrix` data type wraps a SuiteSparse GrB_Matrix handle and
delegates functions from SQL to the library through instances of
this type.
``` postgres-console
\dT+ matrix
                                        List of data types
  Schema   |  Name  | Internal name | Size | Elements |  Owner   | Access privileges | Description 
-----------+--------+---------------+------+----------+----------+-------------------+-------------
 onesparse | matrix | matrix        | var  |          | postgres |                   | 
(1 row)

```
An empty matrix can be constructed many ways, but one of the
simplest is casting a type code to the matrix type.  In this case
`i4` means GrB_INT32.  The type codes are intentionally compressed
to be as short as possible for smaller pg_dumps.
``` postgres-console
select 'i4'::matrix;
 matrix 
--------
 i4[]
(1 row)

```
A matrix can have a fixed number of rows and/or columns.  The
default possible number of rows and columns is defined by the
SuiteSparse library to be `GrB_INDEX_MAX` which is `2^60` power
indexes.  For the purposes of this documentation this will be
referred to as `INDEX_MAX` and matrices and vector dimensions that
are `INDEX_MAX` in size are reffered to as "unbounded".  For
matrices with known dimensions, the dimensions can be provided in
parentesis after the type code.  Here a 10 row by 10 column matrix
is created:
``` postgres-console
select 'i4(10:10)'::matrix;
 matrix 
--------
 i4[]
(1 row)

```
Either dimension can be ommited, this creates a 10 row by unbounded
column matrix.
``` postgres-console
select 'i4(10:)'::matrix;
 matrix 
--------
 i4[]
(1 row)

```
This creates a unbounded row by 10 column matrix.
``` postgres-console
select 'i4(10:)'::matrix;
 matrix 
--------
 i4[]
(1 row)

```
Note that in all the above cases the matrices created are *empty*.
They contain no stored elements.  The memory needed to hold the
matrix contains only stored elements, if there isn't a value stored
at a given row or column position, no memory is consumed.  This is
the "sparse" in sparse matrix.  This is how it's possible to create
an unbounded row by unbounded column matrix without exhausting
memory trying to allocate `2^120` entries.

All graphblas operations are exposed by a series of functions and
operators.  Here we see three very common operations, returning the
number of rows, the number of columns, and the number of store
values.
``` postgres-console
select nrows('i4'::matrix);
        nrows        
---------------------
 1152921504606846976
(1 row)

select ncols('i4'::matrix);
        ncols        
---------------------
 1152921504606846976
(1 row)

select nvals('i4'::matrix);
 nvals 
-------
     0
(1 row)

```
Above you can see the matrix has unbounded rows and columns (the
very large number is the number of *possible* entries).  And the
number of stored values is zero.
Values can be specified after the `type(dimension)` prefix as an
array of elements between square brackets.  Empty brackets imply no
elements:
``` postgres-console
select nrows('i4[]'::matrix);
        nrows        
---------------------
 1152921504606846976
(1 row)

select ncols('i4[]'::matrix);
        ncols        
---------------------
 1152921504606846976
(1 row)

select nvals('i4[]'::matrix);
 nvals 
-------
     0
(1 row)

```
Elements are specified between square brackets are coordinates of
'row_id:column_id:value' separated by spaces:
``` postgres-console
select 'i4[1:1:1 2:2:2 3:3:3]'::matrix;
        matrix         
-----------------------
 i4[1:1:1 2:2:2 3:3:3]
(1 row)

select 'i4(10:)[1:1:1 2:2:2 3:3:3]'::matrix;
        matrix         
-----------------------
 i4[1:1:1 2:2:2 3:3:3]
(1 row)

select 'i4(:10)[1:1:1 2:2:2 3:3:3]'::matrix;
        matrix         
-----------------------
 i4[1:1:1 2:2:2 3:3:3]
(1 row)

```
Below you see the number of rows, columns and spaces for a variety
of combinations:
``` postgres-console
select nrows('i4(10)[1:1:1 2:2:2 3:3:3]'::matrix);
 nrows 
-------
    10
(1 row)

select ncols('i4(10)[1:1:1 2:2:2 3:3:3]'::matrix);
        ncols        
---------------------
 1152921504606846976
(1 row)

select nvals('i4(10)[1:1:1 2:2:2 3:3:3]'::matrix);
 nvals 
-------
     3
(1 row)

select nrows('i4(10:)[1:1:1 2:2:2 3:3:3]'::matrix);
 nrows 
-------
    10
(1 row)

select ncols('i4(10:)[1:1:1 2:2:2 3:3:3]'::matrix);
        ncols        
---------------------
 1152921504606846976
(1 row)

select nvals('i4(10:)[1:1:1 2:2:2 3:3:3]'::matrix);
 nvals 
-------
     3
(1 row)

select nrows('i4(:10)[1:1:1 2:2:2 3:3:3]'::matrix);
        nrows        
---------------------
 1152921504606846976
(1 row)

select ncols('i4(:10)[1:1:1 2:2:2 3:3:3]'::matrix);
 ncols 
-------
    10
(1 row)

select nvals('i4(:10)[1:1:1 2:2:2 3:3:3]'::matrix);
 nvals 
-------
     3
(1 row)

select nrows('i4(10:10)[1:1:1 2:2:2 3:3:3]'::matrix);
 nrows 
-------
    10
(1 row)

select ncols('i4(10:10)[1:1:1 2:2:2 3:3:3]'::matrix);
 ncols 
-------
    10
(1 row)

select nvals('i4(10:10)[1:1:1 2:2:2 3:3:3]'::matrix);
 nvals 
-------
     3
(1 row)

```
# Element-wise operations

The GraphBLAS API has elementwise operations on matrices that
operate pairs of matrices.  `ewise_add` computes the element-wise
“addition” of two matrices A and B, element-wise using any binary
operator.  Elements present on both sides of the operation are
included in the result.
``` postgres-console
select ewise_add('i4[1:1:1 2:2:2 3:3:3]'::matrix, 'i4[1:1:1 2:2:2 3:3:3]'::matrix, 'plus_int32');
       ewise_add       
-----------------------
 i4[1:1:2 2:2:4 3:3:6]
(1 row)

```
`ewise_mult` multiplies elements of two matrices, taking only the
intersection of common elements in both matrices, if an element is
missing from either the left or right side, it is ommited from the
result:
``` postgres-console
select ewise_mult('i4[1:1:1 2:2:2 3:3:3]'::matrix, 'i4[1:1:1 2:2:2 3:3:3]'::matrix, 'times_int32');
      ewise_mult       
-----------------------
 i4[1:1:1 2:2:4 3:3:9]
(1 row)

```
`ewise_union` is like `ewise_add` but differs in how the binary op
is applied. A pair of scalars, `alpha` and `beta` define the inputs
to the operator when entries are present in one matrix but not the
other.
``` postgres-console
select ewise_union('i4[1:1:1 2:2:2 3:3:3]'::matrix, 42, 'i4[1:1:1 2:2:2 3:3:3]'::matrix, 84, 'plus_int32');
      ewise_union      
-----------------------
 i4[1:1:2 2:2:4 3:3:6]
(1 row)

```
The entire matrix can be reduced to a scalar value:
``` postgres-console
select reduce_scalar('i4[1:1:1 2:2:2 3:3:3]'::matrix, 'plus_monoid_int32');
 reduce_scalar 
---------------
 i4:6
(1 row)

```
The matrix can also be reduced to a column vector:
``` postgres-console
select reduce_vector('i4[1:1:1 1:2:3 2:2:2]'::matrix, 'plus_monoid_int32');
 reduce_vector 
---------------
 i4[1:4 2:2]
(1 row)

```
To reduce a row vector, specify that the input should be transposed
with the descriptor `t0`:
``` postgres-console
select reduce_vector('i4[1:1:1 1:2:3 2:2:2]'::matrix, 'plus_monoid_int32', descriptor=>'t0');
 reduce_vector 
---------------
 i4[1:1 2:5]
(1 row)

```
Matrix Multiplication (referred to here as A @ B) is the heart of
linear algebra.  All matrix multiplication happens over a semiring.
For the most common form of matrix multiplication, the outer
opperation is to multiply coresponding elements with the "times"
operator and then reduce those products with the "plus" operator.
This is called the `plus_times` semiring:
``` postgres-console
select mxm('i4[0:0:1 0:1:2]'::matrix, 'i4[0:0:1 0:1:3]'::matrix, 'plus_times_int32');
       mxm       
-----------------
 i4[0:0:1 0:1:3]
(1 row)

```
AxB can also be done with the `@` operator, mimicking the Python
syntax:
``` postgres-console
select 'i4[0:0:1 0:1:2]'::matrix @ 'i4[0:0:1 0:1:3]'::matrix;
    ?column?     
-----------------
 i4[0:0:0 0:1:0]
(1 row)

```
Matrices can be multipled by vectors on the right taking the linear
combination of the matrices columns using the vectors elements as
coefficients:
``` postgres-console
select mxv('i4[0:0:1 0:1:2]'::matrix, 'i4[0:0 1:1]'::vector, 'plus_times_int32');
   mxv   
---------
 i4[0:2]
(1 row)

```
'mxv' is also supported by the `@` operator:
``` postgres-console
select 'i4[0:0:1 0:1:2]'::matrix @ 'i4[0:0 1:1]'::vector;
 ?column? 
----------
 i4[0:0]
(1 row)

```
Matrices can be multipled by vectors on the right taking the linear
combination of the matrices rows using the vectors elements as
coefficients:
``` postgres-console
select vxm('i4[0:0 1:1]'::vector, 'i4[0:0:1 0:1:2]'::matrix, 'plus_times_int32');
     vxm     
-------------
 i4[0:0 1:0]
(1 row)

```
'vxm' is also supported by the `@` operator:
``` postgres-console
select 'i4[0:0 1:1]'::vector @ 'i4[0:0:1 0:1:2]'::matrix;
  ?column?   
-------------
 i4[0:0 1:0]
(1 row)

```
The `selection` method calls the `GrB_select()` API function.  The
name `selection` was chosen not to conflict with the SQL keyword
`select`.  Selection provides a conditional operator called an
`indexunaryop` and a parameter for the operator to use to compare
elements in the matrix.  Below, all elements with values greater
than 1 are returned:
``` postgres-console
select selection('i4[1:1:1 2:2:2 3:3:3]'::matrix, 'valuegt_int32'::indexunaryop, 1::scalar);
    selection    
-----------------
 i4[2:2:2 3:3:3]
(1 row)

```
Here are all values equal to 2:
``` postgres-console
select selection('i4[1:1:1 2:2:2 3:3:3]'::matrix, 'valueeq_int32'::indexunaryop, 2::scalar);
 selection 
-----------
 i4[2:2:2]
(1 row)

```
`apply` takes an operator of type `unaryop` and applies it to every
element of the matrix.  The 'ainv_int32' returned the additive
inverse (the negative value for integers) of every element:
``` postgres-console
select apply('i4[1:1:1 2:2:2 3:3:3]'::matrix, 'ainv_int32'::unaryop);
          apply           
--------------------------
 i4[1:1:-1 2:2:-2 3:3:-3]
(1 row)

```
Elements can be set individually with `set_element`, the modified
input is returned:
``` postgres-console
select set_element('i4[1:1:1 2:2:2 3:3:3]'::matrix, 4, 4, 4);
         set_element         
-----------------------------
 i4[1:1:1 2:2:2 3:3:3 4:4:4]
(1 row)

```
Scalar elements can be extracted individually with `get_element`
``` postgres-console
select get_element('i4[1:1:1 2:2:2 3:3:3]'::matrix, 3, 3);
 get_element 
-------------
 i4:3
(1 row)

```
The `print` function returns a descripton of the matrix from
SuiteSparse.
``` postgres-console
select print('i4[1:1:1 2:2:2 3:3:3]'::matrix);
                                         print                                          
----------------------------------------------------------------------------------------
                                                                                       +
   1152921504606846976x1152921504606846976 GraphBLAS int32_t matrix, hypersparse by row+
   A->matrix, 3 entries, memory: 324 bytes                                             +
                                                                                       +
 
(1 row)

```
The `print` function takes an optional "level" argument that
defaults to `1` which is a short summary.
``` postgres-console
select print('i4[1:1:1 2:2:2 3:3:3]'::matrix, 5);
                                         print                                          
----------------------------------------------------------------------------------------
                                                                                       +
   1152921504606846976x1152921504606846976 GraphBLAS int32_t matrix, hypersparse by row+
   A->matrix, 3 entries, memory: 324 bytes                                             +
                                                                                       +
     (1,1)   1                                                                         +
     (2,2)   2                                                                         +
     (3,3)   3                                                                         +
                                                                                       +
 
(1 row)

```
The `dup` function duplicates a matrix returning a new matrix
object with the same values:
``` postgres-console
select dup('i4[1:1:1 2:2:2 3:3:3]'::matrix);
          dup          
-----------------------
 i4[1:1:1 2:2:2 3:3:3]
(1 row)

```
The `wait` method is used to "complete" a matrix, which may have
pending operations waiting to be performed when using the default
SuiteSparse non-blocking mode.  As a side effect, wait will sort
the elements of the input:
``` postgres-console
select wait('i4[2:2:2 3:3:3 1:1:1]'::matrix);
         wait          
-----------------------
 i4[1:1:1 2:2:2 3:3:3]
(1 row)

```
The `clear` function clears the matrix of all elements and returns
the same object, but empty.  The dimensions do not change:
``` postgres-console
select clear('i4[1:1:1 2:2:2 3:3:3]'::matrix);
 clear 
-------
 i4[]
(1 row)

```