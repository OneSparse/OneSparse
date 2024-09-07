-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION onesparse" to load this file. \quit

CREATE TYPE scalar;

CREATE FUNCTION scalar_in(cstring)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION scalar_out(scalar)
RETURNS cstring
AS '$libdir/onesparse', 'scalar_out'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE scalar (
    input = scalar_in,
    output = scalar_out,
    alignment = int4,
    storage = 'extended',
    internallength = VARIABLE
    );

CREATE FUNCTION nvals(scalar)
RETURNS int2
AS '$libdir/onesparse', 'scalar_nvals'
LANGUAGE C;

-- int64 functions

CREATE FUNCTION scalar(bigint)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_int64'
LANGUAGE C;

CREATE FUNCTION bigint_scalar(scalar)
RETURNS bigint
AS '$libdir/onesparse', 'cast_scalar_int64'
LANGUAGE C;

CREATE FUNCTION scalar_plus(scalar, bigint)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_plus_int64'
LANGUAGE C;

CREATE FUNCTION plus_scalar(bigint, scalar)
RETURNS bigint
AS '$libdir/onesparse', 'plus_scalar_int64'
LANGUAGE C;

CREATE FUNCTION scalar_minus(scalar, bigint)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_minus_int64'
LANGUAGE C;

CREATE FUNCTION minus_scalar(bigint, scalar)
RETURNS bigint
AS '$libdir/onesparse', 'minus_scalar_int64'
LANGUAGE C;

CREATE FUNCTION scalar_mult(scalar, bigint)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_mult_int64'
LANGUAGE C;

CREATE FUNCTION mult_scalar(bigint, scalar)
RETURNS bigint
AS '$libdir/onesparse', 'mult_scalar_int64'
LANGUAGE C;

CREATE FUNCTION scalar_div(scalar, bigint)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_div_int64'
LANGUAGE C;

CREATE FUNCTION div_scalar(bigint, scalar)
RETURNS bigint
AS '$libdir/onesparse', 'div_scalar_int64'
LANGUAGE C;

CREATE OPERATOR + (
    LEFTARG = scalar,
    RIGHTARG = bigint,
    FUNCTION = scalar_plus
    );

CREATE OPERATOR + (
    LEFTARG = bigint,
    RIGHTARG = scalar,
    FUNCTION = plus_scalar
    );

CREATE OPERATOR - (
    LEFTARG = scalar,
    RIGHTARG = bigint,
    FUNCTION = scalar_minus
    );

CREATE OPERATOR - (
    LEFTARG = bigint,
    RIGHTARG = scalar,
    FUNCTION = minus_scalar
    );

CREATE OPERATOR * (
    LEFTARG = scalar,
    RIGHTARG = bigint,
    FUNCTION = scalar_mult
    );

CREATE OPERATOR * (
    LEFTARG = bigint,
    RIGHTARG = scalar,
    FUNCTION = mult_scalar
    );

CREATE OPERATOR / (
    LEFTARG = scalar,
    RIGHTARG = bigint,
    FUNCTION = scalar_div
    );

CREATE OPERATOR / (
    LEFTARG = bigint,
    RIGHTARG = scalar,
    FUNCTION = div_scalar
    );

CREATE CAST (bigint AS onesparse.scalar)
    WITH FUNCTION onesparse.scalar(bigint)
    AS IMPLICIT;

CREATE CAST (onesparse.scalar AS bigint)
    WITH FUNCTION onesparse.bigint_scalar(scalar)
    AS ASSIGNMENT;

-- int32 functions

CREATE FUNCTION scalar(integer)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_int32'
LANGUAGE C;

CREATE FUNCTION integer_scalar(scalar)
RETURNS integer
AS '$libdir/onesparse', 'cast_scalar_int32'
LANGUAGE C;

CREATE FUNCTION scalar_plus(scalar, integer)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_plus_int32'
LANGUAGE C;

CREATE FUNCTION plus_scalar(integer, scalar)
RETURNS integer
AS '$libdir/onesparse', 'plus_scalar_int32'
LANGUAGE C;

CREATE FUNCTION scalar_minus(scalar, integer)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_minus_int32'
LANGUAGE C;

CREATE FUNCTION minus_scalar(integer, scalar)
RETURNS integer
AS '$libdir/onesparse', 'minus_scalar_int32'
LANGUAGE C;

CREATE FUNCTION scalar_mult(scalar, integer)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_mult_int32'
LANGUAGE C;

CREATE FUNCTION mult_scalar(integer, scalar)
RETURNS integer
AS '$libdir/onesparse', 'mult_scalar_int32'
LANGUAGE C;

CREATE FUNCTION scalar_div(scalar, integer)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_div_int32'
LANGUAGE C;

CREATE FUNCTION div_scalar(integer, scalar)
RETURNS integer
AS '$libdir/onesparse', 'div_scalar_int32'
LANGUAGE C;

CREATE OPERATOR + (
    LEFTARG = scalar,
    RIGHTARG = integer,
    FUNCTION = scalar_plus
    );

CREATE OPERATOR + (
    LEFTARG = integer,
    RIGHTARG = scalar,
    FUNCTION = plus_scalar
    );

CREATE OPERATOR - (
    LEFTARG = scalar,
    RIGHTARG = integer,
    FUNCTION = scalar_minus
    );

CREATE OPERATOR - (
    LEFTARG = integer,
    RIGHTARG = scalar,
    FUNCTION = minus_scalar
    );

CREATE OPERATOR * (
    LEFTARG = scalar,
    RIGHTARG = integer,
    FUNCTION = scalar_mult
    );

CREATE OPERATOR * (
    LEFTARG = integer,
    RIGHTARG = scalar,
    FUNCTION = mult_scalar
    );

CREATE OPERATOR / (
    LEFTARG = scalar,
    RIGHTARG = integer,
    FUNCTION = scalar_div
    );

CREATE OPERATOR / (
    LEFTARG = integer,
    RIGHTARG = scalar,
    FUNCTION = div_scalar
    );

-- Casts

CREATE CAST (integer AS onesparse.scalar)
    WITH FUNCTION onesparse.scalar(integer)
    AS IMPLICIT;

CREATE CAST (onesparse.scalar AS integer)
    WITH FUNCTION onesparse.integer_scalar(scalar)
    AS ASSIGNMENT;

-- type tables TODO

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
