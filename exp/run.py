#! /bin/python3
"""
Template program runner using Kiwi
"""
import re
import glob
import time
from kiwi import BaseReporter, Kiwi, Experiment, DEFAULT_SETTINGS, JSONReporter, gen_extractor

# To edit cmd args must go below
EXEC='~/mcqdw/bin/mcqdw'
FILE_GLOB='/home/gustavo/ic2024/kpc/benchmark/*.ampl'
MAX_TIME=60
THREADS=4
OUT_FILE='benchmark_mcqdw60.csv'

pattern = r"(?:Size = (\d+) Vertices = \[.*?\] Weight = (\d+) n = (\d+) ncw = (\d+) cb = ([\d.]+) kb = ([\d.]+) rb = ([\d.]+))|(?:Time = (\d+) ms)"
matcher = [
    ('@size', int, 0),              # Number of selected items
    ('@weight', int, 0),            # Solution weight (profit)
    ('@nodes', int, 0),             # Number of expanded nodes during B&B (Arrived "dead")
    ('@n_cut_weight', int, 0),      # Number of nodes cut by weight restriction
    ('@avg_cc', float, 0.0),        # Average number of colorclasses per color_sort()
    ('@avg_min_k', float, 0.0),     # Average value of min_k per color_sor()
    ('@avg_ratio', float, 0.0),     # Average min_k / no_cc per color_sort()
    ('@time', int, 0),              # Time to solve
]
extractor = gen_extractor(pattern, matcher)

# Create custom reporter
class Reporter(BaseReporter):
    @staticmethod
    def generate(results: list[dict]):
        report_id = time.time_ns()
        with open(f"{OUT_FILE}", "w") as f:
            f.write("instance;weight;size;nodes;n_cut_weight;avg_cc;avg_min_k;avg_ratio;time\n")
            for exp in results:
                for run in exp['_runs']:
                    row = f"{exp['name']};"
                    row += f"{run['@weight']};"
                    row += f"{run['@size']};"
                    row += f"{run['@nodes']};"
                    row += f"{run['@n_cut_weight']};"
                    row += f"{run['@avg_cc']};"
                    row += f"{run['@avg_min_k']};"
                    row += f"{run['@avg_ratio']};"
                    row += f"{run['@time']}\n"
                    f.write(row)



def main():
    SETTINGS = DEFAULT_SETTINGS.copy()
    SETTINGS['threads'] = THREADS

    KiwiRunner = Kiwi(SETTINGS)
    files = glob.glob(FILE_GLOB)

    for file in files:
        id = f"{file.split('/')[-1]}"
        exp_name = f"{id}"
        args = f"bash -c 'timeout {MAX_TIME}s {EXEC} --graph_file {file} --strategy kpc'"
        exp = Experiment(args, exp_name)
        exp.attach_output_handler(extractor)
        KiwiRunner.add_experiment(exp)


    KiwiRunner.run_all()

    KiwiRunner.gen_report(Reporter)

if __name__ == '__main__':
    main()
