set client_min_messages = 'WARNING';
create extension if not exists onesparse;
set search_path = 'onesparse';
select '{type.min}'::{type.pgtype}::scalar;
select '{type.zero}'::{type.pgtype}::scalar;
select '{type.max}'::{type.pgtype}::scalar;
select scalar_{type.pgtype}(({type.min})::{type.pgtype});
select scalar_{type.pgtype}(({type.zero})::{type.pgtype});
select scalar_{type.pgtype}(({type.max})::{type.pgtype});
select {type.pgtype}_scalar(({type.min})::{type.pgtype}::scalar);
select {type.pgtype}_scalar(({type.zero})::{type.pgtype}::scalar);
select {type.pgtype}_scalar(({type.max})::{type.pgtype}::scalar);