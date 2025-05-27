# CSV_FILES := $(filter-out table5.csv, $(wildcard *.csv))

.PHONY: clean

main.pdf: main.tex ub_l2_v.tex ub_l2_i.tex ub_l2_v_ff.tex ub_l2_i_ff.tex ub_l2_v_ffr.tex
	pdflatex main.tex

ub_l2_v.tex ub_l2_i.tex ub_l2_v_ff.tex ub_l2_i_ff.tex ub_l2_v_ffr.tex: latex.R
	Rscript latex.R

clean:
	rm -f ub_l2_v.tex main.pdf main.aux main.fdb_latexmk main.fls main.log ub_l2_v.tex ub_l2_i.tex ub_l2_v_ff.tex ub_l2_i_ff.tex ub_l2_v_ffr.tex

