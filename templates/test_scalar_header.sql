set client_min_messages = 'WARNING';
create extension if not exists onesparse;
set search_path = 'onesparse';

select 'i2:1'::scalar;
select 'i4:2'::scalar;
select 'i8:3'::scalar;
select 'f4:2.0'::scalar;
select 'f8:3.0'::scalar;
