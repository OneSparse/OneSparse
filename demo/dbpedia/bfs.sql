
drop table if exists article;    
create table article (
    id serial,
    code text,
    label text
    );

drop table if exists article_x_article;
create table article_x_article  (
    source integer,
    dest integer
    );


copy article from program 'bzcat /demo/dbpedia/article.csv.bz2';
copy article_x_article from program 'bzcat /demo/dbpedia/article_x_article.csv.bz2';
    
