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
    storage = 'main',
    internallength = VARIABLE
    );
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
    storage = 'main',
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
    storage = 'main',
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
    storage = 'main',
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

CREATE TYPE scalar (
    input = scalar_in,
    output = scalar_out,
    alignment = int4,
    storage = 'main',
    internallength = VARIABLE
    );

CREATE FUNCTION nvals(scalar)
RETURNS int2
AS '$libdir/onesparse', 'scalar_nvals'
LANGUAGE C;

CREATE FUNCTION wait(scalar, waitmode integer default 0)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_wait'
LANGUAGE C;

CREATE FUNCTION dup(scalar)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_dup'
LANGUAGE C;

CREATE FUNCTION clear(scalar)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_clear'
LANGUAGE C;

CREATE FUNCTION scalar_bigint(bigint)
RETURNS scalar
AS '$libdir/onesparse', 'scalar_int64'
LANGUAGE C;

CREATE FUNCTION bigint_scalar(scalar)
RETURNS bigint
AS '$libdir/onesparse', 'cast_scalar_int64'
LANGUAGE C;

CREATE FUNCTION set(scalar, bigint)
RETURNS scalar
AS '$libdir/onesparse', 'set_scalar_int64'
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

CREATE FUNCTION set(scalar, integer)
RETURNS scalar
AS '$libdir/onesparse', 'set_scalar_int32'
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

CREATE FUNCTION set(scalar, smallint)
RETURNS scalar
AS '$libdir/onesparse', 'set_scalar_int16'
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

CREATE FUNCTION set(scalar, float4)
RETURNS scalar
AS '$libdir/onesparse', 'set_scalar_fp32'
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

CREATE FUNCTION set(scalar, float8)
RETURNS scalar
AS '$libdir/onesparse', 'set_scalar_fp64'
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

CREATE FUNCTION set(scalar, bool)
RETURNS scalar
AS '$libdir/onesparse', 'set_scalar_bool'
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

