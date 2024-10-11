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

CREATE FUNCTION selection(
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

CREATE FUNCTION eadd_op(a matrix, b matrix)
RETURNS matrix
RETURN onesparse.eadd(a, b);

CREATE FUNCTION emult_op(a matrix, b matrix)
RETURNS matrix
RETURN onesparse.emult(a, b);

CREATE FUNCTION mxm_op(a matrix, b matrix)
RETURNS matrix
RETURN onesparse.mxm(a, b);

CREATE FUNCTION mxv_op(a matrix, b vector)
RETURNS vector
RETURN onesparse.mxv(a, b);

CREATE FUNCTION vxm_op(a vector, b matrix)
RETURNS vector
RETURN onesparse.vxm(a, b);

CREATE OPERATOR + (
    LEFTARG = matrix,
    RIGHTARG = matrix,
    FUNCTION = eadd_op
    );

CREATE OPERATOR * (
    LEFTARG = matrix,
    RIGHTARG = matrix,
    FUNCTION = emult_op
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
        result text = E'digraph {{\n';
    begin
        for row, col, value in select * from elements(a) loop
            result = result || format(E'%s -> %s [label="%s"]\n', row, col, print(value));
        end loop;
        result = result || E'}}\n';
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
