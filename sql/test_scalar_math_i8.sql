set client_min_messages = 'WARNING';
create extension if not exists onesparse;
set search_path = 'onesparse';
select (-1::bigint)::scalar + 1::bigint;
select -1::bigint + (1::bigint)::scalar;
