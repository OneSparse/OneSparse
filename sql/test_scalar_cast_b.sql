set client_min_messages = 'WARNING';
create extension if not exists onesparse;
set search_path = 'onesparse';
select cast(0::bool as scalar);
select cast(0::bool as scalar);
select cast(1::bool as scalar);
select cast(0::bool::scalar as bool);
select cast(0::bool::scalar as  bool);
select cast(1::bool::scalar as scalar);
