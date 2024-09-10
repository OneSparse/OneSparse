set client_min_messages = 'WARNING';
create extension if not exists onesparse;
set search_path = 'onesparse';
select cast(-1::smallint as scalar);
select cast(0::smallint as scalar);
select cast(1::smallint as scalar);
select cast(-1::smallint::scalar as smallint);
select cast(0::smallint::scalar as  smallint);
select cast(1::smallint::scalar as scalar);
