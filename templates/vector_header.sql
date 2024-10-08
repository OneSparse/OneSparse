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
    op binaryop,
    inout w vector default null,
    mask vector default null,
    accum binaryop default null,
    descriptor descriptor default null
    )
RETURNS vector
AS '$libdir/onesparse', 'vector_ewise_add'
LANGUAGE C STABLE;

CREATE FUNCTION emult(
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

CREATE FUNCTION eunion(
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

CREATE FUNCTION selection(
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
RETURNS void
AS '$libdir/onesparse', 'vector_clear'
LANGUAGE C;

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
