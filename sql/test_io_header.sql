-- # IO Operations
--
-- select serialize(random_matrix(10, 10, 10, seed=>0.42, max=>99)) as matrixbytes \gset
\set grbfile 'testmatrix.grb'
select serialize(random_matrix(10, 10, 10, seed=>0.42, max=>99)) as matrixbytes \gset
select deserialize(:'matrixbytes') = random_matrix(10, 10, 10, seed=>0.42, max=>99);
select serialize_file(random_matrix(10, 10, 10, seed=>0.42, max=>99), :'grbfile');
select deserialize_file(:'grbfile') = random_matrix(10, 10, 10, seed=>0.42, max=>99);
