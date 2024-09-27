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
    a matrix,
    b matrix,
    op binaryop,
    mask matrix default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_ewise_add'
LANGUAGE C STABLE;

CREATE FUNCTION ewise_mult(
    a matrix,
    b matrix,
    op binaryop,
    mask matrix default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_ewise_mult'
LANGUAGE C STABLE;

CREATE FUNCTION ewise_union(
    a matrix,
    alpha scalar,
    b matrix,
    beta scalar,
    op binaryop,
    mask matrix default null,
    descriptor descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_ewise_union'
LANGUAGE C STABLE;

CREATE FUNCTION reduce_vector(
    a matrix,
    op monoid,
    w vector default null,
    mask vector default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'matrix_reduce_vector'
LANGUAGE C STABLE;

CREATE FUNCTION reduce_scalar(
    a matrix,
    op monoid,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS scalar
AS '$libdir/onesparse', 'matrix_reduce_scalar'
LANGUAGE C STABLE;

CREATE FUNCTION matrix_assign(
    a matrix,
    b matrix,
    op binaryop,
    mask matrix default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_assign_matrix'
LANGUAGE C STABLE;

CREATE FUNCTION matrix_extract(
    a matrix,
    b matrix,
    op binaryop,
    mask matrix default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_extract_matrix'
LANGUAGE C STABLE;

CREATE FUNCTION mxm(
    a matrix,
    b matrix,
    op semiring,
    inout c matrix default null,
    mask matrix default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_mxm'
LANGUAGE C STABLE;

CREATE FUNCTION mxv(
    a matrix,
    v vector,
    op semiring,
    inout w vector default null,
    mask matrix default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'matrix_mxv'
LANGUAGE C STABLE;

CREATE FUNCTION vxm(
    u vector,
    b matrix,
    op semiring,
    inout w vector default null,
    mask matrix default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'matrix_vxm'
LANGUAGE C STABLE;

CREATE FUNCTION wait(matrix, waitmode integer default 0)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_wait'
LANGUAGE C;

CREATE FUNCTION dup(matrix)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_dup'
LANGUAGE C;

CREATE FUNCTION clear(matrix)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_clear'
LANGUAGE C;
