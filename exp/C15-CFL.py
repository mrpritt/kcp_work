import time
from kiwi.reporter import BaseReporter
from kiwi.kiwi import Kiwi
from kiwi.experiment import Experiment
from kiwi.defaults import DEFAULT_SETTINGS, JSONReporter

import re
import glob

# Create custom reporter
class CustomReporter(BaseReporter):
    @staticmethod
    def generate(results: list[dict]):
        report_id = time.time_ns()
        with open(f"results-{report_id}.csv", "w") as f:
            f.write("instance;weight;time;valid;\n")
            for exp in results:
                for run in exp['_runs']:
                    row = f"{exp['name']};"
                    row += f"{run['@weight']};"
                    row += f"{run['@time']};"
                    row += f"{int(run['@test'])};"
                    row += "\n"
                    f.write(row)

# Create an output handler
def extract_results(run_info: dict, byte_array: bytes):
    # Default values if run failed
    run_info['@weight'] = 0
    run_info['@time'] = 0
    run_info['@test'] = False
    output = byte_array.decode()
    pattern = r"\[w:(\d+),([0-9.]+)s\]"
    for line in output.splitlines():
        match = re.match(pattern, line)
        if match:
            run_info['@weight'] = int(match.group(1))  # Convert size to an integer
            run_info['@time'] = float(match.group(2))
            run_info['@test'] = True
            break
    
def main():
    SETTINGS = DEFAULT_SETTINGS.copy()
    SETTINGS['threads'] = 4

    KiwiRunner = Kiwi(SETTINGS)
    files = glob.glob('/home/gustavo/ic2024/data/set2/C15/*')

    for file in files:
        id = f"{file.split('/')[-1]}"
        exp_name = f"{id}"
        cmd = f"timeout 10s /home/gustavo/CFS/CFS {file}"
        exp = Experiment(cmd, exp_name)
        exp.attach_output_handler(extract_results)
        KiwiRunner.add_experiment(exp)


    KiwiRunner.run_all()

    KiwiRunner.gen_report(CustomReporter) # Save raw report

if __name__ == '__main__':
    main()
