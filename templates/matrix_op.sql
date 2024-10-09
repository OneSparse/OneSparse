CREATE FUNCTION matrix_agg_{type.pgtype} (state matrix, i bigint, j bigint, v {type.pgtype})
RETURNS matrix
AS '$libdir/onesparse', 'matrix_agg_{type.name}'
LANGUAGE C STABLE;

CREATE AGGREGATE matrix_agg (i bigint, j bigint, v {type.pgtype} )
    (
    SFUNC=matrix_agg_{type.pgtype},
    STYPE=matrix,
    FINALFUNC=matrix_agg_final
    );
