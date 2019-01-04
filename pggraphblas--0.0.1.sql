-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pggraphblas" to load this file. \quit

CREATE AGGREGATE matrix_agg (anyelement)
(
    sfunc = array_append,
    stype = anyarray,
    initcond = '{}'
);

CREATE TYPE matrix;

CREATE FUNCTION matrix_in(cstring)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION matrix_out(matrix)
RETURNS cstring
AS '$libdir/pggraphblas', 'matrix_out'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE matrix (
   input = matrix_in,
   output = matrix_out
);
