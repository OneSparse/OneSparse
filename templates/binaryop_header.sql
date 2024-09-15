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
    storage = 'extended',
    internallength = VARIABLE
    );
