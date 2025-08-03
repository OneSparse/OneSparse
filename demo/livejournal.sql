
create materialized view if not exists livejournal
    as select mmread('/home/postgres/onesparse/demo/Matrix/com-LiveJournal/com-LiveJournal.mtx') as graph;
