-- # Scalar
--
-- This documentation is also tests for the code, the examples below
-- show the literal output of these statements from Postgres.
--
-- Some setup to make sure warnings are shown, and that the extension
-- is installed.

set client_min_messages = 'WARNING';
create extension if not exists onesparse;

\pset linestyle unicode
\pset border 2

-- Describe the scalar type
\dT+ scalar

-- print a scalar, this renders the value with no prefix
select print('int32:42'::scalar);

-- Duplicate a scalar
select dup('int32:42'::scalar);

-- Wait for a scalar to complete in non-blocking mode
select wait('int32:42'::scalar);

-- Clear a scalar, deleting its stored element.
select clear('int32:42'::scalar);

-- ## Scalar Construction
--
-- Various ways to construct scalars from native Postgres values.

-- Construct from literals using cast
select 42::int::scalar;
select 3.14::float8::scalar;
select true::bool::scalar;

-- Construct using explicit type
select 'int32:42'::scalar;
select 'fp64:3.14'::scalar;
select 'bool:true'::scalar;

-- Construct from different integer sizes
select 127::int2::scalar as int16_scalar;
select 32767::int4::scalar as int32_scalar;
select 2147483647::int8::scalar as int64_scalar;

-- Construct from different float sizes
select 3.14::float4::scalar as fp32_scalar;
select 3.14159265::float8::scalar as fp64_scalar;

-- Empty scalar (no value set)
select 'int32'::scalar;
select nvals('int32'::scalar);

-- Scalar with value
select 'int32:42'::scalar;
select nvals('int32:42'::scalar);

-- ## Scalar Arithmetic with Native Types
--
-- Scalars can be combined with native Postgres types using standard operators.

-- Addition: scalar + native
select 'int32:10'::scalar + 5;
select 'fp64:3.14'::scalar + 2.0;

-- Addition: native + scalar
select 5 + 'int32:10'::scalar;
select 2.0 + 'fp64:3.14'::scalar;

-- Subtraction: scalar - native
select 'int32:20'::scalar - 5;
select 'fp64:10.5'::scalar - 2.5;

-- Subtraction: native - scalar
select 30 - 'int32:10'::scalar;
select 15.5 - 'fp64:5.5'::scalar;

-- Multiplication: scalar * native
select 'int32:7'::scalar * 6;
select 'fp64:2.5'::scalar * 3.0;

-- Multiplication: native * scalar
select 6 * 'int32:7'::scalar;
select 3.0 * 'fp64:2.5'::scalar;

-- Division: scalar / native
select 'int32:20'::scalar / 4;
select 'fp64:10.0'::scalar / 2.5;

-- Division: native / scalar
select 100 / 'int32:5'::scalar;
select 15.0 / 'fp64:3.0'::scalar;

-- ## Scalar Type Conversions
--
-- Convert between different scalar types and native Postgres types.

-- Cast scalar to native type
select 'int32:42'::scalar::int4;
select 'fp64:3.14'::scalar::float8;
select 'bool:true'::scalar::bool;

-- Cast between scalar types (int to float)
select cast('int32:42'::scalar as scalar);  -- stays int32
select 'int32:42'::scalar::int4::float8::scalar;  -- through native

-- Cast float to int (truncates)
select 'fp64:3.9'::scalar::int4;
select 'fp64:-3.9'::scalar::int4;

-- Cast bool to int
select 'bool:true'::scalar::int4;
select 'bool:false'::scalar::int4;

-- Cast int to bool (0=false, non-zero=true)
select 0::int::scalar::bool;
select 1::int::scalar::bool;
select 5::int::scalar::bool;
select (-1)::int::scalar::bool;

-- Cast between integer sizes
select 127::int2::scalar::int4;  -- int16 to int32
select 32767::int4::scalar::int8; -- int32 to int64

-- ## Scalar Edge Cases - Integer Types
--
-- Test boundary values for integer types.

