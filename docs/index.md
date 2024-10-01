# TLDR; Docker Demo

Start the onesparse demo docker container, substitute suitable
password and container name:

```
docker run -d -e POSTGRES_PASSWORD=password -it --name onesparse-demo-container onesparse/onesparse:0.1.0
```

The container will start detached in the background, verify it's
running with 'docker ps'.  Now exec a psql process to interact with
the container:

```
$ docker exec -u postgres -it onesparse-demo-container psql
psql (17.0 (Debian 17.0-1.pgdg120+1))
Type "help" for help.

postgres=# select 'i4'::matrix;
 matrix 
--------
 i4[]
(1 row)

postgres=# 
```

See the doctests for some examples for creating matrices and vectors.
You can verify the doctests with docker using:

```
$ docker exec -u postgres -it onesparse-demo-container make installcheck
```

# Summary

OneSparse Postgres extends the [PostgreSQL](https://postgresql.org)
object relational database with the state-of-the-art
[SuiteSparse:GraphBLAS](http://faculty.cse.tamu.edu/davis/GraphBLAS.html)
high performance dense/sparse linear algebra library.  SuiteSparse
contains thousands of algorithms optimizing tasks for algebraic graph
traversal, machine learning, AI, science, and engineering.
SuiteSparse's powerful built-in JIT compiler can target multiple
hardware architectures, including GPUs, with no changes to your code.

Join us in the [OneSparse Discussions
Board](https://github.com/OneSparse/OneSparse/discussions) and say
hi!.
