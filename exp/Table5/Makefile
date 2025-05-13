CSV_FILES := $(filter-out table5.csv, $(wildcard *.csv))

.PHONY: clean

main.pdf: table.tex main.tex
	pdflatex main.tex

table5.csv: format.R $(CSV_FILES)
	Rscript format.R

table.tex: latex.R table5.csv
	Rscript latex.R

clean:
	rm -f table.tex table5.csv main.pdf main.aux main.fdb_latexmk main.fls main.log
