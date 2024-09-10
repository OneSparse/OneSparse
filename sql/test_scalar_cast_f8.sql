set client_min_messages = 'WARNING';
create extension if not exists onesparse;
set search_path = 'onesparse';
select cast(-1::float8 as scalar);
select cast(0::float8 as scalar);
select cast(1::float8 as scalar);
select cast(-1::float8::scalar as float8);
select cast(0::float8::scalar as  float8);
select cast(1::float8::scalar as scalar);
