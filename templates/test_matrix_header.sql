set client_min_messages = 'WARNING';
create extension if not exists onesparse;
\dT+ matrix

select 'i4[1:1:1 2:2:2 3:3:3]'::matrix;

select ewise_add('i4[1:1:1 2:2:2 3:3:3]'::matrix, 'i4[1:1:1 2:2:2 3:3:3]'::matrix, 'plus_int32');

select ewise_mult('i4[1:1:1 2:2:2 3:3:3]'::matrix, 'i4[1:1:1 2:2:2 3:3:3]'::matrix, 'times_int32');

select ewise_union('i4[1:1:1 2:2:2 3:3:3]'::matrix, 42, 'i4[1:1:1 2:2:2 3:3:3]'::matrix, 84, 'plus_int32');
