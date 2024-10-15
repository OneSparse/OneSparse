CREATE FUNCTION vector_agg_{type.pgtype} (state vector, i bigint, v {type.pgtype})
RETURNS vector
AS '$libdir/onesparse', 'vector_agg_{type.name}'
LANGUAGE C STABLE;

CREATE AGGREGATE vector_agg (i bigint, v {type.pgtype} )
    (
    SFUNC=vector_agg_{type.pgtype},
    STYPE=vector,
    FINALFUNC=vector_agg_final
    );
