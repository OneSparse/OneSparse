
-- Attaches gdb to this psql's backend.

select pg_backend_pid() pid \gset
\setenv PID :'pid'
\! tmux split-window -h
\! tmux send-keys 'gdb /usr/bin/postgres ' $PID  'C-m' 'cont' 'C-m'
\! tmux select-pane 
\! tmux send-keys 'y' 'C-m'
\prompt 'Go? (y/n) ' go

