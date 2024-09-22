-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION onesparse" to load this file. \quit

CREATE TYPE semiring;

CREATE FUNCTION semiring_in(cstring)
RETURNS semiring
AS '$libdir/onesparse', 'semiring_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION semiring_out(semiring)
RETURNS cstring
AS '$libdir/onesparse', 'semiring_out'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE semiring (
    input = semiring_in,
    output = semiring_out,
    alignment = int4,
    storage = 'main',
    internallength = VARIABLE
    );