-- INT16 (int2) boundaries
select (-32768)::int2::scalar as int16_min;
select 32767::int2::scalar as int16_max;
select 0::int2::scalar as int16_zero;

-- INT32 (int4) boundaries
select (-2147483648)::int4::scalar as int32_min;
select 2147483647::int4::scalar as int32_max;
select 0::int4::scalar as int32_zero;

-- INT64 (int8) boundaries
select (-9223372036854775808)::int8::scalar as int64_min;
select 9223372036854775807::int8::scalar as int64_max;
select 0::int8::scalar as int64_zero;

-- Arithmetic near boundaries
select 'int32:2147483647'::scalar + 0 as at_max;
select 'int32:-2147483648'::scalar + 0 as at_min;

-- Multiplication preserving range
select 'int32:1000'::scalar * 1000;
select 'int32:-1000'::scalar * 1000;

-- Division by small values
select 'int32:1000'::scalar / 1;
select 'int32:1000'::scalar / 2;
select 'int32:1000'::scalar / 3;

-- ## Scalar Edge Cases - Float Types
--
-- Test special floating-point values.

-- FP32 (float4) boundaries
select 'fp32:3.40282347e+38'::scalar as fp32_max;
select 'fp32:-3.40282347e+38'::scalar as fp32_min;
select 'fp32:1.17549435e-38'::scalar as fp32_smallest_positive;

-- FP64 (float8) boundaries
select 'fp64:1.7976931348623157e+308'::scalar as fp64_max;
select 'fp64:-1.7976931348623157e+308'::scalar as fp64_min;
select 'fp64:2.2250738585072014e-308'::scalar as fp64_smallest_positive;

-- Zero values
select 0.0::float4::scalar as fp32_zero;
select 0.0::float8::scalar as fp64_zero;
select (-0.0)::float8::scalar as fp64_negative_zero;

-- Infinity
select 'Infinity'::float8::scalar as positive_infinity;
select '-Infinity'::float8::scalar as negative_infinity;

-- NaN (Not a Number)
select 'NaN'::float8::scalar as not_a_number;

-- Arithmetic with special values
select 'fp64:1.0'::scalar + 'Infinity'::float8;
select 'fp64:1.0'::scalar * 'Infinity'::float8;
select 'fp64:1.0'::scalar / 'Infinity'::float8;
select 'Infinity'::float8 / 'fp64:2.0'::scalar;

-- Operations producing NaN
select 'fp64:0.0'::scalar / 0.0;  -- 0/0 = NaN
select 'Infinity'::float8 - 'Infinity'::float8::scalar; -- Inf - Inf = NaN

-- Very small numbers
select 'fp64:1e-100'::scalar * 1e-100;
select 'fp64:1e-200'::scalar + 1e-200;

-- Very large numbers
select 'fp64:1e100'::scalar * 1.0;
select 'fp64:1e200'::scalar + 0.0;

-- ## Scalar Boolean Operations
--
-- Test boolean scalar operations.

-- Boolean construction
select true::bool::scalar;
select false::bool::scalar;

-- Boolean with different representations
select 't'::bool::scalar;
select 'f'::bool::scalar;
select 'yes'::bool::scalar;
select 'no'::bool::scalar;

-- Boolean casting
select true::bool::scalar::bool;
select false::bool::scalar::bool;

-- Boolean to int conversion
select true::bool::scalar::int4 as true_as_int;
select false::bool::scalar::int4 as false_as_int;

-- Int to boolean conversion
select 0::int4::scalar::bool as zero_as_bool;
select 1::int4::scalar::bool as one_as_bool;
select 42::int4::scalar::bool as nonzero_as_bool;
select (-1)::int4::scalar::bool as negative_as_bool;

-- ## Scalar Update Operations
--
-- Test the set() function to update scalar values.

-- Basic set operation
select set('int32:42'::scalar, 100);

