#!/bin/bash
. .virt/bin/activate
python3 generate.py
./test.sh
cp results/*.out expected/
python3 doctestify.py
mkdocs build
mkdocs serve
