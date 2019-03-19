
    
create table article (
    id serial,
    code text,
    label text
    );

create table page_link (
    subject text,
    object text
    );


\copy article (code, label) from program 'bzcat /demo/dbpedia/labels_wkd_uris_en.ttl.bz2 | gawk ''FNR > 1 {match($1, /<http:\/\/wikidata.dbpedia.org\/resource\/(.*)>/, a); $1 = ""; $2 = ""; match($0, /(\"[^\"]+\")/, b); print a[1], ", ", b[1]}''' with (format csv)

\copy page_link from program 'bzcat /demo/dbpedia/page_links_wkd_uris_en.ttl.bz2 | gawk ''FNR > 1 {match($1, /<http:\/\/wikidata.dbpedia.org\/resource\/(.*)>/, a); match($3, /<http:\/\/wikidata.dbpedia.org\/resource\/(.*)>/, b); print a[1], ", ", b[1]}''' with (format csv)


