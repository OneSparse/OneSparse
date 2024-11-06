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

COMMENT ON TYPE unaryop IS 'UnaryOps apply a function to an element, returning an element.';

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

COMMENT ON TYPE indexunaryop IS 'IndexUnaryOps apply a function to a positional element, returning an element.';

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

COMMENT ON TYPE binaryop IS 'BinaryOps apply a function to two elements and returning an element.';

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

COMMENT ON TYPE monoid IS 'Monoids apply a BinaryOp and an identity value to two elements, returning an element.';

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

COMMENT ON TYPE semiring IS 'Semirings associate additive and multiplicative operators for matrix multiplication.';

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

COMMENT ON TYPE scalar IS 'Scalars hold individual element values.';

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

COMMENT ON TYPE vector IS 'Vectors hold a sparse array of elements.';

CREATE FUNCTION vector(t type, vsize bigint default -1)
RETURNS vector
AS '$libdir/onesparse', 'vector_new'
LANGUAGE C STABLE;

CREATE FUNCTION vector_agg_final(vector)
RETURNS vector
AS '$libdir/onesparse', 'vector_agg_final'
LANGUAGE C STRICT;

CREATE FUNCTION type(vector)
RETURNS type
AS '$libdir/onesparse', 'vector_type'
LANGUAGE C STABLE;

CREATE FUNCTION elements(b vector)
RETURNS TABLE (i bigint, v scalar)
AS '$libdir/onesparse', 'vector_elements'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION nvals(vector)
RETURNS bigint
AS '$libdir/onesparse', 'vector_nvals'
LANGUAGE C;

CREATE FUNCTION vector_bool(b vector)
RETURNS bool
RETURN nvals(b) > 0;

CREATE CAST (vector AS bool)
    WITH FUNCTION vector_bool(vector)
    AS IMPLICIT;

CREATE FUNCTION size(vector)
RETURNS int8
AS '$libdir/onesparse', 'vector_size'
LANGUAGE C;

