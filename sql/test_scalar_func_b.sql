set client_min_messages = 'WARNING';
create extension if not exists onesparse;
set search_path = 'onesparse';
select '0'::bool::scalar;
select '0'::bool::scalar;
select '1'::bool::scalar;
select scalar_bool((0)::bool);
select scalar_bool((0)::bool);
select scalar_bool((1)::bool);
select bool_scalar((0)::bool::scalar);
select bool_scalar((0)::bool::scalar);
select bool_scalar((1)::bool::scalar);