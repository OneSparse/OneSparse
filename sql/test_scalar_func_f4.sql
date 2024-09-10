set client_min_messages = 'WARNING';
create extension if not exists onesparse;
set search_path = 'onesparse';
select '-1'::float4::scalar;
select '0'::float4::scalar;
select '1'::float4::scalar;
select scalar_float4((-1)::float4);
select scalar_float4((0)::float4);
select scalar_float4((1)::float4);
select float4_scalar((-1)::float4::scalar);
select float4_scalar((0)::float4::scalar);
select float4_scalar((1)::float4::scalar);
