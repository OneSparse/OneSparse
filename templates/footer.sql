
CREATE TABLE grb_type (
    name text PRIMARY KEY,
    struct text
    );

INSERT INTO grb_type (name) VALUES
    ('BOOL'),
    ('INT8'),
    ('INT16'),
    ('INT32'),
    ('INT64'),
    ('UINT8'),
    ('UINT16'),
    ('UINT32'),
    ('UINT64'),
    ('FP32'),
    ('FP64'),
    ('FC32'),
    ('FC64');
