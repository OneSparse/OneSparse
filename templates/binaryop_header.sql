-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION onesparse" to load this file. \quit

CREATE TYPE binaryop;

CREATE FUNCTION binaryop_in(cstring)
RETURNS binaryop
AS '$libdir/onesparse', 'binaryop_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION binaryop_out(binaryop)
RETURNS cstring
AS '$libdir/onesparse', 'binaryop_out'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE binaryop (
    input = binaryop_in,
    output = binaryop_out,
    alignment = int4,
    storage = 'main',
    internallength = VARIABLE
    );

COMMENT ON TYPE binaryop IS 'BinaryOps apply a function to two elements and returning an element.';

CREATE FUNCTION name(binaryop)
RETURNS text
AS '$libdir/onesparse', 'binaryop_name'
LANGUAGE C IMMUTABLE STRICT;
