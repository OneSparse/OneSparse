set client_min_messages = 'WARNING';
create extension if not exists onesparse;
set search_path = 'onesparse';
select cast(-1::integer as scalar);
select cast(0::integer as scalar);
select cast(1::integer as scalar);
select cast(-1::integer::scalar as integer);
select cast(0::integer::scalar as  integer);
select cast(1::integer::scalar as scalar);
