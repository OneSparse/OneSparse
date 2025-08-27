\pset linestyle unicode
\pset border 2
-- # UnaryOp
--
-- UnaryOps apply a function to an element, returning an element.
--
-- ## User Defined Unary Ops
--
insert into user_defined_unaryop (name, ztype, xtype, func) values
    ('abs_udf', 'int64', 'int64',
    $$
    void abs_udf(int64_t *z, const int64_t *x)  {{
        (*z) = llabs((*x));
       }};
    $$);

select apply(random_vector('int64', 4, 'inf', 42), 'abs_udf'::unaryop);

--
-- | OneSparse Name | SuiteSparse Name |
-- |----------------|------------------|
{names}
