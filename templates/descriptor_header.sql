-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION onesparse" to load this file. \quit

CREATE TYPE descriptor;

CREATE FUNCTION descriptor_in(cstring)
RETURNS descriptor
AS '$libdir/onesparse', 'descriptor_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION descriptor_out(descriptor)
RETURNS cstring
AS '$libdir/onesparse', 'descriptor_out'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE descriptor (
    input = descriptor_in,
    output = descriptor_out,
    alignment = int4,
    storage = 'main',
    internallength = VARIABLE
    );

COMMENT ON TYPE descriptor IS 'Descriptors control specific details of GraphBLAS operations.';

CREATE FUNCTION name(descriptor)
RETURNS text
AS '$libdir/onesparse', 'descriptor_name'
LANGUAGE C IMMUTABLE STRICT;
