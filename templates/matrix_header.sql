-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION onesparse" to load this file. \quit

CREATE TYPE matrix;

CREATE FUNCTION matrix_in(cstring)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION matrix_out(matrix)
RETURNS cstring
AS '$libdir/onesparse', 'matrix_out'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE matrix (
    input = matrix_in,
    output = matrix_out,
    alignment = int4,
    storage = 'external',
    internallength = VARIABLE
    );

CREATE FUNCTION nvals(matrix)
RETURNS int8
AS '$libdir/onesparse', 'matrix_nvals'
LANGUAGE C;

CREATE FUNCTION nrows(matrix)
RETURNS int8
AS '$libdir/onesparse', 'matrix_nrows'
LANGUAGE C;

CREATE FUNCTION ncols(matrix)
RETURNS int8
AS '$libdir/onesparse', 'matrix_ncols'
LANGUAGE C;

CREATE FUNCTION ewise_add(
    u matrix,
    v matrix,
    op binaryop,
    mask matrix default null,
    accum binaryop default null,
    descriptor text default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_ewise_add'
LANGUAGE C STABLE;

CREATE FUNCTION ewise_mult(
    u matrix,
    v matrix,
    op binaryop,
    mask matrix default null,
    accum binaryop default null,
    descriptor text default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_ewise_mult'
LANGUAGE C STABLE;

CREATE FUNCTION ewise_union(
    u matrix,
    a scalar,
    v matrix,
    b scalar,
    op binaryop,
    mask matrix default null,
    accum binaryop default null,
    descriptor text default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_ewise_union'
LANGUAGE C STABLE;
