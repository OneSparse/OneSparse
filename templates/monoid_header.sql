-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION onesparse" to load this file. \quit

CREATE TYPE monoid;

CREATE FUNCTION monoid_in(cstring)
RETURNS monoid
AS '$libdir/onesparse', 'monoid_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION monoid_out(monoid)
RETURNS cstring
AS '$libdir/onesparse', 'monoid_out'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE monoid (
    input = monoid_in,
    output = monoid_out,
    alignment = int4,
    storage = 'main',
    internallength = VARIABLE
    );