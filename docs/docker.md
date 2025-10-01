# Quick Start with Docker

Start the onesparse demo docker container, substitute suitable
password and container name:

```
docker run -d -e POSTGRES_PASSWORD=password -it --name onesparse-demo onesparse/onesparse:demo
```

The container will start detached in the background, verify it's
running with 'docker ps'.  Now exec a psql process to interact with
the container:

```
$ docker exec -u postgres -it onesparse-demo psql
psql (18.0 (Debian 18.0-1.pgdg13+3))
Type "help" for help.

postgres# select print(random_matrix('int8', 10, 10)) as random_matrix;
                  random_matrix
--------------------------------------------------
        0   1   2   3   4   5   6   7   8   9    +
     ────────────────────────────────────────    +
   0│                                            +
   1│                                            +
   2│   5  11                         -99        +
   3│ -16                 -10                    +
   4│                         -55                +
   5│                                            +
   6│      22      26                            +
   7│         -63  78                            +
   8│                                            +
   9│                                            +
 
(1 row)
```

Check out the [Getting Started](test_examples_header.md) for some
examples and the [Matrix Documentation](test_matrix_header.md).  
