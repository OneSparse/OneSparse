
create extension if not exists onesparse;
create table if not exists onesparse.test_fixture (
    t type,
    a matrix,
    b matrix,
    u vector,
    v vector,
    unaryop unaryop,
    indexunaryop indexunaryop,
    binaryop binaryop,
    monoid monoid,
    semiring semiring
    );

delete from onesparse.test_fixture;
select 0.0 seed \gset
insert into onesparse.test_fixture (T, A, B, u, v, unaryop, indexunaryop, binaryop, monoid, semiring) values
    ('int16',
        random_matrix(5, 5, 10, 10, :seed + 0.1),
        random_matrix(5, 5, 10, 10, :seed + 0.2),
        random_vector(5, 3, 5, :seed + 0.11),
        random_vector(5, 3, 5, :seed + 0.21),
        'ainv_int16',
        'valuegt_int16',
        'times_int16',
        'plus_monoid_int16',
        'plus_times_int16'),
    ('int32',
        random_matrix(5, 5, 10, 10, :seed + 0.3),
        random_matrix(5, 5, 10, 10, :seed + 0.4),
        random_vector(5, 3, 5, :seed + 0.3),
        random_vector(5, 3, 5, :seed + 0.4),
        'ainv_int32',
        'valuegt_int32',
        'times_int32',
        'plus_monoid_int32',
        'plus_times_int32'),
    ('int64',
        random_matrix(5, 5, 10, 10, :seed + 0.5),
        random_matrix(5, 5, 10, 10, :seed + 0.6),
        random_vector(5, 3, 5, :seed + 0.5),
        random_vector(5, 3, 5, :seed + 0.6),
        'ainv_int64',
        'valuegt_int64',
        'times_int64',
        'plus_monoid_int64',
        'plus_times_int64')
    ;
