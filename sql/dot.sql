\setenv dot_source :dot_source
\! echo $dot_source | dot -Tsvg -Nnodesep=0.1 -Nranksep=0.1 -Nwidth=0.05 -Nheight=0.05 -Nfontsize=8 -Efontsize=8 | sed '/<!DOCTYPE/,/>/d; /<?xml/,/>/d' | sed '1i<div>' | sed '$a</div>\n' | sed 's/^/-- /'
