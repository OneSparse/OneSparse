-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION onesparse" to load this file. \quit

CREATE TYPE indexunaryop;

CREATE FUNCTION indexunaryop_in(cstring)
RETURNS indexunaryop
AS '$libdir/onesparse', 'indexunaryop_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION indexunaryop_out(indexunaryop)
RETURNS cstring
AS '$libdir/onesparse', 'indexunaryop_out'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE indexunaryop (
    input = indexunaryop_in,
    output = indexunaryop_out,
    alignment = int4,
    storage = 'main',
    internallength = VARIABLE
    );
