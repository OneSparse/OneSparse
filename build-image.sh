#!/bin/bash

set -euo pipefail

if [ "$#" -ne 2 ]; then
    echo "usage: $0 <dockerfile> <image-tag>" >&2
    exit 2
fi

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
. "$SCRIPT_DIR/versions.env"

if [ -z "${CONTAINER_RUNTIME:-}" ]; then
    if command -v docker >/dev/null 2>&1; then
        CONTAINER_RUNTIME=docker
    elif command -v podman >/dev/null 2>&1; then
        CONTAINER_RUNTIME=podman
    else
        echo "ERROR: docker or podman is required to build an image" >&2
        exit 1
    fi
elif ! command -v "$CONTAINER_RUNTIME" >/dev/null 2>&1; then
    echo "ERROR: container runtime '$CONTAINER_RUNTIME' was not found" >&2
    exit 1
fi

"$CONTAINER_RUNTIME" build \
    --build-arg POSTGRES_VERSION="$POSTGRES_VERSION" \
    --build-arg POSTGRES_MAJOR="$POSTGRES_MAJOR" \
    --build-arg GRAPHBLAS_VERSION="$GRAPHBLAS_VERSION" \
    --build-arg LAGRAPH_VERSION="$LAGRAPH_VERSION" \
    -f "$1" . -t "$2"
