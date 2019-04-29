CREATE TABLE airport (
    id integer primary key,
    name text,
    lat numeric,
    lon numeric
    );

COPY airport (id, name, lat, lon) FROM PROGRAM
'curl https://raw.githubusercontent.com/jpatokal/openflights/master/data/airports.dat | cut -d "," -f 1,2,7,8 | grep -v "ENRY\|ENSB\|ENTO\|WAPP\|YMPC\|ENOV\|ENFB\|ENRA\|ENRM\|ENSS\|YLVT\|HSFA\|ENEV\|EGCN\|GVNP\|GVSF\|NFTO"' with (format csv);
    
CREATE TABLE route (
    airline integer,
    origin integer,
    destination integer
    );

COPY route (airline, origin, destination) FROM PROGRAM
    'curl https://raw.githubusercontent.com/jpatokal/openflights/master/data/routes.dat | cut -d "," -f 2,4,6'
    with (format csv, NULL '\N');

CREATE OR REPLACE FUNCTION geodistance(alat double precision, alng double precision, blat double precision, blng double precision)
  RETURNS double precision AS
$BODY$
SELECT asin(
  sqrt(
    sin(radians($3-$1)/2)^2 +
    sin(radians($4-$2)/2)^2 *
    cos(radians($1)) *
    cos(radians($3))
  )
) * 12742 AS distance;
$BODY$
  LANGUAGE sql IMMUTABLE
  COST 100;

CREATE VIEW route_matrix AS
SELECT matrix(array_agg(origin),
              array_agg(destination),
              array_agg(geodistance(od.lat, od.lon, dd.lat, dd.lon)))
FROM route
JOIN airport od ON (od.id = origin)
JOIN airport dd ON (dd.id = destination);
