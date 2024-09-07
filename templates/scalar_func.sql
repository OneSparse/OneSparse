
CREATE FUNCTION scalar({type.pgtype})
RETURNS scalar
AS '$libdir/onesparse', 'scalar_{type.name}'
LANGUAGE C;

CREATE FUNCTION {type.pgtype}_scalar(scalar)
RETURNS {type.pgtype}
AS '$libdir/onesparse', 'cast_scalar_{type.name}'
LANGUAGE C;

CREATE FUNCTION scalar_plus(scalar, {type.pgtype})
RETURNS scalar
AS '$libdir/onesparse', 'scalar_plus_{type.name}'
LANGUAGE C;

CREATE FUNCTION plus_scalar({type.pgtype}, scalar)
RETURNS {type.pgtype}
AS '$libdir/onesparse', 'plus_scalar_{type.name}'
LANGUAGE C;

CREATE FUNCTION scalar_minus(scalar, {type.pgtype})
RETURNS scalar
AS '$libdir/onesparse', 'scalar_minus_{type.name}'
LANGUAGE C;

CREATE FUNCTION minus_scalar({type.pgtype}, scalar)
RETURNS {type.pgtype}
AS '$libdir/onesparse', 'minus_scalar_{type.name}'
LANGUAGE C;

CREATE FUNCTION scalar_mult(scalar, {type.pgtype})
RETURNS scalar
AS '$libdir/onesparse', 'scalar_mult_{type.name}'
LANGUAGE C;

CREATE FUNCTION mult_scalar({type.pgtype}, scalar)
RETURNS {type.pgtype}
AS '$libdir/onesparse', 'mult_scalar_{type.name}'
LANGUAGE C;

CREATE FUNCTION scalar_div(scalar, {type.pgtype})
RETURNS scalar
AS '$libdir/onesparse', 'scalar_div_{type.name}'
LANGUAGE C;

CREATE FUNCTION div_scalar({type.pgtype}, scalar)
RETURNS {type.pgtype}
AS '$libdir/onesparse', 'div_scalar_{type.name}'
LANGUAGE C;
