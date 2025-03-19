"""
Run set II C15 instances on our modification of Rozman's B&B (insidrug)

Last run: 19-03-2025
"""
import re
import glob
import time
from kiwi import BaseReporter, Kiwi, Experiment, DEFAULT_SETTINGS, JSONReporter, gen_extractor

pattern = r"(?:Size = (\d+) Vertices = \[.*?\] Weight = (\d+))|(?:Time = (\d+) ms)"
# pattern = r"Size = \d+ Vertices = \[.*?\] Weight = (\d+)"
matcher = [
    ('@size', int, 0),
    ('@weight', int, 0),
    ('@time', int, 0),
]
extractor = gen_extractor(pattern, matcher)

# Create custom reporter
class Reporter(BaseReporter):
    @staticmethod
    def generate(results: list[dict]):
        report_id = time.time_ns()
        with open(f"results-{report_id}.csv", "w") as f:
            f.write("instance;weight;size;time\n")
            for exp in results:
                for run in exp['_runs']:
                    row = f"{exp['name']};"
                    row += f"{run['@weight']};"
                    row += f"{run['@size']};"
                    row += f"{run['@time']}\n"
                    f.write(row)



def main():
    SETTINGS = DEFAULT_SETTINGS.copy()
    SETTINGS['threads'] = 4

    KiwiRunner = Kiwi(SETTINGS)
    files = glob.glob('/home/gustavo/ic2024/data/set2/C15/*.bz2')[4:8]

    for file in files:
        id = f"{file.split('/')[-1]}"
        exp_name = f"{id}"
        args = f"bash -c 'timeout 60s /home/gustavo/mcqdw/release/tools/misc/maxcliquedynweight --graph_file <(bzcat {file}) --strategy kpc'"
        exp = Experiment(args, exp_name)
        exp.attach_output_handler(extractor)
        KiwiRunner.add_experiment(exp)


    KiwiRunner.run_all()

    KiwiRunner.gen_report(JSONReporter)
    KiwiRunner.gen_report(Reporter)

if __name__ == '__main__':
    main()