-- Set with different types
select set('fp64:3.14'::scalar, 2.71);
select set('bool:true'::scalar, false);

-- Set on empty scalar
select set('int32'::scalar, 42);

-- Set to zero
select set('int32:42'::scalar, 0);

-- Set to negative
select set('int32:42'::scalar, -100);

-- Set boundary values
select set('int32:0'::scalar, 2147483647);
select set('int32:0'::scalar, -2147483648);

-- Set float values
select set('fp64:0.0'::scalar, 'Infinity'::float8);
select set('fp64:0.0'::scalar, 'NaN'::float8);
select set('fp64:0.0'::scalar, -0.0);

-- ## Scalar Utility Functions
--
-- Test other scalar utility functions.

-- Print scalar value
select print('int32:42'::scalar);
select print('fp64:3.14'::scalar);
select print('bool:true'::scalar);

-- Print empty scalar
select print('int32'::scalar);

-- Get scalar type
select type('int32:42'::scalar);
select type('fp64:3.14'::scalar);
select type('bool:true'::scalar);
select type('int16:1'::scalar);

-- Count values (always 0 or 1 for scalars)
select nvals('int32:42'::scalar) as has_value;
select nvals('int32'::scalar) as empty;

-- Duplicate scalar
select dup('int32:42'::scalar);
select dup('fp64:3.14'::scalar);

-- Wait for scalar (synchronization in async mode)
select wait('int32:42'::scalar);

-- Clear scalar (removes value)
select nvals(clear('int32:42'::scalar));
select clear('int32:42'::scalar);

-- Clear already empty scalar
select clear('int32'::scalar);

-- ## Scalar Type Promotion
--
-- Test how scalars interact with different native types.

-- Int32 scalar with different int sizes
select 'int32:100'::scalar + 1::int2;  -- scalar + int16
select 'int32:100'::scalar + 1::int4;  -- scalar + int32
select 'int32:100'::scalar + 1::int8;  -- scalar + int64

-- Float scalar with different float sizes
select 'fp64:3.14'::scalar + 1.0::float4;  -- scalar + float32
select 'fp64:3.14'::scalar + 1.0::float8;  -- scalar + float64

-- Int scalar with float (promotes to float)
select 'int32:10'::scalar + 3.14::float8;

-- Small int with large int
select 1::int2 + 'int32:100'::scalar;  -- int16 + scalar
select 1::int8 + 'int32:100'::scalar;  -- int64 + scalar

-- ## Scalar Comparison Context
--
-- While scalars don't have direct comparison operators, they can be
-- extracted and compared as native types.

-- Extract and compare
select ('int32:10'::scalar::int4) = 10;
select ('int32:10'::scalar::int4) > 5;
select ('int32:10'::scalar::int4) < 20;

-- Compare two scalar values via extraction
select ('int32:10'::scalar::int4) = ('int32:10'::scalar::int4);
select ('int32:10'::scalar::int4) < ('int32:20'::scalar::int4);

-- Compare floats
select ('fp64:3.14'::scalar::float8) = 3.14;
select ('fp64:3.14'::scalar::float8) > 3.0;

-- Compare booleans
select ('bool:true'::scalar::bool) = true;
select ('bool:false'::scalar::bool) = false;

-- ## Scalar Edge Cases - Division
--
-- Test division edge cases and error conditions.

-- Integer division truncates
select 'int32:7'::scalar / 2;
select 'int32:7'::scalar / 3;
select 'int32:-7'::scalar / 2;

-- Division by 1 and -1
select 'int32:42'::scalar / 1;
select 'int32:42'::scalar / -1;

-- Float division
select 'fp64:7.0'::scalar / 2.0;
select 'fp64:1.0'::scalar / 3.0;

-- Division producing very small results
select 'fp64:1.0'::scalar / 1e100;

-- Division producing very large results
select 'fp64:1e100'::scalar / 0.001;

