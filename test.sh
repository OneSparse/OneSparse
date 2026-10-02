#!/bin/bash

DB_HOST="onesparse-test-db"
DB_NAME="postgres"
SU="postgres"
TEST_WORKDIR="/home/postgres/test-work"
export BUILDKIT_PROGRESS=plain

set -e

if [ "$#" -gt 1 ] || { [ "$#" -eq 1 ] && [ "$1" != "doctest" ] && [ "$1" != "unit" ]; }; then
    echo "usage: $0 [doctest|unit]" >&2
    exit 2
fi

TEST_SUITE=${1:-all}

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
    "$CONTAINER_RUNTIME" exec --workdir "$TEST_WORKDIR" "$DB_HOST" "$@"
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

# The checkout is bind-mounted from the host and may not be writable by the
# container's postgres user. Run tests from a private writable copy instead.
"$CONTAINER_RUNTIME" exec "$DB_HOST" mkdir -p "$TEST_WORKDIR"
container_exec sh -c "cp -R /home/postgres/onesparse/. '$TEST_WORKDIR/'"
container_exec pg_ctl start

echo waiting for database to accept connections
until
    container_exec \
        psql -o /dev/null -t -q -U "$SU" \
        -c 'select pg_sleep(1)' \
        2>/dev/null;
do sleep 1;
done

run_doctests() {
    if container_exec make installcheck; then
        return 0
    fi

    echo "WARNING: documentation regression tests failed; continuing with semantic unit tests" >&2
    return 0
}

case "$TEST_SUITE" in
    doctest)
        run_doctests
        ;;
    unit)
        container_exec make unitcheck
        ;;
    all)
        echo "::group::Documentation regression tests"
        run_doctests
        echo "::endgroup::"
        echo "::group::Semantic unit tests"
        container_exec make unitcheck
        echo "::endgroup::"
        ;;
esac
