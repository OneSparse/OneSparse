#!/bin/sh

set -eu

test_dir=$(dirname "$0")

if ! perl -MTAP::Parser::SourceHandler::pgTAP -e 1; then
    echo "pg_prove requires TAP::Parser::SourceHandler::pgTAP" >&2
    exit 1
fi

exec pg_prove \
    --dbname "${PGDATABASE:-postgres}" \
    --username "${PGUSER:-postgres}" \
    --ext .sql \
    --recurse \
    "$test_dir/sql"