CREATE TYPE vector (
    input = vector_in,
    output = vector_out,
    alignment = int4,
    storage = 'external',
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
    inout w vector default null,
    mask vector default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_ewise_add'
LANGUAGE C STABLE;

CREATE FUNCTION ewise_mult(
    u vector,
    v vector,
    op binaryop,
    inout w vector default null,
    mask vector default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_ewise_mult'
LANGUAGE C STABLE;

CREATE FUNCTION ewise_union(
    u vector,
    alpha scalar,
    v vector,
    beta scalar,
    op binaryop,
    inout w vector default null,
    mask vector default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_ewise_union'
LANGUAGE C STABLE;

CREATE FUNCTION reduce_scalar(
    a vector,
    op monoid,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS scalar
AS '$libdir/onesparse', 'vector_reduce_scalar'
LANGUAGE C STABLE;

CREATE FUNCTION assign(
    a vector,
    b vector,
    op monoid,
    mask vector default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_assign'
LANGUAGE C STABLE;

CREATE FUNCTION vector_select(
    a vector,
    op indexunaryop,
    y scalar,
    inout c vector default null,
    mask vector default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_select'
LANGUAGE C STABLE;

CREATE FUNCTION wait(vector, waitmode integer default 0)
RETURNS void
AS '$libdir/onesparse', 'vector_wait'
LANGUAGE C;

CREATE FUNCTION dup(vector)
RETURNS vector
AS '$libdir/onesparse', 'vector_dup'
LANGUAGE C;

CREATE FUNCTION clear(vector)
RETURNS void
AS '$libdir/onesparse', 'vector_clear'
LANGUAGE C;























-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION onesparse" to load this file. \quit

CREATE TYPE matrix;

CREATE FUNCTION matrix_in(cstring)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION matrix_out(matrix)
RETURNS cstring
AS '$libdir/onesparse', 'matrix_out'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE matrix (
    input = matrix_in,
    output = matrix_out,
    alignment = int4,
    storage = 'external',
    internallength = VARIABLE
    );

CREATE FUNCTION nvals(matrix)
RETURNS int8
AS '$libdir/onesparse', 'matrix_nvals'
LANGUAGE C;

CREATE FUNCTION nrows(matrix)
RETURNS int8
AS '$libdir/onesparse', 'matrix_nrows'
LANGUAGE C;

CREATE FUNCTION ncols(matrix)
RETURNS int8
AS '$libdir/onesparse', 'matrix_ncols'
LANGUAGE C;

CREATE FUNCTION ewise_add(
    a matrix,
    b matrix,
    op binaryop,
    mask matrix default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_ewise_add'
LANGUAGE C STABLE;

CREATE FUNCTION ewise_mult(
    a matrix,
    b matrix,
    op binaryop,
    mask matrix default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_ewise_mult'
LANGUAGE C STABLE;

CREATE FUNCTION ewise_union(
    a matrix,
    alpha scalar,
    b matrix,
    beta scalar,
    op binaryop,
    mask matrix default null,
    descriptor descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_ewise_union'
LANGUAGE C STABLE;

CREATE FUNCTION reduce_vector(
    a matrix,
    op monoid,
    w vector default null,
    mask vector default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'matrix_reduce_vector'
LANGUAGE C STABLE;

CREATE FUNCTION reduce_scalar(
    a matrix,
    op monoid,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS scalar
AS '$libdir/onesparse', 'matrix_reduce_scalar'
LANGUAGE C STABLE;

CREATE FUNCTION matrix_assign(
    a matrix,
    b matrix,
    op binaryop,
    mask matrix default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_assign_matrix'
LANGUAGE C STABLE;

CREATE FUNCTION matrix_extract(
    a matrix,
    b matrix,
    op binaryop,
    mask matrix default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_extract_matrix'
LANGUAGE C STABLE;

CREATE FUNCTION mxm(
    a matrix,
    b matrix,
    op semiring,
    inout c matrix default null,
    mask matrix default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_mxm'
LANGUAGE C STABLE;

CREATE FUNCTION mxv(
    a matrix,
    v vector,
    op semiring,
    inout w vector default null,
    mask matrix default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'matrix_mxv'
LANGUAGE C STABLE;

CREATE FUNCTION vxm(
    u vector,
    b matrix,
    op semiring,
    inout w vector default null,
    mask matrix default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'matrix_vxm'
LANGUAGE C STABLE;

CREATE FUNCTION matrix_select(
    a matrix,
    op indexunaryop,
    y scalar,
    inout c matrix default null,
    mask matrix default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_select'
LANGUAGE C STABLE;

CREATE FUNCTION wait(matrix, waitmode integer default 0)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_wait'
LANGUAGE C;

CREATE FUNCTION dup(matrix)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_dup'
LANGUAGE C;

CREATE FUNCTION clear(matrix)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_clear'
LANGUAGE C;

CREATE FUNCTION mxm_op(a matrix, b matrix)
RETURNS matrix
RETURN onesparse.mxm(a, b, 'any_secondi_int64'::semiring);

CREATE FUNCTION mxv_op(a matrix, b vector)
RETURNS vector
RETURN onesparse.mxv(a, b, 'any_secondi_int64'::semiring);

CREATE FUNCTION vxm_op(a vector, b matrix)
RETURNS vector
RETURN onesparse.vxm(a, b, 'any_secondi_int64'::semiring);

CREATE OPERATOR @ (
    LEFTARG = matrix,
    RIGHTARG = matrix,
    FUNCTION = mxm_op
    );

CREATE OPERATOR @ (
    LEFTARG = matrix,
    RIGHTARG = vector,
    FUNCTION = mxv_op
    );

CREATE OPERATOR @ (
    LEFTARG = vector,
    RIGHTARG = matrix,
    FUNCTION = vxm_op
    );
