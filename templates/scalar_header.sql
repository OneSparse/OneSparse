-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION onesparse" to load this file. \quit

CREATE TYPE scalar;

CREATE FUNCTION scalar_in(cstring)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION scalar_out(scalar)
RETURNS cstring
AS '$libdir/onesparse', 'scalar_out'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE scalar (
    input = scalar_in,
    output = scalar_out,
    alignment = int4,
    storage = 'main',
    internallength = VARIABLE
    );

CREATE FUNCTION nvals(scalar)
RETURNS int2
AS '$libdir/onesparse', 'scalar_nvals'
LANGUAGE C;

CREATE FUNCTION wait(scalar, waitmode integer default 0)
RETURNS void
AS '$libdir/onesparse', 'scalar_wait'
LANGUAGE C;

CREATE FUNCTION dup(scalar)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_dup'
LANGUAGE C;

CREATE FUNCTION clear(scalar)
RETURNS void
AS '$libdir/onesparse', 'scalar_clear'
LANGUAGE C;
