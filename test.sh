#!/bin/bash

DB_HOST="pggraphblas-test-db"
DB_NAME="postgres"
SU="postgres"
EXEC="docker exec $DB_HOST"
EXECIT="docker exec -it $DB_HOST"

echo force rm previous container
docker rm -f pggraphblas-test-db

set -e

echo building test image
docker build . -t pggraphblas/test

echo running test container
docker run -v $(pwd)/tests/:/tests -v $(pwd)/demo/:/demo -v $(pwd)/../LAGraph/:/LAGraph --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -d --name "$DB_HOST" pggraphblas/test

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


if [ $# -eq 0 ]
then
    echo running tests
    $EXEC tmux new-session -d -s pggraphblas 
    $EXECIT tmux attach-session -t pggraphblas
else
    echo running repl
    $EXEC tmux new-session -d -s pggraphblas $*
    $EXECIT tmux attach-session -t pggraphblas
fi

echo destroying test container and image

docker rm --force "$DB_HOST"
