-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pggraphblas" to load this file. \quit

CREATE TYPE vector;

CREATE FUNCTION vector_in(cstring)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vector_out(vector)
RETURNS cstring
AS '$libdir/pggraphblas', 'vector_out'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE vector (
    input = vector_in,
    output = vector_out,
    alignment = int4
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

CREATE FUNCTION matrix_eq(matrix, matrix)
RETURNS bool
AS '$libdir/pggraphblas', 'matrix_eq'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION matrix_neq(matrix, matrix)
RETURNS bool
AS '$libdir/pggraphblas', 'matrix_neq'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION mxm(matrix, matrix)
RETURNS matrix
AS '$libdir/pggraphblas', 'mxm'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION mxv(matrix, vector)
RETURNS vector
AS '$libdir/pggraphblas', 'mxv'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION vxm(vector, matrix)
RETURNS vector
AS '$libdir/pggraphblas', 'vxm'
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
    input = matrix_in,
    output = matrix_out,
    alignment = int4
);

CREATE OPERATOR = (
    leftarg = matrix,
    rightarg = matrix,
    procedure = matrix_eq,
    negator = <>
);

CREATE OPERATOR <> (
    leftarg = matrix,
    rightarg = matrix,
    procedure = matrix_neq,
    negator = =
);

CREATE OPERATOR * (
    leftarg = matrix,
    rightarg = matrix,
    procedure = matrix_mxm
);

CREATE OPERATOR * (
    leftarg = vector,
    rightarg = matrix,
    procedure = matrix_vxm
);

