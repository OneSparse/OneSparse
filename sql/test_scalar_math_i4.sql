set client_min_messages = 'WARNING';
create extension if not exists onesparse;
set search_path = 'onesparse';
select (-1::integer)::scalar + 1::integer;
select -1::integer + (1::integer)::scalar;
