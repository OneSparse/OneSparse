\setenv draw_source :draw_source
\! echo $draw_source | gnuplot -e "set terminal svg size 600,600; set output '-'; unset key; set view map; set pm3d map explicit; splot '-' using 1:2:3 with pm3d" | sed '/<!DOCTYPE/,/>/d; /<?xml/,/>/d' | sed '1i<div>' | sed '$a</div>\n' | sed 's/^/-- /'