CREATE OPERATOR * (
    leftarg = matrix,
    rightarg = vector,
    procedure = matrix_mxv
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

-- aggregators
    
CREATE FUNCTION matrix_agg_acc_bigint(internal, bigint, bigint, bigint)
RETURNS internal
AS '$libdir/pggraphblas', 'matrix_agg_acc_int64'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION matrix_final_bigint(internal)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_final_int64'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION matrix_agg_acc_integer(internal, bigint, bigint, integer)
RETURNS internal
AS '$libdir/pggraphblas', 'matrix_agg_acc_int32'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION matrix_final_integer(internal)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_final_int32'
LANGUAGE C CALLED ON NULL INPUT;
    
CREATE FUNCTION matrix_agg_acc_smallint(internal, bigint, bigint, smallint)
RETURNS internal
AS '$libdir/pggraphblas', 'matrix_agg_acc_int16'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION matrix_final_smallint(internal)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_final_int16'
LANGUAGE C CALLED ON NULL INPUT;
    
CREATE FUNCTION matrix_agg_acc_float(internal, bigint, bigint, float)
RETURNS internal
AS '$libdir/pggraphblas', 'matrix_agg_acc_float8'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION matrix_final_float(internal)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_final_float8'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION matrix_agg_acc_real(internal, bigint, bigint, real)
RETURNS internal
AS '$libdir/pggraphblas', 'matrix_agg_acc_float4'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION matrix_final_real(internal)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_final_float4'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION matrix_agg_acc_bool(internal, bigint, bigint, bool)
RETURNS internal
AS '$libdir/pggraphblas', 'matrix_agg_acc_bool'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION matrix_final_bool(internal)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_final_bool'
LANGUAGE C CALLED ON NULL INPUT;
    
CREATE AGGREGATE matrix(bigint, bigint, bigint) (
    stype = internal,
    sfunc = matrix_agg_acc_bigint,
    finalfunc = matrix_final_bigint
);

CREATE AGGREGATE matrix(bigint, bigint, integer) (
    stype = internal,
    sfunc = matrix_agg_acc_integer,
    finalfunc = matrix_final_integer
);
    
CREATE AGGREGATE matrix(bigint, bigint, smallint) (
    stype = internal,
    sfunc = matrix_agg_acc_smallint,
    finalfunc = matrix_final_smallint
);
    
CREATE AGGREGATE matrix(bigint, bigint, float) (
    stype = internal,
    sfunc = matrix_agg_acc_float,
    finalfunc = matrix_final_float
);

CREATE AGGREGATE matrix(bigint, bigint, real) (
    stype = internal,
    sfunc = matrix_agg_acc_real,
    finalfunc = matrix_final_real
);
    
CREATE AGGREGATE matrix(bigint, bigint, bool) (
    stype = internal,
    sfunc = matrix_agg_acc_bool,
    finalfunc = matrix_final_bool
);
    
CREATE TYPE matrix_element_bigint AS (index bigint, value bigint);
CREATE TYPE matrix_element_integer AS (index bigint, value integer);
CREATE TYPE matrix_element_smallint AS (index bigint, value smallint);
CREATE TYPE matrix_element_float AS (index bigint, value float);
CREATE TYPE matrix_element_real AS (index bigint, value real);
CREATE TYPE matrix_element_bool AS (index bigint, value bool);

CREATE FUNCTION matrix_elements_bigint(matrix)
RETURNS SETOF matrix_element_bigint
AS '$libdir/pggraphblas', 'matrix_elements_int64'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION matrix_elements_integer(matrix)
RETURNS SETOF matrix_element_integer
AS '$libdir/pggraphblas', 'matrix_elements_int32'
LANGUAGE C STABLE STRICT;
    
CREATE FUNCTION matrix_elements_smallint(matrix)
RETURNS SETOF matrix_element_smallint
AS '$libdir/pggraphblas', 'matrix_elements_int16'
LANGUAGE C STABLE STRICT;
    
CREATE FUNCTION matrix_elements_float(matrix)
RETURNS SETOF matrix_element_float
AS '$libdir/pggraphblas', 'matrix_elements_float8'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION matrix_elements_real(matrix)
RETURNS SETOF matrix_element_real
AS '$libdir/pggraphblas', 'matrix_elements_float4'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION matrix_elements_bool(matrix)
RETURNS SETOF matrix_element_bool
AS '$libdir/pggraphblas', 'matrix_elements_bool'
LANGUAGE C STABLE STRICT;

-- vectors

CREATE FUNCTION vector_ewise_mult(vector, vector)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_ewise_mult'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION vector_ewise_add(vector, vector)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_ewise_add'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION vector_eq(vector, vector)
RETURNS bool
AS '$libdir/pggraphblas', 'vector_eq'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION vector_neq(vector, vector)
RETURNS bool
AS '$libdir/pggraphblas', 'vector_neq'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION vector_size(vector)
RETURNS bigint
AS '$libdir/pggraphblas', 'vector_size'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION vector_nvals(vector)
RETURNS bigint
AS '$libdir/pggraphblas', 'vector_nvals'
LANGUAGE C STABLE STRICT;

CREATE OPERATOR * (
    leftarg = vector,
    rightarg = vector,
    procedure = vector_ewise_mult
);

CREATE OPERATOR + (
    leftarg = vector,
    rightarg = vector,
    procedure = vector_ewise_add
);

CREATE OPERATOR = (
    leftarg = vector,
    rightarg = vector,
    procedure = vector_eq,
    negator = <>
);

CREATE OPERATOR <> (
    leftarg = vector,
    rightarg = vector,
    procedure = vector_neq,
    negator = =
);

-- casting arrays to vectors
    
CREATE FUNCTION vector(bigint[])
RETURNS vector
AS '$libdir/pggraphblas', 'vector_int64'
LANGUAGE C STRICT;

CREATE CAST (bigint[] AS vector) WITH FUNCTION vector(bigint[]) AS IMPLICIT;

CREATE FUNCTION vector(integer[])
RETURNS vector
AS '$libdir/pggraphblas', 'vector_int32'
LANGUAGE C STRICT;

CREATE CAST (integer[] AS vector) WITH FUNCTION vector(integer[]) AS IMPLICIT;
    
CREATE FUNCTION vector(smallint[])
RETURNS vector
AS '$libdir/pggraphblas', 'vector_int16'
LANGUAGE C STRICT;

CREATE CAST (smallint[] AS vector) WITH FUNCTION vector(smallint[]) AS IMPLICIT;
    
CREATE FUNCTION vector(float[])
RETURNS vector
AS '$libdir/pggraphblas', 'vector_float8'
LANGUAGE C STRICT;

CREATE CAST (float[] AS vector) WITH FUNCTION vector(float[]) AS IMPLICIT;
    
CREATE FUNCTION vector(real[])
RETURNS vector
AS '$libdir/pggraphblas', 'vector_float4'
LANGUAGE C STRICT;

CREATE CAST (real[] AS vector) WITH FUNCTION vector(real[]) AS IMPLICIT;
    
CREATE FUNCTION vector(bool[])
RETURNS vector
AS '$libdir/pggraphblas', 'vector_bool'
LANGUAGE C STRICT;

CREATE CAST (bool[] AS vector) WITH FUNCTION vector(bool[]) AS IMPLICIT;

-- aggregators
    
CREATE FUNCTION vector_agg_acc_bigint(internal, bigint, bigint)
RETURNS internal
AS '$libdir/pggraphblas', 'vector_agg_acc_int64'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION vector_final_bigint(internal)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_final_int64'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION vector_agg_acc_integer(internal, bigint, integer)
RETURNS internal
AS '$libdir/pggraphblas', 'vector_agg_acc_int32'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION vector_final_integer(internal)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_final_int32'
LANGUAGE C CALLED ON NULL INPUT;
    
CREATE FUNCTION vector_agg_acc_smallint(internal, bigint, smallint)
RETURNS internal
AS '$libdir/pggraphblas', 'vector_agg_acc_int16'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION vector_final_smallint(internal)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_final_int16'
LANGUAGE C CALLED ON NULL INPUT;
    
CREATE FUNCTION vector_agg_acc_float(internal, bigint, float)
RETURNS internal
AS '$libdir/pggraphblas', 'vector_agg_acc_float8'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION vector_final_float(internal)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_final_float8'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION vector_agg_acc_real(internal, bigint, real)
RETURNS internal
AS '$libdir/pggraphblas', 'vector_agg_acc_float4'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION vector_final_real(internal)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_final_float4'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION vector_agg_acc_bool(internal, bigint, bool)
RETURNS internal
AS '$libdir/pggraphblas', 'vector_agg_acc_bool'
LANGUAGE C CALLED ON NULL INPUT;

CREATE FUNCTION vector_final_bool(internal)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_final_bool'
LANGUAGE C CALLED ON NULL INPUT;
    
CREATE AGGREGATE vector(bigint, bigint) (
    stype = internal,
    sfunc = vector_agg_acc_bigint,
    finalfunc = vector_final_bigint
);

CREATE AGGREGATE vector(bigint, integer) (
    stype = internal,
    sfunc = vector_agg_acc_integer,
    finalfunc = vector_final_integer
);
    
CREATE AGGREGATE vector(bigint, smallint) (
    stype = internal,
    sfunc = vector_agg_acc_smallint,
    finalfunc = vector_final_smallint
);
    
CREATE AGGREGATE vector(bigint, float) (
    stype = internal,
    sfunc = vector_agg_acc_float,
    finalfunc = vector_final_float
);

CREATE AGGREGATE vector(bigint, real) (
    stype = internal,
    sfunc = vector_agg_acc_real,
    finalfunc = vector_final_real
);

CREATE AGGREGATE vector(bigint, bool) (
    stype = internal,
    sfunc = vector_agg_acc_bool,
    finalfunc = vector_final_bool
);

-- tuple formats for vector edges

CREATE TYPE vector_element_bigint AS (index bigint, value bigint);
CREATE TYPE vector_element_integer AS (index bigint, value integer);
CREATE TYPE vector_element_smallint AS (index bigint, value smallint);
CREATE TYPE vector_element_float AS (index bigint, value float);
CREATE TYPE vector_element_real AS (index bigint, value real);
CREATE TYPE vector_element_bool AS (index bigint, value bool);
    
CREATE FUNCTION vector_elements_bigint(vector)
RETURNS SETOF vector_element_bigint
AS '$libdir/pggraphblas', 'vector_elements_int64'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION vector_elements_integer(vector)
RETURNS SETOF vector_element_integer
AS '$libdir/pggraphblas', 'vector_elements_int32'
LANGUAGE C STABLE STRICT;
    
CREATE FUNCTION vector_elements_smallint(vector)
RETURNS SETOF vector_element_smallint
AS '$libdir/pggraphblas', 'vector_elements_int16'
LANGUAGE C STABLE STRICT;
    
CREATE FUNCTION vector_elements_float(vector)
RETURNS SETOF vector_element_float
AS '$libdir/pggraphblas', 'vector_elements_float8'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION vector_elements_real(vector)
RETURNS SETOF vector_element_real
AS '$libdir/pggraphblas', 'vector_elements_float4'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION vector_elements_bool(vector)
RETURNS SETOF vector_element_bool
AS '$libdir/pggraphblas', 'vector_elements_bool'
LANGUAGE C STABLE STRICT;

