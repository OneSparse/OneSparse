set client_min_messages = 'WARNING';
create extension if not exists onesparse;
set search_path = 'onesparse';
select ({type.min}::{type.pgtype})::scalar + {type.max}::{type.pgtype};
select {type.min}::{type.pgtype} + ({type.max}::{type.pgtype})::scalar;
