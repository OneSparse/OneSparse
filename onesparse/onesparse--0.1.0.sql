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
    storage = 'extended',
    internallength = VARIABLE
    );
-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION onesparse" to load this file. \quit

CREATE TYPE unaryop;

CREATE FUNCTION unaryop_in(cstring)
RETURNS unaryop
AS '$libdir/onesparse', 'unaryop_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION unaryop_out(unaryop)
RETURNS cstring
AS '$libdir/onesparse', 'unaryop_out'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE unaryop (
    input = unaryop_in,
    output = unaryop_out,
    alignment = int4,
    storage = 'extended',
    internallength = VARIABLE
    );
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
    storage = 'extended',
    internallength = VARIABLE
    );
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
    storage = 'extended',
    internallength = VARIABLE
    );
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

CREATE FUNCTION scalar_typmod_in(cstring[])
RETURNS integer
AS '$libdir/onesparse', 'scalar_typmod_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION scalar_typmod_out(integer)
RETURNS cstring
AS '$libdir/onesparse', 'scalar_typmod_out'
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

CREATE FUNCTION scalar_bigint(bigint)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_int64'
LANGUAGE C;

CREATE FUNCTION bigint_scalar(scalar)
RETURNS bigint
AS '$libdir/onesparse', 'cast_scalar_int64'
LANGUAGE C;

CREATE CAST (bigint AS onesparse.scalar)
    WITH FUNCTION onesparse.scalar_bigint(bigint)
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

CREATE FUNCTION scalar_integer(integer)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_int32'
LANGUAGE C;

CREATE FUNCTION integer_scalar(scalar)
RETURNS integer
AS '$libdir/onesparse', 'cast_scalar_int32'
LANGUAGE C;

CREATE CAST (integer AS onesparse.scalar)
    WITH FUNCTION onesparse.scalar_integer(integer)
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

CREATE FUNCTION scalar_smallint(smallint)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_int16'
LANGUAGE C;

CREATE FUNCTION smallint_scalar(scalar)
RETURNS smallint
AS '$libdir/onesparse', 'cast_scalar_int16'
LANGUAGE C;

CREATE CAST (smallint AS onesparse.scalar)
    WITH FUNCTION onesparse.scalar_smallint(smallint)
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

CREATE FUNCTION scalar_float4(float4)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_fp32'
LANGUAGE C;

CREATE FUNCTION float4_scalar(scalar)
RETURNS float4
AS '$libdir/onesparse', 'cast_scalar_fp32'
LANGUAGE C;

CREATE CAST (float4 AS onesparse.scalar)
    WITH FUNCTION onesparse.scalar_float4(float4)
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

CREATE FUNCTION scalar_float8(float8)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_fp64'
LANGUAGE C;

CREATE FUNCTION float8_scalar(scalar)
RETURNS float8
AS '$libdir/onesparse', 'cast_scalar_fp64'
LANGUAGE C;

CREATE CAST (float8 AS onesparse.scalar)
    WITH FUNCTION onesparse.scalar_float8(float8)
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

CREATE FUNCTION scalar_bool(bool)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_bool'
LANGUAGE C;

CREATE FUNCTION bool_scalar(scalar)
RETURNS bool
AS '$libdir/onesparse', 'cast_scalar_bool'
LANGUAGE C;

CREATE CAST (bool AS onesparse.scalar)
    WITH FUNCTION onesparse.scalar_bool(bool)
    AS IMPLICIT;

CREATE CAST (onesparse.scalar AS bool)
    WITH FUNCTION onesparse.bool_scalar(scalar)
    AS ASSIGNMENT;

CREATE TABLE grb_type2 (
    name text PRIMARY KEY,
    short_name text UNIQUE,
    type_code integer DEFAULT 0,
    struct text
    );

CREATE INDEX ON grb_type2 (type_code);

INSERT INTO grb_type2 (name, short_name, type_code) VALUES
    ('bool',   'b',  1),
    ('int8',   'i',  2),
    ('uint8',  'u',  3),
    ('int16',  'i2', 4),
    ('uint16', 'u2', 5),
    ('int32',  'i4', 6),
    ('uint32', 'u4', 7),
    ('int64',  'i8', 8),
    ('uint64', 'u8', 9),
    ('fp32',   'f4', 10),
    ('fp64',   'f8', 11),
    ('fc32',   'c4', 7070),
    ('fc64',   'c8', 7071)
;
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
RETURNS int8
AS '$libdir/onesparse', 'vector_nvals'
LANGUAGE C;

CREATE FUNCTION size(vector)
RETURNS int8
AS '$libdir/onesparse', 'vector_size'
LANGUAGE C;

CREATE FUNCTION ewise_add(
    u vector,
    v vector,
    op binaryop,
    mask vector default null,
    descriptor text default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_ewise_add'
LANGUAGE C STABLE;