-- ## Scalar NULL Handling
--
-- Test how scalars interact with SQL NULLs.

-- NULL cast to scalar produces NULL
select NULL::int::scalar;

-- Scalar operations with NULL
select 'int32:42'::scalar + NULL::int;
select NULL::int + 'int32:42'::scalar;

-- Set with NULL (should fail or create empty scalar)
-- Note: This may error depending on implementation
-- select set('int32:42'::scalar, NULL::int);

-- ## Scalar Round-Trip Conversions
--
-- Test that values survive round-trip conversions.

-- Int round-trips
select 42::int4::scalar::int4;
select (-42)::int4::scalar::int4;
select 0::int4::scalar::int4;

-- Float round-trips (may have precision differences)
select 3.14::float8::scalar::float8;
select (-3.14)::float8::scalar::float8;
select 0.0::float8::scalar::float8;

-- Bool round-trips
select true::bool::scalar::bool;
select false::bool::scalar::bool;

-- Large int round-trips
select 2147483647::int4::scalar::int4;
select (-2147483648)::int4::scalar::int4;

-- Special float round-trips
select 'Infinity'::float8::scalar::float8;
select '-Infinity'::float8::scalar::float8;
select 'NaN'::float8::scalar::float8;

-- ## Scalar Precision and Accuracy
--
-- Test precision maintenance across operations.

-- Float precision
select 'fp64:0.1'::scalar + 0.2;  -- famous 0.3 problem
select 'fp64:1.0'::scalar / 3.0 * 3.0;  -- should be close to 1.0

-- Very precise values
select 'fp64:3.141592653589793'::scalar::float8;

-- Addition of very different magnitudes
select 'fp64:1e20'::scalar + 1.0;
select 'fp64:1.0'::scalar + 1e-20;

-- Subtraction producing small differences
select 'fp64:1.000000001'::scalar - 1.0;

-- ## Scalar Memory and Storage
--
-- Test internal representation details.

-- Empty scalars should be lightweight
select nvals('int32'::scalar);
select nvals('fp64'::scalar);
select nvals('bool'::scalar);

-- Scalars with values
select nvals('int32:0'::scalar);  -- zero is still a value
select nvals('fp64:0.0'::scalar);
select nvals('bool:false'::scalar);

-- Type information preserved
select type('int32'::scalar);
select type('int32:42'::scalar);

-- Value preserved after dup
select 'int32:42'::scalar::int4;
select dup('int32:42'::scalar)::int4;

-- ## Scalars with Vector Operations
--
-- Test scalar parameters and results in vector operations.

-- Vector apply with scalar (apply_first: scalar op vector)
select apply(5::int, 'int32(5)[0:10 1:20 2:30]'::vector, 'plus_int32'::binaryop);
select apply(10::int, 'int32(5)[0:1 1:2 2:3 3:4 4:5]'::vector, 'times_int32'::binaryop);
select apply(100::int, 'int32(5)[0:10 1:20 2:30]'::vector, 'minus_int32'::binaryop);

-- Vector apply with scalar (apply_second: vector op scalar)
select apply('int32(5)[0:10 1:20 2:30]'::vector, 5::int, 'plus_int32'::binaryop);
select apply('int32(5)[0:1 1:2 2:3 3:4 4:5]'::vector, 10::int, 'times_int32'::binaryop);
select apply('int32(5)[0:100 1:200 2:300]'::vector, 50::int, 'minus_int32'::binaryop);

-- Vector apply with float scalar
select apply(2.5::double precision, 'fp64(5)[0:1.0 1:2.0 2:3.0]'::vector, 'times_fp64'::binaryop);
select apply('fp64(5)[0:10.0 1:20.0 2:30.0]'::vector, 5.0::double precision, 'div_fp64'::binaryop);

