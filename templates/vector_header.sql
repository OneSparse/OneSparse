-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION onesparse" to load this file. \quit

CREATE TYPE vector;

CREATE FUNCTION vector_in(cstring)
RETURNS vector
AS '$libdir/onesparse', 'vector_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vector_out(vector)
RETURNS cstring
AS '$libdir/onesparse', 'vector_out'
LANGUAGE C IMMUTABLE STRICT;

-- CREATE FUNCTION vector_typmod_in(cstring[])
-- RETURNS integer
-- AS '$libdir/onesparse', 'vector_typmod_in'
-- LANGUAGE C IMMUTABLE STRICT;

-- CREATE FUNCTION vector_typmod_out(integer)
-- RETURNS cstring
-- AS '$libdir/onesparse', 'vector_typmod_out'
-- LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE vector (
    input = vector_in,
    output = vector_out,
    -- typmod_in = vector_typmod_in,
    -- typmod_out = vector_typmod_out,
    alignment = int4,
    storage = 'extended',
    internallength = VARIABLE
    );

CREATE FUNCTION nvals(vector)
RETURNS int2
AS '$libdir/onesparse', 'vector_nvals'
LANGUAGE C;
