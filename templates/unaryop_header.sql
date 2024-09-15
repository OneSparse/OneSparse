-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION onesparse" to load this file. \quit

CREATE TYPE unaryop;

CREATE FUNCTION unaryop_in(cstring)
RETURNS unaryop
AS '$libdir/onesparse', 'unaryop_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION unaryop_out(unaryop)
RETURNS cstring
AS '$libdir/onesparse', 'unaryop_out'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE unaryop (
    input = unaryop_in,
    output = unaryop_out,
    alignment = int4,
    storage = 'extended',
    internallength = VARIABLE
    );