-- Vector set_element with scalar value
select set_element('int32(10)[0:1 2:3 4:5]'::vector, 1, 100::int);
select set_element('int32(10)[0:1 2:3 4:5]'::vector, 5, 42::int);
select set_element('fp64(5)[0:1.5 2:3.5]'::vector, 3, 2.71::double precision);

-- Vector get_element returns scalar (conceptually)
select get_element('int32[0:10 1:20 2:30]'::vector, 1);
select get_element('fp64[0:1.5 1:2.5 2:3.5]'::vector, 2);
select get_element('bool[0:true 1:false 2:true]'::vector, 0);

-- ## Scalars with Matrix Operations
--
-- Test scalar parameters in matrix operations.

-- Matrix set_element with scalar value
select set_element('int32(3,3)[0:0:10 1:1:20 2:2:30]'::matrix, 0, 1, 15::int);
select set_element('fp64(3,3)[0:0:1.5 1:1:2.5]'::matrix, 2, 2, 3.14::double precision);

-- Matrix get_element returns scalar (conceptually)
select get_element('int32[0:0:10 0:1:15 1:0:20 1:1:25]'::matrix, 0, 1);
select get_element('fp64[0:0:1.5 1:1:2.5 2:2:3.5]'::matrix, 1, 1);

-- Matrix apply with scalar (apply_first: scalar op matrix)
select apply(10::int, 'int32(3,3)[0:0:1 1:1:2 2:2:3]'::matrix, 'times_int32'::binaryop);
select apply(100::int, 'int32(3,3)[0:0:10 0:1:20 1:0:30]'::matrix, 'minus_int32'::binaryop);

-- Matrix apply with scalar (apply_second: matrix op scalar)
select apply('int32(3,3)[0:0:10 1:1:20 2:2:30]'::matrix, 5::int, 'plus_int32'::binaryop);
select apply('fp64(3,3)[0:0:2.0 1:1:4.0 2:2:6.0]'::matrix, 2.0::double precision, 'div_fp64'::binaryop);

-- ## Scalar Reductions
--
-- Operations that reduce vectors or matrices to scalars.

-- Vector reduce to scalar
select reduce_scalar('int32[0:10 1:20 2:30]'::vector, 'plus_monoid_int32'::monoid);
select reduce_scalar('int32[0:5 1:10 2:15 3:20]'::vector, 'times_monoid_int32'::monoid);
select reduce_scalar('fp64[0:1.5 1:2.5 2:3.5]'::vector, 'plus_monoid_fp64'::monoid);

-- Vector reduce with different monoids
select reduce_scalar('int32[0:10 1:5 2:20 3:15]'::vector, 'max_monoid_int32'::monoid);
select reduce_scalar('int32[0:10 1:5 2:20 3:15]'::vector, 'min_monoid_int32'::monoid);

-- Matrix reduce to scalar
select reduce_scalar('int32[0:0:1 0:1:2 1:0:3 1:1:4]'::matrix, 'plus_monoid_int32'::monoid);
select reduce_scalar('fp64[0:0:1.5 0:1:2.5 1:0:3.5 1:1:4.5]'::matrix, 'times_monoid_fp64'::monoid);

-- Reduce with identity returns identity for empty
select reduce_scalar('int32(10)[]'::vector, 'plus_monoid_int32'::monoid);
select reduce_scalar('int32(5,5)[]'::matrix, 'times_monoid_int32'::monoid);

-- ## Scalars as Thresholds
--
-- Use scalars as comparison thresholds in select operations.

-- Vector select with scalar threshold
select choose('int32(10)[0:5 1:15 2:10 3:20 4:8]'::vector, 'valuelt_int32'::indexunaryop, 10::int);
select choose('int32(10)[0:5 1:15 2:10 3:20 4:8]'::vector, 'valuegt_int32'::indexunaryop, 10::int);
select choose('int32(10)[0:5 1:15 2:10 3:20 4:8]'::vector, 'valuege_int32'::indexunaryop, 10::int);
select choose('int32(10)[0:5 1:15 2:10 3:20 4:8]'::vector, 'valuele_int32'::indexunaryop, 10::int);
select choose('int32(10)[0:5 1:15 2:10 3:20 4:8]'::vector, 'valueeq_int32'::indexunaryop, 10::int);
select choose('int32(10)[0:5 1:15 2:10 3:20 4:8]'::vector, 'valuene_int32'::indexunaryop, 10::int);

