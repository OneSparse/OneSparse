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

CREATE FUNCTION scalar_typmod_in(cstring[])
RETURNS integer
AS '$libdir/onesparse', 'scalar_typmod_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION scalar_typmod_out(integer)
RETURNS cstring
AS '$libdir/onesparse', 'scalar_typmod_out'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE scalar (
    input = scalar_in,
    output = scalar_out,
    -- typmod_in = scalar_typmod_in,
    -- typmod_out = scalar_typmod_out,
    alignment = int4,
    storage = 'extended',
    internallength = VARIABLE
    );

CREATE FUNCTION nvals(scalar)
RETURNS int2
AS '$libdir/onesparse', 'scalar_nvals'
LANGUAGE C;