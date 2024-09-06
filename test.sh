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
docker build . -t onesparse/test

echo running test container
docker run -v $(pwd)/tests/:/tests -v $(pwd)/demo/:/demo -v --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -d --name "$DB_HOST" onesparse/test

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
    $EXEC tmux new-session -d -s onesparse
    $EXECIT tmux attach-session -t onesparse
else
    echo running repl
    $EXEC tmux new-session -d -s onesparse $*
    $EXECIT tmux attach-session -t onesparse
fi

echo destroying test container and image

docker rm --force "$DB_HOST"