-- Float vector select with float threshold
select choose('fp64(5)[0:1.5 1:2.5 2:1.0 3:3.5 4:2.0]'::vector, 'valuegt_fp64'::indexunaryop, 2.0::double precision);
select choose('fp64(5)[0:1.5 1:2.5 2:1.0 3:3.5 4:2.0]'::vector, 'valuele_fp64'::indexunaryop, 2.0::double precision);

-- Matrix select with scalar threshold
select choose('int32(4,4)[0:0:5 0:1:15 1:0:10 1:1:20 2:2:8]'::matrix, 'valuegt_int32'::indexunaryop, 10::int);
select choose('fp64(3,3)[0:0:1.5 1:1:2.5 2:2:3.5]'::matrix, 'valuele_fp64'::indexunaryop, 2.0::double precision);

-- ## Scalars with eunion Operations
--
-- eunion uses scalar defaults for missing values.

-- Vector eunion with scalar defaults
select eunion('int32(5)[0:10 2:30]'::vector,
    5::int,
    'int32(5)[1:20 3:40]'::vector,
    7::int,
    'plus_int32'::binaryop);

select eunion('fp64(5)[0:1.5 2:3.5]'::vector,
    2.0::double precision,
    'fp64(5)[1:2.5 3:4.5]'::vector,
    3.0::double precision,
    'times_fp64'::binaryop);

-- Matrix eunion with scalar defaults
select eunion('int32(3,3)[0:0:10 1:1:20]'::matrix,
    0::int,
    'int32(3,3)[0:1:15 2:2:30]'::matrix,
    0::int,
    'plus_int32'::binaryop);

-- eunion with different default values
select eunion('int32(5)[0:10 2:30]'::vector,
    0::int,
    'int32(5)[1:20 3:40]'::vector,
    100::int,
    'plus_int32'::binaryop);

-- ## Type Promotion with Scalars
--
-- Test how scalars interact across different types.

-- int32 scalar with int64 vector
select apply(5::int4, 'int64(5)[0:100 1:200 2:300]'::vector, 'plus_int64'::binaryop);

-- int64 scalar with int32 vector
select apply(1000::int8, 'int32(5)[0:10 1:20 2:30]'::vector, 'times_int32'::binaryop);

-- float scalar with int vector (via native operations)
select 'int32(5)[0:10 1:20 2:30]'::vector + 5::int;
select 'fp64(5)[0:1.5 1:2.5 2:3.5]'::vector * 2.0::double precision;

-- ## Scalars as Accumulators
--
-- While scalars aren't directly used as accumulators, test accumulator
-- functionality with scalar results.

-- Reduction with different accumulators would accumulate intermediate scalars
-- (though GraphBLAS handles this internally)

-- Test that accumulation works correctly
select reduce_scalar(apply('int32[0:1 1:2 2:3]'::vector, 'minv_int32'::unaryop), 'plus_monoid_int32'::monoid);

-- ## Scalars with Descriptors
--
-- Test scalar operations with descriptors (limited applicability).

-- Matrix/vector operations with scalars typically don't use descriptors,
-- but verify they work when provided

select apply(10::int, 'int32(5)[0:1 1:2 2:3]'::vector, 'plus_int32'::binaryop);

-- ## Scalar Utility Integration
--
-- Test utility functions in integration contexts.

-- Print scalars from reductions
select print(reduce_scalar('int32[0:10 1:20 2:30]'::vector, 'plus_monoid_int32'::monoid)::scalar);

