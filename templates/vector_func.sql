CREATE FUNCTION vector_{type.pgtype}({type.pgtype}[])
RETURNS vector
AS '$libdir/onesparse', 'vector_{type.name}'
LANGUAGE C;

CREATE FUNCTION {type.pgtype}_vector(vector)
RETURNS {type.pgtype}[]
AS '$libdir/onesparse', 'cast_vector_{type.name}'
LANGUAGE C;
