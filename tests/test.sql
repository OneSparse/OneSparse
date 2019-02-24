select pg_backend_pid() pid \gset
\setenv PID :'pid'
\! tmux split-window -h
\! tmux send-keys 'gdb /usr/bin/postgres ' $PID  'C-m' 'cont' 'C-m'
\! tmux select-pane 
\! tmux send-keys 'y' 'C-m'
\prompt 'Go? (y/n) ' go

\set ECHO none

\pset format unaligned
\pset tuples_only true
\pset pager

\set ON_ERROR_ROLLBACK 1
\set ON_ERROR_STOP true
\set QUIET 1

create extension if not exists pgtap;
create extension if not exists pggraphblas;

\if :go = y
\ir init.sql
\endif
