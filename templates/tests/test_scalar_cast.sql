set client_min_messages = 'WARNING';
create extension if not exists onesparse;
set search_path = 'onesparse';
select cast({type.min}::{type.pgtype} as scalar);
select cast({type.zero}::{type.pgtype} as scalar);
select cast({type.max}::{type.pgtype} as scalar);
select cast({type.min}::{type.pgtype}::scalar as {type.pgtype});
select cast({type.zero}::{type.pgtype}::scalar as  {type.pgtype});
select cast({type.max}::{type.pgtype}::scalar as scalar);
