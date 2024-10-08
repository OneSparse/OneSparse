#!/bin/bash
. .virt/bin/activate
python3 generate.py onesparse/onesparse--0.1.0.sql
docker rm -f onesparse-test-db && ./test.sh make installcheck
cp results/*.out expected/
python3 doctestify.py
mkdocs build
mkdocs serve
