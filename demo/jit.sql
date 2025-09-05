create extension if not exists onesparse;

drop type demo_all_types;
CREATE TYPE demo_all_types AS (
    f_bool      bool,
    f_int2      int2,
    f_int4      int4,
    f_int8      int8,
    f_float4    float4,
    f_float8    float8,
    f_oid       oid,
    f_uuid      uuid,
    f_money     money,
    f_bpchar    char(16),
    f_bit       bit(64),
    f_varbit    varbit(20)
);

INSERT INTO user_defined_type (name, type_def) VALUES
    ('demo_all_types', jit_type('demo_all_types'));

SELECT (scalar_row(row_scalar(t), NULL::demo_all_types)).*
FROM (SELECT ROW(
    true,
    1::int2,
    2::int4,
    3::int8,
    1.25::float4,
    2.5::float8,
    42::oid,
    gen_random_uuid(),
    123::money,
    'ABC'::char(3),
    B'1010'::bit(4),
    B'11111111111111111111'::varbit(20)
    )::demo_all_types AS t) s;
