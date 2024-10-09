
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
    ('int32',
        random_matrix(4, 4, 10, 4, :seed + 0.3),
        random_matrix(4, 4, 10, 4, :seed + 0.4),
        'int32(4)[1:2]'::vector,
        'int32(4)[2:3]'::vector,
        'ainv_int32',
        'valuegt_int32',
        'times_int32',
        'plus_monoid_int32',
        'plus_times_int32');
