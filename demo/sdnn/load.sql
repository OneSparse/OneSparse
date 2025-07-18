
drop table if exists neuron1024 cascade;
create table neuron1024 (
    layer integer,
    i integer,
    j integer,
    weight real
    );


drop table if exists sparse_images cascade;
create table sparse_images1024 (
    i integer,
    j integer,
    weight real
    );


CREATE OR REPLACE FUNCTION load_neuron1024(dir text, upto integer default 120)
RETURNS void AS $$
DECLARE
    file text;
    layer text;
    cmd text;
BEGIN
    FOR file IN
        EXECUTE format(
            'SELECT fname FROM pg_ls_dir(%L) AS fname WHERE fname LIKE %L',
            dir, '%.tsv'
        )
    LOOP
        layer := regexp_replace(file, '^n[0-9]+-l([0-9]+)\.tsv$', '\1');
        if layer::int > upto then
            continue;
        end if;
        cmd := format(
            'awk -v lid=%s ''{print lid "\t" $1-1 "\t" $2-1 "\t" $3}'' %s/%s',
            layer, dir, file
        );
        raise notice 'load layer tsv: %', layer;
        EXECUTE format(
            'COPY neuron1024(layer, i, j, weight) FROM PROGRAM %L WITH (FORMAT text)',
            cmd
        );
    END LOOP;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION load_images(file text)
RETURNS void AS $$
DECLARE
    cmd text;
BEGIN
    cmd := format(
        'awk ''{print $1-1 "\t" $2-1 "\t" $3}'' %s', file);
    EXECUTE format(
        'COPY sparse_images1024(i, j, weight) FROM PROGRAM %L WITH (FORMAT text)',
        cmd
    );
END;
$$ LANGUAGE plpgsql;


select load_neuron1024('/home/postgres/onesparse/demo/sdnn/neuron1024');
select load_images('/home/postgres/onesparse/demo/sdnn/sparse-images-1024.tsv');

create index on neuron1024 (layer) include (i, j);


drop materialized view if exists layer_graph;

CREATE materialized view layer_graph as
    select layer,
           resize(matrix_agg(i, j, weight), 1024, 1024) as edges
    from neuron1024
    where layer <= 120
    group by layer
    order by layer;

drop materialized view if exists images1024;
CREATE materialized view images1024 as
    select resize(matrix_agg(i, j, weight), 60000) as images
    from sparse_images1024;


CREATE OR REPLACE FUNCTION build_network(upto integer default 120)
RETURNS matrix AS $$
DECLARE
    ret matrix = 'fp32';
    rec record;
    row integer;
    col integer;
    val real;
    ii integer = 0;
    jj integer = 0;
BEGIN
    for rec in select layer, edges from layer_graph loop
        jj = jj + nrows(rec.edges);
        for row, col, val in select * from elements(rec.edges) loop
            ret = set_element(ret, row + ii, col + jj, val);
        end loop;
        ii = ii + jj;
    end loop;
    return ret;
END;
$$ LANGUAGE plpgsql set search_path = onesparse,public;

drop materialized view if exists network1024;
create materialized view network1024 as
    select build_network() as network;

-- drop materialized view if exists bias1024;
-- create materialized view bias1024 as
--     select diag(assign(vector('int32',
