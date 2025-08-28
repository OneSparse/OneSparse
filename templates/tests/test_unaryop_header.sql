\pset linestyle unicode
\pset border 2
-- # UnaryOp
--
-- UnaryOps apply a function to an element, returning an element.
--
-- ## User Defined Unary Operators
--
-- User defined unaryop functions can be registered with OneSparse by
-- inserting them into the `onesparse.user_defined_unaryop` table.
-- They can then be JIT compiled into kernels and used in any function
-- that takes a `unaryop` argument:
--

show onesparse.jit_control;  -- This must be set to 'on' in postgres config.

--
insert into user_defined_unaryop (name, ztype, xtype, func) values
    ('abs_udf', 'int64', 'int64',
    $$
    void abs_udf(int64_t *z, const int64_t *x)  {{
        (*z) = llabs((*x));
       }};
    $$);

--

select apply(random_vector('int64', 4, 'inf', 42), 'abs_udf'::unaryop);

-- ## Built-in Operators
--
-- | OneSparse Name | SuiteSparse Name |
-- |----------------|------------------|
{names}
