-- \set ECHO none
-- \set QUIET 1

-- \pset format unaligned
-- \pset tuples_only true
-- \pset pager

-- \set ON_ERROR_ROLLBACK 1
-- \set ON_ERROR_STOP true
-- \set QUIET 1

CREATE EXTENSION pgtap;
CREATE EXTENSION pggraphblas;

BEGIN;
SELECT plan(1);

SELECT lives_ok($$
    with foo(bar) as (
    select '{{1,2,3}, {3, 1, 2}, {1, 1, 1}}'::matrix)
    select bar from foo
$$, 'matrix_in');
    
SELECT * FROM finish();
ROLLBACK;
