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
    -- typmod_in = scalar_typmod_in,
    -- typmod_out = scalar_typmod_out,
    alignment = int4,
    storage = 'extended',
    internallength = VARIABLE
    );

CREATE FUNCTION nvals(scalar)
RETURNS int2
AS '$libdir/onesparse', 'scalar_nvals'
LANGUAGE C;

CREATE FUNCTION scalar(bigint)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_int64'
LANGUAGE C;

CREATE FUNCTION bigint_scalar(scalar)
RETURNS bigint
AS '$libdir/onesparse', 'cast_scalar_int64'
LANGUAGE C;

CREATE CAST (bigint AS onesparse.scalar)
    WITH FUNCTION onesparse.scalar(bigint)
    AS IMPLICIT;

CREATE CAST (onesparse.scalar AS bigint)
    WITH FUNCTION onesparse.bigint_scalar(scalar)
    AS ASSIGNMENT;

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

CREATE FUNCTION scalar(integer)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_int32'
LANGUAGE C;

CREATE FUNCTION integer_scalar(scalar)
RETURNS integer
AS '$libdir/onesparse', 'cast_scalar_int32'
LANGUAGE C;

CREATE CAST (integer AS onesparse.scalar)
    WITH FUNCTION onesparse.scalar(integer)
    AS IMPLICIT;

CREATE CAST (onesparse.scalar AS integer)
    WITH FUNCTION onesparse.integer_scalar(scalar)
    AS ASSIGNMENT;

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

CREATE FUNCTION scalar(smallint)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_int16'
LANGUAGE C;

CREATE FUNCTION smallint_scalar(scalar)
RETURNS smallint
AS '$libdir/onesparse', 'cast_scalar_int16'
LANGUAGE C;

CREATE CAST (smallint AS onesparse.scalar)
    WITH FUNCTION onesparse.scalar(smallint)
    AS IMPLICIT;

CREATE CAST (onesparse.scalar AS smallint)
    WITH FUNCTION onesparse.smallint_scalar(scalar)
    AS ASSIGNMENT;

CREATE FUNCTION scalar_plus(scalar, smallint)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_plus_int16'
LANGUAGE C;

CREATE FUNCTION plus_scalar(smallint, scalar)
RETURNS smallint
AS '$libdir/onesparse', 'plus_scalar_int16'
LANGUAGE C;

CREATE FUNCTION scalar_minus(scalar, smallint)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_minus_int16'
LANGUAGE C;

CREATE FUNCTION minus_scalar(smallint, scalar)
RETURNS smallint
AS '$libdir/onesparse', 'minus_scalar_int16'
LANGUAGE C;

CREATE FUNCTION scalar_mult(scalar, smallint)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_mult_int16'
LANGUAGE C;

CREATE FUNCTION mult_scalar(smallint, scalar)
RETURNS smallint
AS '$libdir/onesparse', 'mult_scalar_int16'
LANGUAGE C;

CREATE FUNCTION scalar_div(scalar, smallint)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_div_int16'
LANGUAGE C;

CREATE FUNCTION div_scalar(smallint, scalar)
RETURNS smallint
AS '$libdir/onesparse', 'div_scalar_int16'
LANGUAGE C;
CREATE OPERATOR + (
    LEFTARG = scalar,
    RIGHTARG = smallint,
    FUNCTION = scalar_plus
    );

CREATE OPERATOR + (
    LEFTARG = smallint,
    RIGHTARG = scalar,
    FUNCTION = plus_scalar
    );

CREATE OPERATOR - (
    LEFTARG = scalar,
    RIGHTARG = smallint,
    FUNCTION = scalar_minus
    );

CREATE OPERATOR - (
    LEFTARG = smallint,
    RIGHTARG = scalar,
    FUNCTION = minus_scalar
    );

CREATE OPERATOR * (
    LEFTARG = scalar,
    RIGHTARG = smallint,
    FUNCTION = scalar_mult
    );

CREATE OPERATOR * (
    LEFTARG = smallint,
    RIGHTARG = scalar,
    FUNCTION = mult_scalar
    );

CREATE OPERATOR / (
    LEFTARG = scalar,
    RIGHTARG = smallint,
    FUNCTION = scalar_div
    );

CREATE OPERATOR / (
    LEFTARG = smallint,
    RIGHTARG = scalar,
    FUNCTION = div_scalar
    );

CREATE FUNCTION scalar(float4)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_fp32'
LANGUAGE C;

CREATE FUNCTION float4_scalar(scalar)
RETURNS float4
AS '$libdir/onesparse', 'cast_scalar_fp32'
LANGUAGE C;

CREATE CAST (float4 AS onesparse.scalar)
    WITH FUNCTION onesparse.scalar(float4)
    AS IMPLICIT;

CREATE CAST (onesparse.scalar AS float4)
    WITH FUNCTION onesparse.float4_scalar(scalar)
    AS ASSIGNMENT;

CREATE FUNCTION scalar_plus(scalar, float4)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_plus_fp32'
LANGUAGE C;

CREATE FUNCTION plus_scalar(float4, scalar)
RETURNS float4
AS '$libdir/onesparse', 'plus_scalar_fp32'
LANGUAGE C;

CREATE FUNCTION scalar_minus(scalar, float4)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_minus_fp32'
LANGUAGE C;

CREATE FUNCTION minus_scalar(float4, scalar)
RETURNS float4
AS '$libdir/onesparse', 'minus_scalar_fp32'
LANGUAGE C;

CREATE FUNCTION scalar_mult(scalar, float4)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_mult_fp32'
LANGUAGE C;

