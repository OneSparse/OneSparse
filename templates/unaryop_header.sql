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
    storage = 'main',
    internallength = VARIABLE
    );

COMMENT ON TYPE unaryop IS 'UnaryOps apply a function to an element, returning an element.';

CREATE FUNCTION name(unaryop)
RETURNS text
AS '$libdir/onesparse', 'unaryop_name'
LANGUAGE C IMMUTABLE STRICT;
