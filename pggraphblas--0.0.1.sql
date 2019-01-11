-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pggraphblas" to load this file. \quit

CREATE TYPE matrix;

CREATE TYPE matrix_tuple AS (i bigint, j bigint, v bigint);

CREATE FUNCTION matrix_agg_acc(internal, bigint, bigint, bigint)
RETURNS internal
AS '$libdir/pggraphblas', 'matrix_agg_acc'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION matrix_final_int4(internal)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_final_int4'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION matrix_tuples(matrix)
RETURNS SETOF matrix_tuple
AS '$libdir/pggraphblas', 'matrix_tuples'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION matrix_in(cstring)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION matrix_out(matrix)
RETURNS cstring
AS '$libdir/pggraphblas', 'matrix_out'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION matrix_nrows(matrix)
RETURNS bigint
AS '$libdir/pggraphblas', 'matrix_nrows'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION matrix_ncols(matrix)
RETURNS bigint
AS '$libdir/pggraphblas', 'matrix_ncols'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION matrix_nvals(matrix)
RETURNS bigint
AS '$libdir/pggraphblas', 'matrix_nvals'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION matrix_x_matrix(matrix, matrix)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_x_matrix'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION matrix_ewise_mult(matrix, matrix)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_ewise_mult'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION matrix_ewise_add(matrix, matrix)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_ewise_add'
LANGUAGE C STABLE STRICT;
    
CREATE TYPE matrix (
    internallength = 8,
    input = matrix_in,
    output = matrix_out
);

CREATE AGGREGATE matrix_agg (bigint, bigint, bigint) (
    sfunc = matrix_agg_acc,
    stype = internal,
    finalfunc = matrix_final_int4
);

CREATE OPERATOR * (
    leftarg = matrix,
    rightarg = matrix,
    procedure = matrix_x_matrix
);

CREATE OPERATOR && (
    leftarg = matrix,
    rightarg = matrix,
    procedure = matrix_ewise_mult
);

CREATE OPERATOR || (
    leftarg = matrix,
    rightarg = matrix,
    procedure = matrix_ewise_add
);
    
CREATE TYPE vector;

CREATE TYPE vector_tuple AS (i bigint, j bigint);

CREATE FUNCTION vector_agg_acc(internal, bigint, bigint)
RETURNS internal
AS '$libdir/pggraphblas', 'vector_agg_acc'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION vector_final_int4(internal)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_final_int4'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION vector_tuples(vector)
RETURNS SETOF vector_tuple
AS '$libdir/pggraphblas', 'vector_tuples'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION vector_in(cstring)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vector_out(vector)
RETURNS cstring
AS '$libdir/pggraphblas', 'vector_out'
LANGUAGE C IMMUTABLE STRICT;
    
CREATE FUNCTION vector_ewise_mult(vector, vector)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_ewise_mult'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION vector_ewise_add(vector, vector)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_ewise_add'
LANGUAGE C STABLE STRICT;

CREATE TYPE vector (
    internallength = 8,
    input = vector_in,
    output = vector_out
);

CREATE AGGREGATE vector_agg (bigint, bigint) (
    sfunc = vector_agg_acc,
    stype = internal,
    finalfunc = vector_final_int4
);

CREATE OPERATOR && (
    leftarg = vector,
    rightarg = vector,
    procedure = vector_ewise_mult
);

CREATE OPERATOR || (
    leftarg = vector,
    rightarg = vector,
    procedure = vector_ewise_add
);
    
