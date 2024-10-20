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

CREATE FUNCTION dense_matrix(
    nrows integer,
    ncols integer,
    fill {type.pgtype})
        RETURNS matrix
        RETURN assign(matrix('{type.name}', nrows, ncols), fill);
