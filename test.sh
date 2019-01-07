#!/bin/bash

DB_HOST="pggraphblas-test-db"
DB_NAME="postgres"
SU="postgres"
EXEC="docker exec $DB_HOST"

echo force rm previous container
docker rm -f pggraphblas-test-db

set -e

echo building test image
docker build . -t pggraphblas/test

echo running test container
docker run --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -d --name "$DB_HOST" pggraphblas/test

$EXEC make clean
$EXEC make install

echo waiting for database to accept connections
until
    $EXEC \
	    psql -o /dev/null -t -q -U "$SU" \
        -c 'select pg_sleep(1)' \
	    2>/dev/null;
do sleep 1;
done

echo running tests
$EXEC pg_prove -U "$SU" /pggraphblas/test.sql

if [ $# -eq 1 ]
then
    echo argument passed so waiting.  Press any key to finish.
    read tmp
fi

echo destroying test container and image

docker rm --force "$DB_HOST"
