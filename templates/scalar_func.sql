
CREATE FUNCTION scalar_{type.pgtype}({type.pgtype})
RETURNS scalar
AS '$libdir/onesparse', 'scalar_{type.name}'
LANGUAGE C;

CREATE FUNCTION {type.pgtype}_scalar(scalar)
RETURNS {type.pgtype}
AS '$libdir/onesparse', 'cast_scalar_{type.name}'
LANGUAGE C;

-- CREATE FUNCTION set_scalar_support_{type.name}(internal)
-- RETURNS internal
-- AS '$libdir/onesparse', 'set_scalar_support_{type.name}'
-- LANGUAGE C;

CREATE FUNCTION set(scalar, {type.pgtype})
RETURNS scalar
AS '$libdir/onesparse', 'set_scalar_{type.name}'
LANGUAGE C;
-- LANGUAGE C SUPPORT set_scalar_support_{type.name};
