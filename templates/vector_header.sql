-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION onesparse" to load this file. \quit

CREATE TYPE vector;

CREATE FUNCTION vector_in(cstring)
RETURNS vector
AS '$libdir/onesparse', 'vector_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vector_out(vector)
RETURNS cstring
AS '$libdir/onesparse', 'vector_out'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE vector (
    input = vector_in,
    output = vector_out,
    alignment = int4,
    storage = 'extended',
    internallength = VARIABLE
    );

CREATE FUNCTION nvals(vector)
RETURNS int8
AS '$libdir/onesparse', 'vector_nvals'
LANGUAGE C;

CREATE FUNCTION size(vector)
RETURNS int8
AS '$libdir/onesparse', 'vector_size'
LANGUAGE C;

CREATE FUNCTION ewise_add(
    u vector,
    v vector,
    op binaryop,
    mask vector default null,
    accum binaryop default null,
    descriptor text default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_ewise_add'
LANGUAGE C STABLE;

CREATE FUNCTION ewise_mult(
    u vector,
    v vector,
    op binaryop,
    mask vector default null,
    accum binaryop default null,
    descriptor text default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_ewise_mult'
LANGUAGE C STABLE;

CREATE FUNCTION ewise_union(
    u vector,
    a scalar,
    v vector,
    b scalar,
    op binaryop,
    mask vector default null,
    accum binaryop default null,
    descriptor text default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_ewise_union'
LANGUAGE C STABLE;
