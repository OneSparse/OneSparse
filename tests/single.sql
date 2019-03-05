
\echo Going to single user mode

\! pg_ctl stop
\! sleep 1
\! postgres --single
\! pg_ctl start
\! sleep 1

\echo Multi user model resumed
\c
