set client_min_messages = 'WARNING';
create extension if not exists onesparse;
set search_path = 'onesparse';
select '{type.min}'::{type.pgtype}::scalar;
select '{type.zero}'::{type.pgtype}::scalar;
select '{type.max}'::{type.pgtype}::scalar;
