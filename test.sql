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

\i fixture.sql
