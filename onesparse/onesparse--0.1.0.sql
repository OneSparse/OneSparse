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

CREATE FUNCTION name(type)
RETURNS text
AS '$libdir/onesparse', 'type_name'
LANGUAGE C IMMUTABLE STRICT;
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

CREATE FUNCTION name(descriptor)
RETURNS text
AS '$libdir/onesparse', 'descriptor_name'
LANGUAGE C IMMUTABLE STRICT;
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

CREATE FUNCTION name(unaryop)
RETURNS text
AS '$libdir/onesparse', 'unaryop_name'
LANGUAGE C IMMUTABLE STRICT;
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

CREATE FUNCTION name(indexunaryop)
RETURNS text
AS '$libdir/onesparse', 'indexunaryop_name'
LANGUAGE C IMMUTABLE STRICT;
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

CREATE FUNCTION name(binaryop)
RETURNS text
AS '$libdir/onesparse', 'binaryop_name'
LANGUAGE C IMMUTABLE STRICT;
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

CREATE FUNCTION name(monoid)
RETURNS text
AS '$libdir/onesparse', 'monoid_name'
LANGUAGE C IMMUTABLE STRICT;
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

CREATE FUNCTION name(semiring)
RETURNS text
AS '$libdir/onesparse', 'semiring_name'
LANGUAGE C IMMUTABLE STRICT;
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

CREATE FUNCTION type(scalar)
RETURNS type
AS '$libdir/onesparse', 'scalar_type'
LANGUAGE C STABLE;

CREATE FUNCTION nvals(scalar)
RETURNS int2
AS '$libdir/onesparse', 'scalar_nvals'
LANGUAGE C;

CREATE FUNCTION print(a scalar)
RETURNS text
AS '$libdir/onesparse', 'scalar_print'
LANGUAGE C STABLE;

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

CREATE FUNCTION vector(t type, vsize bigint default -1)
RETURNS vector
AS '$libdir/onesparse', 'vector_new'
LANGUAGE C STABLE;

CREATE FUNCTION type(vector)
RETURNS type
AS '$libdir/onesparse', 'vector_type'
LANGUAGE C STABLE;

CREATE FUNCTION elements(v vector)
RETURNS TABLE (i bigint, v scalar)
AS '$libdir/onesparse', 'vector_elements'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION nvals(vector)
RETURNS int8
AS '$libdir/onesparse', 'vector_nvals'
LANGUAGE C;

CREATE FUNCTION size(vector)
RETURNS int8
AS '$libdir/onesparse', 'vector_size'
LANGUAGE C;

