#!/bin/bash

DB_HOST="onesparse-test-db"
DB_NAME="postgres"
SU="postgres"
export BUILDKIT_PROGRESS=plain

set -e

if [ -z "${CONTAINER_RUNTIME:-}" ]; then
    if command -v docker >/dev/null 2>&1; then
        CONTAINER_RUNTIME=docker
    elif command -v podman >/dev/null 2>&1; then
        CONTAINER_RUNTIME=podman
    else
        echo "ERROR: docker or podman is required to run tests" >&2
        exit 1
    fi
elif ! command -v "$CONTAINER_RUNTIME" >/dev/null 2>&1; then
    echo "ERROR: container runtime '$CONTAINER_RUNTIME' was not found" >&2
    exit 1
fi

container_exec() {
    "$CONTAINER_RUNTIME" exec "$DB_HOST" "$@"
}

cleanup() {
    "$CONTAINER_RUNTIME" rm --force "$DB_HOST" >/dev/null 2>&1 || true
}

echo "removing previous test container"
cleanup

echo building test image
if [ "$CONTAINER_RUNTIME" = podman ]; then
    BUILD_JOBS=2
else
    BUILD_JOBS=
fi
"$CONTAINER_RUNTIME" build -f Dockerfile-debug --build-arg BUILD_JOBS="$BUILD_JOBS" . -t onesparse/test

"$CONTAINER_RUNTIME" run --mount type=bind,source="$(pwd)",target=/home/postgres/onesparse --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -d --name "$DB_HOST" onesparse/test
trap cleanup EXIT

container_exec pg_ctl start

echo waiting for database to accept connections
until
    container_exec \
        psql -o /dev/null -t -q -U "$SU" \
        -c 'select pg_sleep(1)' \
        2>/dev/null;
do sleep 1;
done

container_exec make installcheck
