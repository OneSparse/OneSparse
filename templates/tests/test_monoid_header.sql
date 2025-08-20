\pset linestyle unicode
\pset border 2
-- # Monoid
--
-- In [abstract
-- algebra](https://en.wikipedia.org/wiki/Abstract_algebra "Abstract
-- algebra"), a **monoid** is a set equipped with an
-- [associative](https://en.wikipedia.org/wiki/Associative
-- "Associative") [binary
-- operation](https://en.wikipedia.org/wiki/Binary_operation "Binary
-- operation") and an [identity
-- element](https://en.wikipedia.org/wiki/Identity_element "Identity
-- element"). For example, the nonnegative
-- [integers](https://en.wikipedia.org/wiki/Integer "Integer") with
-- addition form a monoid, the identity element being 0.
--
-- ## Monoid identity
--
select ident('plus_monoid_int8');

select ident('plus_monoid_int16');

select ident('plus_monoid_fp32');

select ident('min_monoid_int8');

select ident('min_monoid_int16');

select ident('min_monoid_fp32');

select ident('max_monoid_int8');

select ident('max_monoid_int16');

select ident('max_monoid_fp32');

-- ## Monoid terminal
--
-- Monoid terminals tell SuiteSparse when it can terminate a large
-- array operation because the terminal value has been found.
--
select terminal('plus_monoid_int8');

select terminal('plus_monoid_int16');

select terminal('min_monoid_int8');

select terminal('min_monoid_int16');

select terminal('max_monoid_int8');

select terminal('max_monoid_int16');

select terminal('land_monoid_bool');

select terminal('lor_monoid_bool');

-- ## Builtin SuiteSparse Monoids
--
-- | OneSparse Name | SuiteSparse Name |
-- |----------------|------------------|
{names}
