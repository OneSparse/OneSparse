set client_min_messages = 'WARNING';
create extension if not exists onesparse;
set search_path = 'onesparse';
select (-1::smallint)::scalar + 1::smallint;
select -1::smallint + (1::smallint)::scalar;
