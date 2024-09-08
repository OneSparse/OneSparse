
CREATE CAST ({type.pgtype} AS onesparse.scalar)
    WITH FUNCTION onesparse.scalar_{type.pgtype}({type.pgtype})
    AS IMPLICIT;

CREATE CAST (onesparse.scalar AS {type.pgtype})
    WITH FUNCTION onesparse.{type.pgtype}_scalar(scalar)
    AS ASSIGNMENT;
