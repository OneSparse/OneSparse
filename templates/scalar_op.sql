CREATE OPERATOR {type.plus} (
    LEFTARG = scalar,
    RIGHTARG = {type.pgtype},
    FUNCTION = scalar_plus
    );

CREATE OPERATOR {type.plus} (
    LEFTARG = {type.pgtype},
    RIGHTARG = scalar,
    FUNCTION = plus_scalar
    );

CREATE OPERATOR {type.sub} (
    LEFTARG = scalar,
    RIGHTARG = {type.pgtype},
    FUNCTION = scalar_minus
    );

CREATE OPERATOR {type.sub} (
    LEFTARG = {type.pgtype},
    RIGHTARG = scalar,
    FUNCTION = minus_scalar
    );

CREATE OPERATOR {type.mult} (
    LEFTARG = scalar,
    RIGHTARG = {type.pgtype},
    FUNCTION = scalar_mult
    );

CREATE OPERATOR {type.mult} (
    LEFTARG = {type.pgtype},
    RIGHTARG = scalar,
    FUNCTION = mult_scalar
    );

CREATE OPERATOR {type.div} (
    LEFTARG = scalar,
    RIGHTARG = {type.pgtype},
    FUNCTION = scalar_div
    );

CREATE OPERATOR {type.div} (
    LEFTARG = {type.pgtype},
    RIGHTARG = scalar,
    FUNCTION = div_scalar
    );
