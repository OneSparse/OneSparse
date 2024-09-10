set client_min_messages = 'WARNING';
create extension if not exists onesparse;
set search_path = 'onesparse';
select cast(-1::float4 as scalar);
select cast(0::float4 as scalar);
select cast(1::float4 as scalar);
select cast(-1::float4::scalar as float4);
select cast(0::float4::scalar as  float4);
select cast(1::float4::scalar as scalar);