CREATE FUNCTION mult_scalar(float4, scalar)
RETURNS float4
AS '$libdir/onesparse', 'mult_scalar_fp32'
LANGUAGE C;

CREATE FUNCTION scalar_div(scalar, float4)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_div_fp32'
LANGUAGE C;

CREATE FUNCTION div_scalar(float4, scalar)
RETURNS float4
AS '$libdir/onesparse', 'div_scalar_fp32'
LANGUAGE C;
CREATE OPERATOR + (
    LEFTARG = scalar,
    RIGHTARG = float4,
    FUNCTION = scalar_plus
    );

CREATE OPERATOR + (
    LEFTARG = float4,
    RIGHTARG = scalar,
    FUNCTION = plus_scalar
    );

CREATE OPERATOR - (
    LEFTARG = scalar,
    RIGHTARG = float4,
    FUNCTION = scalar_minus
    );

CREATE OPERATOR - (
    LEFTARG = float4,
    RIGHTARG = scalar,
    FUNCTION = minus_scalar
    );

CREATE OPERATOR * (
    LEFTARG = scalar,
    RIGHTARG = float4,
    FUNCTION = scalar_mult
    );

CREATE OPERATOR * (
    LEFTARG = float4,
    RIGHTARG = scalar,
    FUNCTION = mult_scalar
    );

CREATE OPERATOR / (
    LEFTARG = scalar,
    RIGHTARG = float4,
    FUNCTION = scalar_div
    );

CREATE OPERATOR / (
    LEFTARG = float4,
    RIGHTARG = scalar,
    FUNCTION = div_scalar
    );

CREATE FUNCTION scalar(float8)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_fp64'
LANGUAGE C;

CREATE FUNCTION float8_scalar(scalar)
RETURNS float8
AS '$libdir/onesparse', 'cast_scalar_fp64'
LANGUAGE C;

CREATE CAST (float8 AS onesparse.scalar)
    WITH FUNCTION onesparse.scalar(float8)
    AS IMPLICIT;

CREATE CAST (onesparse.scalar AS float8)
    WITH FUNCTION onesparse.float8_scalar(scalar)
    AS ASSIGNMENT;

CREATE FUNCTION scalar_plus(scalar, float8)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_plus_fp64'
LANGUAGE C;

CREATE FUNCTION plus_scalar(float8, scalar)
RETURNS float8
AS '$libdir/onesparse', 'plus_scalar_fp64'
LANGUAGE C;

CREATE FUNCTION scalar_minus(scalar, float8)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_minus_fp64'
LANGUAGE C;

CREATE FUNCTION minus_scalar(float8, scalar)
RETURNS float8
AS '$libdir/onesparse', 'minus_scalar_fp64'
LANGUAGE C;

CREATE FUNCTION scalar_mult(scalar, float8)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_mult_fp64'
LANGUAGE C;

CREATE FUNCTION mult_scalar(float8, scalar)
RETURNS float8
AS '$libdir/onesparse', 'mult_scalar_fp64'
LANGUAGE C;

CREATE FUNCTION scalar_div(scalar, float8)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_div_fp64'
LANGUAGE C;

CREATE FUNCTION div_scalar(float8, scalar)
RETURNS float8
AS '$libdir/onesparse', 'div_scalar_fp64'
LANGUAGE C;
CREATE OPERATOR + (
    LEFTARG = scalar,
    RIGHTARG = float8,
    FUNCTION = scalar_plus
    );

CREATE OPERATOR + (
    LEFTARG = float8,
    RIGHTARG = scalar,
    FUNCTION = plus_scalar
    );

CREATE OPERATOR - (
    LEFTARG = scalar,
    RIGHTARG = float8,
    FUNCTION = scalar_minus
    );

CREATE OPERATOR - (
    LEFTARG = float8,
    RIGHTARG = scalar,
    FUNCTION = minus_scalar
    );

CREATE OPERATOR * (
    LEFTARG = scalar,
    RIGHTARG = float8,
    FUNCTION = scalar_mult
    );

CREATE OPERATOR * (
    LEFTARG = float8,
    RIGHTARG = scalar,
    FUNCTION = mult_scalar
    );

CREATE OPERATOR / (
    LEFTARG = scalar,
    RIGHTARG = float8,
    FUNCTION = scalar_div
    );

CREATE OPERATOR / (
    LEFTARG = float8,
    RIGHTARG = scalar,
    FUNCTION = div_scalar
    );

CREATE FUNCTION scalar(bool)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_bool'
LANGUAGE C;

CREATE FUNCTION bool_scalar(scalar)
RETURNS bool
AS '$libdir/onesparse', 'cast_scalar_bool'
LANGUAGE C;

CREATE CAST (bool AS onesparse.scalar)
    WITH FUNCTION onesparse.scalar(bool)
    AS IMPLICIT;

CREATE CAST (onesparse.scalar AS bool)
    WITH FUNCTION onesparse.bool_scalar(scalar)
    AS ASSIGNMENT;

CREATE TABLE grb_type2 (
    name text PRIMARY KEY,
    type_code integer DEFAULT 0,
    struct text
    );

CREATE INDEX ON grb_type2 (type_code);

INSERT INTO grb_type2 (name, type_code) VALUES
    ('bool',    1),
    ('int8',    2),
    ('uint8',   3),
    ('int16',   4),
    ('uint16',  5),
    ('int32',   6),
    ('uint32',  7),
    ('int64',   8),
    ('uint64',  9),
    ('fp32',    10),
    ('fp64',    11),
    ('fc32',    7070),
    ('fc64',    7071)
;
