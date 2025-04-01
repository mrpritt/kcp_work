"""
Template program runner using Kiwi
"""
import re
import glob
import time
from kiwi import BaseReporter, Kiwi, Experiment, DEFAULT_SETTINGS, JSONReporter, gen_extractor

# To edit cmd args must go below
EXEC='~/mcqdw/release/tools/misc/maxcliquedynweight'
FILE_GLOB='/home/gustavo/ic2024/kpc/benchmark/*.ampl'
MAX_TIME=60
THREADS=4

pattern = r"(?:Size = (\d+) Vertices = \[.*?\] Weight = (\d+)) n = (\d+) cb = ([\d.]+) kb = ([\d.]+) rb = ([\d.]+) |(?:Time = (\d+) ms)"
matcher = [
    ('@size', int, 0),
    ('@weight', int, 0),
    ('@nodes', int, 0),
    ('@avg_c', float, 0.0),
    ('@avg_min_k', float, 0.0),
    ('@avg_ratio', float, 0.0),
    ('@time', int, 0),
]
extractor = gen_extractor(pattern, matcher)

# Create custom reporter
class Reporter(BaseReporter):
    @staticmethod
    def generate(results: list[dict]):
        report_id = time.time_ns()
        with open(f"results-{report_id}.csv", "w") as f:
            f.write("instance;weight;size;nodes;avg_cc;avg_min_k;avg_ratio;time\n")
            for exp in results:
                for run in exp['_runs']:
                    row = f"{exp['name']};"
                    row += f"{run['@weight']};"
                    row += f"{run['@size']};"
                    row += f"{run['@nodes']};"
                    row += f"{run['@avg_c']};"
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

    KiwiRunner.gen_report(JSONReporter)
    KiwiRunner.gen_report(Reporter)

if __name__ == '__main__':
    main()
