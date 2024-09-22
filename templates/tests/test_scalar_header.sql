set client_min_messages = 'WARNING';
create extension if not exists onesparse;
\dT+ scalar

select dup('i4:42'::scalar);

select wait('i4:42'::scalar);

select clear('i4:42'::scalar);
