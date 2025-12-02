\setenv draw_source :draw_source
\! echo $draw_source | ./sql/gt-dot2svg.py 2>/dev/null | sed '/<!DOCTYPE/,/>/d; /<?xml/,/>/d' | sed '1i<div>' | sed '$a</div>\n' | sed 's/^/-- /'
