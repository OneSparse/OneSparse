\setenv draw_source :draw_source
\! echo $draw_source | ./sql/gt-dot2svg.py | sed '/<!DOCTYPE/,/>/d; /<?xml/,/>/d' | sed '1i<div>' | sed '$a</div>\n' | sed 's/^/-- /'
