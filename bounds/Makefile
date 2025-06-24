OUT_TEX_FILES := ub_l2_v.tex ub_l2_i.tex ub_l2_v_ff.tex ub_l2_i_ff.tex \
		 ub_l2_v_ffr.tex ub_l2_comp.tex ub_l2_ascend.tex ub_l2_descend.tex

.PHONY: clean

main.pdf: main.tex $(OUT_TEX_FILES)
	pdflatex main.tex

$(OUT_TEX_FILES): latex.R
	Rscript latex.R

clean:
	rm -f main.pdf main.aux main.fdb_latexmk main.fls main.log $(OUT_TEX_FILES)

