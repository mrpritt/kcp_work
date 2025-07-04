OUT_FILE="bb.csv"

echo "instance,profit,LBi,UBi,h1,h2,nodes,ub_l2_cuts,ub_mt_cuts,ub_p_cuts,valid" > $OUT_FILE

parallel --bar -j 4 'echo -n "{},"; ./build/bin/bb <(bzcat {})' ::: ~/ic2024/data/set2/C1/*.bz2 >> $OUT_FILE

Rscript ./bounds/format_instance_name.R $OUT_FILE
Rscript test.R
Rscript compare.R
