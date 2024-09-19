set client_min_messages = 'WARNING';
create extension if not exists onesparse;
set search_path = 'onesparse';

select 't0'::descriptor;
select 't1'::descriptor;
select 't0t1'::descriptor;

select 'c'::descriptor;
select 'ct0'::descriptor;
select 'ct1'::descriptor;
select 'ct0t1'::descriptor;

select 's'::descriptor;
select 'st0'::descriptor;
select 'st1'::descriptor;
select 'st0t1'::descriptor;

select 'sc'::descriptor;
select 'sct0'::descriptor;
select 'sct1'::descriptor;
select 'sct0t1'::descriptor;

select 'r'::descriptor;
select 'rt0'::descriptor;
select 'rt1'::descriptor;
select 'rt0t1'::descriptor;

select 'rc'::descriptor;
select 'rct0'::descriptor;
select 'rct1'::descriptor;
select 'rct0t1'::descriptor;

select 'rs'::descriptor;
select 'rst0'::descriptor;
select 'rst1'::descriptor;
select 'rst0t1'::descriptor;

select 'rsc'::descriptor;
select 'rsct0'::descriptor;
select 'rsct1'::descriptor;
select 'rsct0t1'::descriptor;
