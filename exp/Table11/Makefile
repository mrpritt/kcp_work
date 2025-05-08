CSV_FILES := $(filter-out table11.csv, $(wildcard *.csv))

.PHONY: clean

main.pdf: table.tex main.tex
	pdflatex main.tex

table11.csv: format.R $(CSV_FILES)
	Rscript format.R

table.tex: latex.R table11.csv
	Rscript latex.R

clean:
	rm -f table.tex table11.csv main.pdf main.aux main.fdb_latexmk main.fls main.log
