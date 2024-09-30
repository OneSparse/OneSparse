#!/bin/bash

DB_HOST="onesparse-test-db"
DB_NAME="postgres"
SU="postgres"
EXEC="docker exec $DB_HOST"
EXECIT="docker exec -it $DB_HOST"
export BUILDKIT_PROGRESS=plain

echo force rm previous container
docker rm -f onesparse-test

set -e

echo building test image
docker build --build-arg UID=$(id -u) --build-arg GID=$(id -g) . -t onesparse/test

echo running test container
docker run --user $(id -u):$(id -g) --mount type=bind,source=$(pwd),target=/home/postgres/onesparse,bind-propagation=rshared --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -d --name "$DB_HOST" onesparse/test

$EXECIT pg_ctl start
# $EXEC make clean
# $EXEC make install

echo waiting for database to accept connections
until
    $EXEC \
	    psql -o /dev/null -t -q -U "$SU" \
        -c 'select pg_sleep(1)' \
	    2>/dev/null;
do sleep 1;
done

$EXEC make installcheck

echo destroying test container and image

docker rm --force "$DB_HOST"
