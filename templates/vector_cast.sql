
CREATE CAST ({type.pgtype}[] AS onesparse.vector)
    WITH FUNCTION onesparse.vector_{type.pgtype}({type.pgtype}[])
    AS IMPLICIT;

CREATE CAST (onesparse.vector AS {type.pgtype}[])
    WITH FUNCTION onesparse.{type.pgtype}_vector(vector)
    AS ASSIGNMENT;