CREATE FUNCTION eadd(
    u vector,
    v vector,
    op binaryop default null,
    inout w vector default null,
    mask vector default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_eadd'
LANGUAGE C STABLE;

CREATE FUNCTION emult(
    u vector,
    v vector,
    op binaryop default null,
    inout w vector default null,
    mask vector default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_emult'
LANGUAGE C STABLE;

CREATE FUNCTION eunion(
    u vector,
    alpha scalar,
    v vector,
    beta scalar,
    op binaryop default null,
    inout w vector default null,
    mask vector default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_eunion'
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

CREATE FUNCTION choose(
    u vector,
    op indexunaryop,
    y scalar,
    inout w vector default null,
    mask vector default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_select'
LANGUAGE C STABLE;

CREATE FUNCTION apply(
    u vector,
    op unaryop,
    inout w vector default null,
    mask vector default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_apply'
LANGUAGE C STABLE;

CREATE FUNCTION apply(
    s scalar,
    a vector,
    op binaryop default null,
    inout c vector default null,
    mask vector default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_apply_first'
LANGUAGE C STABLE;

CREATE FUNCTION apply(
    a vector,
    s scalar,
    op binaryop default null,
    inout c vector default null,
    mask vector default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_apply_second'
LANGUAGE C STABLE;

CREATE FUNCTION set_element(a vector, i bigint, s scalar)
RETURNS vector
AS '$libdir/onesparse', 'vector_set_element'
LANGUAGE C STABLE;

CREATE FUNCTION get_element(a vector, i bigint)
RETURNS scalar
AS '$libdir/onesparse', 'vector_get_element'
LANGUAGE C STABLE;

CREATE FUNCTION remove_element(a vector, i bigint)
RETURNS vector
AS '$libdir/onesparse', 'vector_remove_element'
LANGUAGE C STABLE;

CREATE FUNCTION contains(a vector, i bigint)
RETURNS bool
AS '$libdir/onesparse', 'vector_contains'
LANGUAGE C STABLE;

CREATE FUNCTION eq(a vector, b vector)
RETURNS bool
AS '$libdir/onesparse', 'vector_eq'
LANGUAGE C STABLE;

CREATE FUNCTION neq(a vector, b vector)
RETURNS bool
RETURN NOT eq(a, b);

CREATE FUNCTION info(a vector, level int default 1)
RETURNS text
AS '$libdir/onesparse', 'vector_info'
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
RETURNS vector
AS '$libdir/onesparse', 'vector_clear'
LANGUAGE C;


CREATE FUNCTION eadd_plus_op(a vector, b vector)
RETURNS vector
RETURN onesparse.eadd(a, b, ('plus_' || name(type(a)))::binaryop);

CREATE FUNCTION eadd_minus_op(a vector, b vector)
RETURNS vector
RETURN onesparse.eadd(a, b, ('minus_' || name(type(a)))::binaryop);

CREATE FUNCTION eadd_div_op(a vector, b vector)
RETURNS vector
RETURN onesparse.eadd(a, b, ('div_' || name(type(a)))::binaryop);

CREATE FUNCTION eadd_times_op(a vector, b vector)
RETURNS vector
RETURN onesparse.eadd(a, b, ('times_' || name(type(a)))::binaryop);

CREATE FUNCTION emult_times_op(a vector, b vector)
RETURNS vector
RETURN onesparse.emult(a, b, ('times_' || name(type(a)))::binaryop);

CREATE FUNCTION emult_plus_op(a vector, b vector)
RETURNS vector
RETURN onesparse.emult(a, b, ('plus_' || name(type(a)))::binaryop);

CREATE FUNCTION emult_minus_op(a vector, b vector)
RETURNS vector
RETURN onesparse.emult(a, b, ('minus_' || name(type(a)))::binaryop);

CREATE FUNCTION emult_div_op(a vector, b vector)
RETURNS vector
RETURN onesparse.emult(a, b, ('div_' || name(type(a)))::binaryop);

CREATE FUNCTION plus_first_op(s scalar, a vector)
RETURNS vector
RETURN onesparse.apply(s, a, ('plus_' || name(type(a)))::binaryop);

CREATE FUNCTION plus_second_op(a vector, s scalar)
RETURNS vector
RETURN onesparse.apply(a, s, ('plus_' || name(type(a)))::binaryop);

CREATE FUNCTION minus_first_op(s scalar, a vector)
RETURNS vector
RETURN onesparse.apply(s, a, ('minus_' || name(type(a)))::binaryop);

CREATE FUNCTION minus_second_op(a vector, s scalar)
RETURNS vector
RETURN onesparse.apply(a, s, ('minus_' || name(type(a)))::binaryop);

CREATE FUNCTION div_first_op(s scalar, a vector)
RETURNS vector
RETURN onesparse.apply(s, a, ('div_' || name(type(a)))::binaryop);

CREATE FUNCTION div_second_op(a vector, s scalar)
RETURNS vector
RETURN onesparse.apply(a, s, ('div_' || name(type(a)))::binaryop);

CREATE FUNCTION times_first_op(s scalar, a vector)
RETURNS vector
RETURN onesparse.apply(s, a, ('times_' || name(type(a)))::binaryop);

CREATE FUNCTION times_second_op(a vector, s scalar)
RETURNS vector
RETURN onesparse.apply(a, s, ('times_' || name(type(a)))::binaryop);

CREATE OPERATOR = (
    LEFTARG = vector,
    RIGHTARG = vector,
    FUNCTION = eq
    );

CREATE OPERATOR != (
    LEFTARG = vector,
    RIGHTARG = vector,
    FUNCTION = neq
    );

-- scalar apply ops

CREATE OPERATOR + (
    LEFTARG = scalar,
    RIGHTARG = vector,
    FUNCTION = plus_first_op
    );

CREATE OPERATOR + (
    LEFTARG = vector,
    RIGHTARG = scalar,
    FUNCTION = plus_second_op
    );

CREATE OPERATOR - (
    LEFTARG = scalar,
    RIGHTARG = vector,
    FUNCTION = minus_first_op
    );

CREATE OPERATOR - (
    LEFTARG = vector,
    RIGHTARG = scalar,
    FUNCTION = minus_second_op
    );

CREATE OPERATOR / (
    LEFTARG = scalar,
    RIGHTARG = vector,
    FUNCTION = div_first_op
    );

CREATE OPERATOR / (
    LEFTARG = vector,
    RIGHTARG = scalar,
    FUNCTION = div_second_op
    );

CREATE OPERATOR * (
    LEFTARG = scalar,
    RIGHTARG = vector,
    FUNCTION = times_first_op
    );

CREATE OPERATOR * (
    LEFTARG = vector,
    RIGHTARG = scalar,
    FUNCTION = times_second_op
    );

-- ewise add "OR ops"

CREATE OPERATOR |+ (
    LEFTARG = vector,
    RIGHTARG = vector,
    FUNCTION = eadd_plus_op
    );

CREATE OPERATOR |- (
    LEFTARG = vector,
    RIGHTARG = vector,
    FUNCTION = eadd_minus_op
    );

CREATE OPERATOR |/ (
    LEFTARG = vector,
    RIGHTARG = vector,
    FUNCTION = eadd_div_op
    );

CREATE OPERATOR |* (
    LEFTARG = vector,
    RIGHTARG = vector,
    FUNCTION = eadd_times_op
    );

-- ewise add "AND ops"

CREATE OPERATOR &+ (
    LEFTARG = vector,
    RIGHTARG = vector,
    FUNCTION = emult_plus_op
    );

CREATE OPERATOR &- (
    LEFTARG = vector,
    RIGHTARG = vector,
    FUNCTION = emult_minus_op
    );

CREATE OPERATOR &/ (
    LEFTARG = vector,
    RIGHTARG = vector,
    FUNCTION = emult_div_op
    );

CREATE OPERATOR &* (
    LEFTARG = vector,
    RIGHTARG = vector,
    FUNCTION = emult_times_op
    );

create function print(a vector) returns text language plpgsql as
    $$
    declare
        imax int = size(a) - 1;
        out text = E'\n   \u2500\u2500\u2500\n';
    begin
        for i in 0..imax loop
            out = out || lpad(i::text, 2) || E'\u2502';
            if contains(a, i) then
                out = out || lpad(print(get_element(a, i)), 3);
            else
                out = out || E'   ';
            end if;
            out = out || E'   \n';
        end loop;
        return out;
    end;
    $$;

create function random_vector(
    vsize integer,
    nvals integer,
    max integer default 2^31 - 1,
    seed double precision default null)
    returns vector language plpgsql as
    $$
    declare v vector = vector('int32', vsize);
    prob double precision = nvals::double precision / vsize;
    begin
        if (seed is not null) then
            perform setseed(seed);
        end if;
        for i in 0..vsize-1 loop
            if random() < prob then
                v = set_element(v, i, random(0, max));
            end if;
        end loop;
        return v;
    end;
    $$;

create function dense_vector(
    t type,
    vsize integer,
    fill integer default 0)
        returns vector language plpgsql as
    $$
    declare v vector = vector(t, vsize);
    begin
        for i in 0..vsize-1 loop
            v = set_element(v, i, fill);
        end loop;
        return v;
    end;
    $$;

create or replace function draw(a vector) returns text language plpgsql as
    $$
    declare
        imax int = size(a) - 1;
        value scalar;
        result text = E'digraph vector {\ngraph [rankdir=TB]; node [shape=box];\nsubgraph cluster_vector {\nstyle=dashed; color=black; bgcolor=lightgray;\n';
    begin
        for i in 0..imax loop
            result = result || 'node' || i::text;
            if contains(a, i) then
                result = result || ' [label="' || i::text || ':' || print(get_element(a, i)) || E'"];\n';
            else
                result = result || E' [label=""];\n';
            end if;
        end loop;
        result = result || E'}\n';
        for i in 0..imax loop
            if i != imax then
                result = result || 'node' || i::text || ' -> ';
            else
                result = result || 'node' || i::text ||  E' [style=invis];\n';
            end if;
        end loop;
        result = result || E'}\n';
        return result;
    end;
    $$;























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

CREATE FUNCTION matrix(t type, nrows bigint default -1, ncols bigint default -1)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_new'
LANGUAGE C STABLE;

CREATE FUNCTION elements(a matrix)
RETURNS TABLE (i bigint, j bigint, v scalar)
AS '$libdir/onesparse', 'matrix_elements'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION type(matrix)
RETURNS type
AS '$libdir/onesparse', 'matrix_type'
LANGUAGE C STABLE;

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

CREATE FUNCTION eadd(
    a matrix,
    b matrix,
    op binaryop default null,
    mask matrix default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_eadd'
LANGUAGE C STABLE;

CREATE FUNCTION emult(
    a matrix,
    b matrix,
    op binaryop default null,
    mask matrix default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_emult'
LANGUAGE C STABLE;

CREATE FUNCTION eunion(
    a matrix,
    alpha scalar,
    b matrix,
    beta scalar,
    op binaryop default null,
    mask matrix default null,
    descriptor descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_eunion'
LANGUAGE C STABLE;

CREATE FUNCTION reduce_vector(
    a matrix,
    op monoid default null,
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
    op monoid default null,
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
    op semiring default null,
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
    op semiring default null,
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
    op semiring default null,
    inout w vector default null,
    mask matrix default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'matrix_vxm'
LANGUAGE C STABLE;

CREATE FUNCTION kronecker(
    a matrix,
    b matrix,
    op semiring default null,
    inout c matrix default null,
    mask matrix default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_kron'
LANGUAGE C STABLE;

CREATE FUNCTION choose(
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

CREATE FUNCTION transpose(
    a matrix,
    inout c matrix default null,
    mask matrix default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_transpose'
LANGUAGE C STABLE;

CREATE FUNCTION apply(
    a matrix,
    op unaryop,
    inout c matrix default null,
    mask matrix default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_apply'
LANGUAGE C STABLE;

CREATE FUNCTION apply(
    s scalar,
    a matrix,
    op binaryop default null,
    inout c matrix default null,
    mask matrix default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_apply_first'
LANGUAGE C STABLE;

CREATE FUNCTION apply(
    a matrix,
    s scalar,
    op binaryop default null,
    inout c matrix default null,
    mask matrix default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_apply_second'
LANGUAGE C STABLE;

CREATE FUNCTION matrix_agg_final(state matrix)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_agg_final'
LANGUAGE C STABLE;

CREATE FUNCTION set_element(a matrix, i bigint, j bigint, s scalar)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_set_element'
LANGUAGE C STABLE;

CREATE FUNCTION get_element(a matrix, i bigint, j bigint)
RETURNS scalar
AS '$libdir/onesparse', 'matrix_get_element'
LANGUAGE C STABLE;

CREATE FUNCTION remove_element(a matrix, i bigint, j bigint)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_remove_element'
LANGUAGE C STABLE;

CREATE FUNCTION contains(a matrix, i bigint, j bigint)
RETURNS bool
AS '$libdir/onesparse', 'matrix_contains'
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

CREATE FUNCTION resize(a matrix, i bigint, j bigint)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_resize'
LANGUAGE C STABLE;

CREATE FUNCTION info(a matrix, level int default 1)
RETURNS text
AS '$libdir/onesparse', 'matrix_info'
LANGUAGE C STABLE;

CREATE FUNCTION eadd_plus_op(a matrix, b matrix)
RETURNS matrix
RETURN onesparse.eadd(a, b, ('plus_' || name(type(a)))::binaryop);

CREATE FUNCTION eadd_minus_op(a matrix, b matrix)
RETURNS matrix
RETURN onesparse.eadd(a, b, ('minus_' || name(type(a)))::binaryop);

CREATE FUNCTION eadd_div_op(a matrix, b matrix)
RETURNS matrix
RETURN onesparse.eadd(a, b, ('div_' || name(type(a)))::binaryop);

CREATE FUNCTION eadd_times_op(a matrix, b matrix)
RETURNS matrix
RETURN onesparse.eadd(a, b, ('times_' || name(type(a)))::binaryop);

CREATE FUNCTION emult_times_op(a matrix, b matrix)
RETURNS matrix
RETURN onesparse.emult(a, b, ('times_' || name(type(a)))::binaryop);

CREATE FUNCTION emult_plus_op(a matrix, b matrix)
RETURNS matrix
RETURN onesparse.emult(a, b, ('plus_' || name(type(a)))::binaryop);

CREATE FUNCTION emult_minus_op(a matrix, b matrix)
RETURNS matrix
RETURN onesparse.emult(a, b, ('minus_' || name(type(a)))::binaryop);

CREATE FUNCTION emult_div_op(a matrix, b matrix)
RETURNS matrix
RETURN onesparse.emult(a, b, ('div_' || name(type(a)))::binaryop);

CREATE FUNCTION plus_first_op(s scalar, a matrix)
RETURNS matrix
RETURN onesparse.apply(s, a, ('plus_' || name(type(a)))::binaryop);

CREATE FUNCTION plus_second_op(a matrix, s scalar)
RETURNS matrix
RETURN onesparse.apply(a, s, ('plus_' || name(type(a)))::binaryop);

CREATE FUNCTION minus_first_op(s scalar, a matrix)
RETURNS matrix
RETURN onesparse.apply(s, a, ('minus_' || name(type(a)))::binaryop);

CREATE FUNCTION minus_second_op(a matrix, s scalar)
RETURNS matrix
RETURN onesparse.apply(a, s, ('minus_' || name(type(a)))::binaryop);

CREATE FUNCTION div_first_op(s scalar, a matrix)
RETURNS matrix
RETURN onesparse.apply(s, a, ('div_' || name(type(a)))::binaryop);

CREATE FUNCTION div_second_op(a matrix, s scalar)
RETURNS matrix
RETURN onesparse.apply(a, s, ('div_' || name(type(a)))::binaryop);

CREATE FUNCTION times_first_op(s scalar, a matrix)
RETURNS matrix
RETURN onesparse.apply(s, a, ('times_' || name(type(a)))::binaryop);

CREATE FUNCTION times_second_op(a matrix, s scalar)
RETURNS matrix
RETURN onesparse.apply(a, s, ('times_' || name(type(a)))::binaryop);

CREATE FUNCTION mxm_op(a matrix, b matrix)
RETURNS matrix
RETURN onesparse.mxm(a, b);

CREATE FUNCTION mxv_op(a matrix, b vector)
RETURNS vector
RETURN onesparse.mxv(a, b);

CREATE FUNCTION vxm_op(a vector, b matrix)
RETURNS vector
RETURN onesparse.vxm(a, b);

-- comparison

CREATE FUNCTION eq(a matrix, b matrix)
RETURNS bool
AS '$libdir/onesparse', 'matrix_eq'
LANGUAGE C STABLE;

CREATE FUNCTION neq(a matrix, b matrix)
RETURNS bool
RETURN NOT eq(a, b);

CREATE FUNCTION gt(a matrix, s scalar)
RETURNS matrix
RETURN onesparse.choose(a, ('valuegt_' || name(type(a)))::indexunaryop, s);

CREATE FUNCTION lt(a matrix, s scalar)
RETURNS matrix
RETURN onesparse.choose(a, ('valuelt_' || name(type(a)))::indexunaryop, s);

CREATE FUNCTION ge(a matrix, s scalar)
RETURNS matrix
RETURN onesparse.choose(a, ('valuege_' || name(type(a)))::indexunaryop, s);

CREATE FUNCTION le(a matrix, s scalar)
RETURNS matrix
RETURN onesparse.choose(a, ('valuele_' || name(type(a)))::indexunaryop, s);


CREATE OPERATOR = (
    LEFTARG = matrix,
    RIGHTARG = matrix,
    FUNCTION = eq
    );

CREATE OPERATOR != (
    LEFTARG = matrix,
    RIGHTARG = matrix,
    FUNCTION = neq
    );

CREATE OPERATOR > (
    LEFTARG = matrix,
    RIGHTARG = scalar,
    FUNCTION = gt
    );

CREATE OPERATOR < (
    LEFTARG = matrix,
    RIGHTARG = scalar,
    FUNCTION = lt
    );

CREATE OPERATOR >= (
    LEFTARG = matrix,
    RIGHTARG = scalar,
    FUNCTION = ge
    );

CREATE OPERATOR <= (
    LEFTARG = matrix,
    RIGHTARG = scalar,
    FUNCTION = le
    );

-- scalar apply ops

CREATE OPERATOR + (
    LEFTARG = scalar,
    RIGHTARG = matrix,
    FUNCTION = plus_first_op
    );

CREATE OPERATOR + (
    LEFTARG = matrix,
    RIGHTARG = scalar,
    FUNCTION = plus_second_op
    );

CREATE OPERATOR - (
    LEFTARG = scalar,
    RIGHTARG = matrix,
    FUNCTION = minus_first_op
    );

CREATE OPERATOR - (
    LEFTARG = matrix,
    RIGHTARG = scalar,
    FUNCTION = minus_second_op
    );

CREATE OPERATOR / (
    LEFTARG = scalar,
    RIGHTARG = matrix,
    FUNCTION = div_first_op
    );

CREATE OPERATOR / (
    LEFTARG = matrix,
    RIGHTARG = scalar,
    FUNCTION = div_second_op
    );

CREATE OPERATOR * (
    LEFTARG = scalar,
    RIGHTARG = matrix,
    FUNCTION = times_first_op
    );

CREATE OPERATOR * (
    LEFTARG = matrix,
    RIGHTARG = scalar,
    FUNCTION = times_second_op
    );

-- ewise add "OR ops"

CREATE OPERATOR |+ (
    LEFTARG = matrix,
    RIGHTARG = matrix,
    FUNCTION = eadd_plus_op
    );

CREATE OPERATOR |- (
    LEFTARG = matrix,
    RIGHTARG = matrix,
    FUNCTION = eadd_minus_op
    );

CREATE OPERATOR |/ (
    LEFTARG = matrix,
    RIGHTARG = matrix,
    FUNCTION = eadd_div_op
    );

CREATE OPERATOR |* (
    LEFTARG = matrix,
    RIGHTARG = matrix,
    FUNCTION = eadd_times_op
    );

-- ewise add "AND ops"

CREATE OPERATOR &+ (
    LEFTARG = matrix,
    RIGHTARG = matrix,
    FUNCTION = emult_plus_op
    );

CREATE OPERATOR &- (
    LEFTARG = matrix,
    RIGHTARG = matrix,
    FUNCTION = emult_minus_op
    );

CREATE OPERATOR &/ (
    LEFTARG = matrix,
    RIGHTARG = matrix,
    FUNCTION = emult_div_op
    );

CREATE OPERATOR &* (
    LEFTARG = matrix,
    RIGHTARG = matrix,
    FUNCTION = emult_times_op
    );

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

create function print(a matrix) returns text language plpgsql as
    $$
    declare
        imax int = nrows(a) - 1;
        jmax int = ncols(a) - 1;
        out text = '';
    begin
        out = out || repeat(' ', 3);
        for i in 0..jmax loop
            out = out || lpad(i::text, 3);
        end loop;
        out = out || E'\n   ';
        for i in 0..jmax loop
            out = out || repeat(E'\u2500', 3);
        end loop;
        out = out || E'\n';
        for i in 0..imax loop
            out = out || lpad(i::text, 2) || E'\u2502';
            for j in 0..jmax loop
                if contains(a, i, j) then
                    out = out || lpad(print(get_element(a, i, j)), 3);
                else
                    out = out || E'   ';
                end if;
            end loop;
            out = out || E'   \n';
        end loop;
        return out;
    end;
    $$;

create function random_matrix(
    nrows integer,
    ncols integer,
    nvals integer,
    max integer default 2^31 - 1,
    seed double precision default null)
    returns matrix language plpgsql as
    $$
    declare m matrix = matrix('int32', nrows, ncols);
    prob double precision = nvals::double precision / (nrows * ncols);
    begin
        if (seed is not null) then
            perform setseed(seed);
        end if;
        for i in 0..nrows-1 loop
            for j in 0..ncols-1 loop
                if i = j then continue; end if;
                if random() < prob then
                    m = set_element(m, i, j, random(0, max));
                end if;
            end loop;
        end loop;
        return m;
    end;
    $$;

create function dense_matrix(
    t type,
    nrows integer,
    ncols integer,
    fill integer default 0)
        returns matrix language plpgsql as
    $$
    declare m matrix = matrix(t, nrows, ncols);
    begin
        for i in 0..nrows-1 loop
            for j in 0..ncols-1 loop
                m = set_element(m, i, j, fill);
            end loop;
        end loop;
        return m;
    end;
    $$;


create or replace function draw(a matrix) returns text language plpgsql as
    $$
    declare
        row bigint;
        col bigint;
        value scalar;
        result text = E'digraph {\n';
    begin
        for row, col, value in select * from elements(a) loop
            result = result || format(E'%s -> %s [label="%s"]\n', row, col, print(value));
        end loop;
        result = result || E'}\n';
        return result;
    end;
    $$;

create or replace function kronpower(m matrix, k integer, s semiring default 'plus_times_int32')
    returns matrix language plpgsql as
    $$
    declare
    i integer;
    begin
        for i in select generate_series(0, k-1) loop
            m = kronecker(m, m, s);
        end loop;
    return m;
    end;
    $$;
CREATE FUNCTION matrix_agg_bigint (state matrix, i bigint, j bigint, v bigint)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_agg_int64'
LANGUAGE C STABLE;

CREATE AGGREGATE matrix_agg (i bigint, j bigint, v bigint )
    (
    SFUNC=matrix_agg_bigint,
    STYPE=matrix,
    FINALFUNC=matrix_agg_final
    );
CREATE FUNCTION matrix_agg_integer (state matrix, i bigint, j bigint, v integer)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_agg_int32'
LANGUAGE C STABLE;

CREATE AGGREGATE matrix_agg (i bigint, j bigint, v integer )
    (
    SFUNC=matrix_agg_integer,
    STYPE=matrix,
    FINALFUNC=matrix_agg_final
    );
CREATE FUNCTION matrix_agg_smallint (state matrix, i bigint, j bigint, v smallint)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_agg_int16'
LANGUAGE C STABLE;

CREATE AGGREGATE matrix_agg (i bigint, j bigint, v smallint )
    (
    SFUNC=matrix_agg_smallint,
    STYPE=matrix,
    FINALFUNC=matrix_agg_final
    );
CREATE FUNCTION matrix_agg_float4 (state matrix, i bigint, j bigint, v float4)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_agg_fp32'
LANGUAGE C STABLE;

CREATE AGGREGATE matrix_agg (i bigint, j bigint, v float4 )
    (
    SFUNC=matrix_agg_float4,
    STYPE=matrix,
    FINALFUNC=matrix_agg_final
    );
CREATE FUNCTION matrix_agg_float8 (state matrix, i bigint, j bigint, v float8)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_agg_fp64'
LANGUAGE C STABLE;

CREATE AGGREGATE matrix_agg (i bigint, j bigint, v float8 )
    (
    SFUNC=matrix_agg_float8,
    STYPE=matrix,
    FINALFUNC=matrix_agg_final
    );
