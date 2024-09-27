set client_min_messages = 'WARNING';
create extension if not exists onesparse;
\dT+ matrix

select 'i4[1:1:1 2:2:2 3:3:3]'::matrix;

select 'i4(10:)[1:1:1 2:2:2 3:3:3]'::matrix;

select 'i4(:10)[1:1:1 2:2:2 3:3:3]'::matrix;

select nrows('i4(10:)[1:1:1 2:2:2 3:3:3]'::matrix);

select ncols('i4(10:)[1:1:1 2:2:2 3:3:3]'::matrix);

select nrows('i4(:10)[1:1:1 2:2:2 3:3:3]'::matrix);

select ncols('i4(:10)[1:1:1 2:2:2 3:3:3]'::matrix);

select nrows('i4(10:10)[1:1:1 2:2:2 3:3:3]'::matrix);

select ncols('i4(10:10)[1:1:1 2:2:2 3:3:3]'::matrix);

select ewise_add('i4[1:1:1 2:2:2 3:3:3]'::matrix, 'i4[1:1:1 2:2:2 3:3:3]'::matrix, 'plus_int32');

select ewise_mult('i4[1:1:1 2:2:2 3:3:3]'::matrix, 'i4[1:1:1 2:2:2 3:3:3]'::matrix, 'times_int32');

select ewise_union('i4[1:1:1 2:2:2 3:3:3]'::matrix, 42, 'i4[1:1:1 2:2:2 3:3:3]'::matrix, 84, 'plus_int32');

select reduce_scalar('i4[1:1:1 2:2:2 3:3:3]'::matrix, 'plus_monoid_int32');

select reduce_vector('i4[1:1:1 1:2:3 2:2:2]'::matrix, 'plus_monoid_int32');

select mxm('i4[0:0:1 0:1:2]'::matrix, 'i4[0:0:1 0:1:3]'::matrix, 'plus_times_int32');

select mxv('i4[0:0:1 0:1:2]'::matrix, 'i4[0 1]'::vector, 'plus_times_int32');

select vxm('i4[0 1]'::vector, 'i4[0:0:1 0:1:2]'::matrix, 'plus_times_int32');

select dup('i4[1:1:1 2:2:2 3:3:3]'::matrix);

select wait('i4[2:2:2 3:3:3 1:1:1]'::matrix);

select clear('i4[1:1:1 2:2:2 3:3:3]'::matrix);
