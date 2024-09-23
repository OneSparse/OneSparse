set client_min_messages = 'WARNING';
create extension if not exists onesparse;
\dT+ vector

select 'i4[1 2 3]'::vector;

select ewise_add('i4[1 2 3]'::vector, 'i4[1 2 3]'::vector, 'plus_int32');

select ewise_mult('i4[1 2 3]'::vector, 'i4[1 2 3]'::vector, 'times_int32');

select ewise_union('i4[1 2 3]'::vector, 42, 'i4[1 2 3]'::vector, 84, 'plus_int32');

select reduce_scalar('i4[1 2 3]'::vector, 'plus_monoid_int32');

select wait('i4[1 2 3]'::vector);

select dup('i4[1 2 3]'::vector);

select clear('i4[1 2 3]'::vector);
