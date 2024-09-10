set client_min_messages = 'WARNING';
create extension if not exists onesparse;
set search_path = 'onesparse';
select '-1'::smallint::scalar;
select '0'::smallint::scalar;
select '1'::smallint::scalar;
