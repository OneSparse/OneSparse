-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pggraphblas" to load this file. \quit

CREATE TYPE matrix;

CREATE FUNCTION matrix_agg_acc(internal, bigint[])
RETURNS internal
AS '$libdir/pggraphblas', 'matrix_agg_acc'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION matrix_final_int4(internal)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_final_int4'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION matrix_in(cstring)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION matrix_out(matrix)
RETURNS cstring
AS '$libdir/pggraphblas', 'matrix_out'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE matrix (
    internallength = 8,
    input = matrix_in,
    output = matrix_out
);

CREATE AGGREGATE matrix_agg (bigint[]) (
    sfunc = matrix_agg_acc,
    stype = internal,
    finalfunc = matrix_final_int4
);
