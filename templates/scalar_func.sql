
CREATE FUNCTION scalar_{type.pgtype}({type.pgtype})
RETURNS scalar
AS '$libdir/onesparse', 'scalar_{type.name}'
LANGUAGE C;

CREATE FUNCTION {type.pgtype}_scalar(scalar)
RETURNS {type.pgtype}
AS '$libdir/onesparse', 'cast_scalar_{type.name}'
LANGUAGE C;

CREATE FUNCTION set(scalar, {type.pgtype})
RETURNS scalar
AS '$libdir/onesparse', 'set_scalar_{type.name}'
LANGUAGE C;
