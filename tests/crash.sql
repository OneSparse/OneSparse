
 -- crashing on GxB_init tests

\! tmux new-window /bin/bash
\! tmux send-key -t 1 'pg_ctl restart' 'C-m'
\! tmux next-window
\! sleep 3
\c
select pg_backend_pid() pid \gset
\setenv PID :'pid'
\! tmux split-window -h
\! tmux send-keys 'gdb /usr/bin/postgres ' $PID  'C-m' 'cont' 'C-m'
\! tmux select-pane -l

\prompt 'Go? (y/n) ' go

create extension pggraphblas;

-- two matrix multiplies, the second fails

select vector(array[2,3,4]) * matrix(array[0,1,2], array[1,2,0], array[1,2,3]);
select vector(array[2,3,4]) * matrix(array[0,1,2], array[1,2,0], array[1,2,3]);
