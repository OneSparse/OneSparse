-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION onesparse" to load this file. \quit

CREATE TYPE type;

CREATE FUNCTION type_in(cstring)
RETURNS type
AS '$libdir/onesparse', 'type_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION type_out(type)
RETURNS cstring
AS '$libdir/onesparse', 'type_out'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE type (
    input = type_in,
    output = type_out,
    alignment = int4,
    storage = 'main',
    internallength = VARIABLE
    );

COMMENT ON TYPE type IS 'Types define the structure and behavior of elements.';

CREATE FUNCTION name(type)
RETURNS text
AS '$libdir/onesparse', 'type_name'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION jit_type(regtype)
RETURNS text
AS '$libdir/onesparse', 'jit_type'
LANGUAGE C IMMUTABLE STRICT;
