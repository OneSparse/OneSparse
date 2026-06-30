set client_min_messages = 'WARNING';
create extension if not exists onesparse;
set search_path = 'onesparse';

-- verify defaults
show onesparse.nthreads;
show onesparse.chunk;
show onesparse.print_1based;
show onesparse.hyper_switch;
show onesparse.jit_use_cmake;

-- set and verify session-level GUCs
set onesparse.nthreads = 4;
show onesparse.nthreads;

set onesparse.print_1based = on;
show onesparse.print_1based;

set onesparse.hyper_switch = 0.5;
show onesparse.hyper_switch;

-- reset to defaults
reset onesparse.nthreads;
reset onesparse.print_1based;
reset onesparse.hyper_switch;
show onesparse.nthreads;
show onesparse.hyper_switch;
