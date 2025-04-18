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
RETURNS bigint
AS '$libdir/onesparse', 'vector_size'
LANGUAGE C;

CREATE FUNCTION vector_eadd_support(internal)
RETURNS internal
AS '$libdir/onesparse', 'vector_eadd_support'
LANGUAGE C;

CREATE FUNCTION eadd(
    a vector,
    b vector,
    op binaryop default null,
    c vector default null,
    mask vector default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_eadd'
LANGUAGE C SUPPORT vector_eadd_support;

CREATE FUNCTION vector_emult_support(internal)
RETURNS internal
AS '$libdir/onesparse', 'vector_emult_support'
LANGUAGE C;

CREATE FUNCTION emult(
    a vector,
    b vector,
    op binaryop default null,
    c vector default null,
    mask vector default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_emult'
LANGUAGE C SUPPORT vector_emult_support;

CREATE FUNCTION vector_eunion_support(internal)
RETURNS internal
AS '$libdir/onesparse', 'vector_eunion_support'
LANGUAGE C;

CREATE FUNCTION eunion(
    a vector,
    alpha scalar,
    b vector,
    beta scalar,
    op binaryop default null,
    c vector default null,
    mask vector default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_eunion'
LANGUAGE C SUPPORT vector_eunion_support;

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

CREATE FUNCTION vector_select_support(internal)
RETURNS internal
AS '$libdir/onesparse', 'vector_select_support'
LANGUAGE C;

CREATE FUNCTION choose(
    a vector,
    op indexunaryop,
    y scalar,
    c vector default null,
    mask vector default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_select'
LANGUAGE C SUPPORT vector_select_support;

CREATE FUNCTION vector_apply_support(internal)
RETURNS internal
AS '$libdir/onesparse', 'vector_apply_support'
LANGUAGE C;

CREATE FUNCTION apply(
    a vector,
    op unaryop,
    c vector default null,
    mask vector default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_apply'
LANGUAGE C SUPPORT vector_apply_support;

CREATE FUNCTION vector_apply_first_support(internal)
RETURNS internal
AS '$libdir/onesparse', 'vector_apply_first_support'
LANGUAGE C;

CREATE FUNCTION apply(
    s scalar,
    a vector,
    op binaryop default null,
    c vector default null,
    mask vector default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_apply_first'
LANGUAGE C SUPPORT vector_apply_first_support;

CREATE FUNCTION vector_apply_second_support(internal)
RETURNS internal
AS '$libdir/onesparse', 'vector_apply_second_support'
LANGUAGE C;

CREATE FUNCTION apply(
    a vector,
    s scalar,
    op binaryop default null,
    c vector default null,
    mask vector default null,
    accum binaryop default null,
    descr descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_apply_second'
LANGUAGE C SUPPORT vector_apply_second_support;

CREATE FUNCTION vector_assign_support(internal)
RETURNS internal
AS '$libdir/onesparse', 'vector_assign_support'
LANGUAGE C;

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
LANGUAGE C SUPPORT vector_assign_support;

CREATE FUNCTION vector_assign_scalar_support(internal)
RETURNS internal
AS '$libdir/onesparse', 'vector_assign_scalar_support'
LANGUAGE C;

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
LANGUAGE C SUPPORT vector_assign_scalar_support;

CREATE FUNCTION vector_extract_vector_support(internal)
RETURNS internal
AS '$libdir/onesparse', 'vector_extract_vector_support'
LANGUAGE C;

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
LANGUAGE C SUPPORT vector_extract_vector_support;

CREATE FUNCTION cast_to(a vector, t type)
RETURNS vector
    RETURN apply(a, ('identity_' || name(t))::unaryop, c=>vector(t, size(a)));

CREATE FUNCTION vector_set_element_support(internal)
RETURNS internal
AS '$libdir/onesparse', 'vector_set_element_support'
LANGUAGE C;

CREATE FUNCTION set_element(a vector, i bigint, s scalar)
RETURNS vector
AS '$libdir/onesparse', 'vector_set_element'
LANGUAGE C SUPPORT vector_set_element_support;

CREATE FUNCTION get_element(a vector, i bigint)
RETURNS scalar
AS '$libdir/onesparse', 'vector_get_element'
LANGUAGE C STABLE;

CREATE FUNCTION vector_remove_element_support(internal)
RETURNS internal
AS '$libdir/onesparse', 'vector_remove_element_support'
LANGUAGE C;

CREATE FUNCTION remove_element(a vector, i bigint)
RETURNS vector
AS '$libdir/onesparse', 'vector_remove_element'
LANGUAGE C SUPPORT vector_remove_element_support;

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

CREATE FUNCTION vector_wait_support(internal)
RETURNS internal
AS '$libdir/onesparse', 'vector_wait_support'
LANGUAGE C;

CREATE FUNCTION wait(vector, waitmode integer default 0)
RETURNS vector
AS '$libdir/onesparse', 'vector_wait'
LANGUAGE C SUPPORT vector_wait_support;

CREATE FUNCTION dup(vector)
RETURNS vector
AS '$libdir/onesparse', 'vector_dup'
LANGUAGE C;

CREATE FUNCTION vector_clear_support(internal)
RETURNS internal
AS '$libdir/onesparse', 'vector_clear_support'
LANGUAGE C;

CREATE FUNCTION clear(vector)
RETURNS vector
AS '$libdir/onesparse', 'vector_clear'
LANGUAGE C SUPPORT vector_clear_support;

CREATE FUNCTION vector_resize_support(internal)
RETURNS internal
AS '$libdir/onesparse', 'vector_resize_support'
LANGUAGE C;

CREATE FUNCTION resize(a vector, i bigint default -1)
RETURNS vector
AS '$libdir/onesparse', 'vector_resize'
LANGUAGE C SUPPORT vector_resize_support;

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
        result text = E'digraph vector {{\ngraph [rankdir=TB]; node [shape=box];\nsubgraph cluster_vector {{\nstyle=dashed; color=black; bgcolor=lightgray;\n';
    begin
        for i in 0..imax loop
            result = result || 'node' || i::text;
            if contains(a, i) then
                result = result || ' [label="' || i::text || ':' || print(get_element(a, i)) || E'"];\n';
            else
                result = result || E' [label=""];\n';
            end if;
        end loop;
        result = result || E'}}\n';
        for i in 0..imax loop
            if i != imax then
                result = result || 'node' || i::text || ' -> ';
            else
                result = result || 'node' || i::text ||  E' [style=invis];\n';
            end if;
        end loop;
        result = result || E'}}\n';
        return result;
    end;
    $$;
