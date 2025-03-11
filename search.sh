# GOAL: find a small instance where mcqdw_test fails instance_filename='tmp.ampl'
instance_filename='out.ampl'
kpc_output='kpc_out.txt'
mwis_output='mwis_out.txt'
mcqdw_main_out='mcqdw_main_out.txt'
mcqdw_test_out='mcqdw_test_out.txt'
found=false

while [ "$found" == false ]
do
	# Generate instance
	python ../ic2024/kpc/benchmark/gen_kpc.py $instance_filename 15 0.4 0.1 --type strcorr --format ampl

	# Solve KPC using CPLEX
	./build/build/bin/cplex_kpc $instance_filename ampl > $kpc_output
	kpc_v=$(grep -oP 'KPC:\s*\K\d+' $kpc_output)

	# Solve MWIS using CPLEX
	./build/build/bin/cplex_mwis $instance_filename ampl > $mwis_output
	mwis_v=$(grep -oP 'MWIS:\s*\K\d+' $mwis_output)

	echo "KPC: $kpc_v   MWIS: $mwis_v"

	# Assert KPC != MWIS
	if [ "$kpc_v" -ne "$mwis_v" ]; then
		# Run mcqdw_main and assert equal value
		../mcqdw/mcqdw_main --graph_file $instance_filename --strategy kpc > $mcqdw_main_out
		mcqdw_main_v=$(grep -oP 'Weight =\s*\K\d+' $mcqdw_main_out)

		# Run mcqdw_test and assert equal value
		../mcqdw/mcqdw_test --graph_file $instance_filename --strategy kpc > $mcqdw_test_out
		mcqdw_test_v=$(grep -oP 'Weight =\s*\K\d+' $mcqdw_test_out)

		echo "MAIN: $mcqdw_main_v   TEST: $mcqdw_test_v"

		if [ "$mcqdw_test_v" -ne "$kpc_v" ]; then
			found=true
		fi

		echo ""
	fi
done
