BEGIN;

CREATE EXTENSION IF NOT EXISTS onesparse;
CREATE EXTENSION IF NOT EXISTS pgtap;
SET LOCAL search_path = public, onesparse;

SELECT plan(18);

-- Descriptor construction and stable names.
SELECT is(name('t0'::descriptor), 't0',
          'the default descriptor reports its name');
SELECT is(name('s'::descriptor), 's',
          'the structural descriptor reports its name');
SELECT is(name('c'::descriptor), 'c',
          'the complement descriptor reports its name');
SELECT is(name('r'::descriptor), 'r',
          'the replace descriptor reports its name');
SELECT is(name('sc'::descriptor), 'sc',
          'combined structural complement reports its name');
SELECT is(name('rc'::descriptor), 'rc',
          'combined replace complement reports its name');

-- Value and structural masks.
SELECT results_eq(
    $$SELECT i, v::text FROM elements(apply(
        'int32(4)[0:-10 1:20 2:-30 3:40]'::vector,
        'abs_int32'::unaryop,
        mask=>'bool(4)[0:true 1:false 2:true 3:false]'::vector)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:10'::text),
             (2::bigint, 'int32:30'::text)$$,
    'a value mask admits only true positions');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(apply(
        'int32(4)[0:-10 1:20 2:-30 3:40]'::vector,
        'abs_int32'::unaryop,
        mask=>'int32(4)[0:0 2:0]'::vector,
        descr=>'s'::descriptor)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:10'::text),
             (2::bigint, 'int32:30'::text)$$,
    'a structural mask uses stored positions regardless of values');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(apply(
        'int32(4)[0:-10 1:20 2:-30 3:40]'::vector,
        'abs_int32'::unaryop,
        mask=>'int32(4)[0:1 2:1]'::vector,
        descr=>'c'::descriptor)) ORDER BY i$$,
    $$VALUES (1::bigint, 'int32:20'::text),
             (3::bigint, 'int32:40'::text)$$,
    'a complement mask admits positions outside the mask');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(apply(
        'int32(4)[0:-10 1:20 2:-30 3:40]'::vector,
        'abs_int32'::unaryop,
        mask=>'int32(4)[0:0 2:0]'::vector,
        descr=>'sc'::descriptor)) ORDER BY i$$,
    $$VALUES (1::bigint, 'int32:20'::text),
             (3::bigint, 'int32:40'::text)$$,
    'structural complement combines both mask modifiers');

-- Replace controls whether masked-out values already in C survive.
SELECT results_eq(
    $$SELECT i, v::text FROM elements(apply(
        'int32(4)[0:-10 1:20 2:-30 3:40]'::vector,
        'abs_int32'::unaryop,
        c=>'int32(4)[0:100 1:200 2:300 3:400]'::vector,
        mask=>'bool(4)[0:true 1:false 2:true 3:false]'::vector)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:10'::text),
             (1::bigint, 'int32:200'::text),
             (2::bigint, 'int32:30'::text),
             (3::bigint, 'int32:400'::text)$$,
    'without replace, masked-out C values survive');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(apply(
        'int32(4)[0:-10 1:20 2:-30 3:40]'::vector,
        'abs_int32'::unaryop,
        c=>'int32(4)[0:100 1:200 2:300 3:400]'::vector,
        mask=>'bool(4)[0:true 1:false 2:true 3:false]'::vector,
        descr=>'r'::descriptor)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:10'::text),
             (2::bigint, 'int32:30'::text)$$,
    'replace removes masked-out C values');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(apply(
        'int32(4)[0:-10 1:20 2:-30 3:40]'::vector,
        'abs_int32'::unaryop,
        c=>'int32(4)[0:100 1:200 2:300 3:400]'::vector,
        mask=>'bool(4)[0:true 1:false 2:true 3:false]'::vector,
        descr=>'rc'::descriptor)) ORDER BY i$$,
    $$VALUES (1::bigint, 'int32:20'::text),
             (3::bigint, 'int32:40'::text)$$,
    'replace and complement retain only complemented results');

-- Accumulators combine the computed result with the existing C value.
SELECT results_eq(
    $$SELECT i, v::text FROM elements(apply(
        'int32(4)[0:-10 1:20 2:-30]'::vector,
        'abs_int32'::unaryop,
        c=>'int32(4)[0:100 1:200 2:300]'::vector,
        mask=>'bool(4)[0:true 1:false 2:true]'::vector,
        accum=>'plus_int32'::binaryop)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:110'::text),
             (1::bigint, 'int32:200'::text),
             (2::bigint, 'int32:330'::text)$$,
    'an accumulator combines computed and existing values');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(apply(
        'int32(4)[0:-10 1:20 2:-30]'::vector,
        'abs_int32'::unaryop,
        c=>'int32(4)[0:100 1:200 2:300]'::vector,
        mask=>'bool(4)[0:true 1:false 2:true]'::vector,
        accum=>'plus_int32'::binaryop,
        descr=>'r'::descriptor)) ORDER BY i$$,
    $$VALUES (0::bigint, 'int32:110'::text),
             (2::bigint, 'int32:330'::text)$$,
    'replace clears C before masked accumulation');

-- Descriptors apply consistently to element-wise operations.
SELECT results_eq(
    $$SELECT i, v::text FROM elements(eadd(
        'int32(4)[0:10 1:20 2:30]'::vector,
        'int32(4)[1:5 2:6 3:7]'::vector,
        'plus_int32'::binaryop,
        mask=>'int32(4)[1:1 2:0]'::vector,
        descr=>'s'::descriptor)) ORDER BY i$$,
    $$VALUES (1::bigint, 'int32:25'::text),
             (2::bigint, 'int32:36'::text)$$,
    'structural masks constrain eadd by position');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(emult(
        'int32(4)[0:2 1:3 2:4 3:5]'::vector,
        'int32(4)[0:10 1:20 2:30 3:40]'::vector,
        'times_int32'::binaryop,
        mask=>'int32(4)[0:1 2:1]'::vector,
        descr=>'c'::descriptor)) ORDER BY i$$,
    $$VALUES (1::bigint, 'int32:60'::text),
             (3::bigint, 'int32:200'::text)$$,
    'complement masks constrain emult to outside positions');
SELECT results_eq(
    $$SELECT i, v::text FROM elements(assign(
        'int32(5)[0:1 2:3 4:5]'::vector,
        'int32(2)[0:10 1:20]'::vector,
        ARRAY[1,3]::bigint[],
        mask=>'int32(5)[1:1]'::vector,
        descr=>'r'::descriptor)) ORDER BY i$$,
    $$VALUES (1::bigint, 'int32:10'::text)$$,
    'replace descriptors apply to assignment masks');

SELECT * FROM finish();

ROLLBACK;
