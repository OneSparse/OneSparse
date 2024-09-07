
CREATE FUNCTION scalar({type.pgtype})
RETURNS scalar
AS '$libdir/onesparse', 'scalar_{type.name}'
LANGUAGE C;

CREATE FUNCTION {type.pgtype}_scalar(scalar)
RETURNS {type.pgtype}
AS '$libdir/onesparse', 'cast_scalar_{type.name}'
LANGUAGE C;
