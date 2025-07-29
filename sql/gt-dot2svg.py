#!/usr/bin/env python3
import sys, tempfile, re
from graph_tool.all import load_graph, graph_draw

g = load_graph(sys.stdin, fmt="dot")

with tempfile.NamedTemporaryFile(suffix=".svg", delete=False) as tmp:
    fname = tmp.name

graph_draw(g, output=fname, fit_view=True)

with open(fname, "r", encoding="utf-8") as f:
    svg = f.read()

# Patch in missing xlink namespace if needed
if 'xmlns:xlink' not in svg:
    svg = re.sub(r'<svg\b', '<svg xmlns:xlink="http://www.w3.org/1999/xlink"', svg, count=1)

sys.stdout.buffer.write(svg.encode("utf-8"))
