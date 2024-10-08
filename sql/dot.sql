\setenv dot_source :dot_source
\! echo $dot_source | dot -Tsvg | sed '/<!DOCTYPE/,/>/d; /<?xml/,/>/d' | sed '1i<div>' | sed '$a</div>' | sed 's/^/-- /'
