-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pggraphblas" to load this file. \quit

CREATE TYPE vector;

CREATE FUNCTION vector_in(cstring)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vector_out(A vector)
RETURNS cstring
AS '$libdir/pggraphblas', 'vector_out'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE vector (
    input = vector_in,
    output = vector_out,
    alignment = int4,
    storage = 'extended',
    internallength = -1
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

CREATE FUNCTION nrows(matrix)
RETURNS bigint
AS '$libdir/pggraphblas', 'matrix_nrows'
LANGUAGE C STABLE;

CREATE FUNCTION ncols(matrix)
RETURNS bigint
AS '$libdir/pggraphblas', 'matrix_ncols'
LANGUAGE C STABLE;

CREATE FUNCTION nvals(matrix)
RETURNS bigint
AS '$libdir/pggraphblas', 'matrix_nvals'
LANGUAGE C STABLE;

CREATE FUNCTION matrix_eq(A matrix, B matrix)
RETURNS bool
AS '$libdir/pggraphblas', 'matrix_eq'
LANGUAGE C STABLE;

CREATE FUNCTION matrix_ne(A matrix, B matrix)
RETURNS bool
AS '$libdir/pggraphblas', 'matrix_ne'
LANGUAGE C STABLE;

CREATE FUNCTION print(
    A matrix,
    level integer default 2)
RETURNS text
AS '$libdir/pggraphblas', 'matrix_print'
LANGUAGE C STABLE;
    
CREATE FUNCTION print(
    A vector,
    level integer default 2)
RETURNS text
AS '$libdir/pggraphblas', 'vector_print'
LANGUAGE C STABLE;
    
CREATE TYPE matrix (
    input = matrix_in,
    output = matrix_out,
    alignment = int4,
    storage = 'extended',
    internallength = -1
);

CREATE FUNCTION matrix(bigint[], bigint[], bool[],
    bigint default null, bigint default null)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_bool'
LANGUAGE C STABLE;

CREATE FUNCTION matrix(bigint[], bigint[], bigint[],
    bigint default null, bigint default null)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_int64'
LANGUAGE C STABLE;

CREATE FUNCTION matrix(bigint[], bigint[], integer[],
    bigint default null, bigint default null)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_int32'
LANGUAGE C STABLE;

CREATE FUNCTION matrix(bigint[], bigint[], smallint[],
    bigint default null, bigint default null)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_int16'
LANGUAGE C STABLE;

CREATE FUNCTION matrix(bigint[], bigint[], real[],
    bigint default null, bigint default null)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_float4'
LANGUAGE C STABLE;

CREATE FUNCTION matrix(bigint[], bigint[], float[],
    bigint default null, bigint default null)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_float8'
LANGUAGE C STABLE;

-- matrix extract

CREATE FUNCTION xtract(
    A matrix,
    B matrix,
    C matrix default null,
    doutp text default null,
    dmask text default null,
    dinp0 text default null,
    dinp1 text default null
    )
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_xtract'
LANGUAGE C STABLE;
    
-- empty matrix construction

CREATE FUNCTION matrix_bool(nrows bigint, ncols bigint)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_bool'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION matrix_bigint(nrows bigint, ncols bigint)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_int64'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION matrix_integer(nrows bigint, ncols bigint)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_int32'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION matrix_smallint(nrows bigint, ncols bigint)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_int16'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION matrix_real(nrows bigint, ncols bigint)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_float4'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION matrix_float(nrows bigint, ncols bigint)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_float8'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION mxm(
    A matrix,
    B matrix,
    inout C matrix default null,
    mask matrix default null,
    semiring text default null,
    accum text default null,
    doutp text default null,
    dmask text default null,
    dinp0 text default null,
    dinp1 text default null
    )
RETURNS matrix
AS '$libdir/pggraphblas', 'mxm'
LANGUAGE C STABLE;

CREATE FUNCTION kron(
    A matrix,
    B matrix,
    inout C matrix default null,
    mask matrix default null,
    accum text default null,
    mulop text default null,
    doutp text default null,
    dmask text default null,
    dinp0 text default null,
    dinp1 text default null
    )
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_kron'
LANGUAGE C STABLE;

CREATE FUNCTION transpose(
    A matrix,
    inout C matrix default null,
    mask matrix default null,
    accum text default null,
    doutp text default null,
    dmask text default null,
    dinp0 text default null,
    dinp1 text default null
    )
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_transpose'
LANGUAGE C STABLE;

CREATE FUNCTION mxm_op(A matrix, B matrix)
RETURNS matrix
AS '$libdir/pggraphblas', 'mxm'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION mxv(
    A matrix,
    B vector,
    inout C vector default null,
    mask vector default null,
    semiring text default null,
    accum text default null,
    doutp text default null,
    dmask text default null,
    dinp0 text default null,
    dinp1 text default null
    )
RETURNS vector
AS '$libdir/pggraphblas', 'mxv'
LANGUAGE C STABLE;

CREATE FUNCTION mxv_op(matrix, vector)
RETURNS vector
AS '$libdir/pggraphblas', 'mxv'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION vxm(
    A vector,
    B matrix,
    inout C vector default null,
    mask vector default null,
    semiring text default null,
    accum text default null,
    doutp text default null,
    dmask text default null,
    dinp0 text default null,
    dinp1 text default null
    )
RETURNS vector
AS '$libdir/pggraphblas', 'vxm'
LANGUAGE C STABLE;

CREATE FUNCTION vxm_op(vector, matrix)
RETURNS vector
AS '$libdir/pggraphblas', 'vxm'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION ewise_mult(
    A matrix,
    B matrix,
    inout C matrix default null,
    mask matrix default null,
    op text default null,
    accum text default null,
    doutp text default null,
    dmask text default null,
    dinp0 text default null,
    dinp1 text default null
    )

RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_ewise_mult'
LANGUAGE C STABLE;

CREATE FUNCTION ewise_add(
    A matrix,
    B matrix,
    inout C matrix default null,
    mask matrix default null,
    op text default null,
    accum text default null,
    doutp text default null,
    dmask text default null,
    dinp0 text default null,
    dinp1 text default null
    )

RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_ewise_add'
LANGUAGE C STABLE;

CREATE FUNCTION matrix_ewise_mult_op(A matrix, B matrix)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_ewise_mult'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION matrix_ewise_add_op(A matrix, B matrix)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_ewise_add'
LANGUAGE C STABLE STRICT;

-- matrix reduce to vector

CREATE FUNCTION reduce(A matrix, semiring text default null)
RETURNS vector
AS '$libdir/pggraphblas', 'matrix_reduce_vector'
LANGUAGE C STABLE;

-- matrix reduce scalar

CREATE FUNCTION reduce_bool(A matrix, semiring text default null)
RETURNS bool
AS '$libdir/pggraphblas', 'matrix_reduce_bool'
LANGUAGE C STABLE;

CREATE FUNCTION reduce_bigint(A matrix, semiring text default null)
RETURNS bigint
AS '$libdir/pggraphblas', 'matrix_reduce_int64'
LANGUAGE C STABLE;

CREATE FUNCTION reduce_integer(A matrix, semiring text default null)
RETURNS integer
AS '$libdir/pggraphblas', 'matrix_reduce_int32'
LANGUAGE C STABLE;

CREATE FUNCTION reduce_smallint(A matrix, semiring text default null)
RETURNS smallint
AS '$libdir/pggraphblas', 'matrix_reduce_int16'
LANGUAGE C STABLE;

CREATE FUNCTION reduce_real(A matrix, semiring text default null)
RETURNS real
AS '$libdir/pggraphblas', 'matrix_reduce_float4'
LANGUAGE C STABLE;

CREATE FUNCTION reduce_float(A matrix, semiring text default null)
RETURNS float
AS '$libdir/pggraphblas', 'matrix_reduce_float8'
LANGUAGE C STABLE;

-- matrix assign matrix

CREATE FUNCTION assign(
    A matrix,
    B matrix,
    mask matrix default null)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_assign_matrix'
LANGUAGE C STABLE;

-- matrix assign scalar

CREATE FUNCTION assign(
    A matrix,
    value bool,
    C matrix default null,
    mask matrix default null)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_assign_bool'
LANGUAGE C STABLE;

CREATE FUNCTION assign(
    A matrix,
    value bigint,
    C matrix default null,
    mask matrix default null)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_assign_int64'
LANGUAGE C STABLE;

CREATE FUNCTION assign(
    A matrix,
    value integer,
    C matrix default null,
    mask matrix default null)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_assign_int32'
LANGUAGE C STABLE;

CREATE FUNCTION assign(
    A matrix,
    value smallint,
    C matrix default null,
    mask matrix default null)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_assign_int16'
LANGUAGE C STABLE;

CREATE FUNCTION assign(
    A matrix,
    value real,
    C matrix default null,
    mask matrix default null)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_assign_float4'
LANGUAGE C STABLE;

CREATE FUNCTION assign(
    A matrix,
    value float,
    C matrix default null,
    mask matrix default null)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_assign_float8'
LANGUAGE C STABLE;

-- matrix set element

CREATE FUNCTION set_element(A matrix, i bigint, j bigint, value bool)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_set_element_bool'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION set_element(A matrix, i bigint, j bigint, value bigint)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_set_element_int64'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION set_element(A matrix, i bigint, j bigint, value integer)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_set_element_int32'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION set_element(A matrix, i bigint, j bigint, value smallint)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_set_element_int16'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION set_element(A matrix, i bigint, j bigint, value real)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_set_element_float4'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION set_element(A matrix, i bigint, j bigint, value float)
RETURNS matrix
AS '$libdir/pggraphblas', 'matrix_set_element_float8'
LANGUAGE C STABLE STRICT;



-- matrix operators

CREATE OPERATOR = (
    leftarg = matrix,
    rightarg = matrix,
    procedure = matrix_eq,
    negator = <>
);

CREATE OPERATOR <> (
    leftarg = matrix,
    rightarg = matrix,
    procedure = matrix_ne,
    negator = =
);

CREATE OPERATOR * (
    leftarg = matrix,
    rightarg = matrix,
    procedure = mxm_op
);

CREATE OPERATOR * (
    leftarg = vector,
    rightarg = matrix,
    procedure = vxm_op
);

CREATE OPERATOR * (
    leftarg = matrix,
    rightarg = vector,
    procedure = mxv_op
);

CREATE OPERATOR && (
    leftarg = matrix,
    rightarg = matrix,
    procedure = matrix_ewise_mult_op
);

CREATE OPERATOR || (
    leftarg = matrix,
    rightarg = matrix,
    procedure = matrix_ewise_add_op
);

CREATE TYPE matrix_element_bigint AS (i bigint, j bigint, value bigint);
CREATE TYPE matrix_element_integer AS (i bigint, j bigint, value integer);
CREATE TYPE matrix_element_smallint AS (i bigint, j bigint, value smallint);
CREATE TYPE matrix_element_float AS (i bigint, j bigint, value float);
CREATE TYPE matrix_element_real AS (i bigint, j bigint, value real);
CREATE TYPE matrix_element_bool AS (i bigint, j bigint, value bool);

CREATE FUNCTION elements_bigint(A matrix)
RETURNS SETOF matrix_element_bigint
AS '$libdir/pggraphblas', 'matrix_elements_int64'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION elements_integer(A matrix)
RETURNS SETOF matrix_element_integer
AS '$libdir/pggraphblas', 'matrix_elements_int32'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION elements_smallint(A matrix)
RETURNS SETOF matrix_element_smallint
AS '$libdir/pggraphblas', 'matrix_elements_int16'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION elements_float(A matrix)
RETURNS SETOF matrix_element_float
AS '$libdir/pggraphblas', 'matrix_elements_float8'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION elements_real(A matrix)
RETURNS SETOF matrix_element_real
AS '$libdir/pggraphblas', 'matrix_elements_float4'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION elements_bool(A matrix)
RETURNS SETOF matrix_element_bool
AS '$libdir/pggraphblas', 'matrix_elements_bool'
LANGUAGE C STABLE STRICT;

-- vectors

CREATE FUNCTION ewise_mult(
    A vector,
    B vector,
    inout C vector default null,
    mask vector default null,
    op text default null,
    accum text default null,
    doutp text default null,
    dmask text default null,
    dinp0 text default null,
    dinp1 text default null
    )
RETURNS vector
AS '$libdir/pggraphblas', 'vector_ewise_mult'
LANGUAGE C STABLE;

CREATE FUNCTION ewise_add(
    A vector,
    B vector,
    inout C vector default null,
    mask vector default null,
    op text default null,
    accum text default null,
    doutp text default null,
    dmask text default null,
    dinp0 text default null,
    dinp1 text default null
    )
RETURNS vector
AS '$libdir/pggraphblas', 'vector_ewise_add'
LANGUAGE C STABLE;

CREATE FUNCTION vector_ewise_mult_op(A vector, B vector)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_ewise_mult'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION vector_ewise_add_op(A vector, B vector)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_ewise_add'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION vector_eq(A vector, B vector)
RETURNS bool
AS '$libdir/pggraphblas', 'vector_eq'
LANGUAGE C STABLE;

CREATE FUNCTION vector_ne(A vector, B vector)
RETURNS bool
AS '$libdir/pggraphblas', 'vector_ne'
LANGUAGE C STABLE;

CREATE FUNCTION size(A vector)
RETURNS bigint
AS '$libdir/pggraphblas', 'vector_size'
LANGUAGE C STABLE;

CREATE FUNCTION nvals(A vector)
RETURNS bigint
AS '$libdir/pggraphblas', 'vector_nvals'
LANGUAGE C STABLE;

CREATE FUNCTION xtract(
    A vector,
    B vector,
    C vector default null,
    doutp text default null,
    dmask text default null,
    dinp0 text default null,
    dinp1 text default null
    )   
RETURNS vector
AS '$libdir/pggraphblas', 'vector_xtract'
LANGUAGE C STABLE;

-- vector reduce

CREATE FUNCTION reduce_bool(
    A vector,
    semiring text default null)
RETURNS bool
AS '$libdir/pggraphblas', 'vector_reduce_bool'
LANGUAGE C STABLE;

CREATE FUNCTION reduce_bigint(
    A vector,
    semiring text default null)
RETURNS bigint
AS '$libdir/pggraphblas', 'vector_reduce_int64'
LANGUAGE C STABLE;

CREATE FUNCTION reduce_integer(
    A vector,
    semiring text default null)
RETURNS integer
AS '$libdir/pggraphblas', 'vector_reduce_int32'
LANGUAGE C STABLE;

CREATE FUNCTION reduce_smallint(
    A vector,
    semiring text default null)
RETURNS smallint
AS '$libdir/pggraphblas', 'vector_reduce_int16'
LANGUAGE C STABLE;

CREATE FUNCTION reduce_real(
    A vector,
    semiring text default null)
RETURNS real
AS '$libdir/pggraphblas', 'vector_reduce_float4'
LANGUAGE C STABLE;

CREATE FUNCTION reduce_float(
    A vector,
    semiring text default null)
RETURNS float
AS '$libdir/pggraphblas', 'vector_reduce_float8'
LANGUAGE C STABLE;

-- vector assign

CREATE FUNCTION assign(
    A vector,
    value bool,
    C vector default null,
    mask vector default null)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_assign_bool'
LANGUAGE C STABLE;

CREATE FUNCTION assign(
    A vector,
    value bigint,
    C vector default null,
    mask vector default null)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_assign_int64'
LANGUAGE C STABLE;

CREATE FUNCTION assign(
    A vector,
    value integer,
    C vector default null,
    mask vector default null)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_assign_int32'
LANGUAGE C STABLE;

CREATE FUNCTION assign(
    A vector,
    value smallint,
    C vector default null,
    mask vector default null)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_assign_int16'
LANGUAGE C STABLE;

CREATE FUNCTION assign(
    A vector,
    value real,
    C vector default null,
    mask vector default null)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_assign_float4'
LANGUAGE C STABLE;

CREATE FUNCTION assign(
    A vector,
    value float,
    C vector default null,
    mask vector default null)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_assign_float8'
LANGUAGE C STABLE;

-- vector set element

CREATE FUNCTION set_element(A vector, index bigint, value bool)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_set_element_bool'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION set_element(A vector, index bigint, value bigint)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_set_element_int64'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION set_element(A vector, index bigint, value integer)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_set_element_int32'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION set_element(A vector, index bigint, value smallint)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_set_element_int16'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION set_element(A vector, index bigint, value real)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_set_element_float4'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION set_element(A vector, index bigint, value float)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_set_element_float8'
LANGUAGE C STABLE STRICT;

CREATE OPERATOR * (
    leftarg = vector,
    rightarg = vector,
    procedure = vector_ewise_mult_op
);

CREATE OPERATOR + (
    leftarg = vector,
    rightarg = vector,
    procedure = vector_ewise_add_op
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
    procedure = vector_ne,
    negator = =
);

-- casting arrays to vectors

CREATE FUNCTION vector(bigint[])
RETURNS vector
AS '$libdir/pggraphblas', 'vector_int64'
LANGUAGE C;

CREATE CAST (bigint[] AS vector) WITH FUNCTION vector(bigint[]) AS IMPLICIT;

CREATE FUNCTION vector(integer[])
RETURNS vector
AS '$libdir/pggraphblas', 'vector_int32'
LANGUAGE C;

CREATE CAST (integer[] AS vector) WITH FUNCTION vector(integer[]) AS IMPLICIT;

CREATE FUNCTION vector(smallint[])
RETURNS vector
AS '$libdir/pggraphblas', 'vector_int16'
LANGUAGE C;

CREATE CAST (smallint[] AS vector) WITH FUNCTION vector(smallint[]) AS IMPLICIT;

CREATE FUNCTION vector(float[])
RETURNS vector
AS '$libdir/pggraphblas', 'vector_float8'
LANGUAGE C;

CREATE CAST (float[] AS vector) WITH FUNCTION vector(float[]) AS IMPLICIT;

CREATE FUNCTION vector(real[])
RETURNS vector
AS '$libdir/pggraphblas', 'vector_float4'
LANGUAGE C;

CREATE CAST (real[] AS vector) WITH FUNCTION vector(real[]) AS IMPLICIT;

CREATE FUNCTION vector(bool[])
RETURNS vector
AS '$libdir/pggraphblas', 'vector_bool'
LANGUAGE C;

CREATE CAST (bool[] AS vector) WITH FUNCTION vector(bool[]) AS IMPLICIT;

-- empty vector construction

CREATE FUNCTION vector_bigint(size bigint)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_empty_int64'
LANGUAGE C STRICT;

CREATE FUNCTION vector_integer(size bigint)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_empty_int32'
LANGUAGE C STRICT;

CREATE FUNCTION vector_smallint(size bigint)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_empty_int16'
LANGUAGE C STRICT;

CREATE FUNCTION vector_float(size bigint)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_empty_float8'
LANGUAGE C STRICT;

CREATE FUNCTION vector_real(size bigint)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_empty_float4'
LANGUAGE C STRICT;

CREATE FUNCTION vector_bool(size bigint)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_empty_bool'
LANGUAGE C STRICT;

-- sparse construction

CREATE FUNCTION vector(bigint[], bigint[], bigint default null)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_int64'
LANGUAGE C;

CREATE FUNCTION vector(bigint[], integer[], bigint default null)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_int32'
LANGUAGE C;

CREATE FUNCTION vector(bigint[], smallint[], bigint default null)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_int16'
LANGUAGE C;

CREATE FUNCTION vector(bigint[], float[], bigint default null)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_float8'
LANGUAGE C;

CREATE FUNCTION vector(bigint[], real[], bigint default null)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_float4'
LANGUAGE C;

CREATE FUNCTION vector(bigint[], bool[], bigint default null)
RETURNS vector
AS '$libdir/pggraphblas', 'vector_bool'
LANGUAGE C;

-- tuple formats for vector edges

CREATE TYPE vector_element_bigint AS (index bigint, value bigint);
CREATE TYPE vector_element_integer AS (index bigint, value integer);
CREATE TYPE vector_element_smallint AS (index bigint, value smallint);
CREATE TYPE vector_element_float AS (index bigint, value float);
CREATE TYPE vector_element_real AS (index bigint, value real);
CREATE TYPE vector_element_bool AS (index bigint, value bool);

CREATE FUNCTION elements_bigint(A vector)
RETURNS SETOF vector_element_bigint
AS '$libdir/pggraphblas', 'vector_elements_int64'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION elements_integer(A vector)
RETURNS SETOF vector_element_integer
AS '$libdir/pggraphblas', 'vector_elements_int32'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION elements_smallint(A vector)
RETURNS SETOF vector_element_smallint
AS '$libdir/pggraphblas', 'vector_elements_int16'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION elements_float(A vector)
RETURNS SETOF vector_element_float
AS '$libdir/pggraphblas', 'vector_elements_float8'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION elements_real(A vector)
RETURNS SETOF vector_element_real
AS '$libdir/pggraphblas', 'vector_elements_float4'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION elements_bool(A vector)
RETURNS SETOF vector_element_bool
AS '$libdir/pggraphblas', 'vector_elements_bool'
LANGUAGE C STABLE STRICT;
