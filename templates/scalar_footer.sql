
CREATE TABLE grb_type2 (
    name text PRIMARY KEY,
    short_name text UNIQUE,
    type_code integer DEFAULT 0,
    struct text
    );

CREATE INDEX ON grb_type2 (type_code);

INSERT INTO grb_type2 (name, short_name, type_code) VALUES
    ('bool',   'b',  1),
    ('int8',   'i',  2),
    ('uint8',  'u',  3),
    ('int16',  'i2', 4),
    ('uint16', 'u2', 5),
    ('int32',  'i4', 6),
    ('uint32', 'u4', 7),
    ('int64',  'i8', 8),
    ('uint64', 'u8', 9),
    ('fp32',   'f4', 10),
    ('fp64',   'f8', 11),
    ('fc32',   'c4', 7070),
    ('fc64',   'c8', 7071)
;
