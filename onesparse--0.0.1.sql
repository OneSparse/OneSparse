-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION onesparse" to load this file. \quit

CREATE TYPE scalar;

CREATE FUNCTION scalar_in(cstring)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_in'
LANGUAGE C STRICT;

CREATE FUNCTION scalar_out(scalar)
RETURNS cstring
AS '$libdir/onesparse', 'scalar_out'
LANGUAGE C STRICT;

CREATE TYPE scalar (
    input = scalar_in,
    output = scalar_out,
    alignment = int4,
    storage = 'extended',
    internallength = VARIABLE
    );

-- scalar type methods

CREATE FUNCTION scalar(bigint)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_bigint'
LANGUAGE C;

CREATE FUNCTION nvals(scalar)
RETURNS int2
AS '$libdir/onesparse', 'scalar_nvals'
LANGUAGE C;

CREATE FUNCTION print(scalar)
RETURNS text
AS '$libdir/onesparse', 'scalar_print'
LANGUAGE C;

-- Casts

CREATE CAST (bigint AS onesparse.scalar)
    WITH FUNCTION onesparse.scalar(bigint)
    AS ASSIGNMENT;

CREATE TABLE grb_type (
    name text PRIMARY KEY,
    struct text
    );

INSERT INTO grb_type (name) VALUES
    ('BOOL'),
    ('INT8'),
    ('INT16'),
    ('INT32'),
    ('INT64'),
    ('UINT8'),
    ('UINT16'),
    ('UINT32'),
    ('UINT64'),
    ('FP32'),
    ('FP64'),
    ('FC32'),
    ('FC64');
