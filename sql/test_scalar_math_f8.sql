set client_min_messages = 'WARNING';
create extension if not exists onesparse;
set search_path = 'onesparse';
select (-1::float8)::scalar + 1::float8;
select -1::float8 + (1::float8)::scalar;
