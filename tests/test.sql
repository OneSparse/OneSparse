-- \set ECHO none

-- \pset format unaligned
-- \pset tuples_only true
\pset pager

\set ON_ERROR_ROLLBACK 1
\set ON_ERROR_STOP true
-- \set QUIET 1

create extension if not exists pgtap;
create extension if not exists pggraphblas;

\ir init.sql