CREATE FUNCTION eadd(
    a vector,
    b vector,
    op binaryop default null,
    inout c vector default null,
    mask vector default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_eadd'
LANGUAGE C STABLE;

CREATE FUNCTION emult(
    a vector,
    b vector,
    op binaryop default null,
    inout c vector default null,
    mask vector default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_emult'
LANGUAGE C STABLE;

CREATE FUNCTION eunion(
    a vector,
    alpha scalar,
    b vector,
    beta scalar,
    op binaryop default null,
    inout c vector default null,
    mask vector default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_eunion'
LANGUAGE C STABLE;

CREATE FUNCTION reduce_scalar(
    a vector,
    op monoid default null,
    c scalar default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS scalar
AS '$libdir/onesparse', 'vector_reduce_scalar'
LANGUAGE C STABLE;

CREATE FUNCTION choose(
    a vector,
    op indexunaryop,
    y scalar,
    inout c vector default null,
    mask vector default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_select'
LANGUAGE C STABLE;

CREATE FUNCTION apply(
    a vector,
    op unaryop,
    inout c vector default null,
    mask vector default null,
    accum binaryop default null,
    descr descriptor default null
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
    descr descriptor default null
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
    descr descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_apply_second'
LANGUAGE C STABLE;

CREATE FUNCTION assign(
    c vector,
    a vector,
    i bigint[] default null,
    mask vector default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_assign'
LANGUAGE C STABLE;

CREATE FUNCTION assign(
    c vector,
    s scalar,
    i bigint[] default null,
    mask vector default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_assign_scalar'
LANGUAGE C STABLE;

CREATE FUNCTION xtract(
    a vector,
    i bigint[] default null,
    c vector default null,
    mask vector default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_extract_vector'
LANGUAGE C STABLE;

CREATE FUNCTION set_element(inout a vector, i bigint, s scalar)
RETURNS vector
AS '$libdir/onesparse', 'vector_set_element'
LANGUAGE C VOLATILE;

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

CREATE FUNCTION gt(a vector, s scalar)
RETURNS vector
RETURN onesparse.choose(a, ('valuegt_' || name(type(a)))::indexunaryop, s);

CREATE FUNCTION lt(a vector, s scalar)
RETURNS vector
RETURN onesparse.choose(a, ('valuelt_' || name(type(a)))::indexunaryop, s);

CREATE FUNCTION ge(a vector, s scalar)
RETURNS vector
RETURN onesparse.choose(a, ('valuege_' || name(type(a)))::indexunaryop, s);

CREATE FUNCTION le(a vector, s scalar)
RETURNS vector
RETURN onesparse.choose(a, ('valuele_' || name(type(a)))::indexunaryop, s);

CREATE FUNCTION one(a vector)
RETURNS vector
RETURN onesparse.apply(a, ('one_' || name(type(a)))::unaryop);

CREATE FUNCTION ainv(a vector)
RETURNS vector
RETURN onesparse.apply(a, ('ainv_' || name(type(a)))::unaryop);

CREATE FUNCTION minv(a vector)
RETURNS vector
RETURN onesparse.apply(a, ('minv_' || name(type(a)))::unaryop);

CREATE FUNCTION lnot(a vector)
RETURNS vector
RETURN onesparse.apply(a, ('lnot_' || name(type(a)))::unaryop);

CREATE FUNCTION bnot(a vector)
RETURNS vector
RETURN onesparse.apply(a, ('bnot_' || name(type(a)))::unaryop);

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

CREATE FUNCTION eadd_min(
    a vector,
    b vector,
    c vector default null,
    mask vector default null,
    accum binaryop default null,
    descr descriptor default null)
RETURNS vector
RETURN onesparse.eadd(a, b, ('min_' || name(type(a)))::binaryop, c, mask, accum, descr);

CREATE FUNCTION eadd_min_op(a vector, b vector)
RETURNS vector
RETURN onesparse.eadd_min(a, b);

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

CREATE OPERATOR > (
    LEFTARG = vector,
    RIGHTARG = scalar,
    FUNCTION = gt
    );

CREATE OPERATOR < (
    LEFTARG = vector,
    RIGHTARG = scalar,
    FUNCTION = lt
    );

CREATE OPERATOR >= (
    LEFTARG = vector,
    RIGHTARG = scalar,
    FUNCTION = ge
    );

CREATE OPERATOR <= (
    LEFTARG = vector,
    RIGHTARG = scalar,
    FUNCTION = le
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



CREATE FUNCTION vector_agg_bigint (state vector, i bigint, v bigint)
RETURNS vector
AS '$libdir/onesparse', 'vector_agg_int64'
LANGUAGE C STABLE;

CREATE AGGREGATE vector_agg (i bigint, v bigint )
    (
    SFUNC=vector_agg_bigint,
    STYPE=vector,
    FINALFUNC=vector_agg_final
    );



CREATE FUNCTION vector_agg_integer (state vector, i bigint, v integer)
RETURNS vector
AS '$libdir/onesparse', 'vector_agg_int32'
LANGUAGE C STABLE;

CREATE AGGREGATE vector_agg (i bigint, v integer )
    (
    SFUNC=vector_agg_integer,
    STYPE=vector,
    FINALFUNC=vector_agg_final
    );



CREATE FUNCTION vector_agg_smallint (state vector, i bigint, v smallint)
RETURNS vector
AS '$libdir/onesparse', 'vector_agg_int16'
LANGUAGE C STABLE;

CREATE AGGREGATE vector_agg (i bigint, v smallint )
    (
    SFUNC=vector_agg_smallint,
    STYPE=vector,
    FINALFUNC=vector_agg_final
    );



CREATE FUNCTION vector_agg_float4 (state vector, i bigint, v float4)
RETURNS vector
AS '$libdir/onesparse', 'vector_agg_fp32'
LANGUAGE C STABLE;

CREATE AGGREGATE vector_agg (i bigint, v float4 )
    (
    SFUNC=vector_agg_float4,
    STYPE=vector,
    FINALFUNC=vector_agg_final
    );



CREATE FUNCTION vector_agg_float8 (state vector, i bigint, v float8)
RETURNS vector
AS '$libdir/onesparse', 'vector_agg_fp64'
LANGUAGE C STABLE;

CREATE AGGREGATE vector_agg (i bigint, v float8 )
    (
    SFUNC=vector_agg_float8,
    STYPE=vector,
    FINALFUNC=vector_agg_final
    );



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
    storage = 'external',
    internallength = VARIABLE
    );

COMMENT ON TYPE matrix IS 'Matrices hold sparse rows and columns of elements.';

CREATE FUNCTION matrix(t type, nrows bigint default -1, ncols bigint default -1)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_new'
LANGUAGE C STABLE;

CREATE FUNCTION elements(a matrix)
RETURNS TABLE (i bigint, j bigint, v scalar)
AS '$libdir/onesparse', 'matrix_elements'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION irows(a matrix)
RETURNS SETOF bigint
AS '$libdir/onesparse', 'matrix_rows'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION icols(a matrix)
RETURNS SETOF bigint
AS '$libdir/onesparse', 'matrix_cols'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION type(matrix)
RETURNS type
AS '$libdir/onesparse', 'matrix_type'
LANGUAGE C STABLE;

CREATE FUNCTION nvals(matrix)
RETURNS int8
AS '$libdir/onesparse', 'matrix_nvals'
LANGUAGE C;

CREATE FUNCTION matrix_bool(v matrix)
RETURNS bool
RETURN nvals(v) > 0;

CREATE CAST (matrix AS bool)
    WITH FUNCTION matrix_bool(matrix)
    AS IMPLICIT;

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
    c matrix default null,
    mask matrix default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_eadd'
LANGUAGE C STABLE;

CREATE FUNCTION emult(
    a matrix,
    b matrix,
    op binaryop default null,
    c matrix default null,
    mask matrix default null,
    accum binaryop default null,
    descr descriptor default null
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
    c matrix default null,
    mask matrix default null,
    descr descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_eunion'
LANGUAGE C STABLE;

CREATE FUNCTION reduce_cols(
    a matrix,
    op monoid default null,
    w vector default null,
    mask vector default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'matrix_reduce_vector'
LANGUAGE C STABLE;

CREATE FUNCTION reduce_rows(
    a matrix,
    op monoid default null,
    w vector default null,
    mask vector default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS vector
    RETURN reduce_cols(a, op, w, mask, accum, coalesce(descr, 't0'::descriptor));

CREATE FUNCTION reduce_scalar(
    a matrix,
    op monoid default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS scalar
AS '$libdir/onesparse', 'matrix_reduce_scalar'
LANGUAGE C STABLE;

CREATE FUNCTION assign(
    c matrix,
    a matrix,
    i bigint[] default null,
    j bigint[] default null,
    mask matrix default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_assign_matrix'
LANGUAGE C STABLE;

CREATE FUNCTION assign_row(
    c matrix,
    v vector,
    i bigint,
    j bigint[] default null,
    mask matrix default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_assign_row_vector'
LANGUAGE C STABLE;

CREATE FUNCTION assign_col(
    c matrix,
    v vector,
    j bigint,
    i bigint[] default null,
    mask matrix default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_assign_col_vector'
LANGUAGE C STABLE;

CREATE FUNCTION assign(
    c matrix,
    s scalar,
    i bigint[] default null,
    j bigint[] default null,
    mask matrix default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_assign_scalar'
LANGUAGE C STABLE;

CREATE FUNCTION extract_matrix(
    a matrix,
    i bigint[] default null,
    j bigint[] default null,
    c matrix default null,
    mask matrix default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_extract_matrix'
LANGUAGE C STABLE;

CREATE FUNCTION extract_col(
    a matrix,
    j bigint,
    i bigint[] default null,
    c vector default null,
    mask vector default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'matrix_extract_col_vector'
LANGUAGE C STABLE;

CREATE FUNCTION extract_row(
    a matrix,
    i bigint,
    j bigint[] default null,
    c vector default null,
    mask vector default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS vector
    RETURN extract_col(a, i, j, c, mask, accum, coalesce(descr, 't0'::descriptor));

CREATE FUNCTION mxm(
    a matrix,
    b matrix,
    op semiring default null,
    inout c matrix default null,
    mask matrix default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_mxm'
LANGUAGE C STABLE;

CREATE FUNCTION mxv(
    a matrix,
    b vector,
    op semiring default null,
    inout c vector default null,
    mask vector default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'matrix_mxv'
LANGUAGE C STABLE;

CREATE FUNCTION vxm(
    a vector,
    b matrix,
    op semiring default null,
    inout c vector default null,
    mask vector default null,
    accum binaryop default null,
    descr descriptor default null
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
    descr descriptor default null
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
    descr descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_select'
LANGUAGE C STABLE;

CREATE FUNCTION transpose(
    a matrix,
    inout c matrix default null,
    mask matrix default null,
    accum binaryop default null,
    descr descriptor default null
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
    descr descriptor default null
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
    descr descriptor default null
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
    descr descriptor default null
    )
RETURNS matrix
AS '$libdir/onesparse', 'matrix_apply_second'
LANGUAGE C STABLE;

CREATE FUNCTION nnz(a matrix)
RETURNS scalar
    RETURN reduce_scalar(apply(a, 'one_bool'::unaryop, c=>'int64'::matrix));

CREATE FUNCTION row_degree(a matrix)
RETURNS vector
    RETURN reduce_rows(apply(a, 'one_bool'::unaryop, c=>'int64'::matrix));

CREATE FUNCTION col_degree(a matrix)
RETURNS vector
    RETURN reduce_cols(apply(a, 'one_bool'::unaryop, c=>'int64'::matrix));

CREATE FUNCTION cast_to(a matrix, t type)
RETURNS matrix
    RETURN apply(a, ('identity_' || name(t))::unaryop, c=>matrix(t));

CREATE FUNCTION matrix_agg_matrix(state matrix, a matrix)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_agg_matrix'
LANGUAGE C STABLE;

CREATE FUNCTION matrix_agg_final(matrix)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_agg_final'
LANGUAGE C STRICT;

CREATE AGGREGATE matrix_agg (a matrix)
    (
    SFUNC=matrix_agg_matrix,
    STYPE=matrix,
    FINALFUNC=matrix_agg_final
    );

CREATE FUNCTION set_element(a matrix, i bigint, j bigint, s scalar)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_set_element'
LANGUAGE C VOLATILE;

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

CREATE FUNCTION resize(a matrix, i bigint default -1, j bigint default -1)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_resize'
LANGUAGE C STABLE;

CREATE FUNCTION info(a matrix, level int default 1)
RETURNS text
AS '$libdir/onesparse', 'matrix_info'
LANGUAGE C STABLE;

CREATE FUNCTION eadd_min(
    a matrix,
    b matrix,
    c matrix default null,
    mask matrix default null,
    accum binaryop default null,
    descr descriptor default null)
RETURNS matrix
RETURN onesparse.eadd(a, b, ('min_' || name(type(a)))::binaryop, c, mask, accum, descr);

CREATE FUNCTION eadd_min_op(a matrix, b matrix)
RETURNS matrix
RETURN onesparse.eadd_min(a, b);

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

CREATE FUNCTION tril(a matrix, s scalar default 0::bigint::scalar)
RETURNS matrix
RETURN onesparse.choose(a, 'tril', s);

CREATE FUNCTION triu(a matrix, s scalar default 0::bigint::scalar)
RETURNS matrix
RETURN onesparse.choose(a, 'triu', s);

CREATE FUNCTION one(a matrix)
RETURNS matrix
RETURN onesparse.apply(a, ('one_' || name(type(a)))::unaryop);

CREATE FUNCTION ainv(a matrix)
RETURNS matrix
RETURN onesparse.apply(a, ('ainv_' || name(type(a)))::unaryop);

CREATE FUNCTION minv(a matrix)
RETURNS matrix
RETURN onesparse.apply(a, ('minv_' || name(type(a)))::unaryop);

CREATE FUNCTION lnot(a matrix)
RETURNS matrix
RETURN onesparse.apply(a, ('lnot_' || name(type(a)))::unaryop);

CREATE FUNCTION bnot(a matrix)
RETURNS matrix
RETURN onesparse.apply(a, ('bnot_' || name(type(a)))::unaryop);

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

CREATE FUNCTION min_plus(
        a matrix,
        b matrix,
        c matrix default null,
        mask matrix default null,
        accum binaryop default null,
        descr descriptor default null)
RETURNS matrix
    RETURN mxm(a, b, ('min_plus_' || name(type(a)))::semiring, c, mask, accum, descr);

CREATE FUNCTION min_plus(
        a matrix,
        b vector,
        c vector default null,
        mask vector default null,
        accum binaryop default null,
        descr descriptor default null)
RETURNS vector
    RETURN mxv(a, b, ('min_plus_' || name(type(a)))::semiring, c, mask, accum, descr);

CREATE FUNCTION min_plus(
        a vector,
        b matrix,
        c vector default null,
        mask vector default null,
        accum binaryop default null,
        descr descriptor default null)
RETURNS vector
    RETURN vxm(a, b, ('min_plus_' || name(type(a)))::semiring, c, mask, accum, descr);

CREATE FUNCTION min_plus_min(
        a matrix,
        b matrix,
        c matrix default null,
        mask matrix default null,
        descr descriptor default null)
RETURNS matrix
    RETURN mxm(a, b, ('min_plus_' || name(type(a)))::semiring, c, mask,  ('min_' || name(type(a)))::binaryop, descr);

CREATE FUNCTION min_plus_min(
        a matrix,
        b vector,
        c vector default null,
        mask vector default null,
        descr descriptor default null)
RETURNS vector
    RETURN mxv(a, b, ('min_plus_' || name(type(a)))::semiring, c, mask,  ('min_' || name(type(a)))::binaryop, descr);

CREATE FUNCTION min_plus_min(
        a vector,
        b matrix,
        mask vector default null,
        descr descriptor default null)
RETURNS vector
    RETURN vxm(a, b, ('min_plus_' || name(type(a)))::semiring, a, mask, ('min_' || name(type(a)))::binaryop, descr);

CREATE FUNCTION min_plus_op(
        a matrix,
        b matrix)
RETURNS matrix
    RETURN min_plus(a, b);

CREATE OPERATOR @<+ (
    LEFTARG = matrix,
    RIGHTARG = matrix,
    FUNCTION = min_plus_op
    );

CREATE FUNCTION min_plus_min_op(
        a vector,
        b matrix)
RETURNS vector
    RETURN min_plus_min(a, b);

CREATE OPERATOR @<+< (
    LEFTARG = vector,
    RIGHTARG = matrix,
    FUNCTION = min_plus_min_op
    );

CREATE FUNCTION serialize(a matrix)
RETURNS bytea
AS '$libdir/onesparse', 'matrix_serialize'
LANGUAGE C STRICT;

CREATE FUNCTION serialize_file(a matrix, path text)
RETURNS void
AS '$libdir/onesparse', 'matrix_serialize_file'
LANGUAGE C STRICT;

CREATE FUNCTION deserialize(a bytea)
RETURNS matrix
AS '$libdir/onesparse', 'matrix_deserialize'
LANGUAGE C STRICT;

CREATE FUNCTION deserialize_file(path text)
RETURNS matrix
    RETURN deserialize(pg_read_binary_file(path));

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

create or replace function draw(
    a matrix,
    node_labels vector default null,
    weights bool default true,
    directed bool default true ) returns text language plpgsql as
    $$
    declare
        row bigint;
        col bigint;
        value scalar;
        result text;
        edge text;
    begin
        if directed then
            result = E'digraph {\n';
            edge = '->';
        else
            result = E'graph {\n';
            edge = '--';
        end if;
        if node_labels is not null then
            for row in select * from irows(a) loop
                value = get_element(node_labels, row);
                result = result || format(E'%s [label="%s"]\n', row, print(value));
            end loop;
        end if;
        for row, col, value in select * from elements(a) loop
            result = result || format(E'%s %s %s', row, edge, col);
            if weights then
                result = result || format(E' [label="%s"]\n', print(value));
            else
                result = result || E'\n';
            end if;
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

CREATE FUNCTION dense_matrix(
    nrows integer,
    ncols integer,
    fill bigint)
        RETURNS matrix
        RETURN assign(matrix('int64', nrows, ncols), fill);
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

CREATE FUNCTION dense_matrix(
    nrows integer,
    ncols integer,
    fill integer)
        RETURNS matrix
        RETURN assign(matrix('int32', nrows, ncols), fill);
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

CREATE FUNCTION dense_matrix(
    nrows integer,
    ncols integer,
    fill smallint)
        RETURNS matrix
        RETURN assign(matrix('int16', nrows, ncols), fill);
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

CREATE FUNCTION dense_matrix(
    nrows integer,
    ncols integer,
    fill float4)
        RETURNS matrix
        RETURN assign(matrix('fp32', nrows, ncols), fill);
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

CREATE FUNCTION dense_matrix(
    nrows integer,
    ncols integer,
    fill float8)
        RETURNS matrix
        RETURN assign(matrix('fp64', nrows, ncols), fill);
