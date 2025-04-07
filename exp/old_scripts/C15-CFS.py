"""
Run set II C15 instances on Coniglio's CFS solver

"""
import re
import glob
import time
from kiwi import BaseReporter, Kiwi, Experiment, DEFAULT_SETTINGS, JSONReporter, gen_extractor

# Create custom reporter
class Reporter(BaseReporter):
    @staticmethod
    def generate(results: list[dict]):
        report_id = time.time_ns()
        with open(f"results-{report_id}.csv", "w") as f:
            f.write("instance;weight;time\n")
            for exp in results:
                for run in exp['_runs']:
                    row = f"{exp['name']};"
                    row += f"{run['@weight']};"
                    row += f"{run['@time']}"
                    row += "\n"
                    f.write(row)

pattern = r"\[w:(\d+),([0-9.eE+-]+)s\]"
matcher = [
    ('@weight', int, 0),
    ('@time', float, 0),
]
extractor = gen_extractor(pattern, matcher)

def main():
    SETTINGS = DEFAULT_SETTINGS.copy()
    SETTINGS['threads'] = 4

    KiwiRunner = Kiwi(SETTINGS)
    files = glob.glob('/home/gustavo/ic2024/data/set2/C15/*.bz2')[:8]

    for file in files:
        id = f"{file.split('/')[-1]}"
        exp_name = f"{id}"
        cmd = f"bash -c 'timeout 60s /home/gustavo/kcp_work/bin/CFS <(bzcat {file})'"
        exp = Experiment(cmd, exp_name)
        exp.attach_output_handler(extractor)
        KiwiRunner.add_experiment(exp)


    KiwiRunner.run_all()

    KiwiRunner.gen_report(Reporter)

if __name__ == '__main__':
    main()
