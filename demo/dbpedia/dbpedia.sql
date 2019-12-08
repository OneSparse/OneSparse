\timing on
begin;
create extension if not exists pggraphblas;

drop table if exists article;    
create table article (
    id serial,
    code text,
    label text
    );

drop table if exists page_link;
create table page_link (
    id serial,
    subject text,
    object text
    );

drop table if exists article_x_article;
    create table article_x_article  (
    source integer,
    dest integer
    );

copy article (code, label) from program 'bzcat /demo/dbpedia/dump/labels_wkd_uris_en.ttl.bz2 | gawk ''FNR > 1 {match($1, /<http:\/\/wikidata.dbpedia.org\/resource\/(.*)>/, a); $1 = ""; $2 = ""; match($0, /(\"[^\"]+\")/, b); print a[1]","b[1]}''' with (format csv);

copy page_link (subject, object) from program 'bzcat /demo/dbpedia/dump/page_links_wkd_uris_en.ttl.bz2 | gawk ''FNR > 1 {match($1, /<http:\/\/wikidata.dbpedia.org\/resource\/(.*)>/, a); match($3, /<http:\/\/wikidata.dbpedia.org\/resource\/(.*)>/, b); print a[1]","b[1]}''' with (format csv);

insert into article_x_article
    select s.id, o.id
    from page_link l
    join article s on (l.subject = s.code)
    join article o on (l.object = o.code)
    where o.code is not null and s.code is not null;

create index on article_x_article (source) include (dest);

\ir functions.sql

commit;
