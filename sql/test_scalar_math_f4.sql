set client_min_messages = 'WARNING';
create extension if not exists onesparse;
set search_path = 'onesparse';
select (-1::float4)::scalar + 1::float4;
select -1::float4 + (1::float4)::scalar;