-- Check type of reduced scalar
select type(reduce_scalar('fp64[0:1.5 1:2.5 2:3.5]'::vector, 'plus_monoid_fp64'::monoid)::scalar);

-- ## Empty Vector/Matrix with Scalar Operations
--
-- Test scalar operations on empty structures.

-- Empty vector with scalar
select apply(10::int, 'int32(5)[]'::vector, 'plus_int32'::binaryop);
select apply('int32(5)[]'::vector, 5::int, 'times_int32'::binaryop);

-- Reduce empty vector to scalar
select reduce_scalar('int32(10)[]'::vector, 'plus_monoid_int32'::monoid);

-- Empty matrix with scalar
select apply(5::int, 'int32(3,3)[]'::matrix, 'plus_int32'::binaryop);

-- Reduce empty matrix to scalar
select reduce_scalar('int32(5,5)[]'::matrix, 'times_monoid_int32'::monoid);

-- ## Scalar Chains
--
-- Operations that chain multiple scalar transformations.

-- Extract element, modify via scalar ops, set back
select set_element('int32[0:10 1:20 2:30]'::vector,
    1,
    (get_element('int32[0:10 1:20 2:30]'::vector, 1)::int + 5)::int);

-- Reduce to scalar, use in apply
select apply('int32[0:1 1:2 2:3]'::vector,
    reduce_scalar('int32[0:10 1:20 2:30]'::vector, 'plus_monoid_int32'::monoid)::int,
    'plus_int32'::binaryop);

-- ## Scalars with Boolean Operations
--
-- Test boolean scalars in integration contexts.

-- Boolean scalar with vector apply
select apply(true::bool, 'bool(5)[0:false 1:true 2:false]'::vector, 'lor'::binaryop);
select apply('bool(5)[0:false 1:true 2:false]'::vector, false::bool, 'land'::binaryop);

-- Boolean reduction
select reduce_scalar('bool[0:true 1:false 2:true]'::vector, 'lor_monoid_bool'::monoid);
select reduce_scalar('bool[0:true 1:true 2:true]'::vector, 'land_monoid_bool'::monoid);

-- ## Scalar Precision in Integration
--
-- Test that precision is maintained through operations.

-- High-precision float through operations
select reduce_scalar('fp64[0:3.141592653589793 1:2.718281828459045 2:1.414213562373095]'::vector,
    'plus_monoid_fp64'::monoid);

-- Very small values
select reduce_scalar('fp64[0:1e-100 1:1e-100 2:1e-100]'::vector, 'plus_monoid_fp64'::monoid);

-- Very large values
select reduce_scalar('fp64[0:1e100 1:1e100 2:1e100]'::vector, 'plus_monoid_fp64'::monoid);

-- ## Scalar Results from Matrix-Vector Operations
--
-- Test operations that produce scalars from matrix-vector products.

-- This would test vxm and mxv but returning the result value
-- (GraphBLAS typically returns vectors, but single-element results are scalar-like)

-- Matrix-vector multiply resulting in single value
select get_element(
    mxv('int32[0:0:2 0:1:3]'::matrix, 'int32[0:10 1:20]'::vector),
    0
);

-- Vector-matrix multiply resulting in single value
select get_element(
    vxm('int32[0:5 1:10]'::vector, 'int32[0:0:2 1:0:3]'::matrix),
    0
);

-- ## Cross-Type Scalar Integration
--
-- Test scalars of one type with structures of another.

-- int scalar with float vector
select apply('fp64(5)[0:1.5 1:2.5 2:3.5]'::vector,
    ((5::int)::scalar::int)::double precision,
    'plus_fp64'::binaryop);

-- float scalar with int vector (requires conversion)
select apply('int32(5)[0:10 1:20 2:30]'::vector,
    ((2.5::double precision)::scalar::double precision)::int,
    'times_int32'::binaryop);
