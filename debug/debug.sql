\setenv breakpoint :breakpoint
\! pg_ctl stop && bash debug/debug.sh $breakpoint ; pg_ctl start
