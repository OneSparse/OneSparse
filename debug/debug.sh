#!/bin/bash

if [ -z "$1" ]; then
  echo "Usage: $0 <breakpoint>"
  exit 1
fi

BREAKPOINT=$1

# Path to the PostgreSQL binary, update this if needed
POSTGRES_BINARY=$(which postgres)

if [ -z "$POSTGRES_BINARY" ]; then
  echo "Error: postgres binary not found. Ensure PostgreSQL is installed and in your PATH."
  exit 1
fi

# Run gdb with commands to set breakpoint and start the process
gdb -q -ex "set pagination off" \
    -ex "set debuginfod enabled on" \
    -ex "set breakpoint pending on" \
    -ex "file $POSTGRES_BINARY" \
    -ex "break $BREAKPOINT" \
    -ex "run --single -D /home/postgres/data"
