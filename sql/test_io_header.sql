-- # Input/Output Operations
--
-- OneSparse comes with several functions to read and write different
-- text and binary formats for graph data.
--
-- ## SuiteSparse Binaries
--
-- SuiteSparse provides a very efficient compressed binary
-- representation for matrix and vector objects.

\set grbfile 'testmatrix.grb'

select serialize(random_matrix('int8', 10, 10)) as matrixbytes \gset
select deserialize(:'matrixbytes') = random_matrix('int8', 10, 10);

select serialize_file(random_matrix('int8', 10, 10), :'grbfile');
select deserialize_file(:'grbfile') = random_matrix('int8', 10, 10);

-- ## MatrixMarket Format
--
-- The Matrix Market format is a standard graph distribution format
-- used by the SuiteSparse Matrix Library.  OneSparse can read these
-- files and return matrix objects.

-- ## Large Object Storage
--
-- Matrices bigger than the 1GB TOAST limit can be stored in the Large
-- Object table in Postgres, which supports storage up to 4TB.
