set client_min_messages = 'WARNING';
create extension if not exists onesparse;
\dT+ matrix

select 'i4[1:1:1 2:2:2 3:3:3]'::matrix;

select ewise_add('i4[1:1:1 2:2:2 3:3:3]'::matrix, 'i4[1:1:1 2:2:2 3:3:3]'::matrix, 'plus_int32');

select ewise_mult('i4[1:1:1 2:2:2 3:3:3]'::matrix, 'i4[1:1:1 2:2:2 3:3:3]'::matrix, 'times_int32');

select ewise_union('i4[1:1:1 2:2:2 3:3:3]'::matrix, 42, 'i4[1:1:1 2:2:2 3:3:3]'::matrix, 84, 'plus_int32');

select reduce_scalar('i4[1:1:1 2:2:2 3:3:3]'::matrix, 'plus_monoid_int32');

select reduce_vector('i4[1:1:1 1:2:3 2:2:2]'::matrix, 'plus_monoid_int32');

select dup('i4[1:1:1 2:2:2 3:3:3]'::matrix);

select wait('i4[1:1:1 2:2:2 3:3:3]'::matrix);

select clear('i4[1:1:1 2:2:2 3:3:3]'::matrix);
