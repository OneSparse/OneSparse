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
SELECT plan(2);

SELECT lives_ok($$
    with foo(bar) as (
    select '{{1,2,3}, {3, 1, 2}, {1, 1, 1}}'::matrix)
    select bar from foo
$$, 'matrix_in');

create table graph (
    i bigint,
    j bigint,
    v bigint
    );

insert into graph (i, j, v) values 
    (1, 4, 1),
    (1, 2, 2),
    (2, 7, 3),
    (2, 5, 4),
    (3, 6, 5),
    (4, 3, 6),
    (4, 1, 7),
    (5, 6, 8),
    (6, 3, 9),
    (7, 3, 10);

SELECT lives_ok($$
    with graph as (select matrix_agg(i, j, v) from graph)
    select matrix_extract(i) from graph x(i);
$$, 'matrix_agg');

SELECT * FROM finish();
ROLLBACK;
