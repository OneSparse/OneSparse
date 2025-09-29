\pset linestyle unicode
\pset border 2
-- # BinaryOp
--
-- In [mathematics](https://en.wikipedia.org/wiki/Mathematics
-- "Mathematics"), a **binary operation** or **dyadic operation** is a
-- rule for combining two
-- [elements](https://en.wikipedia.org/wiki/Element_(mathematics)
-- "Element (mathematics)") (called
-- [operands](https://en.wikipedia.org/wiki/Operands "Operands")) to
-- produce another element. More formally, a binary operation is an
-- [operation](https://en.wikipedia.org/wiki/Operation_(mathematics)
-- "Operation (mathematics)") of
-- [arity](https://en.wikipedia.org/wiki/Arity "Arity") two.
--
-- ## User Defined Binary Operators
--
-- User defined functions can be registered with OneSparse by
-- inserting them into the `onesparse.user_defined_binaryop` table.
-- They can then be JIT compiled into kernels and used in any function
-- that takes a `binaryop` argument.
--
-- To use User Defined Operators, OneSparse must enable the
-- SuiteSparse JIT compiler by setting `onesparse.jit_control = 'on'`
-- in your `postgresql.conf`.  The default is `off`.  For maximum
-- security, only enable JIT compilation on development servers and
-- turn it off in production.

show onesparse.jit_control;  -- This must be set to 'on' in postgres config.

--
-- Insert the expected function types and function body into the
-- table.  Here is an example function that computes the [Hamming
-- Distance](https://en.wikipedia.org/wiki/Hamming_distance) between
-- two elements.  See the SuiteSparse User Guide for more details:
--

insert into user_defined_binaryop (name, ztype, xtype, ytype, func) VALUES
    ('binary_hamming_dist', 'int64', 'int64', 'int64',
    $$
    void binary_hamming_dist (int64_t *z, int64_t *x, int64_t *y) {{
        (*z) = __builtin_popcountll((*x)^(*y));
       }};
    $$);

-- The new operator can now be used in functions that take `binaryop`
-- operators like `eadd`:

select eadd(random_vector('int64', 16, 'inf', 42),
            random_vector('int64', 16, 'inf', 43),
            'binary_hamming_dist');

-- ## Built-in Operators
--
-- | OneSparse Name | SuiteSparse Name |
-- |----------------|------------------|
{names}
